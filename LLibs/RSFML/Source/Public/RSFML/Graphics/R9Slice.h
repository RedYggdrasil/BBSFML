#pragma once
#include <SFML/Graphics.hpp>
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
namespace RSfml
{
	constexpr size_t NINE_SLICE_VERT_COUNT = 3 * 3 * 6;
	bool Create9SliceSquareAutoScale(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, const sf::Color& InVertexColor = sf::Color::White);
	bool Create9SliceSquareAutoScale(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, float InExpSteps, const sf::Color& InVertexColor = sf::Color::White);
	bool Create9SliceSquare(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, const sf::Vector2f& InBorderScaling = { 1.f, 1.f }, const sf::Color& InVertexColor = sf::Color::White);
}