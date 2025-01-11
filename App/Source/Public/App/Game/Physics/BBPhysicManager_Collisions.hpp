#pragma once
#include "RSFML/Tools/REnum.h"
#include "RSFML/Tools/RLog.h"
#include "App/Defines/pch.h"
#include "App/Game/Physics/BBPhysicManager.h"

namespace BB
{
	class BBPhysicManager_Collisions
	{
	public:
		friend BBPhysicManager;
		STATIC_CLASS_R5(BBPhysicManager_Collisions);
	private:

		inline static bool ComputeCollisionsAndOverlaps(BBPhysicManager& pm, const sf::Time& InDeltaTime, std::vector<ColbodyIDXsAndColResult>& OutCollisions, std::vector<std::pair<size_t, size_t>>& OutOverlaps)
		{
			OutCollisions.clear();
			OutOverlaps.clear();

			for (size_t i = 0, length = pm.m_physicBodies.Values.size(); i < length; ++i)
			{
				BBPhysicBody& pBOne = pm.m_physicBodies.Values[i];
				const BBColliderBody& e1CBody = pBOne.Colliders.GetWorld();
				for (size_t j = i + 1; j < length; ++j)
				{
					BBPhysicBody& pBTwo = pm.m_physicBodies.Values[j];
					const BBColliderBody& e2CBody = pBTwo.Colliders.GetWorld();

					BBCollisionType colType = e1CBody.FillCollidesOrOverlaps(e2CBody, /*Out*/OutCollisions, true);

					if (colType == BBCollisionType::None) { continue; }
					else if (colType == BBCollisionType::Overlap)
					{
						OutOverlaps.push_back({ i, j });
					}
					else if (colType == BBCollisionType::Collide)
					{
						//We are not intrested in witch specific colliders collided but only PhysicBody there
						ColbodyIDXsAndColResult& newIDXsCollision = OutCollisions.back();
						newIDXsCollision.FirstElementIndex = i;
						newIDXsCollision.SecondElementIndex = j;
					}
				}
			}
			return true;
		}
	};
};