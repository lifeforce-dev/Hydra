//---------------------------------------------------------------
//
// Renderable.h
//

#pragma once

#include "client/RenderEngineTypes.h"

#include <glm/vec2.hpp>

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

	// Returns the position we should be rendered at.
	virtual glm::vec2 GetPosition() const = 0;
	
	// Returns the width of the renderable.
	virtual float GetWidth() const = 0;

	// Returns the height of the renderable.
	virtual float GetHeight() const = 0;

	// Must be called from render loop.
	virtual void Render() const = 0;
};

//===============================================================================

} // namespace Client
