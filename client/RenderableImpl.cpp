//---------------------------------------------------------------
//
// RenderableImpl.cpp
//

#include "RenderableImpl.h"

#include "client/RenderEngine.h"
#include "client/Game.h"

namespace Client {

//===============================================================================

namespace {
	uint64_t s_currentId = 0;
}

//-------------------------------------------------------------------------------

RenderableImpl::RenderableImpl()
	: m_id(s_currentId++)
	, m_renderer(g_game->GetRenderEngine()->GetRenderer())
{
}

RenderableImpl::~RenderableImpl()
{
	m_renderer = nullptr;
}

SDL_Texture* RenderableImpl::GetTexture() const
{
	return m_textureData.get();
}

uint64_t RenderableImpl::GetId() const
{
	return m_id;
}

bool RenderableImpl::IsVisible() const
{
	return m_isVisible;
}

int32_t RenderableImpl::GetZOrder() const
{
	// TODO: Make this a real value.
	return 1;
}

//===============================================================================

} // namespace Client
