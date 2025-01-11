#pragma once
#include "RSFML/Defines/RSFML.h"
#include "App/Game/Physics/BBCollisionResult.h"
#include "App/Game/Physics/BBPhysicBody.h"
#include "App/Memory/IBBSceneObject.h"
#include <SFML/Graphics.hpp>

namespace BB
{
	class BBPhysicManager;
	class BBSolver : public IBBSceneObject
	{
		BBPhysicManager* m_physicManager;
	public:
		virtual void Solve(BBPhysicManager& pm, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, sf::Time InDeltaTime) = 0;
	
	protected:
		std::vector<BBPhysicBody>& GetPhysicManagerPhysicBodies(BBPhysicManager& InPhysicManager) const;
	public :
		BBSolver(BBPhysicManager* InPhysicManager);
		R_VIRTUAL_IMPLICIT ~BBSolver() R_OVERRIDE_IMPLICIT;
	};
};