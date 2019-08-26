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
#include <SDL_image.h>

namespace Client {

//===============================================================================

namespace {
	const int32_t s_screenWidth = 2560;
	const int32_t s_screenHeight = 1440;

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

enum struct Feature : uint16_t
{
	KeyPresses,
	StretchImages,
	LoadPng,
	HardwareAcceleration,
};

using SDL_SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;
using SDL_TexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using SDL_RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

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

		switch (m_currentFeature)
		{
		case Feature::KeyPresses:
			HandleTransitioningFeatures();
			SDL_BlitSurface(m_currentSurface, nullptr, m_screenSurface, nullptr);
			SDL_UpdateWindowSurface(m_window);
			SDL_FillRect(m_screenSurface, nullptr, SDL_MapRGB(m_screenSurface->format, 0x00, 0x00, 0x00));
			break;
		case Feature::StretchImages:
			HandleTransitioningFeatures();
			SDL_BlitScaled(m_foxSurface.get(), nullptr, m_screenSurface, &m_foxRect);
			SDL_UpdateWindowSurface(m_window);
			SDL_FillRect(m_screenSurface, nullptr, SDL_MapRGB(m_screenSurface->format, 0x00, 0x00, 0x00));
			break;
		case Feature::LoadPng:
		{
			HandleTransitioningFeatures();
			SDL_BlitSurface(m_foxSurface.get(), nullptr, m_screenSurface, nullptr);
			SDL_UpdateWindowSurface(m_window);
			SDL_FillRect(m_screenSurface, nullptr, SDL_MapRGB(m_screenSurface->format, 0x00, 0x00, 0x00));
			break;
		}
		case Feature::HardwareAcceleration:
			HandleTransitioningFeatures();
			SDL_RenderClear(m_renderer.get());
			SDL_RenderCopy(m_renderer.get(), m_foxTexture.get(), nullptr, &m_foxRect);
			SDL_RenderPresent(m_renderer.get());
			break;
		default:
			break;
		}
	}

private:

	// Handle any initialization that needs to happen when transitioning features.
	void HandleTransitioningFeatures()
	{
		if (!m_justTransitionedFeatures)
		{
			return;
		}

		switch (m_currentFeature)
		{
		case Feature::KeyPresses:
			break;
		case Feature::StretchImages:
		{
			// If you do this you preserve format of the screen, which doesn't have alpha.
			//auto optimized = SDL_ConvertSurface(m_foxSurface.get(), m_screenSurface->format, 0);
			m_foxSurface = SDL_SurfacePtr(std::move(SDL_LoadBMP("resources/sprites/fox-alpha.bmp")), SDL_FreeSurface);
			auto optimized = SDL_ConvertSurfaceFormat(m_foxSurface.get(), SDL_PIXELFORMAT_RGBA8888, 0);
			m_foxSurface.reset(std::move(optimized));
			m_foxRect.x = 65;
			m_foxRect.y = 65;
			m_foxRect.w = 170;
			m_foxRect.h = 98;
			m_currentSurface = optimized;
		}
		break;
		case Feature::LoadPng:
		{
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				SPDLOG_LOGGER_ERROR(s_logger, "Failed to load png. error= {}", IMG_GetError());
				return;
			}
			m_foxSurface = SDL_SurfacePtr(std::move(IMG_Load("resources/sprites/fox-alpha.png")),
				SDL_FreeSurface);
			auto optimized = SDL_ConvertSurfaceFormat(m_foxSurface.get(), SDL_PIXELFORMAT_RGBA8888, 0);
			if (!optimized)
			{
				SPDLOG_LOGGER_ERROR(s_logger, "Failed to load png. error= {}", SDL_GetError());
				return;
			}
			m_currentSurface = optimized;
		}
			break;
		case Feature::HardwareAcceleration:
		{
			if (!m_renderer)
			{
				m_renderer = SDL_RendererPtr(
					std::move(SDL_CreateRenderer(m_window,-1, SDL_RENDERER_ACCELERATED)),
						SDL_DestroyRenderer);
			}

			SDL_SetRenderDrawColor(m_renderer.get(), 0x00, 0x00, 0x00, 0xff);

			// Load into surface
			// Load surface into texture
			// Render the texture.
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				SPDLOG_LOGGER_ERROR(s_logger, "Failed to load png. error= {}", IMG_GetError());
				return;
			}
			m_foxSurface = SDL_SurfacePtr(std::move(IMG_Load("resources/sprites/fox-alpha.png")),
				SDL_FreeSurface);
			auto optimized = SDL_ConvertSurfaceFormat(m_foxSurface.get(), SDL_PIXELFORMAT_RGBA8888, 0);
			if (!optimized)
			{
				SPDLOG_LOGGER_ERROR(s_logger, "Failed to load png. error= {}", SDL_GetError());
				return;
			}
			m_currentSurface = optimized;
			m_foxTexture = SDL_TexturePtr(std::move(SDL_CreateTextureFromSurface(m_renderer.get(),
				m_currentSurface)), SDL_DestroyTexture);
			if (!m_foxTexture)
			{
				SPDLOG_LOGGER_ERROR(s_logger, "Failed to create texture. error= {}", SDL_GetError());
			}
			m_foxRect.x = 65;
			m_foxRect.y = 65;
			m_foxRect.w = 170;
			m_foxRect.h = 98;
		}
			break;
		default:
			break;
		}

		m_justTransitionedFeatures = false;
	}

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
					if (m_currentFeature != Feature::KeyPresses)
					{
						m_justTransitionedFeatures = true;
					}
					m_currentFeature = Feature::KeyPresses;
					
					break;
				case SDLK_DOWN:
					if (m_currentFeature != Feature::KeyPresses)
					{
						m_justTransitionedFeatures = true;
					}
					m_currentSurface = m_keySurfaces.at(ImageKey::Down).get();
					m_currentFeature = Feature::KeyPresses;
					break;
				case SDLK_LEFT:
					if (m_currentFeature != Feature::KeyPresses)
					{
						m_justTransitionedFeatures = true;
					}
					m_currentSurface = m_keySurfaces.at(ImageKey::Left).get();
					m_currentFeature = Feature::KeyPresses;
					break;
				case SDLK_RIGHT:
					if (m_currentFeature != Feature::KeyPresses)
					{
						m_justTransitionedFeatures = true;
					}
					m_currentSurface = m_keySurfaces.at(ImageKey::Right).get();
					m_currentFeature = Feature::KeyPresses;
					break;
				case SDLK_s:
					m_currentFeature = Feature::StretchImages;
					m_justTransitionedFeatures = true;
					break;
				case SDLK_p:
					m_currentFeature = Feature::LoadPng;
					m_justTransitionedFeatures = true;
					break;
				case SDLK_h:
					m_currentFeature = Feature::HardwareAcceleration;
					m_justTransitionedFeatures = true;
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

	// Image to stretch
	SDL_SurfacePtr m_foxSurface = SDL_SurfacePtr(nullptr, SDL_FreeSurface);
	SDL_Rect m_foxRect;

	bool m_justTransitionedFeatures = false;
	Feature m_currentFeature = Feature::KeyPresses;

	// Hardware accelerated rendering.
	SDL_RendererPtr m_renderer = SDL_RendererPtr(nullptr, SDL_DestroyRenderer);
	SDL_TexturePtr m_foxTexture = SDL_TexturePtr(nullptr, SDL_DestroyTexture);

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

