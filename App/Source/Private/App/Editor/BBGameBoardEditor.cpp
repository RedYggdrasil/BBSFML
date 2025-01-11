#include "App/Editor/BBGameBoardEditor.h"
#include "App/Defines/BBSFML.h"
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
#include "App/Game/Physics/BBPhysicManager.h"

#include "App/Game/Level/BBLevelJsonReader.h"
#include <format>

using namespace BB;
using Super = BB::BBGameBoard;


static const sf::Font EMPTY_FONT;

void BBGameBoardEditor::UpdateBrushPreviewSprite()
{
	const auto& brushData = m_currentBrushData.GetCorrected();
	const BBBrickData& currentBrickBrush = m_currentBrushes[brushData.m_currentBrushIndex];
	std::string brushKey = std::string(currentBrickBrush.TextureName.data());
	m_brushPreview = RSfml::RGDXTPSprite
	(
		RSfml::RGDXTPSpriteType::NineSlice,
		*m_gameBoardTexturePack.pAsset.get(),
		m_gameBoardAtlasData.pAsset->TextureDatas[brushKey],
		sf::Color(255, 255, 255, uint8_t((255.f)*(0.75f)) )
		);
	m_brushPreview.SetVertOriginPercent(sf::Vector2f(0.5f, 0.5f));
	m_brushPreview.SetPosition(sf::Vector2f(0.f, 0.f));
	m_brushPreview.SetRotation(sf::Angle::Zero);
	m_brushPreview.SetScale(sf::Vector2f(0.5f, 0.5f));
}

BBGameBoardEditor::BBGameBoardEditor(BBGameBoardScene* InParentSceneObject)
	:Super(InParentSceneObject), 
	m_fontAsset(), m_commandInstruction(EMPTY_FONT),
	m_drawableGrid(F_BOARD_RECT, sf::Vector2f(256.f, 256.f), sf::Vector2f(4.f, 4.f), sf::Color(255, 0, 0, 192)),
	m_cursorPBID(), m_inputs(), m_currentBrushes(), m_currentBrushData(),
	m_brushPreview(), m_currentlyOverlappingBrick(), m_levelPath()
{
	m_currentBrushData.bSnapPosToGrid = true;
	m_currentBrushData.bSnapRotToGrid = true;
	m_currentBrushData.CurrentGridScale = m_drawableGrid.GetGridStep();
	m_currentBrushData.CurrentAngleSnapScale = sf::degrees(5.f);
}

void BBGameBoardEditor::Initialize(std::weak_ptr<BBGameBoard> InThisWPtr, const std::string_view InSpritesheetRelativePath)
{
	bool bAllSucessfull = true;
	Super::Initialize(InThisWPtr, InSpritesheetRelativePath);
	bAllSucessfull = (!m_thisWPtr.expired()) && bAllSucessfull;
	if (bAllSucessfull)
	{
		static const std::string fontAssetPath = "Fonts/FRIZQUAD.TTF";
		m_fontAsset = m_programInstance->GetTextureLibrary()->GetAsset<sf::Font>(fontAssetPath);
		if (!m_fontAsset)
		{
			bAllSucessfull = false;
		}
	}

	m_currentBrushes.resize(1);
	m_currentBrushes[0] = BBBrickData
	{
		.m_size = sf::Vector2f(400, 128) / 2.f,
		.m_pos = sf::Vector2f(0.f, 0.f),
		.m_rotDegree = 0.f,
		.TextureName = "RectBrick0",
		.LifePoints = 1,
		.PhysIndex = 0,
		.GraphIndex = 0
	};
	m_levelPath = m_programInstance->GetLevelReader()->GetPathOfEndFile();
	UpdateBrushPreviewSprite();
	bAllSucessfull = InitializeTexts() && bAllSucessfull;
	bAllSucessfull = InitializePhysics() && bAllSucessfull;
}

bool BBGameBoardEditor::DeInitialize()
{
	bool bAllSucessfull = true;
	m_levelPath = std::filesystem::path();
	bAllSucessfull = DeInitializePhysics() && bAllSucessfull;
	bAllSucessfull = DeInitializeTexts() && bAllSucessfull;

	m_brushPreview = RSfml::RGDXTPSprite();
	m_currentBrushData = GBEditorBrushData();
	m_currentBrushes.clear();
	m_inputs = GBEditorInputs();
	if (bAllSucessfull && !m_thisWPtr.expired())
	{
		m_fontAsset.ResetAndReport(m_programInstance->GetTextureLibrary());
	}
	else
	{
		m_fontAsset.SilentReset();
	}
	bAllSucessfull = Super::DeInitialize() && bAllSucessfull;
	return bAllSucessfull;
}

