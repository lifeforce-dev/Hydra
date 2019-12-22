//---------------------------------------------------------------
//
// GameObjectView.h
//

#pragma once

#include "client/View.h"

#include "client/GameObjectData.h"
#include "client/RenderEngineTypes.h"

#include <cstdint>
namespace Client {

//===============================================================================

class GameObjectView : public View {
public:
	GameObjectView();
	virtual ~GameObjectView();

	void Initialize(GameObjectViewData* data);

	// View impl
	virtual SDL_Texture* GetTexture() const override;
	virtual uint64_t GetId() const override;
	virtual int32_t GetZOrder() const override;
	virtual bool IsVisible() const override;
	virtual void Render() const override;
	virtual ViewType GetViewType() const override;

private:
	bool m_isVisible = true;
	SDL_TexturePtr m_textureData = SDL_TexturePtr(nullptr, SDL_DestroyTexture);
	SDL_Renderer* m_renderer;
	const GameObjectViewData* m_data;
	uint64_t m_id = -1;
};

//===============================================================================

} // namespace Client 
