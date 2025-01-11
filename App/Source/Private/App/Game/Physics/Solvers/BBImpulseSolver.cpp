#include "App/Game/Physics/Solvers/BBImpulseSolver.h"
#include "RSFML/Tools/RMath.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/GameSystem/BBProgramInstance.h"
#include "App/Game/Physics/BBPhysicBody.h"

using namespace BB;

void UpdatePaddleCollisionNormal(ColbodyIDXsAndColResult& InIDXsCol, const sf::Transform& InPaddleTransform, const sf::Transform& InBallTransform, bool bInPaddleObj0, sf::Vector2f& InOutCorrectedNormal)
{
	static const sf::Vector2f Right(1.f, 0.f);

	sf::Vector2f PaddlePos = RSfml::GetTransformPosition(InPaddleTransform);
	sf::Vector2f BallPos = RSfml::GetTransformPosition(InBallTransform);

	if ((std::abs(InOutCorrectedNormal.dot(Right)) > 0.85f) || (BallPos.y > PaddlePos.y))
	{
		InOutCorrectedNormal = (PaddlePos - BallPos).normalized();
		InIDXsCol.CollisionResult.P1 = InIDXsCol.CollisionResult.P2 = PaddlePos;
		InIDXsCol.CollisionResult.P2 = InIDXsCol.CollisionResult.P1 + (InOutCorrectedNormal * 100.f);
		return;
		//RLog::Log(LOG_DISPLAY,"Dropped Angle '{}' with dot '{}'", RSfml::_tostring(InOutCorrectedNormal), std::abs(InOutCorrectedNormal.dot(Right)));

	}
	PaddlePos.y += 250.f;
	sf::Vector2f otherPointPos = InIDXsCol.CollisionResult.P1;
	//if (PaddlePos.y <= (otherPointPos.y + 5.f))
	//{
	//	PaddlePos.y += 200.f;
	//}
	InOutCorrectedNormal = (PaddlePos - otherPointPos).normalized();// (bInPaddleObj0 ? 1.f : -1.f);
	InIDXsCol.CollisionResult.P1 = InIDXsCol.CollisionResult.P2 = PaddlePos;
	InIDXsCol.CollisionResult.P2 = InIDXsCol.CollisionResult.P1 + (InOutCorrectedNormal * 100.f);
}

static void Solve_Generic(BBPhysicManager& pm, ColbodyIDXsAndColResult& InOutIDXsCollision, BBPhysicBody& InOutFirstBody, BBPhysicBody& InOutSecondBody,  sf::Time InDeltaTime);
static void Solve_BallPaddle(BBPhysicManager& pm, ColbodyIDXsAndColResult& InOutIDXsCollision, BBPhysicBody& InOutFirstBody, BBPhysicBody& InOutSecondBody, sf::Time InDeltaTime);


void BBImpulseSolver::Solve(BBPhysicManager& pm, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, sf::Time InDeltaTime)
{
	constexpr EBBColMask PADDLE_AND_BALL_MASK = (EBBColMask::BALL | EBBColMask::PADDLE);
	//constexpr EBBColMask PADDLE_AND_BALL_MASK_2 = EBBColMask::ALL ^ (EBBColMask::BALL | EBBColMask::PADDLE);

	//return;
	std::vector<BBPhysicBody>& physicBodies = GetPhysicManagerPhysicBodies(pm);

	for (ColbodyIDXsAndColResult& IDXsCol : InIDXsCollisions)
	{
		/// Load values ///

		BBPhysicBody& firstBody = physicBodies[IDXsCol.FirstElementIndex];
		BBPhysicBody& secondBody = physicBodies[IDXsCol.SecondElementIndex];
		EBBColMask bothMasks = firstBody.GetBehavior().SelfMask | secondBody.GetBehavior().SelfMask;

		if ((bothMasks & PADDLE_AND_BALL_MASK) == PADDLE_AND_BALL_MASK)
		{
			Solve_BallPaddle(/*InOut*/pm, /*InOut*/IDXsCol, firstBody, secondBody, InDeltaTime);
		}
		else
		{
			Solve_Generic(/*InOut*/pm, /*InOut*/IDXsCol, firstBody, secondBody, InDeltaTime);
		}
	}
}

BBImpulseSolver::BBImpulseSolver(BBPhysicManager* InPhysicManager)
: BBSolver(InPhysicManager)
{
}

BBImpulseSolver::~BBImpulseSolver()
{
}

