#pragma once
#include "RSFML/Tools/RCollection.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
//#include "App/GameSystem/BBSoundPlayer.h"
#include "App/Memory/IBBProgramObject.h"
#include <memory>
#include <utility>
#include <SFML/Audio/SoundBuffer.hpp>

namespace BB
{
#if _DEBUG
	void DEBUG_BBAssetResult_REPORT_ASSET_NOT_CLEANED(const std::string_view InKey);
#endif
	class BBTextureLibrary;
	/// <summary>
	/// Result of a library request for asset, the move constructor and affectation
	/// exist many for Debug not to report uncleaned pointers for RValues/volatile results
	/// </summary>
	/// <typeparam name="TAsset">The asset type</typeparam>
	template <typename TAsset>
	struct BBAssetResult
	{
		std::shared_ptr<TAsset> pAsset;
		std::string Key;
	public:
		bool IsUnset() const { return Key.empty(); }
		explicit operator bool() const { return !IsUnset(); }
		void ResetAndReport(BBTextureLibrary* InLibrary);

		void SilentReset()
		{
			pAsset.reset();
			Key = std::string();
		}
		BBAssetResult() = default;
		BBAssetResult(std::shared_ptr<TAsset> InAsset, const std::string_view InView)
			: pAsset(InAsset), Key(InView)
		{}
		BBAssetResult(std::shared_ptr<TAsset>&& InAsset, const std::string_view InView)
			: pAsset(std::move(InAsset)), Key(InView)
		{}
		BBAssetResult(BBAssetResult&& other) noexcept 
			: pAsset(std::move(other.pAsset)), Key(std::move(other.Key)) 
		{ 
			other.pAsset.reset(); 
			other.Key.clear(); 
		}
		BBAssetResult& operator=(BBAssetResult&& other) noexcept
		{
			if (this != &other)
			{
				pAsset = std::move(other.pAsset);
				Key = std::move(other.Key);
				other.pAsset.reset();
				other.Key.clear();
			}
			return *this;
		}
		~BBAssetResult()
		{
#if _DEBUG
			if ((bool)pAsset)
			{
				DEBUG_BBAssetResult_REPORT_ASSET_NOT_CLEANED(Key);
			}
#endif
		}
	private:
		void INNER_ResetAndReport(BBTextureLibrary* InLibrary);
	};

	using BBAtlasResult			= BBAssetResult <RSfml::RGDXTPAtlasData>;
	using BBTextureResult		= BBAssetResult <sf::Texture>;
	using BBFontResult			= BBAssetResult<sf::Font>;
	using BBSoundBufferResult	= BBAssetResult<sf::SoundBuffer>;

	class BBTextureLibrary : public IBBProgramObject
	{
	protected:
		RSfml::RUnorderedStringMap<std::weak_ptr<RSfml::RGDXTPAtlasData>> m_atlasDatas;
		RSfml::RUnorderedStringMap<std::weak_ptr<sf::Texture>> m_textures;
		RSfml::RUnorderedStringMap<std::weak_ptr<sf::Font>> m_fonts;
		RSfml::RUnorderedStringMap<std::weak_ptr<sf::SoundBuffer>> m_sounds;

		template <typename TAsset>
		RSfml::RUnorderedStringMap<std::weak_ptr<TAsset>>& GetAssetMap(); 
		
		template <> RSfml::RUnorderedStringMap<std::weak_ptr<RSfml::RGDXTPAtlasData>>& GetAssetMap<RSfml::RGDXTPAtlasData>() { return m_atlasDatas; } 
		template <> RSfml::RUnorderedStringMap<std::weak_ptr<sf::Texture>>& GetAssetMap<sf::Texture>() { return m_textures; } 
		template <> RSfml::RUnorderedStringMap<std::weak_ptr<sf::Font>>& GetAssetMap<sf::Font>() { return m_fonts; } 
		template <> RSfml::RUnorderedStringMap<std::weak_ptr<sf::SoundBuffer>>& GetAssetMap<sf::SoundBuffer>() { return m_sounds; }

