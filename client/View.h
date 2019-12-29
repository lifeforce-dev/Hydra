//---------------------------------------------------------------
//
// View.h
//

#pragma once
#include "client/SDLTypes.h"

namespace Client {

//===============================================================================

enum struct ViewType : uint32_t
{
	// UI has its own z-ordering and is always rendered above game objects.
	UI = 0,

	// Game objects have their own z-ordering and are always rendered below UI.
	GameObject,
};

class View {
public:
	virtual ~View() {}

	// Returns the raw texture data used for rendering.
	virtual SDL_Texture* GetTexture() const = 0;

	// Returns the id of the View.
	virtual uint64_t GetId() const = 0;

	// Returns the z-ordering. Used for sorting all Views before being rendered.
	virtual int32_t GetZOrder() const = 0;
	
	// Returns whether or not the texture should be rendered.
	virtual bool IsVisible() const = 0;

	// Must be called from render loop.
	virtual void Render() const = 0;

	// Returns the type of view this is. Required for proper render ordering.
	virtual ViewType GetViewType() const = 0;
};

//===============================================================================

} // namespace Client