void Solve_Generic(BBPhysicManager& pm, ColbodyIDXsAndColResult& InOutIDXsCollision, BBPhysicBody& InOutFirstBody, BBPhysicBody& InOutSecondBody, sf::Time InDeltaTime)
{
	sf::Vector2f lFirstVelocity = InOutFirstBody.Dynamics.Velocity;
	sf::Vector2f lSecondVelocity = InOutSecondBody.Dynamics.Velocity;

	//relative velocity of how fast the object are moving relative to the other
	// Exemple : Because A is moving in +X and B -X, B is going "twice" as fast toward -X in realation to X
	// EX : [ A ->(X:+1)  (X:-1)<- B ]  ==> [ -1 - +1 ] ==> [x:-2]
	sf::Vector2f lRelativeVelocity = lSecondVelocity - lFirstVelocity;

	//The normal from contact points A->B
	sf::Vector2f lNormal = InOutIDXsCollision.CollisionResult.Normal;

	//Speed alongside the contact normal
	float contactNormalSpeed = lRelativeVelocity.dot(lNormal);


	/// Start computing Impulse ///
	if (contactNormalSpeed < 0.f)
	{
		//RLog::Log(LOG_DISPLAY, "No speed along normal !");
		return;
	}

	//Bounciness, or elesticity of elements, default 1, anything above create stronger bounce, smaller give weaker bounce
	float e = InOutFirstBody.Dynamics.Bounciness * InOutSecondBody.Dynamics.Bounciness;

	float j = (contactNormalSpeed * -(1.0f + e));

	sf::Vector2f impulse = lNormal * j;

	constexpr bool bUSE_SIMPLE = false;
	if (InOutFirstBody.Dynamics.bIsDynamic)
	{
		if constexpr (bUSE_SIMPLE)
		{
			InOutFirstBody.Dynamics.Velocity = -lNormal * InOutFirstBody.Dynamics.Velocity.dot(lNormal);
		}
		else
		{
			//float l = lFirstVelocity.length();
			lFirstVelocity -= impulse * 1.f;
			//float l2 = lFirstVelocity.length();
			//if (l2 < l)
			//{
			//	RLog::Log(LOG_DISPLAY, "Correct speed by {} !", (l / l2));
			//	lFirstVelocity *= (l / l2);
			//}
			InOutFirstBody.Dynamics.Velocity = lFirstVelocity;
		}
	}

	if (InOutSecondBody.Dynamics.bIsDynamic)
	{
		if constexpr (bUSE_SIMPLE)
		{
			InOutSecondBody.Dynamics.Velocity = -InOutSecondBody.Dynamics.Velocity;
		}
		else
		{
			//float l = lSecondVelocity.length();
			lSecondVelocity += impulse * 1.f;
			//float l2 = lSecondVelocity.length();
			//if (l2 < l)
			//{
			//	RLog::Log(LOG_DISPLAY, "Correct speed by {} !", (l / l2));
			//	lSecondVelocity *= (l / l2);
			//}
			InOutSecondBody.Dynamics.Velocity = lSecondVelocity;
		}
	}
}

void Solve_BallPaddle(BBPhysicManager& pm, ColbodyIDXsAndColResult& InOutIDXsCollision, BBPhysicBody& InOutFirstBody, BBPhysicBody& InOutSecondBody, sf::Time InDeltaTime)
{
	bool bBallIsFirst = InOutFirstBody.GetBehavior().IsA(EBBColMask::BALL);
	BBPhysicBody* ballPB = bBallIsFirst ? &InOutFirstBody : &InOutSecondBody;
	BBPhysicBody* paddlePB = bBallIsFirst ? &InOutSecondBody : &InOutFirstBody;


	sf::Vector2f lFirstVelocity = InOutFirstBody.Dynamics.Velocity;
	sf::Vector2f lSecondVelocity = InOutSecondBody.Dynamics.Velocity;

	//relative velocity of how fast the object are moving relative to the other
	// Exemple : Because A is moving in +X and B -X, B is going "twice" as fast toward -X in realation to X
	// EX : [ A ->(X:+1)  (X:-1)<- B ]  ==> [ -1 - +1 ] ==> [x:-2]
	sf::Vector2f lRelativeVelocity = lSecondVelocity - lFirstVelocity;

	//The normal from contact points A->B
	sf::Vector2f lNormal = InOutIDXsCollision.CollisionResult.Normal;

	//Speed alongside the contact normal
	float contactNormalSpeed = lRelativeVelocity.dot(lNormal);


	/// Start computing Impulse ///
	if (contactNormalSpeed < 0.f)
	{
		//RLog::Log(LOG_DISPLAY, "No speed along normal !");
		return;
	}
	UpdatePaddleCollisionNormal(InOutIDXsCollision, paddlePB->Transform, ballPB->Transform, !bBallIsFirst, /*InOut*/lNormal);
	InOutIDXsCollision.CollisionResult.Normal = lNormal;
	float ballVelocity = ballPB->Dynamics.Velocity.length();

	//Bounciness, or elesticity of elements, default 1, anything above create stronger bounce, smaller give weaker bounce
	float e = InOutFirstBody.Dynamics.Bounciness * InOutSecondBody.Dynamics.Bounciness;

	if (ballPB->Dynamics.bIsDynamic)
	{
		ballPB->Dynamics.Velocity = lNormal * -ballVelocity * e;

	}
	return;
}
