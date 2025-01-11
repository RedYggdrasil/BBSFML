#pragma once
#include "RSFML/Tools/RCollection.h"
#include "App/GameSystem/BBSoundPlayerData.h"
#include <utility>
#include <string>
#include <unordered_map>

namespace BB
{
	struct BBMusicPlayerData
	{
	public:
		RSfml::RUnorderedStringMap<BBSoundPlayerData> Musics;
	public:
		static const BBMusicPlayerData DEFAULT;
	};
};