#include "App/Game/BBGameBoard.h"
#if _DEBUG
#include "App/Game/BBGameBoard_Debug.h"
#endif
#include "RSFML/Tools/Textures/RGDXTPAtlasReader.h"
#include "RSFML/Tools/RMath.h"
#include "App/Defines/BBGameBoardDef.h"
#include "App/Game/BBBall.h"
#include "App/Game/BBBorders.h"
#include "App/Game/BBPaddle.h"
#include "App/Game/BBGameBoardScene.h"
#include "App/GameSystem/BBPaths.h"
#include "App/GameSystem/BBProgramInstance.h"
#include "RSFML/Tools/RLog.h"


using namespace BB;

static const sf::Texture EMPTY_TEXTURE;

BBGameBoard::BBGameBoard(BBGameBoardScene* InParentSceneObject)
	: IBBGameBoardObject((IBBSceneObject*)InParentSceneObject), IBBPhysicBodyListener(), b_isRunning(false),
	m_thisWPtr(), m_gameBoardState(GameBoardState::CreateDefault()), m_gameProgress(BBGameProgress::CreateEmptyProgress()),
	m_gameBoardAtlasData(), m_gameBoardTexturePack()/*, m_paddle({256.f, 44.f}), m_paddleSprite(EMPTY_TEXTURE)*/,
	m_BBBricks(std::make_shared<BBBricks>((IBBGameBoardObject*)this)), m_BBBalls(), m_BBBorders(), m_paddles()/*,m_BBBallProjectileSolver(this)*/
{
#if _DEBUG
	m_pGBDebug = std::make_unique<BBGameBoard_Debug>(this);
#endif
}


void BB::BBGameBoard::Initialize(std::weak_ptr<BBGameBoard> InThisWPtr, const std::string_view InSpritesheetRelativePath)
{
	m_thisWPtr = InThisWPtr;
	if (!InitTextureByPath(InSpritesheetRelativePath))
	{
		return;
	}
	if (!InitSprites())
	{
		return;
	}
	bool bAllSucessfull = true;
#if _DEBUG
	bAllSucessfull = m_pGBDebug->Initialize(m_programInstance->GetTextureLibrary()) && bAllSucessfull;
#endif
}

bool BB::BBGameBoard::DeInitialize()
{
	bool bAllSucessfull = true;
#if _DEBUG
	bAllSucessfull = m_pGBDebug->DeInitialize(m_programInstance->GetTextureLibrary()) && bAllSucessfull;
#endif
	bAllSucessfull = DeInitializeSprites() && bAllSucessfull;
	bAllSucessfull = DeInitializeTexture() && bAllSucessfull;
	m_thisWPtr.reset();
	return bAllSucessfull;
}

void BBGameBoard::Start()
{
	b_isRunning = true;
	m_gameProgress = BBGameProgress::CreateEmptyProgress();
	StartLevel(m_gameProgress.CurrentThisLevel);
}

void BB::BBGameBoard::Stop()
{
	ClearCurrentLevel(false);
	m_gameProgress = BBGameProgress::CreateEmptyProgress();
	m_gameBoardState = GameBoardState::CreateDefault();
	b_isRunning = false;
}

void BBGameBoard::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const float InPaddleDirection, const bool bInDirectInput)
{
	if (!GetIsRunning())
	{
		return;
	}
	else if (m_gameBoardState.GetGameBoardState() == EGameBoardState::InLevelTransition)
	{
		if (InGameTime > (m_gameBoardState.GetLastTranslationTime() + sf::seconds(0.1f)))
		{
			StartLevel(m_gameProgress.CurrentThisLevel);
		}
	}
	else if (m_gameBoardState.GetGameBoardState() == EGameBoardState::PlayingALevel)
	{
		this->UpdatePaddle(InDeltaTime, InPaddleDirection, bInDirectInput);
	}

#if _DEBUG
	m_pGBDebug->Tick(InGameTime, InDeltaTime);
#endif
}

