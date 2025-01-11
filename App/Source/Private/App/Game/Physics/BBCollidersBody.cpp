#include "App/Game/Physics/BBCollidersBody.h"
#include "App/Game/Physics/BBCollisionFuncs.hpp"

using namespace BB;

void BBColliderBodyLWPair::RecomputeWorld(const sf::Transform& InOuterWorldMatrix, bool bInForceDirty)
{
	if (bInForceDirty || m_bLocalToWorldDirty)
	{
		BBColliderBody::ComputeWorldFromLocal(InOuterWorldMatrix, this->m_localPB, /*Out*/this->m_worldPB);
		m_bLocalToWorldDirty = false;
	}
}

BBColliderBodyLWPair::BBColliderBodyLWPair(const BBColBehavior& InColBehavior)
	: m_localPB(InColBehavior), m_worldPB(InColBehavior)
{
}

BBColliderBodyLWPair BBColliderBodyLWPair::FromLocalCollidersBody(const BBColliderBody& InLocalColliders, const sf::Transform& InOuterWorldMatrix)
{
	BBColliderBodyLWPair result = BBColliderBodyLWPair(InLocalColliders.Behavior);
	BBColliderBody* localColliders = &result.DirtyGetLocal();
	(*localColliders) = InLocalColliders;

	localColliders->ComputeAABB();
	result.RecomputeWorld(InOuterWorldMatrix, true);
	return result;
}

void BBColliderBody::ComputeAABB()
{
	if (Colliders.size() < 1)
	{
		AABB = sf::FloatRect({ 0.f,0.f }, { 0.f,0.f });
		return;
	}
	AABB = Colliders[0].ComputeBoundBox();
	for (size_t i = 1; i < Colliders.size(); ++i)
	{
		Colliders[i].ExtendBoundBox(&AABB);
	}
}

void BBColliderBody::ComputeWorldFromLocal(const sf::Transform& InOuterWorldMatrix, const BBColliderBody& InLocal, BBColliderBody& OutWorld)
{
	OutWorld = InLocal;
	for (size_t i = 0, length = InLocal.Colliders.size(); i < length; ++i)
	{
		OutWorld.Colliders[i] = InLocal.Colliders[i].TransformCopy(InOuterWorldMatrix);
	}
	OutWorld.ComputeAABB();
}

void BBColliderBody::ComputeLocalFromWorld(const sf::Transform& InOuterWorldMatrix, const BBColliderBody& InWorld, BBColliderBody& OutLocal)
{
	OutLocal = InWorld;

	// Compute the inverse of the parent's world transformation matrix
	sf::Transform invParentWorld = InOuterWorldMatrix.getInverse();

	for (size_t i = 0, length = InWorld.Colliders.size(); i < length; ++i)
	{
		// Compute the inverse of the parent's world transformation matrix
		OutLocal.Colliders[i] = InWorld.Colliders[i].TransformCopy(invParentWorld);
	}
	OutLocal.ComputeAABB();
}

bool BBColliderBody::Intersects(const BBColliderBody& InLeft, const BBColliderBody& InRight)
{
	if (!InLeft.Behavior.Interact(InRight.Behavior))
	{
		return false;
	}

	if (!InLeft.AABB.findIntersection(InRight.AABB).has_value())
	{
		return false;
	}
	for (const BBCollider& leftCol : InLeft.Colliders)
	{
		for (const BBCollider& rightCol : InRight.Colliders)
		{
			if (leftCol.OverlapWith(rightCol))
			{
				return true;
			}
		}
	}
	return false;
}

