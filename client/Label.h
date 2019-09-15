//---------------------------------------------------------------
//
// Label.h
//

#pragma once

#include "client/RenderableImpl.h"

#include <memory>
#include <string>

namespace Client {

// List of options. All of these options would cause
// recreation of the label texture if set individually
// Its a good idea to use these options on creation.
struct LabelOptions
{
	bool shouldWrapText = true;
	int32_t maxWidth = 0;
	SDL_Color color = { 0, 0, 0, 255 };
	glm::ivec2 position;
};

class Label : public RenderableImpl {
public:
	Label();
	Label(const std::string& text, const LabelOptions& options);
	virtual ~Label();

	void Initialize();

	// Getters and setters.
	const std::string& GetText() const;
	void SetText(const std::string& text);

	const SDL_Color& GetColor() const;
	void SetColor(const SDL_Color& color);

	const SDL_Rect& GetGeometry() const;
	void SetGeometry(const SDL_Rect& geometry);

	glm::ivec2 GetPosition() const;
	void SetPosition(const glm::ivec2& position);

	GeometricSize GetSize() const;
	void SetSize(const GeometricSize& size);

	uint32_t GetMaxWidth() const;
	void SetMaxWidth(uint32_t width);

	bool ShouldWrapText() const;
	void SetShouldWrapText(bool shouldWrapText);

	// RenderableImpl impl
	virtual void Render() const override;

private:

	// WARNING: Not meant for frequently updating text. Creates a whole new
	// texture for the updated text. For frequently changing text, a different approach is needed.
	void UpdateTextureData();

	// Updates internal geometry cache
	void UpdateLabelSize(uint32_t width, uint32_t height);

private:

	// Text to render.
	std::string m_text;

	// By default, text is wrapped.
	bool m_shouldWrapText = true;

	// Used for determining at which point to wrap text.
	int32_t m_maxWidth = 0;

	// Indicates what color to render the glyphs as.
	SDL_Color m_color{0, 0, 0, 255};

	// Label geometry as it is rendered within the world.
	// Notes:
	// When asked for geometry externally, this is what you want.
	// Unless explicitly set, this should be equivalent to the glyph rect cache.
	SDL_Rect m_geometry{ 0, 0, 0, 0 };

	// Internal only. Contains width and height of label. Top left is always (0, 0). Render() requires this
	// and rather than create it every call, we cache it when geometry updates.
	SDL_Rect m_sourceGlyphRectCache{ 0, 0, 0, 0 };
};

} // namespace Ui
