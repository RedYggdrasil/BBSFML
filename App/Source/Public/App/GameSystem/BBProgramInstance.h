#pragma once
#include "App/Defines/pch.h"
#include "App/GameSystem/BBSoundPlayer.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include "App/Game/Level/BBLevelJsonReader.h"
#include "App/Memory/IBBProgramObject.h"
#include <memory>
#include <vector>

namespace BB
{
	class BBScene;
	struct BBTime
	{
	private:
		bool m_bIsPaused;
		sf::Clock m_programClock;
		sf::Time m_timeAtLastPause;
	public:
		sf::Time CurrentTime;
		sf::Time CurrentElipsedTime;
	public:
		void StartTimer();
		void PauseTimer();
		void ResumeTimer();
		void TickTimer();
	};
	class BBProgramInstance : public IBBProgramObject
	{
	protected:
		bool bUsingFullscreenMode;
		bool m_bShouldRunEditor;
		sf::RenderWindow m_window;
		BBTime m_time;
		BBTextureLibrary m_textureLibrary;
		Level::BBLevelJsonReader m_levelJsonReader;
		BBSoundPlayer m_soundPlayer;

	protected:
		std::vector<std::shared_ptr<BBScene>> m_activeScenes;

	public:
		bool GetIsStartInEditor() const { return m_bShouldRunEditor; }
		const BBTime& GetTimeData() const { return m_time; }
		inline BBTextureLibrary* GetTextureLibrary() { return &m_textureLibrary; };
		inline Level::BBLevelJsonReader* GetLevelReader() { return &m_levelJsonReader; };
		inline BBSoundPlayer* GetSoundPlayer() { return &m_soundPlayer; };
		const sf::RenderWindow& GetWindow() const { return m_window; }
		sf::RenderWindow& GetWindow() { return m_window; }

	public:
		void PlaySound(std::shared_ptr<sf::SoundBuffer> InSoundBuffer, const BBSoundPlayerData& InPlayerData = BBSoundPlayerData::DEFAULT)
		{ m_soundPlayer.PlaySound(InSoundBuffer, InPlayerData); }
		void PlaySound(const std::vector<BBSoundBufferResult>& InAvalableSoundBuffers, const BBSoundPlayerData& InPlayerData = BBSoundPlayerData::DEFAULT)
		{ m_soundPlayer.PlaySound(InAvalableSoundBuffers, InPlayerData); }
		void PlaySound(const std::vector<std::pair<BBSoundBufferResult, BBSoundPlayerData>>&InAvalableSoundBuffers)
		{
			m_soundPlayer.PlaySound(InAvalableSoundBuffers);
		}

	public:
		BBProgramInstance();
		void Run(int argc, char** argv);
		R_VIRTUAL_IMPLICIT ~BBProgramInstance() R_OVERRIDE_IMPLICIT;

		void Init();
		void Shutdown();

		void OnWindowResized(const sf::Vector2u InNewSize);

	private:
		void ApplyWindowFullscreenMode(const bool bInNewWindowFullscreenMode);
		bool InitializePaths() const;
	};
};