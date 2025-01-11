#include "App/Debugs/Physics/BBDebugSAT.h"

#include "RSFML/Tools/RLog.h"
#include "RSFML/Tools/RMath.h"
#include "App/Game/Physics/BBCollider.h"
#include "App/GameSystem/BBProgramInstance.h"
#include "App/Game/Physics/BBCollidersBody.h"
#include "App/Game/Physics/BBPhysicManager.h"

using namespace BB::Debug;


static constexpr bool bUseSphereMovable = false;
static constexpr bool bDrawFirstObject = false;
static constexpr bool bDrawSecondObject = false;
static constexpr bool bSecondObjectIsSphere = false;

void BB::Debug::BBDebugSAT::Tick(const sf::Time InDeltaTime, sf::View InView)
{
	constexpr float DebugRotationSpeed = 180.f;
	constexpr float DebugSecondAngleRotationSpeed = 40.f;

	static sf::Angle m_secondPosAngle = sf::Angle();
	m_secondPosAngle += sf::degrees(20.f * InDeltaTime.asSeconds());

	m_secondPos = RSfml::AngleToVector(m_secondPosAngle) * 100.f;

	m_secondAngle += sf::degrees(DebugSecondAngleRotationSpeed * InDeltaTime.asSeconds());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
	{
		m_debugAngle += sf::degrees(DebugRotationSpeed * InDeltaTime.asSeconds());
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
	{
		m_debugAngle += sf::degrees(-DebugRotationSpeed * InDeltaTime.asSeconds());
	}

	const auto& window = m_programInstance->GetWindow();
	const auto& sceneView = InView;
	sf::Vector2i pos = sf::Mouse::getPosition(window);
	m_debugMousePos = window.mapPixelToCoords(pos, sceneView);

	//ResetColor
	m_movableShape.setFillColor(sf::Color::Red);

	m_movableShape.setPosition(m_debugMousePos);
	m_movableShape.setRotation(m_debugAngle);

	//m_secondShape.setPosition(m_secondPos);
	//m_secondShape.setRotation(m_secondAngle);

	//m_secondCircleShape.setPosition(m_secondPos);
	//m_secondCircleShape.setRotation(m_secondAngle);

	m_movableCircleShape.setFillColor(sf::Color::Red);

	m_movableCircleShape.setPosition(m_debugMousePos);
	m_movableCircleShape.setRotation(m_debugAngle);
	//RLog::Log(LOG_DISPLAY, "MousePos : {}", RSfml::_tostring(m_debugMousePos));

}

void BB::Debug::BBDebugSAT::Start(std::weak_ptr<BBDebugSAT> InThisWPtr, BBProgramInstance* InContext, BBPhysicManager* InPhysicManager, BBScene* InScene)
{
	m_thisWPtr = InThisWPtr;
	m_programInstance = InContext;
	m_physicManager = InPhysicManager;
	m_scene = InScene;

	m_movableShape = sf::RectangleShape({ 256, 64 });
	m_movableShape.setOrigin(m_movableShape.getSize() * 0.5f);
	m_movableShape.setPosition(sf::Vector2f());
	m_movableShape.setRotation(sf::Angle());
	m_movableShape.setFillColor(sf::Color::White);

	m_movableCircleShape = sf::CircleShape(16.f,36);
	m_movableCircleShape.setPosition(sf::Vector2f());
	m_movableCircleShape.setOrigin(sf::Vector2f(m_movableCircleShape.getRadius(), m_movableCircleShape.getRadius()));
	m_movableCircleShape.setRotation(sf::Angle());
	m_movableCircleShape.setFillColor(sf::Color::White);

	m_secondShape = sf::RectangleShape({ 192, 192 });
	m_secondShape.setOrigin(m_secondShape.getSize() * 0.5f);
	m_secondShape.setPosition(sf::Vector2f());
	m_secondShape.setRotation(sf::Angle());
	m_secondShape.setFillColor(sf::Color::Red);

	m_secondCircleShape = sf::CircleShape(256.f, 36);
	m_secondCircleShape.setPosition(sf::Vector2f());
	m_secondCircleShape.setOrigin(sf::Vector2f(m_secondCircleShape.getRadius(), m_secondCircleShape.getRadius()));
	m_secondCircleShape.setRotation(sf::Angle());
	m_secondCircleShape.setFillColor(sf::Color::White);


	sf::Transform trsMovable = m_movableShape.getTransform();
	trsMovable.translate(m_movableShape.getOrigin());

	BBColliderBody localMovable = BBColliderBody(WORLD_DYNAMIC);

	localMovable.Colliders.push_back(BBCollider(OrientedBox::FromSizeAndAngleAtOrigin(m_movableShape.getSize(), sf::Angle())));
	localMovable.ComputeAABB();

	BBColliderBodyLWPair movablePair = BBColliderBodyLWPair::FromLocalCollidersBody(localMovable, trsMovable);
	BBPhysicBody movablePB = BB::BBPhysicBody::CreatePhysicBodyWithID
	(
		std::move(trsMovable),
		std::move(movablePair),
		std::move(NoDynamics(10000.f/*kg*/)),
		m_thisWPtr
	);
	m_movablePBID = movablePB.Id();


	sf::Transform trsSecond = m_secondShape.getTransform();
	//trsSecond.translate(m_secondShape.getOrigin());

	BBColliderBody localSecond = BBColliderBody(WORLD_STATIC);

	localSecond.Colliders.push_back(BBCollider(OrientedBox::FromSizeAndAngleAtOrigin(m_secondShape.getSize(), sf::Angle())));
	localSecond.ComputeAABB();

	BBColliderBodyLWPair secondPair = BBColliderBodyLWPair::FromLocalCollidersBody(localSecond, trsSecond);
	BBPhysicBody secondPB = BB::BBPhysicBody::CreatePhysicBodyWithID
	(
		std::move(trsSecond),
		std::move(secondPair),
		std::move(NoDynamics(10000.f/*kg*/)),
		m_thisWPtr
	);
	m_secondPBID = secondPB.Id();

	sf::Transform trsCircleMovable = m_movableCircleShape.getTransform();
	trsCircleMovable.translate(m_movableCircleShape.getOrigin());

	BBColliderBody localCircleMovable = BBColliderBody(WORLD_DYNAMIC);

	localCircleMovable.Colliders.push_back(BBCollider(FloatCircle{ .Position = sf::Vector2f(), .Radius = m_movableCircleShape.getRadius()}));
	localCircleMovable.ComputeAABB();

	BBColliderBodyLWPair movableCirclePair = BBColliderBodyLWPair::FromLocalCollidersBody(localCircleMovable, trsCircleMovable);
	BBPhysicBody movableCirclePB = BB::BBPhysicBody::CreatePhysicBodyWithID
	(
		std::move(trsCircleMovable),
		std::move(movableCirclePair),
		std::move(NoDynamics(10000.f/*kg*/, /*Bounciness*/1.f)),
		m_thisWPtr
	);
	m_movableCirclePBID = movableCirclePB.Id();

	sf::Transform trsSecondCircle = m_secondCircleShape.getTransform();
	trsSecondCircle.translate(m_secondCircleShape.getOrigin());

	BBColliderBody localSecondCircle = BBColliderBody(WORLD_STATIC);

	localSecondCircle.Colliders.push_back(BBCollider(FloatCircle{ .Position = sf::Vector2f(), .Radius = m_secondCircleShape.getRadius() }));
	localSecondCircle.ComputeAABB();

	BBColliderBodyLWPair secondCirclePair = BBColliderBodyLWPair::FromLocalCollidersBody(localSecondCircle, trsSecondCircle);
	BBPhysicBody secondCirclePB = BB::BBPhysicBody::CreatePhysicBodyWithID
	(
		std::move(trsSecondCircle),
		std::move(secondCirclePair),
		std::move(NoDynamics(10000.f/*kg*/)),
		m_thisWPtr
	);
	m_secondCirclePBID = secondCirclePB.Id();


	if (bDrawFirstObject)
	{
		if (bUseSphereMovable)
		{
			m_physicManager->AddPhysicalEntity(movableCirclePB);
			m_movablePBID = 0;
		}
		else
		{
			m_physicManager->AddPhysicalEntity(movablePB);
			m_movableCirclePBID = 0;
		}
	}
	if (bDrawSecondObject)
	{

		if (bSecondObjectIsSphere)
		{
			m_physicManager->AddPhysicalEntity(secondCirclePB);
			m_secondPBID = 0;
		}
		else
		{
			m_physicManager->AddPhysicalEntity(secondPB);
			m_secondCirclePBID = 0;
		}
	}
	else
	{
		m_secondPBID = 0;
		m_secondCirclePBID = 0;
	}
}

void BB::Debug::BBDebugSAT::Draw(sf::RenderWindow* InWindow)
{
	if (bDrawFirstObject)
	{
		if (bUseSphereMovable)
		{
			InWindow->draw(m_movableCircleShape);
		}
		else
		{
			InWindow->draw(m_movableShape);
		}
	}
	if (bDrawSecondObject)
	{
		if (bSecondObjectIsSphere)
		{
			InWindow->draw(m_secondCircleShape);
		}
		else
		{
			InWindow->draw(m_secondShape);
		}
	}
}

BB::Debug::BBDebugSAT::~BBDebugSAT()
{
	if (m_physicManager)
	{
		if ((bool)m_secondCirclePBID)
		{
			m_physicManager->MarkRemovePhysicalEntity(m_secondCirclePBID);
		}
		if ((bool)m_movableCirclePBID)
		{
			m_physicManager->MarkRemovePhysicalEntity(m_movableCirclePBID);
		}
		if ((bool)m_secondPBID)
		{
			m_physicManager->MarkRemovePhysicalEntity(m_secondPBID);
		}
		if ((bool)m_movablePBID)
		{
			m_physicManager->MarkRemovePhysicalEntity(m_movablePBID);
		}
	}
}

void BB::Debug::BBDebugSAT::OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody)
{
	IBBPhysicBodyListener::OnOverlapWith(InThisPhysicBody, InOtherPhysicBody);
	if (InThisPhysicBody.Id() == m_movablePBID)
	{
		m_movableShape.setFillColor(sf::Color::Green);
	}
	else if (InThisPhysicBody.Id() == m_movableCirclePBID)
	{
		m_movableCircleShape.setFillColor(sf::Color::Green);
	}
	//RLog::Log(LOG_DISPLAY, "PhysicBody '{}' Overlapped with '{}'", InThisPhysicBody.Id(), InOtherPhysicBody.Id());
}

