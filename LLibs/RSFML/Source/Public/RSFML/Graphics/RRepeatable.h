#pragma once

#include <SFML/Graphics.hpp>
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
namespace RSfml
{
	bool CreateRepeatable(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, const sf::Color& InVertexColor = sf::Color::White);
}