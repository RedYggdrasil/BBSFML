#include "App/GameSystem/BBTextureLibrary.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasReader.h"
#include "RSFML/Tools/RLog.h"
#include "App/GameSystem/BBPaths.h"
#include <filesystem>

using namespace BB;


template<>
void BBAssetResult<RSfml::RGDXTPAtlasData>::ResetAndReport(BBTextureLibrary* InLibrary)
{
	return INNER_ResetAndReport(InLibrary);
}
template<>
void BBAssetResult<sf::Texture>::ResetAndReport(BBTextureLibrary* InLibrary)
{
	return INNER_ResetAndReport(InLibrary);
}
template<>
void BBAssetResult<sf::Font>::ResetAndReport(BBTextureLibrary* InLibrary)
{
	return INNER_ResetAndReport(InLibrary);
}
template<>
void BBAssetResult<sf::SoundBuffer>::ResetAndReport(BBTextureLibrary* InLibrary)
{
	return INNER_ResetAndReport(InLibrary);
}

template<typename TAsset>
void BB::BBAssetResult<TAsset>::INNER_ResetAndReport(BBTextureLibrary* InLibrary)
{
	if ((bool)pAsset)
	{
		pAsset.reset();
		bool bValue = InLibrary->ReportAssetUsageEnded<TAsset>(Key);
	}
	SilentReset();
}


BBTextureLibrary::BBTextureLibrary(IBBProgramObject* InProgramInstance)
: IBBProgramObject(InProgramInstance), m_atlasDatas(), m_textures()
{

}

BBTextureLibrary::~BBTextureLibrary()
{
}



template<typename TAsset>
bool BB::BBTextureLibrary::INNER_ReportAssetUsageEnded(const std::string_view InAssetKey)
{
	RSfml::RUnorderedStringMap<std::weak_ptr<TAsset>>& map = GetAssetMap<TAsset>();
	auto iter = map.find(InAssetKey);
	if (iter != map.end())
	{
		if (iter->second.expired())
		{
			map.erase(iter->first);
		}
		else
		{
			return false;
		}
	}
	return true;
}

template<typename TAsset>
BBAssetResult<TAsset> BB::BBTextureLibrary::INNER_GetAsset(const std::string_view InAssetPath)
{
	RSfml::RUnorderedStringMap<std::weak_ptr<TAsset>>& map = GetAssetMap<TAsset>();
	auto iter = map.find(InAssetPath);
	if (iter != map.end())
	{
		return INNER_GetLoadAsset<TAsset>(iter);
	}
	bool changedFolderPath = false;
	std::filesystem::path atlasKeyPath = BB::BBPaths::ExtractAssetFolderPath(InAssetPath,/*Out*/ changedFolderPath);

	if (!changedFolderPath)
	{
		std::string atlasKeyPathSTR = atlasKeyPath.string();
		return INNER_AddLoadAsset<TAsset>(atlasKeyPathSTR);
	}

	iter = map.find(atlasKeyPath.string());
	if (iter != map.end())
	{
		return INNER_GetLoadAsset<TAsset>(iter);
	}
	else
	{
		std::string altasKeyPathSTR = atlasKeyPath.string();
		return INNER_AddLoadAsset<TAsset>(altasKeyPathSTR);
	}
}

template <>
BBAssetResult<RSfml::RGDXTPAtlasData> BBTextureLibrary::GetAsset<RSfml::RGDXTPAtlasData>(const std::string_view InAssetPath)
{
	return INNER_GetAsset<RSfml::RGDXTPAtlasData>(InAssetPath);
}
template <>
BBAssetResult<sf::Texture> BBTextureLibrary::GetAsset<sf::Texture> (const std::string_view InAssetPath)
{
	return INNER_GetAsset<sf::Texture>(InAssetPath);
}

template <>
BBAssetResult<sf::Font> BBTextureLibrary::GetAsset<sf::Font>(const std::string_view InAssetPath)
{
	return INNER_GetAsset<sf::Font>(InAssetPath);
}
template <>
BBAssetResult<sf::SoundBuffer> BBTextureLibrary::GetAsset<sf::SoundBuffer>(const std::string_view InAssetPath)
{
	return INNER_GetAsset<sf::SoundBuffer>(InAssetPath);
}