void BB::Debug::BBDebugSAT::OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	if (InOutThisPhysicBody.Id() == m_movablePBID)
	{
		sf::Transform trsMovable = m_movableShape.getTransform();
		trsMovable.translate(m_movableShape.getOrigin());
		InOutThisPhysicBody.Transform = trsMovable;
		InOutThisPhysicBody.Colliders.RecomputeWorld(InOutThisPhysicBody.Transform, true);
	}
	else if (InOutThisPhysicBody.Id() == m_movableCirclePBID)
	{
		sf::Transform trsCircleMovable = m_movableCircleShape.getTransform();
		trsCircleMovable.translate(m_movableCircleShape.getOrigin());
		InOutThisPhysicBody.Transform = trsCircleMovable;
		InOutThisPhysicBody.Colliders.RecomputeWorld(InOutThisPhysicBody.Transform, true);
	}
	else if (InOutThisPhysicBody.Id() == m_secondPBID)
	{
		sf::Transform trsSecond = m_secondShape.getTransform();
		trsSecond.translate(m_secondShape.getOrigin());
		InOutThisPhysicBody.Transform = trsSecond;
		InOutThisPhysicBody.Colliders.RecomputeWorld(InOutThisPhysicBody.Transform, true);
	}
	else if (InOutThisPhysicBody.Id() == m_secondCirclePBID)
	{
		sf::Transform trsSecondCircle = m_secondCircleShape.getTransform();
		trsSecondCircle.translate(m_secondCircleShape.getOrigin());
		InOutThisPhysicBody.Transform = trsSecondCircle;
		InOutThisPhysicBody.Colliders.RecomputeWorld(InOutThisPhysicBody.Transform, true);
	}
	
}

void BB::Debug::BBDebugSAT::OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
	IBBPhysicBodyListener::OnPhysicPassEnded(InOutThisPhysicBody, InGameTime, InDeltaTime);


	//RLog::Log(LOG_DISPLAY, "OnPhysicPassEnded '{}'", InOutThisPhysicBody.Id());
}
