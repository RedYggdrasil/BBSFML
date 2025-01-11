#include "App/Game/Kajamight/BBKajamightUI.h"
#include "App/Game/Kajamight/BBKajamightScene.h"
#include "App/GameSystem/BBProgramInstance.h"
#include <format>


static const sf::Font EMPTY_FONT;
using namespace BB;
using Super = BBGameUI;
BBKajamightUI::BBKajamightUI(BBKajamightScene* InSceneObject)
:Super(InSceneObject), m_initialized(false), _kmScene(InSceneObject), m_fontAsset(),
m_currentLevelText(EMPTY_FONT), m_scoreText(EMPTY_FONT), m_currentLevelRect(), m_scoreRect()
{
}

bool BB::BBKajamightUI::Initialize()
{
	bool bAllSucessfull = true;
	BBTextureLibrary* library = m_programInstance->GetTextureLibrary();
	static const std::string fontAssetPath = "Fonts/FRIZQUAD.TTF";
	m_fontAsset = library->GetAsset<sf::Font>(fontAssetPath);
	if (!m_fontAsset.pAsset)
	{
		bAllSucessfull = false;
	}
	if (bAllSucessfull)
	{
		bAllSucessfull = InitializeGraphics() && bAllSucessfull;
	}
	m_initialized = bAllSucessfull;
	return bAllSucessfull;
}

bool BB::BBKajamightUI::InitializeGraphics()
{
	bool bAllSucessfull = true;

	constexpr sf::Text::Style textsStyle = (sf::Text::Style)(sf::Text::Bold/* | sf::Text::Underlined*/);
	constexpr uint32_t fontSize = 86u;
	m_currentLevelText = sf::Text(*m_fontAsset.pAsset.get());
	m_scoreText = sf::Text(*m_fontAsset.pAsset.get());

	m_currentLevelText.setStyle(textsStyle);
	m_scoreText.setStyle(textsStyle);

	m_currentLevelText.setCharacterSize(fontSize);
	m_scoreText.setCharacterSize(fontSize);

	m_currentLevelText.setString("Current Level :");
	m_scoreText.setString("Current Score :");
	return bAllSucessfull;
}

void BBKajamightUI::ReportProgress(const BBGameProgress& InNewProgressState)
{
	Super::ReportProgress(InNewProgressState);
	m_currentLevelText.setString(std::format("Level : {}", m_lastReportedProgress.CurrentThisLevel));
	m_scoreText.setString(std::format("Score : {}", m_lastReportedProgress.ClearedLevelsScore + m_lastReportedProgress.ScoreThisLevel));
}

void BBKajamightUI::Render(sf::RenderWindow* InWindow)
{
	Super::Render(InWindow);
	InWindow->draw(m_currentLevelText);
	InWindow->draw(m_scoreText);
}

bool BB::BBKajamightUI::DeInitialize()
{
	bool bAllSucessfull = true;
	if (m_initialized)
	{
		bAllSucessfull = DeInitializeGraphics() && bAllSucessfull;

		BBTextureLibrary* library = m_programInstance->GetTextureLibrary();

		if (!m_fontAsset.IsUnset())
		{
			m_fontAsset.ResetAndReport(library);
		}

		bool bAllSucessfull = m_fontAsset.Key.empty();
		m_fontAsset = BBFontResult();
	}
	return bAllSucessfull;
}

bool BB::BBKajamightUI::DeInitializeGraphics()
{
	bool bAllSucessfull = true;
	m_scoreText = sf::Text(EMPTY_FONT);
	m_currentLevelText = sf::Text(EMPTY_FONT);
	return bAllSucessfull;
}

void BB::BBKajamightUI::SetCurrentLevelRect(const sf::FloatRect& InCurrentLevelRect)
{
	m_currentLevelRect = InCurrentLevelRect;

	sf::FloatRect bounds = m_currentLevelText.getLocalBounds();
	m_currentLevelText.setOrigin(sf::Vector2f(0.f, bounds.size.y));
	m_currentLevelText.setPosition(m_currentLevelRect.position + sf::Vector2(0.f, m_currentLevelRect.size.y * 0.5f));

}

void BB::BBKajamightUI::SetScoreRect(const sf::FloatRect& InScoreRect)
{
	m_scoreRect = InScoreRect;
	sf::FloatRect bounds = m_scoreText.getLocalBounds();
	m_scoreText.setOrigin(sf::Vector2f(0.f, bounds.size.y));
	m_scoreText.setPosition(m_scoreRect.position + sf::Vector2(0.f, m_currentLevelRect.size.y * 0.5f));
}

BBKajamightUI::~BBKajamightUI()
{
	DeInitialize();
}