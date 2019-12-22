//---------------------------------------------------------------
//
// GameObjectView.cpp
//

#include "GameObjectView.h"


#include "client/RenderEngine.h"
#include "client/Game.h"

#include <cstdint>

namespace Client {

//===============================================================================

namespace {
	uint64_t s_currentId = 0;
}

//-------------------------------------------------------------------------------

GameObjectView::GameObjectView()
	: m_id(s_currentId++)
	, m_renderer(g_game->GetRenderEngine()->GetRenderer())
{
}

GameObjectView::~GameObjectView()
{
	m_renderer = nullptr;
}

void GameObjectView::Initialize(GameObjectViewData* data)
{
	m_data = data;
}

SDL_Texture* GameObjectView::GetTexture() const
{
	return m_textureData.get();
}

uint64_t GameObjectView::GetId() const
{
	return m_id;
}

bool GameObjectView::IsVisible() const
{
	return m_isVisible;
}

void GameObjectView::Render() const
{
	SDL_Rect rect;
	rect.w = static_cast<size_t>(m_data->size.x);
	rect.h = static_cast<size_t>(m_data->size.y);
	rect.x = static_cast<size_t>(m_data->position.x);
	rect.y = static_cast<size_t>(m_data->position.y);

	SDL_SetRenderDrawColor(RenderEngine::GetRenderer(), 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(RenderEngine::GetRenderer(), &rect);
}

ViewType GameObjectView::GetViewType() const
{
	return ViewType::GameObject;
}

int32_t GameObjectView::GetZOrder() const
{
	// TODO: Make this a real value.
	return 1;
}

//===============================================================================

} // namespace Client 
