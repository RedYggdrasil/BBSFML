#include "App/Game/Physics/BBBallProjectileSolver.h"
#include "App/Game/BBBall.h"

using namespace BB;

//BBBallProjectileSolver::BBBallProjectileSolver(IBBSceneObject* InParentObject)
//	:IBBProjectileSolver(), IBBSceneObject(InParentObject)
//{
//}
//
//
//
//bool BB::BBBallProjectileSolver::SolveProjectiles(IBBProjectile* InProjectileArray, size_t nbElements)
//{
//	bool bAllSucessFull = true;
//	BBBall* balls = static_cast<BBBall*>(InProjectileArray);
//	IBBProjectileSolver::ProjectileRange<BBBall> ballsRange(static_cast<BBBall*>(InProjectileArray), nbElements);
//
//	for (BBBall& ball : ballsRange)
//	{
//
//	}
//	return bAllSucessFull;
//}
//
//BBBallProjectileSolver::~BBBallProjectileSolver()
//{
//
//}