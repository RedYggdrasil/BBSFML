#include "App/Game/BBBall.h"

#include "RSFML/Tools/RLog.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
#include "App/Game/BBGameBoard.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/Game/Physics/BBPhysicBody.h"

using namespace BB;

static const sf::Texture EMPTY_TEXTURE;

BBBall::BBBall(IBBGameBoardObject* InParentObject)
	:BBBall(InParentObject, nullptr, nullptr)
{
}

static BBDynamics BallDynamicValue(const bool bInIsDynamic)
{
	return bInIsDynamic ? Dynamics(10.f/*kg*/, /*scaleGravity*/sf::Vector2f(0.f, 0.f)) : NoDynamics(10.f/*kg*/);
}

BBBall::BBBall(IBBGameBoardObject* InParentObject, std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas)
	:IBBGameBoardObject(InParentObject), m_bLoadedTextures(false), m_physicBodyID(0), m_bCurrentlyDynamic(false),
	m_ballSprite(EMPTY_TEXTURE), m_thisWPtr(), m_texture(InTexture), m_atlasData(InTextureAtlas),
	m_minSpeed(500.f), m_startSpeed(750.f), m_maxSpeed(1500.f)
{
	if ((bool)InTexture && (bool)InTextureAtlas)
	{
		SetGameboardTexture(InTexture, InTextureAtlas);
	}
}

void BB::BBBall::Init(const std::weak_ptr<BBBall> InThisWPtr, const bool bInIsDynamic)
{
	m_thisWPtr = InThisWPtr.lock();
	m_bCurrentlyDynamic = bInIsDynamic;
	InitializePhysicalBody();
}

void BB::BBBall::SetDynamic(const bool bInIsDynamic)
{
	bool bDone = false;
	if ((bool)m_physicBodyID)
	{
		auto result = BBPhysicManager::Get(this)->GetPhysicalEntity(m_physicBodyID);
		if (result/*.has_value()*/)
		{
			m_bCurrentlyDynamic = bInIsDynamic;
			BBPhysicBody& body = *result/*->value*/;
			body.Dynamics = BallDynamicValue(m_bCurrentlyDynamic);
			bDone = true;
		}
	}

	if (!bDone)
	{
		m_bCurrentlyDynamic = bInIsDynamic;
		InitializePhysicalBody();
	}
}

bool BB::BBBall::DeInitialize()
{
	bool bAllSucessfull = true;
	bAllSucessfull = DeInitializePhysicalBody() && bAllSucessfull;
	m_bCurrentlyDynamic = false;
	bAllSucessfull = ClearGameBoardTexture() && bAllSucessfull;
	m_thisWPtr.reset();
	return bAllSucessfull;
}

void BB::BBBall::SetPosition(const sf::Vector2f InPosition)
{
	auto result = BBPhysicManager::Get(this)->GetPhysicalEntity(m_physicBodyID);
	if (result/*.has_value()*/)
	{
		BBPhysicBody& body = *result/*->value*/;

		RSfml::SetTransformPosition(/*InOut*/body.Transform, InPosition);
		OnPhysicPassEnded(body, sf::Time::Zero, sf::Time::Zero);
		//m_ballSprite.setPosition(newPos);

	}
}

void BB::BBBall::Launch(const sf::Vector2f InDirection, const float InLaunchSpeed)
{
	auto result = BBPhysicManager::Get(this)->GetPhysicalEntity(m_physicBodyID);
	if (result/*.has_value()*/)
	{
		BBPhysicBody& body = *result/*->value*/;
		body.Dynamics.Velocity += InDirection * InLaunchSpeed;
	}
}

void BBBall::Render(sf::RenderWindow* InWindow) const
{
	InWindow->draw(m_ballSprite);
}

