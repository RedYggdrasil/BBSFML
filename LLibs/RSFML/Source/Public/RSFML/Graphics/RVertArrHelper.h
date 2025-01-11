#pragma once
#include <SFML/Graphics.hpp>
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
namespace RSfml
{
	bool RotateVertices(sf::VertexArray& InOutVertArray, const size_t InVertStartIndex, const size_t InVertCount, const sf::Angle InRotation, const sf::Vector2f InOrigin);
	bool TransformVertices(sf::VertexArray& InOutVertArray, const size_t InVertStartIndex, const size_t InVertCount, const sf::Transform& InTransform);
};