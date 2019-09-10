//---------------------------------------------------------------
//
// Label.cpp
//

#include "client/Label.h"

#include "client/Game.h"
#include "client/RenderEngine.h"
#include "common/Log.h"

namespace Client {

//===============================================================================

namespace {

// Logger for the render engine.
std::shared_ptr<spdlog::logger> s_logger;

} // namespace

//-------------------------------------------------------------------------------

Label::Label()
	: RenderableImpl()
{
	Initialize();
}

Label::Label(const std::string& text)
	: RenderableImpl()
	, m_text(text)
{
	Initialize();
	UpdateTextureData();
}

Label::~Label()
{
}

void Label::Initialize()
{
	REGISTER_LOGGER("Label");
	s_logger = Log::Logger("Label");
}

void Label::SetText(const std::string& text)
{
	if (m_text != text)
	{
		m_text = text;
		UpdateTextureData();
	}
}

const std::string& Label::GetText() const
{
	return m_text;
}

void Label::Render() const
{
	if (!m_textureData || m_text.empty())
	{
		return;
	}

	SDL_RenderCopy(m_renderer, m_textureData.get(), &rect, nullptr);
}

void Label::UpdateTextureData()
{
	SDL_SurfacePtr textSurface = SDL_SurfacePtr(std::move(
		TTF_RenderText_Solid(g_game->GetDefaultFont(), m_text.c_str(), m_color)), SDL_FreeSurface);
	if (!textSurface)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Unable to create surface data for label. error=", SDL_GetError());
		return;
	}

	m_textureData = SDL_TexturePtr(std::move(
		SDL_CreateTextureFromSurface(m_renderer, textSurface.get())), SDL_DestroyTexture);

	if (!m_textureData)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Unable to create texture data for label. error=", SDL_GetError());
		return;
	}

	m_width = textSurface->w;
	m_height = textSurface->h;
}

//===============================================================================

} // namespace Ui
