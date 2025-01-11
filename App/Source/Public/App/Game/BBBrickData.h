#pragma once
#include "App/Defines/pch.h"
#include "App/Game/Physics/BBPhysicBodyID.h"
#include <array>
#include <string>

namespace BB
{
	struct BBBrickData
	{
	public:
		sf::Vector2f m_size;
		sf::Vector2f m_pos;
		float m_rotDegree;
		std::array<char, 64> TextureName;
		uint8_t LifePoints;
		BBPhysicBodyID PhysIndex;
		size_t GraphIndex;
	};
};