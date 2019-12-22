//---------------------------------------------------------------
//
// RenderEngineTypes.h
//

#pragma once

#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <glm/vec2.hpp>

namespace Client {

//===============================================================================

using SDL_SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;
using SDL_TexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using SDL_RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using SDL_WindowPtr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using SDL_FontPtr = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;

// x-> width, y -> height
using GeometricSize = glm::ivec2;

static inline bool operator==(const SDL_Color& lhs, const SDL_Color& rhs)
{
	return lhs.r == rhs.r
		&& lhs.g == rhs.g
		&& lhs.b == rhs.b
		&& lhs.a == rhs.a;
}

static inline bool operator!=(const SDL_Color& lhs, const SDL_Color& rhs)
{
	return !(lhs == rhs);
}

static inline bool operator==(const SDL_Rect& lhs, const SDL_Rect& rhs)
{
	return lhs.h == rhs.h
		&& lhs.w == rhs.w
		&& lhs.x == rhs.x
		&& lhs.y == rhs.y;
}

static inline bool operator!=(const SDL_Rect& lhs, const SDL_Rect& rhs)
{
	return !(lhs == rhs);
}

//===============================================================================

} // namespace Client 