bool BB::BBGameBoardEditor::InitializePhysics()
{
	bool bAllSucessfull = true;
	sf::Transform cursorTrs = sf::Transform();
	//trsCircleMovable.translate(m_movableCircleShape.getOrigin());

	BBColliderBody localCursorCBody = BBColliderBody(WORLD_DYNAMIC);

	localCursorCBody.Colliders.push_back(BBCollider(FloatCircle{ .Position = sf::Vector2f(), .Radius = 32.f }));
	localCursorCBody.ComputeAABB();

	BBColliderBodyLWPair cursorBodyPair = BBColliderBodyLWPair::FromLocalCollidersBody(localCursorCBody, cursorTrs);
	BBPhysicBody cursorPB = BB::BBPhysicBody::CreatePhysicBodyWithID
	(
		std::move(cursorTrs),
		std::move(cursorBodyPair),
		std::move(NoDynamics(10000.f/*kg*/, /*Bounciness*/1.f)),
		m_thisWPtr
	);
	m_cursorPBID = cursorPB.Id();

	BBPhysicManager* physicManager = m_scene->GetPhysicManager();

	if ((bool)m_cursorPBID && physicManager)
	{
		physicManager->AddPhysicalEntity(cursorPB);
	}
	else
	{
		bAllSucessfull = false;
	}

	return bAllSucessfull;
}

bool BB::BBGameBoardEditor::DeInitializePhysics()
{
	bool bAllSucessfull = true;
	if ((bool)m_cursorPBID)
	{
		BBPhysicManager* physicManager = m_scene->GetPhysicManager();
		if (physicManager)
		{
			physicManager->MarkRemovePhysicalEntity(m_cursorPBID);
		}
		else
		{
			bAllSucessfull = false;
		}
	}
	return bAllSucessfull;
}

bool BB::BBGameBoardEditor::InitializeTexts()
{
	if (m_fontAsset.IsUnset())
	{
		return false;
	}
	m_commandInstruction = sf::Text(*m_fontAsset.pAsset);
	m_commandInstruction.setPosition(RSfml::RectBottomLeft(F_BOARD_RECT) + sf::Vector2f(+40.f, -380.f));
	m_commandInstruction.setCharacterSize(46);
	m_commandInstruction.setStyle(sf::Text::Style::Bold);
	m_commandInstruction.setOutlineColor(sf::Color::Black);
	m_commandInstruction.setLetterSpacing(1.f);
	m_commandInstruction.setOutlineThickness(5.f);
	UpdateInstructions();
	return true;
}

bool BB::BBGameBoardEditor::DeInitializeTexts()
{
	m_commandInstruction = sf::Text(EMPTY_FONT);
	return true;
}

void BB::BBGameBoardEditor::UpdateInstructions()
{

	sf::Vector2f currentGridStep = m_drawableGrid.GetGridStep();
	sf::Angle currentAngleStep = m_currentBrushData.CurrentAngleSnapScale;
	std::string instructions = "";
	if (!m_levelPath.empty())
	{
		instructions.append(std::format("Working on file \"{}\"\n", m_levelPath.filename().string()));
	}
	instructions.append("Save : \"CTRL LEFT + S\"\nAdd \"Left Click\", Del \"Right Click\"\nRotate  \"A, D\"");
	instructions.append(std::format("\nGrid[{}], toggle  \"G\"", GetGridToggleState() ? "On " : "Off"));
	if (GetGridToggleState())
	{
		instructions.append(
			std::format(
				" Size : [{}, {}], Decrease \"F\", Increase \"H\"",
				std::round(currentGridStep.x), std::round(currentGridStep.y)
			));
	}
	instructions.append(std::format("\nSnapRotation[{}], toggle  \"R\"", GetRotToggleState() ? "On " : "Off"));
	if (GetRotToggleState())
	{
		instructions.append(
			std::format(
				" Size : [{}], Decrease \"Q\", Increase \"E\"",
				std::round(currentAngleStep.asDegrees())
			));
	}
	m_commandInstruction.setString(instructions);
}

