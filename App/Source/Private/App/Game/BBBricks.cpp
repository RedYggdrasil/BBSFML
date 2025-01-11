#include "App/Game/BBBricks.h"
#include "App/Defines/BBSFML.h"
#include "App/Game/Physics/BBCollisionResult.h"
#include "App/GameSystem/BBProgramInstance.h"
#include "RSFML/Graphics/R9Slice.h"
#include "RSFML/Graphics/RVertArrHelper.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/RLog.h"
#include "App/Game/BBGameBoard.h"
#include "App/Game/Level/BBLevel0.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/Game/Level/BBLevelJsonReader.h"
#include "App/Defines/BBGameBoardDef.h"
#include <string>

using namespace BB;

constexpr size_t VERTEX_PER_BRICK = 3 * 3 * 6; //9 slice (3*3) of rects (2 trinagles, 2*3)

const std::string BRICK_METAL_IMPACTS[] = 
{
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_01.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_02.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_03.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_04.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_05.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_06.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_07.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_08.ogg",
    "Sounds/Impacts/2h_macehit_metal_blockparry_wood_09.ogg"
};

BBBricks::BBBricks(IBBGameBoardObject* InGameBoardObject)
    :IBBGameBoardObject(InGameBoardObject), sf::Drawable(), sf::Transformable(), BB::IBBPhysicBodyListener(),
    m_currentLevelID(0), m_originalBrickCount(0), m_bricks(), m_vertices(), m_gameBoardTexturePack(nullptr)
{

}

void BB::BBBricks::Initialize(std::weak_ptr<BBBricks> InThisWPtr, std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas)
{
    bool bAllSucessfull = true;
    m_thisWPtr = InThisWPtr;
    SetGameboardTexture(InTexture, InTextureAtlas);
    BBTextureLibrary* library = m_programInstance->GetTextureLibrary();

    BBSoundPlayerData useSoundData = BBSoundPlayerData::DEFAULT;
    useSoundData.Volume = 25.f;

    bAllSucessfull = library->GetAssetsToVector<sf::SoundBuffer>(BRICK_METAL_IMPACTS, _countof(BRICK_METAL_IMPACTS), useSoundData, /*InOut*/m_brickSounds) && bAllSucessfull;
}

bool BB::BBBricks::DeInitialize()
{
    bool bAllSucessfull = true;
    if (IsCurrentlyInALevel())
    {
        RLog::Log(LOG_ERROR, "Called DeInitialize on Bricks still in a level, this is unexpected !");
        return false;
    }
    BBTextureLibrary* library = m_programInstance->GetTextureLibrary();
    if (library)
    {
        library->ReportAssetUsageEndedFromVector(/*InOut*/m_brickSounds, true);
    }
    else
    {
        bAllSucessfull = false;
    }
    m_gameBoardAtlasData.reset();
    m_gameBoardTexturePack.reset();
    m_thisWPtr.reset();
    return bAllSucessfull;
}

void BB::BBBricks::SetGameboardTexture(std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas)
{
    m_gameBoardTexturePack = InTexture;
    m_gameBoardAtlasData = InTextureAtlas;
}

void BBBricks::InitializeLevel(size_t InLevelID)
{
    std::vector<BBBrickData> bricks; 
    m_currentLevelID = InLevelID;

    if (InLevelID == EDITOR_NEW_LEVEL)
    {
        bricks = std::vector<BBBrickData>();
        InitializeLevel(std::move(bricks), true);
    }
    else if (InLevelID > 0)
    {
        size_t levelIndex = InLevelID - 1;
        bricks = m_programInstance->GetLevelReader()->LoadLevel(levelIndex);

        //bricks.assign(Level::Level0.begin(), Level::Level0.end()/*Level::Level0.begin() + 1*/);

        //std::string toto = m_programInstance->GetLevelReader()->LevelToJsonString(bricks);
        InitializeLevel(std::move(bricks), true);
        //InitializeLevel(std::move(bricks));
    }
    else
    {
        switch (InLevelID)
        {
        case 1:
            bricks.assign(Level::Level0.begin(), Level::Level0.end()/*Level::Level0.begin() + 1*/);
            InitializeLevel(std::move(bricks), true);
            break;
        case 2:
            bricks.assign(Level::Level0.begin(), Level::Level0.begin() + 2);
            InitializeLevel(std::move(bricks), true);
            break;
        case 3:
            bricks.assign(Level::Level0.begin(), Level::Level0.begin() + 3);
            InitializeLevel(std::move(bricks), true);
            break;
        case 4:
            bricks.assign(Level::Level0.begin(), Level::Level0.end());
            InitializeLevel(std::move(bricks), true);
            break;
        default:
            m_currentLevelID = 0;
            RLog::Log(LOG_ERROR, "Unknown Level with ID : '{}' !", InLevelID);
            break;
        }
    }
}

