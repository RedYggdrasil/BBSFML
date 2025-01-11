#pragma once
#include "RSFML/Defines/RSFML.h"

#include <filesystem>
#include <string>
#include <string_view>
namespace BB
{
	struct BBPaths
	{
	public:
		STATIC_STRUCT_R5(BBPaths);
	private:
		static std::filesystem::path m_runnablePath;
		static std::string m_runnablePath_str;
		static std::string_view m_runnablePath_view;
		static std::filesystem::path m_assetsDiskPath;
		static std::string m_assetsDiskPath_str;
		static std::string_view m_assetsDiskPath_view;

		static std::filesystem::path m_assetFolderAndSeparator;
		static std::string m_assetFolderAndSeparator_str;
		static std::string_view m_assetFolderAndSeparator_view;

	public:
		inline static const std::filesystem::path& RunnablePath() { return m_runnablePath; }
		inline static const std::string& RunnablePathSTR() { return m_runnablePath_str; }
		inline static std::string_view RunnablePathVIEW() { return m_runnablePath_view; }
		inline static const std::filesystem::path& AssetsDiskPath() { return m_assetsDiskPath; }
		inline static const std::string& AssetsDiskPathSTR() { return m_assetsDiskPath_str; }
		inline static std::string_view AssetsDiskPathVIEW() { return m_assetsDiskPath_view; }

		inline static const std::filesystem::path& AssetFolderAndSeparator() { return m_assetFolderAndSeparator; }
		inline static const std::string& AssetFolderAndSeparatorSTR() { return m_assetFolderAndSeparator_str; }
		inline static std::string_view AssetFolderAndSeparatorVIEW() { return m_assetFolderAndSeparator_view; }

		static std::filesystem::path ExtractAssetFolderPath(const std::string_view InAssetPath, bool& OutChanged);
		inline static std::filesystem::path ExtractAssetFolderPath(const std::string_view InAssetPath)
		{ bool _; return ExtractAssetFolderPath(InAssetPath, /*Out*/_); }
	private:
		friend class BBProgramInstance;
	};
}