	public:
		template <typename TAsset> 
		BBAssetResult<TAsset> GetAsset(const std::string_view InAssetPath);

		template <typename TAsset>
		bool ReportAssetUsageEnded(const std::string_view InAssetKey);

		private:
		template <typename TAsset>
		bool INNER_ReportAssetUsageEnded(const std::string_view InAssetKey);

		template <typename TAsset>
		BBAssetResult<TAsset> INNER_GetAsset(const std::string_view InAssetPath);

		template <typename TAsset>
		BBAssetResult<TAsset> INNER_GetLoadAsset(RSfml::RUnorderedStringMap<std::weak_ptr<TAsset>>::iterator InIterator);

		template <typename TAsset>
		BBAssetResult<TAsset> INNER_AddLoadAsset(const std::string_view InKeyPath);
		template <typename TAsset>
		std::shared_ptr<TAsset> INNER_CreateAsset(const std::string_view InKeyPath);

	private:
		BBTextureLibrary(IBBProgramObject* InProgramInstance);
		friend BBProgramInstance;
	public:
		virtual ~BBTextureLibrary();

		template <typename TAsset>
		bool GetAssetsToVector(const std::string* InAssetPaths, const size_t InKeyCount, std::vector<BBAssetResult<TAsset>>& InOutArray)
		{
			bool bAllSucessfull = true;
			size_t startIndex = InOutArray.size();
			InOutArray.reserve(startIndex + InKeyCount);

			for (size_t i = 0; i < InKeyCount; ++i)
			{
				BBAssetResult<TAsset> tempResult = this->GetAsset<TAsset>(InAssetPaths[i]);
				if (tempResult.IsUnset())
				{
					bAllSucessfull = false;
					continue;
				}
				InOutArray.push_back(std::move(tempResult));
			}
			return bAllSucessfull;
		};
		template <typename TAsset, typename TOtherData>
		bool GetAssetsToVector(const std::string* InAssetPaths, const size_t InKeyCount, const TOtherData& InOtherDefaultValue, std::vector<std::pair<BBAssetResult<TAsset>, TOtherData>>& InOutArray)
		{
			bool bAllSucessfull = true;
			size_t startIndex = InOutArray.size();
			InOutArray.reserve(startIndex + InKeyCount);
		
			for (size_t i = 0; i < InKeyCount; ++i)
			{
				BBAssetResult<TAsset> tempResult = this->GetAsset<TAsset>(InAssetPaths[i]);
				if (tempResult.IsUnset())
				{
					bAllSucessfull = false;
					continue;
				}
				InOutArray.push_back(std::pair<BBAssetResult<TAsset>, TOtherData>(std::move(tempResult), InOtherDefaultValue));
			}
			return bAllSucessfull;
		};

		template <typename TAsset>
		void ReportAssetUsageEndedFromVector(std::vector<BBAssetResult<TAsset>>& InOutArray, const bool bInClearArray = true)
		{
			size_t keyCount = InOutArray.size();
			for (size_t i = 0; i < keyCount; ++i)
			{
				BBAssetResult<TAsset>& assetReport = InOutArray[i];
				if (!assetReport.IsUnset())
				{
					assetReport.ResetAndReport(this);
				}
			}
			if (bInClearArray)
			{
				InOutArray.clear();
			}
		}

		template <typename TAsset, typename TOtherData>
		void ReportAssetUsageEndedFromVector(std::vector<std::pair<BBAssetResult<TAsset>, TOtherData>>& InOutArray, const bool bInClearArray = true)
		{
			size_t keyCount = InOutArray.size();
			for (size_t i = 0; i < keyCount; ++i)
			{
				BBAssetResult<TAsset>& assetReport = InOutArray[i].first;
				if (!assetReport.IsUnset())
				{
					assetReport.ResetAndReport(this);
				}
			}
			if (bInClearArray)
			{
				InOutArray.clear();
			}
		}
	};


};