void BBBricks::InitializeLevel(std::vector<BBBrickData>&& InBricks, const bool bPos0IsTopLeft)
{
	m_bricks = std::move(InBricks);
    m_originalBrickCount = m_bricks.size();
    if (bPos0IsTopLeft)
    {
        for (auto& brick : m_bricks)
        {
            brick.m_pos = brick.m_pos + F_BOARD_RECT.position;
        }
    }
    CreateVertexArrayFromBricks();
    CreatePhysicData(true);
}

void BB::BBBricks::ClearCurrentLevel()
{
    const size_t initialBrickSize = m_bricks.size();
    for (size_t i = initialBrickSize; i > 0; --i)
    {
        RemoveBrick(i - 1);
    }
    if (
        m_bricks.size() > 0
        ||
        m_vertices.getVertexCount() > 0
        )
    {
        RLog::Log(LOG_ERROR, "Brick level cleared failed with brick count :'{}' and vertexCount '{}' !", m_bricks.size(), m_vertices.getVertexCount());
    }
    m_bricks.clear();
    m_vertices.clear();
    m_originalBrickCount = 0;
    m_currentLevelID = 0;
}


void BB::BBBricks::CreateVertexArrayFromBricks()
{
    
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);

    m_vertices.resize(m_bricks.size() * VERTEX_PER_BRICK);

    sf::Texture* texture = m_gameBoardTexturePack.get();
    sf::Vector2i textureSize = (sf::Vector2i)texture->getSize();

    bool bAllSucessfull = true;
    for (size_t i = 0; i < m_bricks.size(); ++i)
    {
        BBBrickData& brick = m_bricks[i];
        const auto& textureData = m_gameBoardAtlasData->TextureDatas[brick.TextureName.data()];
        size_t startIndex = i * VERTEX_PER_BRICK;
        bAllSucessfull = RSfml::Create9SliceSquare(/*InOut*/m_vertices, startIndex, textureData, sf::FloatRect{ brick.m_pos - (brick.m_size * 0.5f),  brick.m_size }, sf::Vector2f(0.5f, 0.5f)) && bAllSucessfull;
        bAllSucessfull = RSfml::RotateVertices(/*InOut*/m_vertices, startIndex, RSfml::NINE_SLICE_VERT_COUNT, sf::degrees(brick.m_rotDegree), brick.m_pos) && bAllSucessfull;
        

        brick.GraphIndex = i;
    }
}

void BB::BBBricks::CreatePhysicData(bool bInOverrideExistingID)
{
    if (m_bricks.size() < 1)
    {
        return;
    }
    BBPhysicManager* pm = BBPhysicManager::Get(this);

    bool bAllSucessfull = true;
    std::vector<BBPhysicBody> physicBodies; physicBodies.reserve(m_bricks.size());


    for (size_t i = 0; i < m_bricks.size(); ++i)
    {
        BBBrickData& brick = m_bricks[i];

        if ((bool)brick.PhysIndex)
        {
            if ((bool)bInOverrideExistingID)
            {
                //override is handled by AddPhysicalEntity
                //pm->RemovePhysicalEntity(brick.PhysIndex);
            }
            else
            {
                continue;
            }
        }
        if (i == 0)
        {
            RLog::Log(LOG_DISPLAY, "POS {}, SZE {}", RSfml::_tostring(brick.m_pos), RSfml::_tostring(brick.m_size));
        }
        sf::Transform brickTrs;
        brickTrs.translate(brick.m_pos);
        brickTrs.rotate(sf::degrees(brick.m_rotDegree));
        BBColliderBody localColliderBody = BBColliderBody(BRICK);

        localColliderBody.Colliders.push_back
        (
            BBCollider
            (
                OrientedBox(sf::Vector2f(), brick.m_size * 0.5f, sf::Angle())
            )
        );
        localColliderBody.ComputeAABB();

        BBColliderBodyLWPair ColliderBodyPair = BBColliderBodyLWPair::FromLocalCollidersBody(localColliderBody, brickTrs);

        physicBodies.push_back
        (
            BB::BBPhysicBody::CreatePhysicBodyWithID
            (
                std::move(brickTrs),
                std::move(ColliderBodyPair),
                std::move(NoDynamics(10000.f)),
                m_thisWPtr
            )
        );
        brick.PhysIndex = physicBodies.back().Id();
        
    }
    pm->AddPhysicalEntities(&physicBodies[0], physicBodies.size());
}

