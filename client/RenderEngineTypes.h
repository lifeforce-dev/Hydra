//---------------------------------------------------------------
//
// RenderEngineTypes.h
//

#pragma once

#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace Client {

//===============================================================================

// 
using SDL_SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;
using SDL_TexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using SDL_RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using SDL_WindowPtr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using TTF_FontPtr = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;

//===============================================================================

} // namespace Client 
