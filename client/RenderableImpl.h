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
	virtual bool IsVisible() const override;
	virtual int32_t GetZOrder() const override;

protected:
	bool m_isVisible = true;

	SDL_TexturePtr m_textureData = SDL_TexturePtr(nullptr, SDL_DestroyTexture);
	SDL_Renderer* m_renderer;

private:
	uint64_t m_id = -1;
};

//===============================================================================

} // namespace Client
