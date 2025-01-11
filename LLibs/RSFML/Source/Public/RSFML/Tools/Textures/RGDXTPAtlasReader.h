#pragma once

#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
#include <string_view>

namespace RSfml
{
	class RGDXTPAtlasReader
	{
	public:
		STATIC_CLASS_R5(RGDXTPAtlasReader);

	public:
		static bool ReadFile(const std::string_view InFilePath, RGDXTPAtlasData& OutAtlasData);
	};
}