void BB::BBBricks::OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody)
{
}

void BB::BBBricks::OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
}

void BB::BBBricks::OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
}


void BB::BBBricks::HandleBrickTouched(const size_t InBrickIndex, const BBPhysicBody& InBrickPhysicBody, const BBPhysicBody& InBallPhysicBody, const BBIDXsAndCollisionResult& InCollisionResult)
{
    m_programInstance->PlaySound(m_brickSounds);
    RemoveBrick(InBrickIndex);
    m_gameBoard->ReportBrick(100, m_bricks.size(), m_originalBrickCount);
}

void BB::BBBricks::RemoveBrick(const size_t InBrickIndex)
{
    BBPhysicManager* pm = BBPhysicManager::Get(this);
    BBBrickData* pThisBrickData = &m_bricks[InBrickIndex];
    BBBrickData* pLastBrickData = &m_bricks.back();
    size_t newThisBrickVertexIndex = (m_bricks.size() - 1) * VERTEX_PER_BRICK;
    if (pThisBrickData != pLastBrickData)
    {
        std::swap(m_bricks[InBrickIndex], m_bricks.back());
        std::swap(pThisBrickData, pLastBrickData);
        pLastBrickData->GraphIndex = InBrickIndex;

        pThisBrickData->GraphIndex = m_bricks.size() - 1;

        size_t newLastBrickVertexIndex = InBrickIndex * VERTEX_PER_BRICK;
        std::swap_ranges(&m_vertices[newLastBrickVertexIndex], &m_vertices[newLastBrickVertexIndex + VERTEX_PER_BRICK], &m_vertices[newThisBrickVertexIndex]);
    }
    m_vertices.resize(m_vertices.getVertexCount() - VERTEX_PER_BRICK);
    pm->MarkRemovePhysicalEntity(pThisBrickData->PhysIndex);
    pThisBrickData = nullptr;
    m_bricks.pop_back();
}

void BB::BBBricks::EDITOR_AddBrick(const BBBrickData& InNewBrick, bool bPositionRelativeToBoard)
{
    BBBrickData brickToAdd = InNewBrick;
    if (bPositionRelativeToBoard)
    {
        brickToAdd.m_pos = InNewBrick.m_pos + F_BOARD_RECT.position;
    }
    std::vector<BBBrickData> newBricks = m_bricks;
    newBricks.push_back(brickToAdd);
    ClearCurrentLevel();
    InitializeLevel(std::move(newBricks), false);
}

void BB::BBBricks::EDITOR_RemoveBrick(const BBPhysicBodyID& InIDToRemove)
{
    for (size_t i = 0; i < m_bricks.size(); ++i)
    {
        if (InIDToRemove == m_bricks[i].PhysIndex)
        {
            RemoveBrick(i);
            break;
        }
    }
}

void BB::BBBricks::OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const ColbodyIDXsAndColResult& CollisionResult)
{
    if (!InOtherPhysicBody.GetBehavior().IsA(EBBColMask::BALL))
    {
        return;
    }
    for (size_t i = 0; i < m_bricks.size(); ++i)
    {
        if (InThisPhysicBody.Id() == m_bricks[i].PhysIndex)
        {
            HandleBrickTouched(i, InThisPhysicBody, InOtherPhysicBody, CollisionResult);
            break;
        }
    }
}

void BBBricks::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

    // apply the texture
    states.texture = m_gameBoardTexturePack.get();

    // you may also override states.shader or states.blendMode if you want

    // draw the vertex array
    target.draw(m_vertices, states);
}

BBBricks::~BBBricks()
{
    DeInitialize();
}
