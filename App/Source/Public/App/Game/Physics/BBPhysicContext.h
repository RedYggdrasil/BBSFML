#pragma once
#include <SFML/Graphics.hpp>
namespace BB
{
	class BBPhysicContext
	{
	public:
		sf::Vector2f GlobalGravity;
	};
	namespace PhysicContext
	{
		constexpr BBPhysicContext DEFAULT_EARTH =
		{
			.GlobalGravity = sf::Vector2f{ 0.f, 9.81f }
		};
		constexpr BBPhysicContext DEFAULT_MOON =
		{
			.GlobalGravity = sf::Vector2f{ 0.f, 1.62f }
		};
	};
};