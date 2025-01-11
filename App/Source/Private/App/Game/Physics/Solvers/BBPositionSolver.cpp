#include "App/Game/Physics/Solvers/BBPositionSolver.h"
#include "RSFML/Tools/RMath.h"
#include "App/Game/Physics/BBCollisionResult.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/GameSystem/BBProgramInstance.h"
#include "App/Game/Physics/BBPhysicBody.h"

using namespace BB;

#define lPhys_USE_ORIGINAL false
void BBPositionSolver::Solve(BBPhysicManager& pm, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, sf::Time InDeltaTime)
{
	std::vector<BBPhysicBody>& physicBodies = GetPhysicManagerPhysicBodies(pm);

	for (ColbodyIDXsAndColResult& IDXsCol : InIDXsCollisions)
	{
		BBPhysicBody& firstBody = physicBodies[IDXsCol.FirstElementIndex];
		BBPhysicBody& secondBody = physicBodies[IDXsCol.SecondElementIndex];


		sf::Vector2f lNorm = IDXsCol.CollisionResult.Normal;
		lNorm = lNorm * IDXsCol.CollisionResult.Depth;
#if lPhys_USE_ORIGINAL
		//In this approch each object get the full scaled normal applied if both dynamics
		//Also normal is contact point 1 to 2, but object 1 is scaled normal negative, this seems the wrong way around
		//I suspect original author has depth as a negative value, mine is an absolute value
		float firstRejectApply = (float)(int32_t)!firstBody.Dynamics.bIsDynamic;
		float secondRejectApply = (float)(int32_t)!secondBody.Dynamics.bIsDynamic;

		//This effectively divide by : 
		// 0 Static -> 1, 1 static -> 1, 2 static -> 2)
		lNorm = lNorm * (1 / std::max(1.f, firstRejectApply + secondRejectApply));

		//Apply first pos
		sf::Vector2f newPos = RSfml::GetTransformPosition(firstBody.Transform);
		newPos = newPos - (lNorm * (1.f - firstRejectApply));
		RSfml::SetTransformPosition(/*InOut*/firstBody.Transform, newPos);

		//Apply second pos
		newPos = RSfml::GetTransformPosition(secondBody.Transform);
		newPos = newPos + (lNorm * (1.f - secondRejectApply));
		RSfml::SetTransformPosition(/*InOut*/secondBody.Transform, newPos);
#else
		//This is my interpretation of original author's intent, and makes more sens to me
		//In this approch each object get half the scaled normal applied if the other also move, or full is the other is static
		//Also normal is contact point 1 to 2, So I apply positive the normal case 1, and negative case 2 (from contact 2 to 1)

		float firstDynamic = firstBody.Dynamics.bIsDynamic ? 1.f : 0.f;
		float secondDynamic = secondBody.Dynamics.bIsDynamic ? 1.f : 0.f;

		//This effectively divide by : 
		// 0 dynamic -> 1, 1 dynamic -> 1, 2 dynamic -> 2)
		lNorm = lNorm * (1 / std::max(1.f, firstDynamic + secondDynamic));


		//Apply first pos
		sf::Vector2f newPos = RSfml::GetTransformPosition(firstBody.Transform);
		newPos = newPos + (lNorm * firstDynamic);
		RSfml::SetTransformPosition(/*InOut*/firstBody.Transform, newPos);

		//Apply second pos
		newPos = RSfml::GetTransformPosition(secondBody.Transform);
		newPos = newPos - (lNorm * secondDynamic);
		RSfml::SetTransformPosition(/*InOut*/secondBody.Transform, newPos);
#endif
	}
};


#undef lPhys_USE_ORIGINAL

BBPositionSolver::BBPositionSolver(BBPhysicManager* InPhysicManager)
	: BBSolver(InPhysicManager)
{
}

BBPositionSolver::~BBPositionSolver()
{
}