void BBGameBoardEditor::Start()
{
	//Super::Start();
	b_isRunning = true;
	m_gameProgress = BBGameProgress::CreateEmptyProgress();
	StartLevel(EDITOR_NEW_LEVEL);
	for (auto& ball : m_BBBalls)
	{
		ball->SetDynamic(false);
	}
	
}

void BBGameBoardEditor::Stop()
{
	Super::Stop();
}

void BBGameBoardEditor::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const float InPaddleDirection, const bool bInDirectInput)
{
	Super::Tick(InGameTime, InDeltaTime, InPaddleDirection, bInDirectInput);

	m_inputs.Tick(InGameTime, InDeltaTime, m_programInstance->GetWindow(), m_scene->GetView());
	m_currentBrushData.Tick(InGameTime, InDeltaTime, m_inputs);

	const auto& CorrectedBrushData = m_currentBrushData.GetCorrected();

	m_brushPreview.SetPosition(CorrectedBrushData.m_brushRawPosition + F_BOARD_RECT.position);
	m_brushPreview.SetRotation(CorrectedBrushData.m_brushRawRotation);

	bool bBrushInBoard = F_BOARD_RECT.contains(m_brushPreview.getPosition());
	if (m_inputs.GetLeftClick() && bBrushInBoard)
	{
		const auto& brushData = m_currentBrushData.GetCorrected();
		BBBrickData brickToAdd = m_currentBrushes[brushData.m_currentBrushIndex];
		brickToAdd.m_pos = brushData.m_brushRawPosition;
		brickToAdd.m_rotDegree = brushData.m_brushRawRotation.asDegrees();
		m_BBBricks->EDITOR_AddBrick(brickToAdd);
	}
	else if (m_inputs.GetRightClick() && (bool)m_currentlyOverlappingBrick)
	{
		m_BBBricks->EDITOR_RemoveBrick(m_currentlyOverlappingBrick);
	}
	m_currentlyOverlappingBrick = BBPhysicBodyID();


	if (m_inputs.GetSaveClick())
	{
		const std::vector<BBBrickData>& m_bricks = m_BBBricks->EDITOR_GetBricks();
		std::vector<BBBrickData> bricksCopy = m_bricks;
		for (BBBrickData& brick : bricksCopy)
		{
			brick.m_pos = brick.m_pos - F_BOARD_RECT.position;
		}
		bool bSavedSucessfully = m_programInstance->GetLevelReader()->SaveLevel(m_levelPath, bricksCopy);
	}

	if (m_inputs.GetToggleGridClick())
	{
		ToggleGrid();
	}
	else if (GetGridToggleState())
	{
		if (m_inputs.GetIncreaseGridClick())
		{
			ChangeGridSize(true);
		}
		else if(m_inputs.GetDecreaseGridClick())
		{
			ChangeGridSize(false);
		}
	}

	if (m_inputs.GetToggleRotClick())
	{
		ToggleRot();
	}
	else if (GetRotToggleState())
	{
		if (m_inputs.GetIncreaseRotClick())
		{
			ChangeRotSize(true);
		}
		else if (m_inputs.GetDecreaseRotClick())
		{
			ChangeRotSize(false);
		}
	}
}

void BBGameBoardEditor::Render(sf::RenderWindow* InWindow)
{
	Super::Render(InWindow);
	if (GetGridToggleState())
	{
		InWindow->draw(m_drawableGrid);
	}
	InWindow->draw(m_commandInstruction);

	bool bBrushInBoard = F_BOARD_RECT.contains(m_brushPreview.getPosition());
	if (bBrushInBoard)
	{
		InWindow->draw(m_brushPreview);
	}
}

void BBGameBoardEditor::Shutdown()
{
	Super::Shutdown();
}

void BBGameBoardEditor::OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody)
{
	Super::OnOverlapWith(InThisPhysicBody, InOtherPhysicBody);
	if (InThisPhysicBody.Id() == m_cursorPBID)
	{
		if (InOtherPhysicBody.GetBehavior().IsA(EBBColMask::BRICK))
		{
			m_currentlyOverlappingBrick = InOtherPhysicBody.Id();
		}
	}
}

