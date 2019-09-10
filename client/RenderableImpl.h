//---------------------------------------------------------------
//
// RenderableImpl.h
//

#pragma once

#include "Renderable.h"

namespace Client {

//===============================================================================

class RenderableImpl : public Renderable {
public:
	RenderableImpl();
	virtual ~RenderableImpl();

	// Renderable impl
	virtual SDL_Texture* GetTexture() const override;
	virtual uint64_t GetId() const override;
	virtual int32_t GetZOrder() const override;
	virtual glm::vec2 GetPosition() const override;
	virtual float GetWidth() const override;
	virtual float GetHeight() const override;

protected:
	SDL_Color m_color{0, 0, 0, 0};
	bool m_isVisible = true;
	float m_width = 0.0f;
	float m_height = 0.0f;

	glm::vec2 m_position;

	SDL_TexturePtr m_textureData = SDL_TexturePtr(nullptr, SDL_DestroyTexture);
	SDL_Renderer* m_renderer;

private:
	uint64_t m_id = -1;
};

//===============================================================================

} // namespace Client
