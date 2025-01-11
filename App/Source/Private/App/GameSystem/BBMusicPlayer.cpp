#include "App/GameSystem/BBMusicPlayer.h"
#include "App/GameSystem/BBPaths.h"
#include <random>

using namespace BB;

static const std::string EMPTY_STRING = std::string();

BBMusicPlayer::BBMusicPlayer(IBBProgramObject* InProgramInstance)
: IBBProgramObject(InProgramInstance), m_bInitialized(false), m_musicPlayerRunning(BBMusicPlayerData::DEFAULT),
m_musicPlayer()
{
	m_musicPlayer.setLooping(false);
}

bool BBMusicPlayer::Initialize()
{
	bool bAllSucessfull = true;

	m_bInitialized = bAllSucessfull;
	return bAllSucessfull;
}

void BBMusicPlayer::SetMusicPlayerData(const BBMusicPlayerData& InMusicData)
{
	m_musicPlayer.stop();
	m_musicPlayerRunning = BBMusicPlayerData(InMusicData);

}

bool BBMusicPlayer::DeInitialize()
{
	bool bAllSucessfull = true;
	m_musicPlayer.stop();

	m_bInitialized = false;
	return bAllSucessfull;
}

void BBMusicPlayer::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	if (!m_musicPlayerRunning.HasMusicToPlay())
	{
		return;
	}
	sf::SoundSource::Status status = m_musicPlayer.getStatus();
	if (status == sf::SoundSource::Status::Stopped)
	{
		std::string musicToPlay = m_musicPlayerRunning.GetUnplayedMusicAndSetAsPlayingAndPlayed();
		if (!musicToPlay.empty())
		{
			INNER_StartPlayingMusic(musicToPlay);
		}
	}

}

bool BBMusicPlayer::INNER_StartPlayingMusic(const std::string& InMusicToPlay)
{
	const BBSoundPlayerData& soundPlayerData = m_musicPlayerRunning.MusicPlayerData.Musics[InMusicToPlay];

	std::filesystem::path completePath = BB::BBPaths::AssetsDiskPath() / InMusicToPlay;
	completePath = completePath.lexically_normal();

	if (!m_musicPlayer.openFromFile(completePath))
	{
		return false;
	}
	m_musicPlayer.setVolume(soundPlayerData.Volume);
	m_musicPlayer.setPitch(soundPlayerData.Pitch);
	m_musicPlayer.play();
	return true;
}

BBMusicPlayer::~BBMusicPlayer()
{
	if (m_bInitialized)
	{
		DeInitialize();
	}
}

BBMusicPlayer::BBMusicPlayerRunning::BBMusicPlayerRunning(const BBMusicPlayerData& InMusicPlayerData)
	: MusicPlayerData(InMusicPlayerData), MusicPlayed(), MusicPlaying(EMPTY_STRING)
{
	MusicPlayed.reserve(MusicPlayerData.Musics.size());
	//for (size_t i = 0; i < MusicPlayerData.Musics.size(); ++i)
	//{
	//
	//}
}

std::string BBMusicPlayer::BBMusicPlayerRunning::GetUnplayedMusicAndSetAsPlayingAndPlayed()
{

	if (MusicPlayerData.Musics.size() < 1)
	{
		return EMPTY_STRING;
	}
	if (MusicPlayed.size() == MusicPlayerData.Musics.size())
	{
		//All music already played
		MusicPlayed.clear();
	}

	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::vector<std::string> availableMusics; 
	for (const auto& pair : MusicPlayerData.Musics)
	{
		if (MusicPlayed.find(pair.first) == MusicPlayed.end())
		{ 
			availableMusics.push_back(pair.first);
		}
	}

	std::uniform_int_distribution<size_t> dis(0, availableMusics.size() - 1);
	
	size_t randomIndex = dis(gen);
	std::string selectedMusic = availableMusics[randomIndex]; 
	MusicPlayed.insert(selectedMusic);
	MusicPlaying = selectedMusic;
	return selectedMusic;
}