void BBGameBoardEditor::OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	if (InOutThisPhysicBody.Id() == m_cursorPBID)
	{
		sf::Transform trsCursor = sf::Transform();
		trsCursor.translate(m_inputs.m_mouseWorldPosition);
		InOutThisPhysicBody.Transform = trsCursor;
		InOutThisPhysicBody.Colliders.RecomputeWorld(InOutThisPhysicBody.Transform, true);
	}
}

void BBGameBoardEditor::OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
}

void BBGameBoardEditor::OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const BBIDXsAndCollisionResult& InCollisionResult)
{
}

void BBGameBoardEditor::ToggleGrid()
{
	SetGridToggleState(!GetGridToggleState(), true);
}

bool BBGameBoardEditor::GetGridToggleState()
{
	return m_currentBrushData.bSnapPosToGrid;
}

void BBGameBoardEditor::SetGridToggleState(const bool InbNewGridState, const bool InbUpdateInstruction)
{
	m_currentBrushData.bSnapPosToGrid = InbNewGridState;
	if (InbUpdateInstruction)
	{
		UpdateInstructions();
	}
}

void BBGameBoardEditor::ChangeGridSize(const bool bIsIncrease)
{
	sf::Vector2f currentSize = GetGridSnapSize();
	currentSize.x = std::round(currentSize.x);
	currentSize.y = std::round(currentSize.y);
	if (
			(bIsIncrease && currentSize.x >= 255.9f)
			||
			(!bIsIncrease && currentSize.x <= 32.1f)
		)
	{
		return;
	}
	sf::Vector2f newSize = currentSize;
	if (bIsIncrease)
	{
		newSize = newSize * 2.f;
	}
	else
	{
		newSize = newSize * 0.5f;
	}
	newSize.x = std::round(newSize.x);
	newSize.y = std::round(newSize.y);
	SetGridSnapSize(newSize);
	UpdateInstructions();
}

sf::Vector2f BB::BBGameBoardEditor::GetGridSnapSize() const
{
	return m_drawableGrid.GetGridStep();
}

void BB::BBGameBoardEditor::SetGridSnapSize(const sf::Vector2f InNewSize)
{
	m_drawableGrid.SetGridStep(InNewSize, true);
	m_currentBrushData.CurrentGridScale = InNewSize;
	m_currentBrushData.ComputeCorrectedBrushData();
}

void BB::BBGameBoardEditor::ToggleRot()
{
	SetRotToggleState(!GetRotToggleState(), true);
}

bool BB::BBGameBoardEditor::GetRotToggleState()
{
	return m_currentBrushData.bSnapRotToGrid;
}

void BB::BBGameBoardEditor::SetRotToggleState(const bool InbNewGridState, const bool InbUpdateInstruction)
{
	m_currentBrushData.bSnapRotToGrid = InbNewGridState;
	if (InbUpdateInstruction)
	{
		UpdateInstructions();
	}
}

void BB::BBGameBoardEditor::ChangeRotSize(const bool bIsIncrease)
{
	sf::Angle currentSize = GetRotSnapSize();
	float deg = std::round(currentSize.asDegrees());
	if (
		(bIsIncrease && deg >= 179.f)
		||
		(!bIsIncrease && deg <= 5.1f)
		)
	{
		return;
	}
	float newSizeFlt = deg;
	if (bIsIncrease)
	{
		newSizeFlt = newSizeFlt + 5.f;
	}
	else
	{
		newSizeFlt = newSizeFlt - 5.f;
	}
	newSizeFlt = std::round(newSizeFlt);
	SetRotSnapSize(sf::degrees(newSizeFlt));
	UpdateInstructions();
}

sf::Angle BB::BBGameBoardEditor::GetRotSnapSize() const
{
	return m_currentBrushData.CurrentAngleSnapScale;
}

void BB::BBGameBoardEditor::SetRotSnapSize(const sf::Angle InAngle)
{
	m_currentBrushData.CurrentAngleSnapScale = InAngle;
	m_currentBrushData.ComputeCorrectedBrushData();
}

BBGameBoardEditor::~BBGameBoardEditor()
{

}


