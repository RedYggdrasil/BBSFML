#pragma once
#include "App/Game/Physics/BBSolver.h"

namespace BB
{
	class BBImpulseSolver : public BBSolver
	{
	public:
		virtual void Solve(BBPhysicManager& InPhysicManager, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, sf::Time InDeltaTime) R_PURE_OVERRIDE;
	public:
		BBImpulseSolver(BBPhysicManager* InPhysicManager);
		R_VIRTUAL_IMPLICIT ~BBImpulseSolver() R_OVERRIDE_IMPLICIT;
	};
}
