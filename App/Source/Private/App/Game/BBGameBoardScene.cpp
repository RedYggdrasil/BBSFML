#include "App/Game/BBGameBoardScene.h"
#include "RSFML/Tools/RLog.h"
#include "App/Defines/BBGameBoardDef.h"
#include "App/GameSystem/BBProgramInstance.h"
#include "App/Game/Physics/BBCollider.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/Game/Physics/Solvers/BBImpulseSolver.h"
#include "App/Game/Physics/Solvers/BBPositionSolver.h"
#include "App/Editor/BBGameBoardEditor.h"

#define DEBUG_SAT true
#if DEBUG_SAT
#include "App/Debugs/Physics/BBDebugSAT.h"
static std::shared_ptr<BB::Debug::BBDebugSAT> s_debugSAT;
#endif

using namespace BB;

using Super = BBScene;


BBGameBoardScene::BBGameBoardScene(IBBProgramObject* InParentObject)
: Super(InParentObject), m_gameBoard(nullptr), m_bbUserInputs(nullptr)
{
	m_gameBoardRect = I_BOARD_RECT;
	constexpr sf::Vector2u MINIMAL_VIEW = (sf::Vector2u)	(I_BOARD_SIZE_AROUND_CENTER + sf::Vector2i((256), (128)));
	constexpr sf::Vector2u TARGET_VIEW = (sf::Vector2u)		(I_BOARD_SIZE_AROUND_CENTER + sf::Vector2i((512), (512 + 112)));
	constexpr sf::Vector2u MAXIMAL_VIEW = sf::Vector2u		(3840, TARGET_VIEW.y + 512);

	m_sceneMinimalView = MINIMAL_VIEW;
	m_sceneTargetView = TARGET_VIEW;
	m_sceneMaximalView = MAXIMAL_VIEW;
};

void BBGameBoardScene::Start()
{
	Super::Start();
	m_bbPhysicManager->AddPhysicSolver(std::make_shared<BBImpulseSolver>(m_bbPhysicManager.get()));
	m_bbPhysicManager->AddPhysicSolver(std::make_shared<BBPositionSolver>(m_bbPhysicManager.get()));

	m_gameBoard = this->CreateGameBoard(m_programInstance->GetIsStartInEditor());

	m_bbUserInputs = this->CreateUserInputs();
	m_bbUserInputs->SetLeftScanKey(sf::Keyboard::Scancode::A);
	m_bbUserInputs->SetRightScanKey(sf::Keyboard::Scancode::D);
	m_bbUserInputs->SetIsUsingMouse(true);

	static const std::string DEFAULT_GAMEBOARD_ATLAS = "Sprites/Gameboards/Default.atlas";
	m_gameBoard->Initialize(m_gameBoard, DEFAULT_GAMEBOARD_ATLAS);
	m_gameBoard->Start();
#if DEBUG_SAT
	s_debugSAT = std::make_shared<BB::Debug::BBDebugSAT>();
	s_debugSAT->Start(s_debugSAT, m_programInstance, m_bbPhysicManager.get(), this);
#endif
}

void BBGameBoardScene::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime)
{
#if DEBUG_SAT
	s_debugSAT->Tick(InDeltaTime, m_scene->GetView());
#endif

	float paddleDirection = m_bbUserInputs->UpdateState();
	m_gameBoard->Tick(InGameTime, InDeltaTime, paddleDirection, m_bbUserInputs->GetIsUsingMouse());
	//Call Physic Pass
	Super::Tick(InGameTime, InDeltaTime);
}

void BBGameBoardScene::Render(sf::RenderWindow* InWindow)
{
	Super::Render(InWindow);
	this->RenderUnderBoard(InWindow);
	m_gameBoard->Render(InWindow);
	this->RenderOverBoard(InWindow);
	this->DrawDebug(InWindow);
}

void BBGameBoardScene::Shutdown()
{
#if DEBUG_SAT
	//DebugSat should Deinitialize by itself
	s_debugSAT.reset();
#endif
	m_gameBoard->Stop();
	m_gameBoard->DeInitialize();
	m_gameBoard.reset();
	m_bbUserInputs.reset();
	Super::Shutdown();
}

void BB::BBGameBoardScene::RenderUnderBoard(sf::RenderWindow* InWindow)
{
	sf::RectangleShape rectangleOut{ (sf::Vector2f)m_sceneMaximalView };
	rectangleOut.setOrigin(rectangleOut.getSize() * 0.5f);
	rectangleOut.setPosition({ 0.0f, 0.0f });
	rectangleOut.setFillColor(sf::Color::Red);
	InWindow->draw(rectangleOut);

	sf::RectangleShape rectangleExact{ (sf::Vector2f)m_sceneTargetView };
	rectangleExact.setOrigin(rectangleExact.getSize() * 0.5f);
	rectangleExact.setPosition({ 0.0f, 0.0f });
	rectangleExact.setFillColor(sf::Color::Green);
	InWindow->draw(rectangleExact);

	sf::RectangleShape rectangleIn{ (sf::Vector2f)m_sceneMinimalView };
	rectangleIn.setOrigin(rectangleIn.getSize() * 0.5f);
	rectangleIn.setPosition({ 0.0f, 0.0f });
	rectangleIn.setFillColor(sf::Color::Blue);
	InWindow->draw(rectangleIn);

}

void BB::BBGameBoardScene::RenderOverBoard(sf::RenderWindow* InWindow)
{

#if DEBUG_SAT
	s_debugSAT->Draw(InWindow);
#endif
	
	//BBCollider rectMove = BBCollider(BB::OrientedFloatRect(sf::FloatRect(128, 64)))
}

BBGameBoardScene::~BBGameBoardScene()
{
}

std::shared_ptr<BBGameBoard> BB::BBGameBoardScene::CreateGameBoard(const bool bInIsEditor) const
{
	if (!bInIsEditor)
	{
		return std::make_shared<BBGameBoard>((BBGameBoardScene*)this);
	}
	else
	{
		return std::make_shared<BBGameBoardEditor>((BBGameBoardScene*)this);
	}
}
