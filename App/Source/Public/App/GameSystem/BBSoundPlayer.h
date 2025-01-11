#pragma once

#include "App/Defines/pch.h"
#include "RSFML/Tools/RCollection.h"
#include "App/GameSystem/BBMusicPlayer.h"
#include "App/GameSystem/BBSoundPlayerData.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include "App/Memory/IBBProgramObject.h"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <memory>
#include <vector>
#include <utility>

namespace BB
{
	struct BBSoundSource
	{
	public:
		sf::Sound SoundSource;
		std::shared_ptr<sf::SoundBuffer> SoundBuffer;
		BBSoundPlayerData PlayData;
	public:
		BBSoundSource();
		bool IsStopped() const { return SoundSource.getStatus() == sf::SoundSource::Status::Stopped; }
		bool IsPlaying() const { return SoundSource.getStatus() == sf::SoundSource::Status::Playing; }

		bool NeedCleaning() const { return IsBufferLoaded() && IsStopped(); }
		bool IsBufferLoaded() const { return (bool)SoundBuffer; }
		void FreeResource();
		void LoadPlaySound(std::shared_ptr<sf::SoundBuffer>&& InSoundBuffer, const BBSoundPlayerData& InPlayerData);
	};

	class BBSoundPlayer : public IBBProgramObject
	{
	protected:
		bool m_bInitialized;
		std::vector<BBSoundSource> m_soundSources;
		BBMusicPlayer m_musicPlayer;

	private:
		BBSoundPlayer(IBBProgramObject* InProgramInstance);
		friend BBProgramInstance;

	public:
		bool Initialize();
		inline void SetMusicPlayerData(const BBMusicPlayerData& InMusicData) { m_musicPlayer.SetMusicPlayerData(InMusicData); }
		bool DeInitialize();
		void PlaySound(std::shared_ptr<sf::SoundBuffer> InSoundBuffer, const BBSoundPlayerData& InPlayerData = BBSoundPlayerData::DEFAULT);
		void PlaySound(const std::vector<BB::BBSoundBufferResult>& InAvalableSoundBuffers, const BBSoundPlayerData& InPlayerData = BBSoundPlayerData::DEFAULT);
		void PlaySound(const std::vector<std::pair<BBSoundBufferResult, BBSoundPlayerData>>& InAvalableSoundBuffers);

		
		void StopAllSounds();
		void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime);

	public:
		R_VIRTUAL_IMPLICIT ~BBSoundPlayer() R_OVERRIDE_IMPLICIT;
	};
};