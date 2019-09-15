//---------------------------------------------------------------
//
// Renderable.h
//

#pragma once

#include "client/RenderEngineTypes.h"

namespace Client {

//===============================================================================

class Renderable {
public:
	virtual ~Renderable() {}

	// Returns the raw texture data used for rendering.
	virtual SDL_Texture* GetTexture() const = 0;

	// Returns the id of the renderable.
	virtual uint64_t GetId() const = 0;

	// Returns the z-ordering. Used for sorting all renderables before being rendered.
	virtual int32_t GetZOrder() const = 0;
	
	// Returns whether or not the texture should be rendered.
	virtual bool IsVisible() const = 0;

	// Must be called from render loop.
	virtual void Render() const = 0;
};

//===============================================================================

} // namespace Client
