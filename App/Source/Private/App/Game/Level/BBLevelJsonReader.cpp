#include "App/Game/Level/BBLevelJsonReader.h"
#include "App/Defines/BBGameBoardDef.h"
#include "RSFML/Tools/RLog.h"
#include "App/GameSystem/BBPaths.h"
//#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


using namespace BB;
using namespace BB::Level;

static bool s_bHasReadSourceFolder;
static size_t s_foundLevelCount = 0;

const char* LevelFolderName = "Levels";
const char* LevelFileNamePrefix = "Level_";

std::vector<BBBrickData> BBLevelJsonReader::LoadLevel(const size_t InLevelIndex)
{
	if (InLevelIndex < GetLevelCount())
	{
		return LoadLevel(m_levelFiles[InLevelIndex]);
	}
	return std::vector<BBBrickData>();
}

static std::vector<BBBrickData> LoadLevelFromJson(json& InData);
std::vector<BBBrickData> BB::Level::BBLevelJsonReader::LoadLevel(const std::filesystem::path& InLevelPath)
{
	std::vector<BBBrickData> result;


	std::ifstream levelFile(InLevelPath);
	if (levelFile)
	{
		try
		{
			json data = json::parse(levelFile);
			return LoadLevelFromJson(data);
		}
		catch (const std::exception& InException)
		{
			RLog::Log(LOG_ERROR, "Cannot open level '{}' with raised Json exception '{}'.", InLevelPath.string(), InException.what());
			return std::vector<BBBrickData>();
		}
		RLog::Log(LOG_DISPLAY, "File opened !");
	}
	return result;
}

std::filesystem::path BB::Level::BBLevelJsonReader::GetPathOfEndFile()
{
	std::filesystem::path levelsPath = BB::BBPaths::AssetsDiskPath() / LevelFolderName;
	//std::string path = "."; // Specify your directory path here

	std::string newFileName = LevelFileNamePrefix;

	if (m_levelFiles.size() == 0)
	{
		newFileName = newFileName.append("0");
	}
	else
	{
		std::string lastFilename = m_levelFiles.back().filename().string();
		std::string defaultPrefix = LevelFileNamePrefix;
		size_t prefixPos = lastFilename.find(defaultPrefix);
		if (prefixPos != std::string::npos)
		{
			lastFilename.erase(prefixPos, defaultPrefix.length());
			size_t foundLastIndex = std::stoul(lastFilename);
			newFileName = newFileName.append(std::to_string(++foundLastIndex));
		}
		else
		{
			RLog::Log(LOG_ERROR, "BBLevelJsonReader::GetPathOfEndFile failed");
		}
	}
	newFileName.append(".json");
	std::filesystem::path newFilePath = levelsPath / newFileName;
	newFilePath = newFilePath.lexically_normal();
	return newFilePath;
}

BBLevelJsonReader::BBLevelJsonReader(IBBProgramObject* InProgramInstance)
: IBBProgramObject(InProgramInstance), m_bIsInitialized(false), m_levelFiles()
{

}

bool BB::Level::BBLevelJsonReader::Initialize()
{
	std::filesystem::path levelsPath = BB::BBPaths::AssetsDiskPath() / LevelFolderName;
	//std::string path = "."; // Specify your directory path here

	const std::string prefix = LevelFileNamePrefix;

	std::unordered_map<size_t, std::filesystem::path> foundLevelFiles;
	std::vector<size_t> foundIndexes;
	for (const auto& entry : std::filesystem::directory_iterator(levelsPath))
	{
		std::string filename = entry.path().filename().string();
		size_t prefixPos = filename.find(prefix);
		if (prefixPos != std::string::npos)
		{
			filename.erase(prefixPos, prefix.length());
			size_t foundIndex = std::stoul(filename);
			foundLevelFiles.insert({ foundIndex, entry.path() });
			foundIndexes.push_back(foundIndex);
		}
	}
	std::sort(foundIndexes.begin(), foundIndexes.end());
	for (const auto& foundIndex : foundIndexes)
	{ 
		m_levelFiles.push_back(foundLevelFiles[foundIndex]);
	}
	m_bIsInitialized = true;
	return m_bIsInitialized;
}
bool BB::Level::BBLevelJsonReader::DeInitialize()
{
	m_levelFiles.clear();
	m_bIsInitialized = false;
	return true;
}
BBLevelJsonReader::~BBLevelJsonReader()
{
	if (m_bIsInitialized)
	{
		DeInitialize();
	}
}

void from_json(const json& InJson, BB::BBBrickData& data) 
{
	InJson.at("m_size").at("x").get_to(data.m_size.x);
	InJson.at("m_size").at("y").get_to(data.m_size.y);

	InJson.at("m_pos").at("x").get_to(data.m_pos.x);
	InJson.at("m_pos").at("y").get_to(data.m_pos.y);

	InJson.at("m_rotDegree").get_to(data.m_rotDegree);

	std::string textureName = InJson.at("TextureName").get<std::string>();
	std::copy(textureName.begin(), textureName.end(), data.TextureName.begin());
	data.TextureName[textureName.size()] = '\0';

	data.LifePoints = InJson.at("LifePoints").get<uint8_t>();

	data.PhysIndex = 0;
	data.GraphIndex = 0;
}

// Define how to convert BBBrickData to json
void to_json(json& j, const BBBrickData& data) 
{
	j = json{
		{"m_size", {{"x", data.m_size.x}, {"y", data.m_size.y}}},
		{"m_pos", {{"x", data.m_pos.x}, {"y", data.m_pos.y}}},
		{"m_rotDegree", data.m_rotDegree},
		{"TextureName", std::string(data.TextureName.data())},
		{"LifePoints", data.LifePoints}
		// Add additional fields as needed
	};
}

std::vector<BBBrickData> LoadLevelFromJson(json& InData)
{
	 std::vector<BBBrickData> results;

	 for (const auto& item : InData["Blocks"])
	 {
		 BBBrickData data;
		 from_json(item, data);
		 results.push_back(data);
	 }
	 return results;
}



std::string BB::Level::BBLevelJsonReader::LevelToJsonString(const std::vector<BBBrickData>& InLevel)
{
	//json j = { {"Blocks", InLevel} };

	json j;
	j["Blocks"] = json::array(); 
	for (const auto& block : InLevel)
	{
		json b;
		to_json(/*InOut*/b, block);
		j["Blocks"].push_back(b);
	}
	return j.dump(4);
}

bool BB::Level::BBLevelJsonReader::SaveLevel(const std::filesystem::path& InPath, const std::vector<BBBrickData>& InData)
{
	std::string levelAsString = LevelToJsonString(InData);

	std::ofstream file(InPath, std::ios::out | std::ios::trunc);
	// Check if the file is open 
	if (!file.is_open()) 
	{ 
		RLog::Log(LOG_ERROR, "Failed to open the file: '{}'", InPath.string()); 
		return false;
	}
	file << levelAsString;
	file.close();
	RLog::Log(LOG_DISPLAY, "Wrote to disk file: '{}' with {} bricks", InPath.string(), InData.size());
	return true;
}











