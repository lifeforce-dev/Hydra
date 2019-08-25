//---------------------------------------------------------------
//
// Game.cpp
//

#include "Game.h"

#include "common/Log.h"
#include "client/GameClient.h"
#include "client/GameController.h"

#include <algorithm>
#include <SDL.h>

namespace Client {

//===============================================================================

namespace {
	const int32_t s_screenWidth = 1920;
	const int32_t s_screenHeight = 1080;

	std::shared_ptr<spdlog::logger> s_logger;

}

enum struct ImageKey : uint16_t
{
	None = 0,
	Up,
	Down,
	Left,
	Right
};

using SDL_SurfacePtr = std::unique_ptr < SDL_Surface, decltype(&SDL_FreeSurface)>;

class MainWindow {
public:

	MainWindow()
	{
	}

	~MainWindow()
	{
		Close();
	}

	bool Init()
	{
		if (m_window)
		{
			return true;
		}

		m_window = nullptr;
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize SDL. error={}", SDL_GetError());
			return false;
		}

		m_window = SDL_CreateWindow("Hydra", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			s_screenWidth, s_screenHeight, SDL_WINDOW_SHOWN);

		if (!m_window)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Failed to create SDL window. error={}", SDL_GetError());
			return false;
		}

		m_isOpen = true;

		m_screenSurface = SDL_GetWindowSurface(m_window);

		// Initialize surface.
		SDL_FillRect(m_screenSurface, nullptr, SDL_MapRGB(m_screenSurface->format, 0x00, 0x00, 0x00));

		if (!LoadMedia())
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Failed to load media", SDL_GetError());
		}
		else
		{
			SDL_BlitSurface(m_currentSurface, nullptr, m_screenSurface, nullptr);
		}

		return true;
	}
	
	void Close()
	{
		if (m_window)
		{
			SDL_DestroyWindow(m_window);
			m_window = nullptr;
		}

		m_currentSurface = nullptr;

		SDL_Quit();
	}

	bool IsOpen()
	{
		return m_isOpen;
	}

	void Update()
	{
		HandleInput();

		SDL_BlitSurface(m_currentSurface, nullptr, m_screenSurface, nullptr);
		SDL_UpdateWindowSurface(m_window);
	}

private:

	bool LoadMedia()
	{
		m_keySurfaces.emplace(ImageKey::Up, std::move(LoadSurfaceFromPath("resources/icons/up.bmp")));
		m_keySurfaces.emplace(ImageKey::Down, std::move(LoadSurfaceFromPath("resources/icons/down.bmp")));
		m_keySurfaces.emplace(ImageKey::Left, std::move(LoadSurfaceFromPath("resources/icons/left.bmp")));
		m_keySurfaces.emplace(ImageKey::Right, std::move(LoadSurfaceFromPath("resources/icons/right.bmp")));

		return std::all_of(std::cbegin(m_keySurfaces), std::cend(m_keySurfaces),
			[](const auto& surface)
		{
			return surface.second != nullptr;
		});
	}

	SDL_SurfacePtr LoadSurfaceFromPath(const std::string& path)
	{
		SDL_SurfacePtr s(SDL_LoadBMP(path.c_str()), SDL_FreeSurface);
		if (!s)
		{
			SPDLOG_LOGGER_ERROR(s_logger, "Failed to load resource. error{} resource{}",
				SDL_GetError(), path);

			return SDL_SurfacePtr(nullptr, SDL_FreeSurface);
		}
		return s;
	}

	void HandleInput()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			// Handle Quit
			if (e.type == SDL_QUIT)
			{
				m_isOpen = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					m_currentSurface = m_keySurfaces.at(ImageKey::Up).get();
					break;
				case SDLK_DOWN:
					m_currentSurface = m_keySurfaces.at(ImageKey::Down).get();
					break;
				case SDLK_LEFT:
					m_currentSurface = m_keySurfaces.at(ImageKey::Left).get();
					break;
				case SDLK_RIGHT:
					m_currentSurface = m_keySurfaces.at(ImageKey::Up).get();
					break;
				default:
					break;
				}
			}
		}
	}

private:

	// Whether the window is open or not.
	bool m_isOpen = false;

	// The window we will render to.
	SDL_Window* m_window = nullptr;

	// The rendering surface covering the entire rendering pane of the window.
	SDL_Surface* m_screenSurface = nullptr;

	// Pointer to the current image data we're rendering.
	SDL_Surface*  m_currentSurface = nullptr;

	// Map of image keys to image data.
	std::unordered_map<ImageKey, SDL_SurfacePtr> m_keySurfaces;
};

//-------------------------------------------------------------------------------

Game::Game()
	: m_gameController(std::make_unique<GameController>(this))
	, m_client(std::make_unique<GameClient>(this))
	, m_mainWindow(std::make_unique<MainWindow>())
{
	REGISTER_LOGGER("Game");
	s_logger = Log::Logger("Game");
}

Game::~Game()
{
}

bool Game::Init()
{
	m_isInitialized = m_mainWindow->Init();

	if (!m_isInitialized)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize game. Exiting...");
	}

	return m_isInitialized;
}

void Game::ConnectToServer()
{
	m_client->Start();
}


void Game::ProcessCallbackQueue()
{
	m_callbackQueue.SwapWithEmpty(m_processCbQueue);

	for (auto cb : m_processCbQueue)
	{
		if (cb)
		{
			cb();
		}
	}

	if (!m_processCbQueue.empty())
	{
		std::deque<std::function<void()>>().swap(m_processCbQueue);
	}
}

void Game::Run()
{
	ConnectToServer();
	while (m_mainWindow->IsOpen())
	{
		ProcessCallbackQueue();
		m_gameController->Run();
		m_mainWindow->Update();
	}
}

void Game::PostToMainThread(const std::function<void()>& cb)
{
	m_callbackQueue.Push(cb);
}

//===============================================================================

} // namespace Game

