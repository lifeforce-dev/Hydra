//---------------------------------------------------------------
//
// RenderEngine.cpp
//

#include "RenderEngine.h"
#include "common/log.h"

namespace Client {

//===============================================================================

namespace {
std::shared_ptr<spdlog::logger> s_logger;

// Render state and API for rendering.
SDL_RendererPtr s_renderer = SDL_RendererPtr(nullptr, SDL_DestroyRenderer);

} // anon namespace

RenderEngine::RenderEngine()
{
	REGISTER_LOGGER("RenderEngine");
	s_logger = Log::Logger("RenderEngine");
}

RenderEngine::~RenderEngine()
{
}

bool RenderEngine::Initialize()
{
	// Init SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize SDL. error={}", SDL_GetError());
		return false;
	}

	// Init image loader.
	int imgFlags = (IMG_INIT_PNG | IMG_INIT_JPG);
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize PNG loader. error= {}", IMG_GetError());
		return false;
	}

	// Init font loader.
	if (TTF_Init() == -1)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize font loader. error={}", TTF_GetError());
		return false;
	}

	SPDLOG_LOGGER_INFO(s_logger, "SDL systems initialized successfully");
	return true;
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
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to create renderer. error=", SDL_GetError());
		return false;
	}

	return true;
}

SDL_Renderer* RenderEngine::GetRenderer()
{
	return s_renderer.get();
}

//===============================================================================

} // namespace Client