template<>
bool BB::BBTextureLibrary::ReportAssetUsageEnded<RSfml::RGDXTPAtlasData>(const std::string_view InAssetKey)
{
	return INNER_ReportAssetUsageEnded<RSfml::RGDXTPAtlasData>(InAssetKey);
}

template<>
bool BB::BBTextureLibrary::ReportAssetUsageEnded<sf::Texture>(const std::string_view InAssetKey)
{
	return INNER_ReportAssetUsageEnded<sf::Texture>(InAssetKey);
}

template<>
bool BB::BBTextureLibrary::ReportAssetUsageEnded<sf::Font>(const std::string_view InAssetKey)
{
	return INNER_ReportAssetUsageEnded<sf::Font>(InAssetKey);
}

template<>
bool BB::BBTextureLibrary::ReportAssetUsageEnded<sf::SoundBuffer>(const std::string_view InAssetKey)
{
	return INNER_ReportAssetUsageEnded<sf::SoundBuffer>(InAssetKey);
}

template <typename TAsset>
BBAssetResult<TAsset> BBTextureLibrary::INNER_GetLoadAsset(RSfml::RUnorderedStringMap<std::weak_ptr<TAsset>>::iterator InIterator)
{
	BBAssetResult<TAsset> result;
	result.pAsset = InIterator->second.lock();
	if (!result.pAsset)
	{	//Expired texture, reload it
		result = INNER_AddLoadAsset<TAsset>(InIterator->first);
	}
	result.Key = InIterator->first;
	return result;
}
template <typename TAsset>
BBAssetResult<TAsset> BBTextureLibrary::INNER_AddLoadAsset(const std::string_view InKeyPath)
{
	RSfml::RUnorderedStringMap<std::weak_ptr<TAsset>>& map = GetAssetMap<TAsset>();
	std::shared_ptr<TAsset> asset = INNER_CreateAsset<TAsset>(InKeyPath);// INNER_CreateAltasData(InKeyPath);
	map.emplace(InKeyPath, asset);
	return BBAssetResult<TAsset>
		(
			/*.pAsset = */asset,
			/*.Key = */std::string(InKeyPath)
		);
}
template <typename TAsset>
std::shared_ptr<TAsset> BBTextureLibrary::INNER_CreateAsset(const std::string_view InKeyPath)
{
	return nullptr;
}
template <>
std::shared_ptr<RSfml::RGDXTPAtlasData> BBTextureLibrary::INNER_CreateAsset<RSfml::RGDXTPAtlasData>(const std::string_view InKeyPath)
{
	std::filesystem::path atlasPath = BBPaths::AssetsDiskPath();
	atlasPath.append(InKeyPath);
	atlasPath = atlasPath.lexically_normal();
	RSfml::RGDXTPAtlasData atlasData;
	if (!RSfml::RGDXTPAtlasReader::ReadFile(atlasPath.string(), /*Out*/atlasData))
	{
		return nullptr;
	}
	return std::make_shared<RSfml::RGDXTPAtlasData>(std::move(atlasData));
}
template <>
std::shared_ptr<sf::Texture> BBTextureLibrary::INNER_CreateAsset<sf::Texture>(const std::string_view InKeyPath)
{
	std::filesystem::path completePath = BB::BBPaths::AssetsDiskPath() / InKeyPath;
	return std::make_shared<sf::Texture>(completePath, false);
}
template <>
std::shared_ptr<sf::Font> BBTextureLibrary::INNER_CreateAsset<sf::Font>(const std::string_view InKeyPath)
{
	std::filesystem::path completePath = BB::BBPaths::AssetsDiskPath() / InKeyPath;
	return std::make_shared<sf::Font>(completePath);
}
template <>
std::shared_ptr<sf::SoundBuffer> BBTextureLibrary::INNER_CreateAsset<sf::SoundBuffer>(const std::string_view InKeyPath)
{
	std::filesystem::path completePath = BB::BBPaths::AssetsDiskPath() / InKeyPath;
	return std::make_shared<sf::SoundBuffer>(completePath);
}

#if _DEBUG
void BB::DEBUG_BBAssetResult_REPORT_ASSET_NOT_CLEANED(const std::string_view InKey)
{
	RLog::Log(LOG_ERROR, "Uncleaned asset with key {} !", InKey);
}
#endif
