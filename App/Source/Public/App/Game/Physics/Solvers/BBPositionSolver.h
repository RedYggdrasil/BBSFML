#pragma once
#include "App/Game/Physics/BBSolver.h"

namespace BB
{
	class BBPositionSolver : public BBSolver
	{
	public:
		virtual void Solve(BBPhysicManager& InPhysicManager, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, sf::Time InDeltaTime) R_PURE_OVERRIDE;
	public:
		BBPositionSolver(BBPhysicManager* InPhysicManager);
		R_VIRTUAL_IMPLICIT ~BBPositionSolver() R_OVERRIDE_IMPLICIT;
	};
}
