#include "App/Game/Physics/BBCollisionResult.h"

const BB::BBCollisionResult BB::BBCollisionResult::NO_COLLISION =
BB::BBCollisionResult
{
	.P1 = sf::Vector2f(),
	.P2 = sf::Vector2f(),
	.Normal = sf::Vector2f(),
	.Depth = 0.f,
	.bDidCollide = false
};