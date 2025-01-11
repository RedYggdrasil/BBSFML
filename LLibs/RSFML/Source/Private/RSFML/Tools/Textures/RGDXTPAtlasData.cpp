#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"

using namespace RSfml;

const RGDXTPAtlasTextureData RGDXTPAtlasTextureData::DEFAULT = 
{
	.ImageName = "",
	.Index = 0,
	.Bounds = {0, 0, 0, 0},
	.Split = {0, 0, 0, 0},
	.Pad = {0, 0, 0, 0}
};
const RGDXTPAtlasData RGDXTPAtlasData::DEFAULT = 
{
	.ImageFileName = "",
	.TextureDatas = RSfml::RUnorderedStringMap<RGDXTPAtlasTextureData>(),
	.Size = {0, 0},
	.Repeat = "none"
};

std::filesystem::path RGDXTPAtlasData::TexturePathFromAtlasPath(const std::filesystem::path& InAtlasPath)
{
	std::filesystem::path texturePath = InAtlasPath;
	if (texturePath.has_filename())
	{
		texturePath.remove_filename();
	}
	texturePath /= this->ImageFileName;
	texturePath = texturePath.lexically_normal();
	return texturePath;
}