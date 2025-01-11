#include "App/Game/BBPaddle.h"
#include "RSFML/Graphics/R9Slice.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
#include "App/GameSystem/BBProgramInstance.h"

#include "RSFML/Tools/RLog.h"
#include "RSFML/Tools/RMath.h"
#include "App/Game/Physics/BBPhysicBody.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/GameSystem/BBTextureLibrary.h"

using namespace BB;

const std::string PADDLE_BALL_IMPACTS[] =
{
	"Sounds/Impacts/go_8fx_visions_titan_cleansinglight_close_metal_impact_01.ogg",
	"Sounds/Impacts/go_8fx_visions_titan_cleansinglight_close_metal_impact_02.ogg",
	"Sounds/Impacts/go_8fx_visions_titan_cleansinglight_close_metal_impact_03.ogg",
	"Sounds/Impacts/go_8fx_visions_titan_cleansinglight_close_metal_impact_04.ogg"
};



BBPaddle::BBPaddle(IBBGameBoardObject* InParentObject)
	: IBBGameBoardObject(InParentObject), IBBPhysicBodyListener(), sf::Transformable(),
	m_bInitialized(false), m_currentSize(0.f, 0.f), m_physicBodyID(0),
	m_paddleMesh(), m_thisWPtr(), m_texture(), m_atlasData()
{
}

BBPaddle::~BBPaddle()
{
	if (m_bInitialized)
	{
		DeInitialize();
	}
}

void BBPaddle::Initialize(const std::weak_ptr<BBPaddle> InThisWPtr, std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas)
{
	bool bAllSucessfull = true;
	m_thisWPtr = InThisWPtr;
	bAllSucessfull = (!m_thisWPtr.expired()) && bAllSucessfull;
	bAllSucessfull = InitializeTexture(InTexture, InTextureAtlas) && bAllSucessfull;
	bAllSucessfull = InitializePhysicalBody() && bAllSucessfull;
	bAllSucessfull = InitializeSounds() && bAllSucessfull;
	m_bInitialized = bAllSucessfull;


}

bool BB::BBPaddle::InitializeSounds()
{

	bool bAllSucessfull = true;
	BBTextureLibrary* library = m_programInstance->GetTextureLibrary();

	BBSoundPlayerData useSoundData = BBSoundPlayerData::DEFAULT;
	useSoundData.Volume = 50.f;

	bAllSucessfull = library->GetAssetsToVector<sf::SoundBuffer>(PADDLE_BALL_IMPACTS, _countof(PADDLE_BALL_IMPACTS), useSoundData, /*InOut*/m_paddleBallSounds) && bAllSucessfull;

	return bAllSucessfull;
}

bool BBPaddle::InitializeTexture(std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas)
{
	m_texture = InTexture;
	m_atlasData = InTextureAtlas;

	bool bAllSucessfull = (bool)m_texture && (bool)m_atlasData;
	if (bAllSucessfull)
	{
		constexpr size_t vertexPerBricks = 3 * 3 * 6; //9 slice (3*3) of rects (2 trinagles, 2*3)
		m_paddleMesh.setPrimitiveType(sf::PrimitiveType::Triangles);

		m_paddleMesh.resize(vertexPerBricks);

		sf::Texture* texture = m_texture.get();
		sf::Vector2i textureSize = (sf::Vector2i)texture->getSize();

		bool bAllSucessfull = true;
		const RSfml::RGDXTPAtlasTextureData& textureData = m_atlasData->TextureDatas["DragonflightWidebar"];
		sf::FloatRect worldRect = textureData.RectFromBounds<float>();
		worldRect.size.x = worldRect.size.x * 0.33f;
		worldRect.size.y = worldRect.size.y * 0.33f;
		m_currentSize = worldRect.size;
		worldRect.position = worldRect.size * -0.5f;
		
		bAllSucessfull = RSfml::Create9SliceSquareAutoScale(/*InOut*/m_paddleMesh, /*StartIndex*/0, textureData, worldRect, 2.f) && bAllSucessfull;

	}
	return bAllSucessfull;
}

