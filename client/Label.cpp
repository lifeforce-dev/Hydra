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

Label::Label(const std::string& text, const LabelOptions& options)
	: RenderableImpl()
	, m_text(text)
	, m_shouldWrapText(options.shouldWrapText)
	, m_color(options.color)
	, m_maxWidth(options.maxWidth)
	, m_geometry({ options.position.x, options.position.y, 0, 0 })
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

const std::string& Label::GetText() const
{
	return m_text;
}

void Label::SetText(const std::string& text)
{
	if (m_text != text)
	{
		m_text = text;
		UpdateTextureData();
	}
}

const SDL_Color& Label::GetColor() const
{
	return m_color;
}

void Label::SetColor(const SDL_Color& color)
{
	if (color != m_color)
	{
		m_color = color;
		UpdateTextureData();
	}
}

const SDL_Rect& Label::GetGeometry() const
{
	return m_geometry;
}

void Label::SetGeometry(const SDL_Rect& geometry)
{
	if (geometry == m_geometry)
	{
		return;
	}

	if (m_shouldWrapText
		&& geometry.w > m_maxWidth)
	{
		SetMaxWidth(geometry.w);
	}

	m_geometry = geometry;
}

glm::ivec2 Label::GetPosition() const
{
	return glm::vec2(m_geometry.x, m_geometry.y);
}

void Label::SetPosition(const glm::ivec2& position)
{
	if (position.x != m_geometry.x
		&& position.y != m_geometry.y)
	{
		m_geometry.x = position.x;
		m_geometry.y = position.y;
	}
}

GeometricSize Label::GetSize() const
{
	return glm::vec2(m_geometry.w, m_geometry.h);
}

void Label::SetSize(const GeometricSize& size)
{

	if (m_geometry.w != size.x
		&& m_geometry.h != size.y)
	{
		m_geometry.w = size.x;
		m_geometry.h = size.y;
	}
}

uint32_t Label::GetMaxWidth() const
{
	return m_maxWidth;
}

void Label::SetMaxWidth(uint32_t width)
{
	if (m_maxWidth != width)
	{
		m_maxWidth = width;
		UpdateTextureData();
	}
}

bool Label::ShouldWrapText() const
{
	return m_shouldWrapText;

}

void Label::SetShouldWrapText(bool shouldWrapText)
{
	if (m_shouldWrapText != shouldWrapText)
	{
		m_shouldWrapText = shouldWrapText;
	}
}

void Label::Render() const
{
	if (!m_textureData || m_text.empty())
	{
		return;
	}

	SDL_RenderCopy(m_renderer, m_textureData.get(), &m_sourceGlyphRectCache, &m_geometry);
}

void Label::UpdateTextureData()
{
	SDL_SurfacePtr textSurface = SDL_SurfacePtr(nullptr, SDL_FreeSurface);
	if (m_shouldWrapText)
	{
		textSurface = SDL_SurfacePtr(
			TTF_RenderText_Blended_Wrapped(g_game->GetDefaultFont(), m_text.c_str(), m_color, m_maxWidth),
			SDL_FreeSurface);
	}
	else
	{
		textSurface = SDL_SurfacePtr(
			TTF_RenderText_Blended(g_game->GetDefaultFont(), m_text.c_str(), m_color),
			SDL_FreeSurface);
	}

	if (!textSurface)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Unable to create surface data for label. error={}", SDL_GetError());
		return;
	}

	m_textureData = SDL_TexturePtr(
		SDL_CreateTextureFromSurface(m_renderer, textSurface.get()), SDL_DestroyTexture);

	if (!m_textureData)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Unable to create texture data for label. error={}", SDL_GetError());
		return;
	}

	UpdateLabelSize(textSurface->w, textSurface->h);
}

void Label::UpdateLabelSize(uint32_t width, uint32_t height)
{
	// Cache the size of the glyph texture. This is a minor optimization.
	m_sourceGlyphRectCache.w = width;
	m_sourceGlyphRectCache.h = height;

	m_geometry.w = width;
	m_geometry.h = height;
}

//===============================================================================

} // namespace Ui