void BB::BBGameBoard::UpdatePaddle(const sf::Time InDeltaTime, const float InPaddleDirection, const bool bInDirectInput)
{
	float newPosX = 0.f;

	float boardDist = m_paddleMax - m_paddleMin;
	float midPoint = m_paddleMin + (boardDist * 0.5f);
	for (size_t i = 0; i < m_paddles.size(); ++i)
	{
		BBPaddle* paddle = m_paddles[i].get();
		const float paddleWidth = paddle->GetCurrentWidth() * 0.5f;
		if (bInDirectInput)
		{
			newPosX = InPaddleDirection;
		}
		else
		{
			const auto previousPos = m_paddles[i]->getPosition();
			newPosX = previousPos.x;
			newPosX = newPosX + (std::clamp(InPaddleDirection, -1.f, 1.f) * (m_paddleSpeed * InDeltaTime.asSeconds()));
		}

		//RLog::Log(LOG_DISPLAY, "paddleWidth : '{}'", paddleWidth);
		if (paddleWidth * 2.f > boardDist)
		{
			m_paddles[i]->setPosition({ midPoint, m_paddles[i]->getPosition().y });
		}
		else
		{
			m_paddles[i]->setPosition({ std::clamp(newPosX, m_paddleMin + paddleWidth, m_paddleMax - paddleWidth) , m_paddles[i]->getPosition().y });
		}
		//m_paddles[i]->setScale(sf::Vector2f(0.25f, 0.25f));
	}
}

void BBGameBoard::Render(sf::RenderWindow* InWindow)
{
	InWindow->draw(*m_BBBricks.get());

	for (const std::shared_ptr<BBPaddle>& paddle : m_paddles)
	{
		InWindow->draw(*paddle.get());
	}

	for (const std::shared_ptr<BBBall>& ball : m_BBBalls)
	{
		ball->Render(InWindow);
	}

#if _DEBUG
	m_pGBDebug->Render(InWindow);
#endif
}

void BBGameBoard::Shutdown()
{
	b_isRunning = false;
}

void BB::BBGameBoard::ReportDestroyedBall(std::weak_ptr<BBBall> InBallWPtr)
{
	std::shared_ptr<BBBall> ballPtr = InBallWPtr.lock();
	if (ballPtr)
	{
		bool bDidFindBall = false;
		for (size_t i = 0; i < m_BBBalls.size(); ++i)
		{
			if (m_BBBalls[i] == ballPtr)
			{
				RemoveBall(i);
				bDidFindBall = true;
				break;
			}
		}
		if (!bDidFindBall)
		{
			RLog::Log(LOG_ERROR, "Cannot find ball to remove in this board !");
		}

		if (m_BBBalls.size() < 1)
		{
			OnLevelFailed();
		}
	}
}

void BB::BBGameBoard::ReportBrick(size_t InAddedScore, size_t RemainingBricks, size_t OriginalBricksNumber)
{
	m_gameProgress.ScoreThisLevel += InAddedScore;
	m_gameProgress.BrickDestroyedThisLevel = OriginalBricksNumber - RemainingBricks;
	if (RemainingBricks == 0)
	{
		OnLevelCleared();
	}
}

void BB::BBGameBoard::OnLevelFailed()
{
	ClearCurrentLevel(true);
	m_gameProgress = BBGameProgress::CreateEmptyProgress();
}

void BB::BBGameBoard::OnLevelCleared()
{
	ClearCurrentLevel(false);
	m_gameProgress.LevelCleared();
}

