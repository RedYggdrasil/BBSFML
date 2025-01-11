#pragma once
#include "App/Memory/IBBProgramObject.h"
#include "App/Game/BBBrickData.h"
#include <vector>
#include <filesystem>
#include <string>

namespace BB
{
	namespace Level
	{
		class BBLevelJsonReader : public IBBProgramObject
		{
		private:
			bool m_bIsInitialized;
			std::vector<std::filesystem::path> m_levelFiles;
		public:
			inline size_t GetLevelCount() const { return m_levelFiles.size(); };
			std::vector<BBBrickData> LoadLevel(const size_t InLevelIndex);
			std::vector<BBBrickData> LoadLevel(const std::filesystem::path& InLevelPath);
			std::string LevelToJsonString(const std::vector<BBBrickData>& InLevel);
			bool SaveLevel(const std::filesystem::path& InPath, const std::vector<BBBrickData>& InData);
			
			/// <summary>
			/// The path a file beyond currently last would have, meaning last + 1
			/// </summary>
			std::filesystem::path GetPathOfEndFile();
		private:
			BBLevelJsonReader(IBBProgramObject* InProgramInstance);
			friend BBProgramInstance;
		public:
			bool Initialize();
			bool DeInitialize();
			virtual ~BBLevelJsonReader();
		};
	}
}