bool BBPaddle::InitializePhysicalBody()
{
	BBPhysicManager* pm = BBPhysicManager::Get(this);
	sf::Transform trsMovable = sf::Transform();// this->getTransform();
	//trsMovable.translate(m_ballSprite.getOrigin());
	BBColliderBody localMovable = BBColliderBody(PADDLE);

	localMovable.Colliders.push_back
	(
		BBCollider
		(
			OrientedBox
			{
				sf::Vector2f(0.f, 0.f),
				m_currentSize * 0.5f,
				sf::degrees(0.f)
			}
		)
	);
	localMovable.ComputeAABB();

	BBColliderBodyLWPair movablePair = BBColliderBodyLWPair::FromLocalCollidersBody(localMovable, trsMovable);

	BBPhysicBody movablePB = BB::BBPhysicBody::CreatePhysicBodyWithID
	(
		std::move(trsMovable),
		std::move(movablePair),
		std::move(NoDynamics(999.f, /*Bounciness*/ 1.1f)),
		m_thisWPtr
	);
	m_physicBodyID = movablePB.Id();

	pm->AddPhysicalEntity(movablePB);
	return true;
}

bool BB::BBPaddle::DeInitialize()
{
	bool bAllSucessfull = true;
	m_bInitialized = false;
	bAllSucessfull = DeInitializeSounds() && bAllSucessfull;
	bAllSucessfull = DeInitializePhysicalBody() && bAllSucessfull;
	bAllSucessfull = DeInitializeTexture() && bAllSucessfull;
	m_thisWPtr.reset();
	return bAllSucessfull;
}

bool BB::BBPaddle::DeInitializePhysicalBody()
{
	if ((bool)m_physicBodyID)
	{
		BBPhysicManager* pm = BBPhysicManager::Get(this);
		if (pm)
		{
			pm->MarkRemovePhysicalEntity(m_physicBodyID);
		}
		else
		{
			RLog::Log(LOG_ERROR, "BBPaddle Deinit Failed to get BBPhysicManager !");
			return false;
		}
	}
	m_physicBodyID = 0;
	return true;
}

bool BB::BBPaddle::DeInitializeTexture()
{
	m_paddleMesh.clear();
	m_currentSize = sf::Vector2f();
	m_atlasData.reset();
	m_texture.reset();
	return true;
}

bool BB::BBPaddle::DeInitializeSounds()
{
	bool bAllSucessfull = true;
	BBTextureLibrary* library = m_programInstance->GetTextureLibrary();
	if (library)
	{
		library->ReportAssetUsageEndedFromVector(/*InOut*/m_paddleBallSounds, true);
	}
	else
	{
		bAllSucessfull = false;
	}
	return bAllSucessfull;
}

void BBPaddle::OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody)
{
	IBBPhysicBodyListener::OnOverlapWith(InThisPhysicBody, InOtherPhysicBody);
	if (InThisPhysicBody.Id() == m_physicBodyID)
	{
		const BBColBehavior& behavior = InOtherPhysicBody.GetBehavior(true);
		if (behavior.IsA(EBBColMask::BALL))
		{

		}
	}
}

void BBPaddle::OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	if (InOutThisPhysicBody.Id() == m_physicBodyID)
	{
		sf::Transform trsMovable = this->getTransform();
		//trsMovable.translate(m_currentSize  *- 0.5f);
		InOutThisPhysicBody.Transform = trsMovable;
		InOutThisPhysicBody.Colliders.RecomputeWorld(InOutThisPhysicBody.Transform, true);
	}
}

void BBPaddle::OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
}
void BB::BBPaddle::OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const BBIDXsAndCollisionResult& CollisionResult)
{
	if (InThisPhysicBody.Id() == m_physicBodyID && InOtherPhysicBody.GetBehavior().IsA(EBBColMask::BALL))
	{
		m_programInstance->PlaySound(m_paddleBallSounds);
	}
}
void BBPaddle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

	// apply the texture
	states.texture = m_texture.get();

	// you may also override states.shader or states.blendMode if you want

	// draw the vertex array
	target.draw(m_paddleMesh, states);
}