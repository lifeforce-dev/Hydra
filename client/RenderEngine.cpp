//---------------------------------------------------------------
//
// RenderEngine.cpp
//

#include "RenderEngine.h"

#include "client/Renderable.h"
#include "common/log.h"

#include <algorithm>

namespace Client {

//===============================================================================

namespace {

// Logger for the render engine.
std::shared_ptr<spdlog::logger> s_logger;

// Render state and API for rendering.
SDL_RendererPtr s_renderer = SDL_RendererPtr(nullptr, SDL_DestroyRenderer);

// Incrementing render id ensures UID on all renderable objects.
uint64_t s_currentRenderId = 0;

} // anon namespace

//-------------------------------------------------------------------------------

// Static API
SDL_Renderer* RenderEngine::GetRenderer()
{
	return s_renderer.get();
}

RenderEngine::RenderEngine()
{
	REGISTER_LOGGER("RenderEngine");
	s_logger = Log::Logger("RenderEngine");
}

RenderEngine::~RenderEngine()
{
	m_renderables.clear();
}

bool RenderEngine::Initialize(SDL_Window* window)
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

	if (!CreateRenderer(window))
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to create renderer.");
		return false;
	}

	SPDLOG_LOGGER_INFO(s_logger, "SDL systems initialized successfully");
	return true;
}

void RenderEngine::Render() const
{
	SDL_SetRenderDrawColor(s_renderer.get(), 0, 0,0, 0xFF);
	SDL_RenderClear(s_renderer.get());

	// Render all registered renderables.
	for (auto r : m_renderables)
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

	SDL_RenderPresent(s_renderer.get());
}

void RenderEngine::RegisterRenderable(Renderable* renderable)
{
	m_renderables.insert(std::upper_bound(std::cbegin(m_renderables),
		std::cend(m_renderables), renderable,
		[](const Renderable* lhs, const Renderable* rhs)
		{
			return lhs->GetZOrder() > rhs->GetZOrder();
		}), renderable);
}

void RenderEngine::UnregisterRenderable(uint32_t id)
{
	m_renderables.erase(std::remove_if(std::begin(m_renderables), std::end(m_renderables),
		[id](Renderable* renderable)
		{
			return id == renderable->GetId();
		}));
}


// Helpers
void RenderEngine::UpdateDrawOrder()
{
	// Stable insertion sort. Should profile this, but I expect this to be quick
	// since our list of renderables should always be mostly sorted.
	for (auto it = std::begin(m_renderables); it != std::end(m_renderables); ++it)
	{

		auto insertIndex = std::upper_bound(std::begin(m_renderables), it,
			*it,
			[](const Renderable* lhs, const Renderable* rhs)
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

bool RenderEngine::CreateRenderer(SDL_Window* window)
{
	if (s_renderer)
	{
		return true;
	}

	s_renderer = SDL_RendererPtr(std::move(
		SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)),
		SDL_DestroyRenderer);

	if (!s_renderer)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to create renderer. error={}", SDL_GetError());
		return false;
	}

	return true;
}

//===============================================================================

} // namespace Client
