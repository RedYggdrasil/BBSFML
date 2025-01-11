#include "App/GameSystem/BBSoundPlayer.h"
#include "RSFML/Tools/RLog.h"
#include "App/GameSystem/BBProgramInstance.h"
#include <random>
#include <Tracy.hpp>

using namespace BB;

static const sf::SoundBuffer EMPTY_SOUND_BUFFER = sf::SoundBuffer();
BBSoundPlayer::BBSoundPlayer(IBBProgramObject* InProgramInstance)
	: IBBProgramObject(InProgramInstance), m_bInitialized(false), m_soundSources(10), m_musicPlayer(this)
{
	{

		ZoneScopedN("EMPTY_SOUND_BUFFER first read");
		//For no apparent reasons first creation of a sf::sound in the program can take up to a half a second !
		//theory was than EMPTY_SOUND_BUFFER buffer was full of unititalized samples but seems less likely after debugging.
		sf::Sound sound = sf::Sound(EMPTY_SOUND_BUFFER);
		if (m_soundSources.size() < 10)
		{
			m_soundSources.resize(10);
		}
	}
}

bool BB::BBSoundPlayer::Initialize()
{
	bool bAllSucessfull = true;

	m_bInitialized = bAllSucessfull;
	return bAllSucessfull;
}

bool BBSoundPlayer::DeInitialize()
{
	bool bAllSucessfull = true;
	StopAllSounds();
	bAllSucessfull = m_musicPlayer.DeInitialize() && bAllSucessfull;
	m_bInitialized = false;
	return bAllSucessfull;
}

void BBSoundPlayer::PlaySound(std::shared_ptr<sf::SoundBuffer> InSoundBuffer, const BBSoundPlayerData& InPlayerData)
{

	ZoneScopedN("PlaySound(std::shared_ptr<sf::SoundBuffer> InSoundBuffer, const BBSoundPlayerData& InPlayerData)");
	{
		for (size_t i = 0; i < m_soundSources.size(); ++i)
		{
			BBSoundSource& soundSource = m_soundSources[i];
			if (!soundSource.IsBufferLoaded())
			{
				soundSource.LoadPlaySound(std::move(InSoundBuffer), InPlayerData);
				return;
			}
		}
	}
	{
		RLog::Log(LOG_DISPLAY, "Resize sound source with new size [{}]", m_soundSources.size() + 1);
		ZoneScopedN("PlaySound resized soundSourceArray !");
		m_soundSources.resize(m_soundSources.size() + 1);
	}
	{
		m_soundSources.back().LoadPlaySound(std::move(InSoundBuffer), InPlayerData);
	}
}

void BBSoundPlayer::PlaySound(const std::vector<BB::BBSoundBufferResult>& InAvalableSoundBuffers, const BBSoundPlayerData& InPlayerData)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	
	std::uniform_int_distribution<size_t> dis(0, InAvalableSoundBuffers.size() - 1);
	PlaySound(InAvalableSoundBuffers[dis(gen)].pAsset, InPlayerData);
}

void BB::BBSoundPlayer::PlaySound(const std::vector<std::pair<BBSoundBufferResult, BBSoundPlayerData>>& InAvalableSoundBuffers)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_int_distribution<size_t> dis(0, InAvalableSoundBuffers.size() - 1);
	size_t selectedIndex = dis(gen);
	PlaySound(InAvalableSoundBuffers[selectedIndex].first.pAsset, InAvalableSoundBuffers[selectedIndex].second);
}

void BBSoundPlayer::StopAllSounds()
{
	for (size_t i = 0; i < m_soundSources.size(); ++i)
	{
		BBSoundSource& soundSource = m_soundSources[i];
		soundSource.FreeResource();
	}
}

void BBSoundPlayer::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	for (size_t i = 0; i < m_soundSources.size(); ++i)
	{
		BBSoundSource& soundSource = m_soundSources[i];
		if (soundSource.NeedCleaning())
		{
			soundSource.FreeResource();
		}
	}
	m_musicPlayer.Tick(InGameTime, InDeltaTime);
}

BBSoundPlayer::~BBSoundPlayer()
{
	if (m_bInitialized)
	{
		DeInitialize();
	}
}

BBSoundSource::BBSoundSource()
	:SoundSource(EMPTY_SOUND_BUFFER), SoundBuffer(nullptr), PlayData()
{
	//RLog::Log(LOG_DISPLAY, "BBSoundSource called with buffer sample count {} ", EMPTY_SOUND_BUFFER.getSampleCount());
}

void BBSoundSource::FreeResource()
{
	if (!IsBufferLoaded())
	{
		return;
	}
	if (SoundSource.getStatus() != sf::SoundSource::Status::Stopped)
	{
		SoundSource.stop();
	}
	//SoundSource.setBuffer(EMPTY_SOUND_BUFFER);
	SoundBuffer.reset();
	PlayData = BBSoundPlayerData();
	SoundSource = sf::Sound(EMPTY_SOUND_BUFFER);
}

void BBSoundSource::LoadPlaySound(std::shared_ptr<sf::SoundBuffer>&& InSoundBuffer, const BBSoundPlayerData& InPlayerData)
{
	SoundBuffer = std::move(InSoundBuffer);
	SoundSource = sf::Sound(*SoundBuffer.get());
	SoundSource.setVolume(InPlayerData.Volume);
	SoundSource.setPitch(InPlayerData.Pitch);
	SoundSource.play();
}
