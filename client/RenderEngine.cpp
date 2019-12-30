//---------------------------------------------------------------
//
// RenderEngine.cpp
//

#include "RenderEngine.h"

#include "client/Game.h"
#include "client/DebugController.h"
#include "client/RenderEngineEvents.h"
#include "client/View.h"
#include "common/log.h"

// Must be included first, otherwise would be alphabetical.
#include <GL/glew.h>

#include <algorithm>
#include <glm/vec4.hpp>
#include <imgui/imgui.h>

namespace Client {

//===============================================================================

namespace {
// Logger for the render engine.
std::shared_ptr<spdlog::logger> s_logger;

glm::vec4 s_clearColor = { 0.45f, 0.55f, 0.60f, 1.00f };

// TODO:
// Figure this out glDebugMessageCallback

} // anon namespace

//-------------------------------------------------------------------------------

SDL_GLContext RenderEngine::GetGLContext()
{
	return m_glContext;
}

RenderEngine::RenderEngine()
{
	REGISTER_LOGGER("RenderEngine");
	s_logger = Log::Logger("RenderEngine");
}

RenderEngine::~RenderEngine()
{
	m_views.clear();
}

bool RenderEngine::Initialize()
{
	// Init image engine.
	int imgFlags = (IMG_INIT_PNG | IMG_INIT_JPG);
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize PNG loader. error= {}", IMG_GetError());
		return false;
	}

	// Init font engine.
	if (TTF_Init() == -1)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize font loader. error={}", TTF_GetError());
		return false;
	}

	SPDLOG_LOGGER_INFO(s_logger, "SDL systems initialized successfully");

	SubscribeEvents();

	return true;
}

void RenderEngine::Render() const
{
	if (!m_glContext)
	{
		return;
	}

#ifdef DEBUG_BUILD
	// Log framerate
	//using namespace std::chrono;
	//static auto currentTime = time_point_cast<seconds>(steady_clock::now());
	//static int frameCount = 0;
	//static int frameRate = 0;
	//auto previousTime = currentTime;
	//currentTime = time_point_cast<seconds>(steady_clock::now());
	//++frameCount;
	//if (currentTime != previousTime)
	//{
	//	frameRate = frameCount;
	//	frameCount = 0;
	//}

	//SPDLOG_LOGGER_DEBUG(s_logger, "Frame rate:{}", frameRate);
#endif


	glViewport(0, 0, 1280, 720);
	glClearColor(s_clearColor.x, s_clearColor.y, s_clearColor.z, s_clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);

	// TODO: 

	// Render Debug UI
	// - Right now, this is a hack.
	g_game->GetDebugController()->Render();

	// Render UI
	// - Figure out what this means.

	// Render game objects
	for (auto r : m_views)
	{
		if (!r)
		{
			SPDLOG_LOGGER_WARN(s_logger, "Warning: Attempting to render non-existant element.");
			continue;
		}

		if (r->IsVisible())
		{
			r->Render();
		}
	}

	SDL_GL_SwapWindow(m_window);
}

void RenderEngine::RegisterView(View* view)
{
	m_views.insert(std::upper_bound(std::cbegin(m_views),
		std::cend(m_views), view,
		[](const View* lhs, const View* rhs)
		{
			return lhs->GetZOrder() > rhs->GetZOrder();
		}), view);
}

void RenderEngine::UnregisterView(uint32_t id)
{
	m_views.erase(std::remove_if(std::begin(m_views), std::end(m_views),
		[id](View* View)
		{
			return id == View->GetId();
		}));
}

void RenderEngine::SubscribeEvents()
{
	auto& events = g_game->GetRenderEngineEvents();
	events.GetViewAboutToBeDestroyedEvent().subscribe([this](uint32_t id)
	{
		UnregisterView(id);
	});

	events.GetViewCreatedEvent().subscribe([this](View* view)
	{
		RegisterView(view);
	});

	events.GetMainWindowSizeChangedEvent().subscribe([this]()
	{
		RepaintWindow();
	});

	events.GetMainWindowExposedEvent().subscribe([this]()
	{
		RepaintWindow();
	});

	events.GetMainWindowCreatedEvent().subscribe([this](SDL_Window* window)
	{
		m_window = window;
		m_glContext = SDL_GL_CreateContext(window);
	});

	events.GetMainWindowAboutToBeDestroyedEvent().subscribe([this](SDL_Window* window)
	{
		SDL_GL_DeleteContext(m_glContext);
		m_glContext = nullptr;
		m_window = nullptr;
	});
}

void RenderEngine::RepaintWindow()
{
	// TODO: Figure out what repaint means with OpenGL
}

// Helpers
void RenderEngine::UpdateDrawOrder()
{
	// Stable insertion sort. Should profile this, but I expect this to be quick
	// since our list of Views should always be mostly sorted.
	for (auto it = std::begin(m_views); it != std::end(m_views); ++it)
	{

		auto insertIndex = std::upper_bound(std::begin(m_views), it,
			*it,
			[](const View* lhs, const View* rhs)
			{
				if (lhs->GetZOrder() == rhs->GetZOrder())
				{
					return false;
				}
				return lhs->GetZOrder() > rhs->GetZOrder();
			});

#ifdef DEBUG_BUILD
		auto reorderDistance = std::distance(insertIndex, it);

		// We want to know if we're consistently horribly out of order. If so, maybe we don't want
		// an insertion sort.
		if (reorderDistance > 5)
		{
			SPDLOG_LOGGER_TRACE(s_logger, "Zordering large distance. distance=", reorderDistance);
		}
#endif // DEBUG_BUILD

		std::rotate(insertIndex, it, it + 1);
	}
}

//===============================================================================

} // namespace Client
