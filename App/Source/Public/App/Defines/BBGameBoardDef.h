#pragma once
#include "RSFML/Tools/RMath.h"
#include <SFML/Graphics/Rect.hpp>

namespace BB
{
	constexpr sf::Vector2i I_BOARD_CENTER(0, -256);
	constexpr sf::Vector2i I_BOARD_SIZE(2048, 1536);
	constexpr sf::Vector2i I_BOARD_HALF_SIZE(I_BOARD_SIZE.x/2, I_BOARD_SIZE.y/2);
	constexpr sf::Vector2i I_BOARD_MAX_DISTANCE_TO_CENTER
	(
		std::max(RSfml::RSfmlAbs(I_BOARD_CENTER.x - I_BOARD_HALF_SIZE.x), RSfml::RSfmlAbs(I_BOARD_CENTER.x + I_BOARD_HALF_SIZE.x)),
		std::max(RSfml::RSfmlAbs(I_BOARD_CENTER.y - I_BOARD_HALF_SIZE.y), RSfml::RSfmlAbs(I_BOARD_CENTER.y + I_BOARD_HALF_SIZE.y))
	);
	constexpr sf::Vector2i I_BOARD_SIZE_AROUND_CENTER  = I_BOARD_MAX_DISTANCE_TO_CENTER * 2 ;

	constexpr sf::Vector2f F_BOARD_CENTER = (sf::Vector2f)I_BOARD_CENTER;
	constexpr sf::Vector2f F_BOARD_SIZE = (sf::Vector2f)I_BOARD_SIZE;

	constexpr sf::IntRect I_BOARD_RECT(-sf::Vector2i(I_BOARD_SIZE.x / 2, I_BOARD_SIZE.y / 2) - I_BOARD_CENTER, I_BOARD_SIZE);
	constexpr sf::FloatRect F_BOARD_RECT(( - F_BOARD_SIZE * 0.5f) - F_BOARD_CENTER, F_BOARD_SIZE);
};