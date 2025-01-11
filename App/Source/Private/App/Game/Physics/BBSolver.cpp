#include "App/Game/Physics/BBSolver.h"
#include "App/Game/Physics/BBPhysicManager.h"

using namespace BB;

std::vector<BBPhysicBody>& BB::BBSolver::GetPhysicManagerPhysicBodies(BBPhysicManager& InPhysicManager) const
{
	return InPhysicManager.m_physicBodies.Values.GetUnderlyingVector();
}

BBSolver::BBSolver(BBPhysicManager* InPhysicManager)
: IBBSceneObject((IBBSceneObject*)InPhysicManager), m_physicManager(InPhysicManager)
{
}

BBSolver::~BBSolver()
{

}