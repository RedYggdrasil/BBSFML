#pragma once
#include "App/GameSystem/BBMusicPlayerData.h"
#include "App/Memory/IBBProgramObject.h"
#include <unordered_set>
#include <SFML/Audio/Music.hpp>

namespace BB
{
	class BBMusicPlayer : public IBBProgramObject
	{
		struct BBMusicPlayerRunning
		{
		public:
			BBMusicPlayerData MusicPlayerData;
			std::unordered_set<std::string> MusicPlayed;
			std::string MusicPlaying;
		public:
			BBMusicPlayerRunning(const BBMusicPlayerData& InMusicPlayerData);
			std::string GetUnplayedMusicAndSetAsPlayingAndPlayed();
			bool HasMusicToPlay() const { return MusicPlayerData.Musics.size() > 0; }
		};
	protected:
		bool m_bInitialized;
		BBMusicPlayerRunning m_musicPlayerRunning;
		sf::Music m_musicPlayer;
	private:
		BBMusicPlayer(IBBProgramObject* InProgramInstance);
		friend class BBSoundPlayer;
	public:
		bool Initialize();
		void SetMusicPlayerData(const BBMusicPlayerData& InMusicData);
		bool DeInitialize();
		void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime);

	protected:
		bool INNER_StartPlayingMusic(const std::string& InMusicToPlay);

	public:
		R_VIRTUAL_IMPLICIT ~BBMusicPlayer() R_OVERRIDE_IMPLICIT;
	};
};