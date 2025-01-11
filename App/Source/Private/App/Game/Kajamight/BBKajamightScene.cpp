#include "App/Game/Kajamight/BBKajamightScene.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasReader.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/RLog.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include "App/GameSystem/BBPaths.h"
#include "App/GameSystem/BBProgramInstance.h"

using namespace BB;

using Super = BBGameBoardScene;

static const sf::Texture EMPTY_TEXTURE;

static void FillMapGoblinsBFA(BBMusicPlayerData& InOutData, const bool bInResetCurrent)
{

	constexpr float defaultVolume = 25.f;
	constexpr float defaultPitch = 1.f;

	if (bInResetCurrent)
	{
		InOutData = BBMusicPlayerData::DEFAULT;
	}

	InOutData.Musics.emplace
	(
		"Musics/mus_80_goblingreed_a.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);

	InOutData.Musics.emplace
	(
		"Musics/mus_80_goblingreed_b.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);

	InOutData.Musics.emplace
	(
		"Musics/mus_80_goblingreed_c.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);

	InOutData.Musics.emplace
	(
		"Musics/mus_80_goblingreed_d.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);

	InOutData.Musics.emplace
	(
		"Musics/mus_80_goblingreed_e.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);

	InOutData.Musics.emplace
	(
		"Musics/mus_80_goblingreed_h.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
}
static void FillMapGoblinsCata(BBMusicPlayerData& InOutData, const bool bInResetCurrent)
{

	constexpr float defaultVolume = 25.f;
	constexpr float defaultPitch = 1.f;

	if (bInResetCurrent)
	{
		InOutData = BBMusicPlayerData::DEFAULT;
	}

	InOutData.Musics.emplace
	(
		"Musics/mus_goblin_uu15.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
	InOutData.Musics.emplace
	(
		"Musics/mus_goblin_uu16.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
	InOutData.Musics.emplace
	(
		"Musics/mus_goblin_uu17.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
	InOutData.Musics.emplace
	(
		"Musics/mus_goblin_uu47.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
}
static void FillMapGoblinsParty(BBMusicPlayerData& InOutData, const bool bInResetCurrent)
{

	constexpr float defaultVolume = 25.f;
	constexpr float defaultPitch = 1.f;

	if (bInResetCurrent)
	{
		InOutData = BBMusicPlayerData::DEFAULT;
	}
	InOutData.Musics.emplace
	(
		"Musics/mus_goblinhotrodradioloop_eu01.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
	InOutData.Musics.emplace
	(
		"Musics/bf_goblins1.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
	InOutData.Musics.emplace
	(
		"Musics/bf_goblins2.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
	InOutData.Musics.emplace
	(
		"Musics/bf_goblins3.mp3",
		BBSoundPlayerData
		{
			.Volume = defaultVolume,
			.Pitch = defaultPitch
		}
	);
}
BBKajamightScene::BBKajamightScene(IBBProgramObject* InParentObject)
	:Super(InParentObject), m_kmGameUI(this), m_gbSprite(EMPTY_TEXTURE), m_bgSprite(EMPTY_TEXTURE)
{
	FillMapGoblinsBFA(/*InOut*/m_musicPlayerData, true);
	FillMapGoblinsCata(/*InOut*/m_musicPlayerData, false);
}

void BBKajamightScene::Start()
{
	Super::Start();


	static const std::string assetAtlasPath = "Sprites/KajamightScene/Scene.atlas";
	bool bAllSucessfull = true;
	bAllSucessfull = InitTextureByPath(assetAtlasPath) && bAllSucessfull;
	bAllSucessfull = m_kmGameUI.Initialize() && bAllSucessfull;//Need to be before sprite for correct SetPosition
	bAllSucessfull = InitializeSprites() && bAllSucessfull;
	RLog::Log(LOG_DISPLAY, "Kajascene texture sucessfully loaded '{}'", bAllSucessfull);
}

void BBKajamightScene::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	//Call Physic Pass
	Super::Tick(InGameTime, InDeltaTime);
	m_kmGameUI.ReportProgress(m_gameBoard->GetProgress());
}


void BBKajamightScene::Shutdown()
{
	bool bAllSucessfull = true;
	bAllSucessfull = m_kmGameUI.DeInitialize() && bAllSucessfull;
	bAllSucessfull = DeInitializeSprites() && bAllSucessfull;
	bAllSucessfull = DeInitializeTexture() && bAllSucessfull;
	RLog::Log(LOG_DISPLAY, "Kajascene texture sucessfully unloaded '{}'", bAllSucessfull);
	Super::Shutdown();
}

void BB::BBKajamightScene::RenderUnderBoard(sf::RenderWindow* InWindow)
{
	Super::RenderUnderBoard(InWindow);
	InWindow->draw(m_bgSprite);
	
	InWindow->draw(m_gameboardBackgroundSprite);
	InWindow->draw(m_gbSprite);
	//sf::RectangleShape rectangle({ 500, 500 });
	//rectangle.setFillColor(sf::Color::Green);
	//rectangle.setOrigin(sf::Vector2f(0.5f * rectangle.getSize().x, 0.5f* rectangle.getSize().y));
	//rectangle.setPosition(sf::Vector2f());
	//InWindow->draw(rectangle);
	//
	//sf::CircleShape shape(50);
	//shape.setPosition(sf::Vector2f());
	//InWindow->draw(shape);
}

void BB::BBKajamightScene::RenderOverBoard(sf::RenderWindow* InWindow)
{
	Super::RenderOverBoard(InWindow);
	InWindow->draw(m_gbFrame);
	m_kmGameUI.Render(InWindow);
	InWindow->draw(m_topCharacter);
}

bool BB::BBKajamightScene::InitTextureByPath(const std::string_view InSpritesheetRelativePath)
{
	bool bAllSucessfull = true;
	BBTextureLibrary* library = m_programInstance->GetTextureLibrary();

	//auto texRes = library->GetAsset<sf::Texture>(InSpritesheetRelativePath);
	
	m_atlasData = library->GetAsset<RSfml::RGDXTPAtlasData>(InSpritesheetRelativePath);
	if ((bool)m_atlasData)
	{
		std::filesystem::path textureRelativePath = m_atlasData.pAsset->TexturePathFromAtlasPath(InSpritesheetRelativePath);
		m_texturePack = library->GetAsset<sf::Texture>(textureRelativePath.string());
		if (!(bool)m_texturePack)
		{
			bAllSucessfull = false;
		}
	}
	else
	{
		bAllSucessfull = false;
	}
	static const std::string gbTexture = "Sprites/KajamightScene/Backgrounds/KajamightGameboardBackground.png";
	m_gbTexture = library->GetAsset<sf::Texture>(gbTexture);
	if (!m_gbTexture.pAsset)
	{
		bAllSucessfull = false;
	}

	static const std::string bgTexture = "Sprites/KajamightScene/Backgrounds/BackgroundTexture.png";
	m_bgTexture = library->GetAsset<sf::Texture>(bgTexture);
	if (!m_bgTexture.pAsset)
	{
		bAllSucessfull = false;
	}
	return bAllSucessfull;
}

bool BB::BBKajamightScene::DeInitializeTexture()
{
	if (m_programInstance)
	{
		BBTextureLibrary* library = m_programInstance->GetTextureLibrary();
		if (library)
		{
			if (!m_gbTexture.Key.empty())
			{
				m_gbTexture.ResetAndReport(library);
			}
			if (!m_bgTexture.Key.empty())
			{
				m_bgTexture.ResetAndReport(library);
			}
			if (!m_texturePack.Key.empty())
			{
				m_texturePack.ResetAndReport(library);
			}
			if (!m_atlasData.Key.empty())
			{
				m_atlasData.ResetAndReport(library);
			}
			return true;
		}
	}
	bool bAllSucessfull = 
		m_gbTexture.IsUnset() &&
		m_bgTexture.IsUnset() &&
		m_texturePack.IsUnset() &&
		m_atlasData.IsUnset();
	if (!bAllSucessfull)
	{
		m_gbTexture.SilentReset();
		m_bgTexture.SilentReset();
		m_texturePack.SilentReset();
		m_atlasData.SilentReset();
	}
	return bAllSucessfull;
}

bool BB::BBKajamightScene::InitializeSprites()
{
	bool bAllSucessfull = true;
	m_gameboardBackgroundSprite = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::Loop, *m_texturePack.pAsset.get(), m_atlasData.pAsset->TextureDatas["11xp_goblin_zeppelin01_WoodFloorLight_90d"],
		(sf::FloatRect)m_gameBoardRect, sf::Vector2f(0.5f, 0.5f));
	
	sf::Vector2u backgroundTextureSize = m_bgTexture.pAsset.get()->getSize();
	float scaleFactor = RSfml::ScaleToFill((sf::Vector2f)backgroundTextureSize, (sf::Vector2f)m_sceneMaximalView);

	scaleFactor *= 10.f;
	scaleFactor = std::ceil(scaleFactor);
	scaleFactor *= 0.1f;

	m_bgSprite = sf::Sprite(*m_bgTexture.pAsset.get());
	m_bgSprite.setOrigin((sf::Vector2f)backgroundTextureSize * 0.5f);
	m_bgSprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
	//m_gbSprite.setPosition((sf::Vector2f)m_gameBoardRect.position);
	//m_gbSprite.setColor(sf::Color(255, 255, 255, 128 + 64));

	m_gbSprite = sf::Sprite(*m_gbTexture.pAsset.get());
	m_gbSprite.setPosition((sf::Vector2f)m_gameBoardRect.position);
	m_gbSprite.setColor(sf::Color(255, 255, 255, 128 + 64));
	bAllSucessfull = InitializeBGFrame() && bAllSucessfull;
	return bAllSucessfull;
}

bool BB::BBKajamightScene::DeInitializeSprites()
{
	bool bAllSucessfull = true;
	bAllSucessfull = DeInitializeBGFrame() && bAllSucessfull;
	m_bgSprite = sf::Sprite(EMPTY_TEXTURE);
	m_gbSprite = sf::Sprite(EMPTY_TEXTURE);
	m_gameboardBackgroundSprite = RSfml::RGDXTPSprite();
	return bAllSucessfull;
}

bool BB::BBKajamightScene::InitializeBGFrame()
{
	bool bAllSucessfull = true;
	sf::IntRect frameToPRect = m_atlasData.pAsset->TextureDatas["uiframemechagon_top_big"].RectFromBounds<int>();
	int FrameHeight = frameToPRect.size.y;
	int FrameWidth = frameToPRect.size.y;
	int FrameHalfHeight = FrameHeight / 2;
	int FrameHalfWidth = FrameWidth / 2;

	const auto& ConterData = m_atlasData.pAsset->TextureDatas["uiframemechagon_corner_big"];
	const auto& BarData = m_atlasData.pAsset->TextureDatas["uiframemechagon_top_big"];
	sf::Vector2i CornerSize = ConterData.RectFromBounds<int>().size;

	sf::Vector2i TopLeft		= RSfml::RectTopLeft(m_gameBoardRect)		+ sf::Vector2i(-FrameHalfWidth, -FrameHalfHeight);
	sf::Vector2i TopRight		= RSfml::RectTopRight(m_gameBoardRect)		+ sf::Vector2i(+FrameHalfWidth, -FrameHalfHeight);

	sf::Vector2i BottomLeft		= RSfml::RectBottomLeft(m_gameBoardRect)	+ sf::Vector2i(-FrameHalfWidth, +FrameHalfHeight);
	sf::Vector2i BottomRight	= RSfml::RectBottomRight(m_gameBoardRect)	+ sf::Vector2i(+FrameHalfWidth, +FrameHalfHeight);

	int middleBarWidth = TopRight.x - TopLeft.x - (CornerSize.x * 2);
	int middleBarHeight = BottomLeft.y - TopLeft.y - (CornerSize.y * 2);

	std::vector<RSfml::RGDXTPSprite> frameSprites;
	frameSprites.resize(8);
	frameSprites[0] = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::Strench, *m_texturePack.pAsset.get(), ConterData,
		(sf::FloatRect)sf::IntRect(TopLeft, CornerSize), sf::Vector2f(0.5f, 0.5f));
	
	frameSprites[1] = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::Strench, *m_texturePack.pAsset.get(), ConterData,
		(sf::FloatRect)sf::IntRect(TopRight - sf::Vector2i(CornerSize.x, 0), CornerSize), sf::Vector2f(0.5f, 0.5f));
	frameSprites[1].SetRotation(sf::degrees(90.f));

	frameSprites[2] = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::Strench, *m_texturePack.pAsset.get(), ConterData,
		(sf::FloatRect)sf::IntRect(BottomRight - CornerSize, CornerSize), sf::Vector2f(0.5f, 0.5f));
	frameSprites[2].SetRotation(sf::degrees(180.f));

	frameSprites[3] = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::Strench, *m_texturePack.pAsset.get(), ConterData,
		(sf::FloatRect)sf::IntRect(BottomLeft - sf::Vector2i(0, CornerSize.y), CornerSize), sf::Vector2f(0.5f, 0.5f));
	frameSprites[3].SetRotation(sf::degrees(270.f));

	frameSprites[4] = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::Loop, *m_texturePack.pAsset.get(), BarData,
		(sf::FloatRect)sf::IntRect(TopLeft + sf::Vector2i(CornerSize.x, 0), sf::Vector2i(middleBarWidth, FrameHeight)), sf::Vector2f(0.5f, 0.5f));


	sf::Vector2i barSideSize = sf::Vector2i(middleBarHeight, FrameHeight);
	frameSprites[5] = RSfml::RGDXTPSprite (RSfml::RGDXTPSpriteType::Loop, *m_texturePack.pAsset.get(), BarData);
	frameSprites[5].SetVertWorldSize((sf::Vector2f)barSideSize);
	frameSprites[5].SetPosition((sf::Vector2f)(m_gameBoardRect.getCenter() + sf::Vector2i(m_gameBoardRect.size.x /2, 0)));
	frameSprites[5].SetRotation(sf::degrees(90.f));

	frameSprites[6] = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::Loop, *m_texturePack.pAsset.get(), BarData,
		(sf::FloatRect)sf::IntRect(BottomLeft + sf::Vector2i(CornerSize.x, -FrameHeight), sf::Vector2i(middleBarWidth, FrameHeight)), sf::Vector2f(0.5f, 0.5f));
	frameSprites[6].SetRotation(sf::degrees(180.f));


	frameSprites[7] = RSfml::RGDXTPSprite(RSfml::RGDXTPSpriteType::Loop, *m_texturePack.pAsset.get(), BarData);
	frameSprites[7].SetVertWorldSize((sf::Vector2f)barSideSize);
	frameSprites[7].SetPosition((sf::Vector2f)(m_gameBoardRect.getCenter() - sf::Vector2i(m_gameBoardRect.size.x / 2, 0)));
	frameSprites[7].SetRotation(sf::degrees(270.f));

	m_gbFrame = RSfml::RGDXTPMetaSprite(std::move(frameSprites));
	sf::IntRect characterTextureRect = m_atlasData.pAsset->TextureDatas["CharacterTop"].RectFromBounds<int>();

	m_topCharacter = RSfml::RGDXTPSprite
	( RSfml::RGDXTPSpriteType::Strench, *m_texturePack.pAsset.get(), m_atlasData.pAsset->TextureDatas["CharacterTop"]);
	m_topCharacter.SetVertOriginPercent(sf::Vector2f(0.5f, 1.f));
	m_topCharacter.SetPosition(sf::Vector2f(0.f, TopLeft.y + 24.f + 8.f));


	sf::FloatRect characterRect = m_atlasData.pAsset->TextureDatas["CharacterTop"].RectFromBounds<float>();
	float totalWidth = (float)(TopRight.x - TopLeft.x);

	float avalableWidthForScore = totalWidth - characterRect.size.x;
	float halfAvalableWidthForScore = avalableWidthForScore * 0.5f;
	float textRectHeight = 100.f;
	m_kmGameUI.SetCurrentLevelRect
	(
		sf::FloatRect
		(
			(sf::Vector2f)TopLeft + sf::Vector2f(0.f, -textRectHeight),
			sf::Vector2f(halfAvalableWidthForScore, textRectHeight)
		)
	);
	m_kmGameUI.SetScoreRect
	(
		sf::FloatRect
		(
			(sf::Vector2f)TopRight + sf::Vector2f(-halfAvalableWidthForScore, -textRectHeight),
			sf::Vector2f(halfAvalableWidthForScore, textRectHeight)
		)
	);
	return bAllSucessfull;
}

bool BB::BBKajamightScene::DeInitializeBGFrame()
{
	bool bAllSucessfull = true;
	m_topCharacter = RSfml::RGDXTPSprite();
	m_gbFrame = RSfml::RGDXTPMetaSprite();
	//for (size_t i = _countof(m_gbFrame); i > 0; --i)
	//{
	//	m_gbFrame[i - 1] = RSfml::RGDXTPSprite();
	//}
	return bAllSucessfull;
}

BBKajamightScene::~BBKajamightScene()
{
}