void BBBall::SetGameboardTexture(std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas)
{
	m_texture = InTexture;
	m_atlasData = InTextureAtlas;
	m_bLoadedTextures = (bool)m_texture && (bool)m_atlasData;
	if (m_bLoadedTextures)
	{
		m_ballSprite = sf::Sprite(*m_texture.get());
		m_ballSprite.setTextureRect(m_atlasData->TextureDatas["BallDefault"].RectFromBoundsMinusPad<int>());
		m_ballSprite.setOrigin((sf::Vector2f)m_ballSprite.getTextureRect().size * 0.5f);
		//m_ballSprite.setRotation(sf::degrees(270.0f));
		m_ballSprite.setScale(sf::Vector2f(0.25f, 0.25f));
		m_ballSprite.setPosition({ 0.f, 0.f });
	}
}

bool BB::BBBall::ClearGameBoardTexture()
{
	if (m_bLoadedTextures)
	{
		m_ballSprite.setTexture(EMPTY_TEXTURE, true);
		m_bLoadedTextures = false;
	}
	m_atlasData.reset();
	m_texture.reset();
	return true;
}

void BB::BBBall::InitializePhysicalBody()
{
	BBPhysicManager* pm = BBPhysicManager::Get(this);
	sf::Transform trsMovable = m_ballSprite.getTransform();
	trsMovable.translate(m_ballSprite.getOrigin());
	BBColliderBody localMovable = BBColliderBody(BALL);

	localMovable.Colliders.push_back
	(
		BBCollider
		(
			FloatCircle 
			{
				.Position = { 0.0f, 0.0f },
				.Radius = m_ballSprite.getTextureRect().size.x * 0.5f
			}
		)
	);
	localMovable.ComputeAABB();

	BBColliderBodyLWPair movablePair = BBColliderBodyLWPair::FromLocalCollidersBody(localMovable, trsMovable);

	BBPhysicBody movablePB = BB::BBPhysicBody::CreatePhysicBodyWithID
	(
		std::move(trsMovable),
		std::move(movablePair),
		std::move(BallDynamicValue(m_bCurrentlyDynamic)),
		m_thisWPtr
	);
	m_physicBodyID = movablePB.Id();

	pm->AddPhysicalEntity(movablePB);
}

bool BB::BBBall::DeInitializePhysicalBody()
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
			RLog::Log(LOG_ERROR, "BBBall Deinit Failed to get BBPhysicManager !");
			return false;
		}
	}
	m_physicBodyID = 0;
	return true;
}

void BB::BBBall::OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody)
{
	IBBPhysicBodyListener::OnOverlapWith(InThisPhysicBody, InOtherPhysicBody);
	//if (InThisPhysicBody.Id() == m_physicBodyID)
	//{
	//	m_ballSprite.setColor(sf::Color::Green);
	//}
	if (InOtherPhysicBody.GetBehavior().IsA(EBBColMask::GAMEBOARD_DEADZONE))
	{
		if (m_bCurrentlyDynamic)
		{
			m_gameBoard->ReportDestroyedBall(m_thisWPtr);
		}
	}
}

void BB::BBBall::OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	//m_ballSprite.setColor(sf::Color::Red);
	if (InOutThisPhysicBody.Id() == m_physicBodyID)
	{
		sf::Transform trsMovable = m_ballSprite.getTransform();
		trsMovable.translate(m_ballSprite.getOrigin());
		InOutThisPhysicBody.Transform = trsMovable;
		InOutThisPhysicBody.Colliders.RecomputeWorld(InOutThisPhysicBody.Transform, true);
		if (GetCurrentlyDynamic())
		{
			float currentVelocity = InOutThisPhysicBody.Dynamics.Velocity.length();
			if (currentVelocity > 0.1f)
			{
				float clampedVelocity = std::clamp(currentVelocity, m_minSpeed, m_maxSpeed);
				float factor = clampedVelocity / currentVelocity;
				InOutThisPhysicBody.Dynamics.Velocity *= factor;
			}
		}
	}
}

void BB::BBBall::OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	if (InOutThisPhysicBody.Id() == m_physicBodyID)
	{
		
		sf::Transform computedTRS = InOutThisPhysicBody.Transform;
		//computedTRS.translate(-m_ballSprite.getOrigin());
		sf::Vector2f newPos = RSfml::GetTransformPosition(computedTRS);

		m_ballSprite.setPosition(newPos);
	}
}

BBBall::~BBBall()
{
	DeInitialize();
}