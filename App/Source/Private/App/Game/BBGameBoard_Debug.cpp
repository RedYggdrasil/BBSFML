#if _DEBUG
#include "App/Game/BBGameBoard_Debug.h"
#include "RSFML/Tools/RLog.h"
#include "App/Game/BBBall.h"

#include "App/Game/Physics/BBPhysicManager.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include <format>

using namespace BB;


static const sf::Font EMPTY_FONT;
bool DebugRenderData::Initialize(const sf::Vector2f InTopLeftPosition)
{
	InfoRect = sf::FloatRect(InTopLeftPosition, sf::Vector2f(200.f, 200.f));
	return true;
}
sf::Vector2f DebugRenderData::PositionBelow() const
{
	return InfoRect.position + sf::Vector2f(0.f, InfoRect.size.y + 20.f);
}

DebugRenderData::DebugRenderData(BBGameBoard_Debug* InGameBoardDebug)
	:m_gbDebug(InGameBoardDebug)
{
}

DebugRenderData::~DebugRenderData()
{
}


bool BB::BBGameBoard_Debug::OnBallsChanged()
{
	auto it = std::remove_if
	(
		m_renderDatas.begin(), m_renderDatas.end(), 
		[](const std::unique_ptr<DebugRenderData>& data) 
		{ 
			DebugRenderData* datass = data.get();
			BallDebugRenderData* data_casted = dynamic_cast<BallDebugRenderData*>(datass);
			return dynamic_cast<BallDebugRenderData*>(data.get()) != nullptr; 
		}
	);
	m_renderDatas.erase(it, m_renderDatas.end());
	sf::Vector2f renderPosition = GetNextDebugLocation();
	for (auto& ball : m_gameBoard->m_BBBalls)
	{
		m_renderDatas.push_back(std::make_unique<BallDebugRenderData>(this, ball->GetPhysicBodyID()));
		m_renderDatas.back()->Initialize(renderPosition);
		renderPosition = m_renderDatas.back()->PositionBelow();
	}
	return true;
}

BBGameBoard_Debug::BBGameBoard_Debug(BBGameBoard* InGameBoard)
	:IBBGameBoardObject(InGameBoard), m_avalableSpace(InGameBoard->DEBUG_GetDebugLocation())
{
}

bool BB::BBGameBoard_Debug::Initialize(BBTextureLibrary* InLibrary)
{
	bool bAllSucessfull = true;
	bAllSucessfull = InitializeAssets(InLibrary) && bAllSucessfull;
	if (bAllSucessfull)
	{
		bAllSucessfull = InitializeElements() && bAllSucessfull;
	}
	return bAllSucessfull;
}

bool BBGameBoard_Debug::InitializeAssets(BBTextureLibrary* InLibrary)
{
	if (!InLibrary)
	{
		return false;
	}
	bool bAllSucessfull = true;
	static const std::string fontAssetPath = "Fonts/FRIZQUAD.TTF";
	m_fontAsset = InLibrary->GetAsset<sf::Font>(fontAssetPath);
	if (!m_fontAsset.pAsset)
	{
		bAllSucessfull = false;
	}
	return bAllSucessfull;
}

bool BBGameBoard_Debug::InitializeElements()
{
	bool bAllSucessfull = true;
	sf::Vector2f renderPosition = GetNextDebugLocation();

	bAllSucessfull = OnBallsChanged() && bAllSucessfull;
	return bAllSucessfull;
}

void BB::BBGameBoard_Debug::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	for (size_t i = 0; i < m_renderDatas.size(); ++i)
	{
		auto& renderData = m_renderDatas[i];
		renderData->UpdateDatas(InGameTime, InDeltaTime, m_gameBoard);
	}
}

void BB::BBGameBoard_Debug::Render(sf::RenderWindow* InWindow)
{
	for (size_t i = 0; i < m_renderDatas.size(); ++i)
	{
		auto& renderData = m_renderDatas[i];
		renderData->Render(InWindow);
	}
}

bool BBGameBoard_Debug::DeInitialize(BBTextureLibrary* InLibrary)
{
	bool bAllSucessfull = true;
	bAllSucessfull = DeInitializeElements() && bAllSucessfull;
	bAllSucessfull = DeInitializeAssets(InLibrary) && bAllSucessfull;
	return bAllSucessfull;
}

bool BBGameBoard_Debug::DeInitializeAssets(BBTextureLibrary* InLibrary)
{
	if (!InLibrary)
	{
		m_fontAsset.SilentReset();
		return true;
	}
	bool bAllSucessfull = true;
	m_fontAsset.ResetAndReport(InLibrary);
	return bAllSucessfull;
}

bool BBGameBoard_Debug::DeInitializeElements()
{
	m_renderDatas.clear();
	return true;
}

sf::Vector2f BB::BBGameBoard_Debug::GetNextDebugLocation() const
{
	if (m_renderDatas.size() > 0)
	{
		return m_renderDatas.back()->PositionBelow();
	}
	return m_avalableSpace.position;
}

BBGameBoard_Debug::~BBGameBoard_Debug()
{
}


BallDebugRenderData::BallDebugRenderData(BBGameBoard_Debug* InGameBoardDebug, const BBPhysicBodyID InBall_ID)
	:DebugRenderData(InGameBoardDebug), Ball_ID(InBall_ID), Velocity(), m_speedText(EMPTY_FONT)
{
	
}

bool BallDebugRenderData::Initialize(const sf::Vector2f InTopLeftPosition)
{
	bool bAllSucessfull = DebugRenderData::Initialize(InTopLeftPosition);
	m_speedText = sf::Text(*m_gbDebug->m_fontAsset.pAsset);
	m_speedText.setPosition(InfoRect.position + sf::Vector2f(10.f, 10.f));
	m_speedText.setCharacterSize(38);
	m_speedText.setString(std::format("Ball [{}] speed : {}", Ball_ID, RSfml::_tostring(Velocity)));
	return bAllSucessfull;
}

void BallDebugRenderData::UpdateDatas(const sf::Time InGameTime, const sf::Time InDeltaTime, BBGameBoard* InBBGameBoard)
{
	bool bAllSucessfull = false;
	BBBall* ball = InBBGameBoard->GetBallByPhysicID(Ball_ID);
	BBPhysicManager* pm = BBPhysicManager::Get(m_gbDebug);
	if (ball && pm)
	{
		const BBPhysicBody* pb = pm->GetPhysicalEntity(Ball_ID);
		if (pb)
		{
			Velocity = pb->Dynamics.Velocity;
			bAllSucessfull = true;
		}
	}

	if (bAllSucessfull)
	{
		m_speedText.setString(std::format("Ball [{}] speed : '{}' ({})", Ball_ID, Velocity.length(), RSfml::_tostring(Velocity)));
	}
}


void BallDebugRenderData::Render(sf::RenderWindow* InWindow)
{
	InWindow->draw(m_speedText);
}

BallDebugRenderData::~BallDebugRenderData()
{
}
#endif