void BB::BBGameBoardEditor::GBEditorInputs::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const sf::RenderWindow& InWindow, const sf::View& InView)
{
	ResetFrameValues();

	constexpr sf::Time CLICK_MAX_TIME = sf::seconds(0.5f);

	sf::Vector2i pos = sf::Mouse::getPosition(InWindow);
	m_mouseWorldPosition = InWindow.mapPixelToCoords(pos, InView);

	for (auto& click : m_clicks)
	{
		click.Tick(InGameTime, InDeltaTime);
	}
}

void BBGameBoardEditor::GBEditorInputs::ResetFrameValues()
{
	for (auto& click : m_clicks)
	{
		click.ResetFrameValues();
	}
	m_mouseWorldPosition = sf::Vector2f();
}

BB::BBGameBoardEditor::GBEditorInputs::GBEditorInputs()
	: m_clicks
	(
		{
			/* LClick */BGClick([]()-> bool { return sf::Mouse::isButtonPressed(sf::Mouse::Button::Left); }),
			/* RClick */BGClick([]()-> bool { return sf::Mouse::isButtonPressed(sf::Mouse::Button::Right); }),
			/* Save   */BGClick([]()-> bool
				{ 
					return
						(
							sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl)
							&&
							sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)
						);
				}),
		/* TGrid  */BGClick([]()-> bool { return sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::G); }),
		/* +Grid  */BGClick([]()-> bool { return sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::H); }),
		/* -Grid  */BGClick([]()-> bool { return sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::F); }),

		/* T Rot  */BGClick([]()-> bool { return sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R); }),
		/* + Rot  */BGClick([]()-> bool { return sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E); }),
		/* - Rot  */BGClick([]()-> bool { return sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q); }),
		}
	)
{
}

void BBGameBoardEditor::GBEditorBrushData::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const GBEditorInputs& InFrameInputs)
{
	constexpr float DebugRotationSpeed = 180.f;
	constexpr float DebugSecondAngleRotationSpeed = 40.f;

	m_rawBrushData.m_brushRawPosition = InFrameInputs.m_mouseWorldPosition - F_BOARD_RECT.position;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
	{
		m_rawBrushData.m_brushRawRotation += sf::degrees(-DebugRotationSpeed * InDeltaTime.asSeconds());
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
	{
		m_rawBrushData.m_brushRawRotation += sf::degrees(+DebugRotationSpeed * InDeltaTime.asSeconds());
	}
	ComputeCorrectedBrushData();
}

void BBGameBoardEditor::GBEditorBrushData::ComputeCorrectedBrushData()
{
	m_correctedBrushData = m_rawBrushData;
	if (bSnapPosToGrid)
	{
		sf::Vector2f pos = m_correctedBrushData.m_brushRawPosition;
		sf::Vector2f topLeft = F_BOARD_RECT.position;
		pos.x = topLeft.x + std::round((pos.x - topLeft.x) / CurrentGridScale.x) * CurrentGridScale.x;
		pos.y = topLeft.y + std::round((pos.y - topLeft.y) / CurrentGridScale.y) * CurrentGridScale.y;
		m_correctedBrushData.m_brushRawPosition = pos;
	}
	if (bSnapRotToGrid)
	{
		float angleDegrees = m_correctedBrushData.m_brushRawRotation.asDegrees();
		float stepDegrees = CurrentAngleSnapScale.asDegrees(); 
		float snappedDegrees = std::round(angleDegrees / stepDegrees) * stepDegrees; 

		m_correctedBrushData.m_brushRawRotation = sf::degrees(snappedDegrees);
	}
}

void BBGameBoardEditor::BGClick::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	ResetFrameValues();
	if (m_getIsHeldInput())
	{
		if (m_lastInputStarted == sf::Time::Zero)
		{
			m_lastInputStarted = InGameTime;
		}
		else
		{
			//Already down last frame, continue
		}
	}
	//Maybe click Event this Frame
	else if (m_lastInputStarted != sf::Time::Zero)
	{
		if ((InGameTime - m_lastInputStarted) <= m_timeout)
		{
			m_bClick = true;
		}
		m_lastInputStarted = sf::Time::Zero;
	}
}

void BBGameBoardEditor::BGClick::ResetFrameValues()
{
	m_bClick = false;
}

BBGameBoardEditor::BGClick::BGClick(std::function<bool()> InGetDownState)
	:m_timeout(sf::seconds(0.5f)), m_lastInputStarted(), 
	m_getIsHeldInput(InGetDownState),m_bClick(false)
{
}