bool BB::BBGameBoard::InitTextureByPath(const std::string_view InSpritesheetRelativePath)
{
	BBTextureLibrary* library = m_programInstance->GetTextureLibrary();
	m_gameBoardAtlasData = library->GetAsset<RSfml::RGDXTPAtlasData>(InSpritesheetRelativePath);
	if ((bool)m_gameBoardAtlasData)
	{
		std::filesystem::path textureRelativePath = m_gameBoardAtlasData.pAsset->TexturePathFromAtlasPath(InSpritesheetRelativePath);
		m_gameBoardTexturePack = library->GetAsset<sf::Texture>(textureRelativePath.string());
		if ((bool)m_gameBoardTexturePack)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool BB::BBGameBoard::InitSprites()
{	
	float boardWidth = F_BOARD_SIZE.x;
	m_paddleMin = boardWidth * -0.5f;
	m_paddleMax = boardWidth * 0.5f;


	//going from one side to the other take 0.75 seconds
	m_paddleSpeed = boardWidth / 0.75f;

	m_BBBorders = std::make_shared<BBBorders>((IBBGameBoardObject*)this);
	m_BBBorders->Initialize(m_BBBorders, F_BOARD_RECT);

	m_BBBricks->Initialize(m_BBBricks, m_gameBoardTexturePack.pAsset, m_gameBoardAtlasData.pAsset);

	return true;
}

bool BB::BBGameBoard::DeInitializeSprites()
{
	//mmBricks should Deinitialize by itself if no level loaded
	m_BBBricks.reset();
	//mmBricks should Deinitialize by itself if no level loaded
	m_BBBorders.reset();
	m_paddleSpeed = 0.f;

	return true;
}

bool BB::BBGameBoard::DeInitializeTexture()
{
	if (m_programInstance)
	{
		BBTextureLibrary* library = m_programInstance->GetTextureLibrary();
		if (library)
		{
			if (!m_gameBoardTexturePack.IsUnset())
			{
				m_gameBoardTexturePack.ResetAndReport(library);
			}
			if (!m_gameBoardAtlasData.IsUnset())
			{
				m_gameBoardAtlasData.ResetAndReport(library);
			}
			return true;
		}
	}
	bool bAllSucessfull = m_gameBoardAtlasData.IsUnset()
		&& m_gameBoardTexturePack.IsUnset();
	if (!bAllSucessfull)
	{
		m_gameBoardTexturePack.SilentReset();
		m_gameBoardAtlasData.SilentReset();
	}
	return bAllSucessfull;
}

void BB::BBGameBoard::StartLevel(const size_t InLevelID)
{
	m_BBBricks->InitializeLevel(InLevelID);
	m_gameBoardState.MoveToPlayLevel(m_programInstance->GetTimeData().CurrentTime);
	size_t paddleIDX = CreateNewPaddle();
	
	CreateNewBall(m_paddles[paddleIDX]->getPosition()  + sf::Vector2f( 0.f, - m_paddles[paddleIDX]->GetCurrentHeight() * 1.5f));
}

void BB::BBGameBoard::ClearCurrentLevel(const bool bInReasonIsPlayerLost)
{
	m_gameBoardState.MoveToLevelTransition(m_programInstance->GetTimeData().CurrentTime, bInReasonIsPlayerLost);
	ClearBalls();
	ClearPaddles();
	m_BBBricks->ClearCurrentLevel();
}

size_t BB::BBGameBoard::CreateNewBall(const sf::Vector2f InLocation)
{
	return CreateNewBall
		(
			InLocation,
			RSfml::AngleToVector(-sf::degrees(90.f))
		);
}

size_t BB::BBGameBoard::CreateNewBall(const sf::Vector2f InLocation, const sf::Vector2f InDirection)
{
	size_t ballIdx = m_BBBalls.size();
	m_BBBalls.emplace_back(std::make_shared<BBBall>(this, m_gameBoardTexturePack.pAsset, m_gameBoardAtlasData.pAsset));
	std::shared_ptr<BBBall> newBall = m_BBBalls[ballIdx];
	newBall->Init(newBall, true);
	newBall->SetPosition(InLocation);
	newBall->LaunchAtStartSpeed(InDirection);

#if _DEBUG
	m_pGBDebug->OnBallsChanged();
#endif // _DEBUG
	return ballIdx;
}

void BB::BBGameBoard::RemoveBall(const size_t InBallIndex)
{
	std::shared_ptr<BBBall> ballToRemove = m_BBBalls[InBallIndex];
	if (ballToRemove)
	{
		//ball destructor should DeInitialize by itself
		ballToRemove.reset();
	}
	m_BBBalls.erase(m_BBBalls.begin() + InBallIndex);
#if _DEBUG
	m_pGBDebug->OnBallsChanged();
#endif // _DEBUG
}

void BB::BBGameBoard::ClearBalls()
{
	const size_t initialBallSize = m_BBBalls.size();
	for (size_t i = initialBallSize; i > 0; --i)
	{
		RemoveBall(i - 1);
	}

}

size_t BB::BBGameBoard::CreateNewPaddle()
{
	
	return CreateNewPaddle
	(
		sf::Vector2f{ 0.f, F_BOARD_RECT.position.y + F_BOARD_RECT.size.y - 128.f}// + sf::Vector2f(0.f, -m_paddleSprite.getScale().y)
	);
}

size_t BB::BBGameBoard::CreateNewPaddle(const sf::Vector2f InLocation)
{
	size_t paddleIdx = m_paddles.size();
	m_paddles.emplace_back(std::make_shared<BBPaddle>(this));
	std::shared_ptr<BBPaddle> newPaddle = m_paddles[paddleIdx];
	newPaddle->Initialize(newPaddle, m_gameBoardTexturePack.pAsset, m_gameBoardAtlasData.pAsset);
	newPaddle->setPosition(InLocation + sf::Vector2f(0.f, -newPaddle->GetCurrentHeight() * 0.5f));
	return paddleIdx;
}

void BB::BBGameBoard::RemovePaddle(size_t InPaddleIndex)
{
	std::shared_ptr<BBPaddle> paddleToRemove = m_paddles[InPaddleIndex];
	if (paddleToRemove)
	{
		//paddle destructor should DeInitialize by itself
		paddleToRemove.reset();
	}
	m_paddles.erase(m_paddles.begin() + InPaddleIndex);
}

void BB::BBGameBoard::ClearPaddles()
{
	const size_t initialPaddleSize = m_paddles.size();
	for (size_t i = initialPaddleSize; i > 0; --i)
	{
		RemovePaddle(i - 1);
	}
}

#if _DEBUG
sf::FloatRect BB::BBGameBoard::DEBUG_GetDebugLocation() const
{
	constexpr sf::Vector2f BORDER_SIZE = sf::Vector2f(30.f, 30.f);
	return sf::FloatRect
	(
		F_BOARD_RECT.position + BORDER_SIZE,
		F_BOARD_RECT.size - (BORDER_SIZE * 2.f)
	);
}
BBBall* BB::BBGameBoard::GetBallByPhysicID(const BBPhysicBodyID InPhysicBodyID) const
{
	for (auto& ball : m_BBBalls)
	{
		if (ball->GetPhysicBodyID() == InPhysicBodyID)
		{
			return ball.get();
		}
	}
	return nullptr;
}
#endif

BBGameBoard::~BBGameBoard()
{
}

void BB::BBGameBoard::GameBoardState::MoveToLevelTransition(const sf::Time InTime, const bool bInHasLost)
{
	m_eGameBoardState = EGameBoardState::InLevelTransition;
	m_bPlayerLost = bInHasLost;
	m_lastTranslationTime = InTime;
}

void BB::BBGameBoard::GameBoardState::MoveToPlayLevel(const sf::Time InTime)
{
	m_eGameBoardState = EGameBoardState::PlayingALevel;
	m_bPlayerLost = false;
	m_lastTranslationTime = InTime;
}

BBGameBoard::GameBoardState BB::BBGameBoard::GameBoardState::CreateDefault()
{
	GameBoardState result;
	result.m_eGameBoardState = EGameBoardState::Unknown;
	result.m_bPlayerLost = false;
	result.m_lastTranslationTime = sf::Time();
	return result;
}