BBCollisionType BBColliderBody::FillCollidesOrOverlaps(const BBColliderBody& InLeft, const BBColliderBody& InRight, std::vector<ColIDXsAndColResult>& OutIDXCollisionResults, bool bFillOnlyDeepestCollision)
{
	//Collision always take precedence over overlaps
	bool bCanCollide = InLeft.Behavior.Collide(InRight.Behavior);
	if (!bCanCollide)
	{
		if (!InLeft.Behavior.Interact(InRight.Behavior))
		{
			return BBCollisionType::None; //Cannot collide or overlaps, so return
		}
	}
	if (!InLeft.AABB.findIntersection(InRight.AABB).has_value())
	{
		return BBCollisionType::None; //Objects are too far away
	}
	if (!bCanCollide)
	{
		return TestCollidersForOneOverlap(InLeft, InRight) ? BBCollisionType::Overlap : BBCollisionType::None;
	}
	else
	{
		if (bFillOnlyDeepestCollision)
		{
			ColIDXsAndColResult temp = TestCollidersForDeepestCollision(InLeft, InRight);
			if (temp.CollisionResult.bDidCollide)
			{
				OutIDXCollisionResults.push_back(temp);
				return BBCollisionType::Collide;
			}
			return BBCollisionType::None;
		}
		else
		{
			size_t nbNewCollisions = TestCollidersForCollisions(InLeft, InRight, /*InOut*/OutIDXCollisionResults);
			return (nbNewCollisions > 0) ? BBCollisionType::Collide : BBCollisionType::None;
		}
	}
}

bool BBColliderBody::TestCollidersForOneOverlap(const BBColliderBody& InLeft, const BBColliderBody& InRight)
{
	for (const BBCollider& leftCol : InLeft.Colliders)
	{
		for (const BBCollider& rightCol : InRight.Colliders)
		{
			if (leftCol.OverlapWith(rightCol))
			{
				return true;
			}
		}
	}
	return false;
}

ColIDXsAndColResult BBColliderBody::TestCollidersForDeepestCollision(const BBColliderBody& InLeft, const BBColliderBody& InRight)
{
	ColIDXsAndColResult result;
	result.CollisionResult.bDidCollide = false;
	result.CollisionResult.Depth = 0.f;

	BBCollisionResult temp;

	for (size_t i = 0; i < InLeft.Colliders.size(); ++i)
	{
		for (size_t j = 0; j < InRight.Colliders.size(); ++j)
		{
			temp = BBCols::ResolveCollision(&InLeft.Colliders[i], &InRight.Colliders[j]);
			if (temp.bDidCollide)
			{
				if (
					!result.CollisionResult.bDidCollide ||
					result.CollisionResult.Depth < temp.Depth
					)
				{
					result.FirstElementIndex = i;
					result.SecondElementIndex = j;
					result.CollisionResult = temp;
				}
			}
		}
	}
	return result;
}

size_t BBColliderBody::TestCollidersForCollisions(const BBColliderBody& InLeft, const BBColliderBody& InRight, std::vector<ColIDXsAndColResult>& InOutColResults)
{
	size_t nbCollisions = 0;
	BBCollisionResult temp;
	for (size_t i = 0; i < InLeft.Colliders.size(); ++i)
	{
		for (size_t j = 0; j < InRight.Colliders.size(); ++j)
		{
			temp = BBCols::ResolveCollision(&InLeft.Colliders[i], &InRight.Colliders[j]);
			if (temp.bDidCollide)
			{
				++nbCollisions;
				InOutColResults.push_back
				(
					ColIDXsAndColResult
					{
						.FirstElementIndex = i,
						.SecondElementIndex = j,
						.CollisionResult = temp
					}
				);
			}
		}
	}
	return nbCollisions;
}


BBColliderBody::BBColliderBody(const BBColBehavior& InColBehavior)
	:Behavior(InColBehavior)
{
}

BBColliderBody::BBColliderBody(const BBColliderBody& InOther)
	: Behavior(InOther.Behavior), AABB(InOther.AABB), Colliders(InOther.Colliders.size())
{
	std::copy(InOther.Colliders.begin(), InOther.Colliders.end(), Colliders.begin());
}

BBColliderBody& BBColliderBody::operator=(const BBColliderBody& InOther)
{
	this->Behavior = InOther.Behavior;
	this->AABB = InOther.AABB;
	this->Colliders.resize(InOther.Colliders.size());

	std::copy(InOther.Colliders.begin(), InOther.Colliders.end(), this->Colliders.begin());
	return *this;
}

