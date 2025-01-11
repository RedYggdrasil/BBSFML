#pragma once

#include "RSFML/Defines/RSFML.h"
#include <filesystem>
#include <string>
#include "RSFML/Tools/RCollection.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>


namespace RSfml
{
	struct RGDXTPAtlasTextureData
	{
		static const RGDXTPAtlasTextureData DEFAULT;
	public:
		std::string ImageName;
		uint32_t Index;
		uint32_t Bounds[4];
		uint32_t Split[4];
		uint32_t ParentTextureSize[2];
		/// <summary>
		/// Left - Right - Top - Bottom
		/// </summary>
		uint32_t Pad[4];
		template<typename TNumerical>
		sf::Rect<TNumerical> RectFromBounds() const
		{
			return sf::Rect<TNumerical>({ (TNumerical)Bounds[0], (TNumerical)Bounds[1] }, { (TNumerical)Bounds[2], (TNumerical)Bounds[3] });
		}
		template<typename TNumerical>
		sf::Rect<TNumerical> RectFromBoundsMinusPad() const
		{
			sf::Rect<TNumerical> rect = RectFromBounds<TNumerical>();
			rect.position += sf::Vector2<TNumerical>(Pad[0], Pad[2]);
			rect.size -= sf::Vector2<TNumerical>(Pad[0] + Pad[1], Pad[2] + Pad[3]);
			return rect;
		}
	};

	struct RGDXTPAtlasData
	{
		static const RGDXTPAtlasData DEFAULT;
	public:
		std::string ImageFileName;
		RSfml::RUnorderedStringMap<RGDXTPAtlasTextureData> TextureDatas;

	public:
		uint32_t Size[2];
		std::string Repeat;
	public:
		std::filesystem::path TexturePathFromAtlasPath(const std::filesystem::path& InAtlasPath);
	};
}