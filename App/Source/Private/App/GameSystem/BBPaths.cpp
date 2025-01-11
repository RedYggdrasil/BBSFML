#include "App/GameSystem/BBPaths.h"

using namespace BB;

std::filesystem::path BBPaths::m_runnablePath;
std::string BBPaths::m_runnablePath_str;
std::string_view BBPaths::m_runnablePath_view;

std::filesystem::path BBPaths::m_assetsDiskPath;
std::string BBPaths::m_assetsDiskPath_str;
std::string_view BBPaths::m_assetsDiskPath_view;

std::filesystem::path BBPaths::m_assetFolderAndSeparator;
std::string BBPaths::m_assetFolderAndSeparator_str;
std::string_view BBPaths::m_assetFolderAndSeparator_view;

std::filesystem::path BBPaths::ExtractAssetFolderPath(const std::string_view InAssetPath, bool& OutChanged)
{
	std::filesystem::path path = InAssetPath;
	std::string strPath = "";
	std::string result = "";

	size_t pos = 0;
	std::string delimiter = BBPaths::AssetFolderAndSeparatorSTR();
	size_t delimiter_length = delimiter.length();
	size_t start = 0;
	while ((pos = strPath.find(delimiter, start)) != std::string::npos)
	{
		//result = strPath.substr(start, pos - start);
		start = pos + delimiter_length;
	} 
	if (start > 0)
	{
		result = strPath.substr(start);
		path = result;
		OutChanged = true;
	}
	std::filesystem::path lexicallyNormal = path.lexically_normal();

	if (!OutChanged)
	{
		OutChanged = (path != lexicallyNormal);
	}
	return lexicallyNormal;
}
