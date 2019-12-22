//---------------------------------------------------------------
//
// GameObjectData.h
//

#pragma once

#include <glm/vec2.hpp>

namespace Client {

//===============================================================================

class GameObjectViewData
{
public:
	glm::vec2 size = { 0.0f, 0.0f };
	glm::vec2 position = { 0.0f, 0.0f };
};

struct GameObjectData
{
	glm::vec2 currentDirection = { 0.0f, 0.0f };
	float speed = 0.0f;
};

//===============================================================================

} // namespace Client 
