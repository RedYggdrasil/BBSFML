#pragma once
#define NOMINMAX
#define BB_DEBUG_PHYSIC _DEBUG
#include "RCB/Tools/RCBVectorMap.h"
#include "RSFML/Tools/REnum.h"
#include "App/Defines/pch.h"
#include "App/Memory/IBBSceneObject.h"
#include "App/Game/Physics/BBCollidersBody.h"
#include "App/Game/Physics/BBPhysicBody.h"
#if BB_DEBUG_PHYSIC
#include "App/Game/Physics/BBCollisionResult.h"
#endif
#include "App/Game/Physics/BBPhysicContext.h"
#include "App/Game/Physics/BBSolver.h"
//#include "App/Physic/Solvers/RSRSolver.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace BB
{
	class BBProgramInstance;

	class BBPhysicManager : public BB::IBBSceneObject
	{
	private:
		size_t m_optimisation_count_reindexing = 0;
		bool m_currentlyResolvingPhysics;
		BBPhysicContext m_physicContext;

		rcb::RCBVectorMap<BBPhysicBodyID, BBPhysicBody, true> m_physicBodies;

		std::vector<std::shared_ptr<BBSolver>> m_physicalSolvers;
#if BB_DEBUG_PHYSIC
		static constexpr size_t DEBUG_FRAME_COUNT = 10;
		std::array<std::vector<ColbodyIDXsAndColResult>, DEBUG_FRAME_COUNT>  DEBUG_lastCollisions;
		size_t DEBUG_nextArrayToSet = 0;
#endif

	public:
		void AddPhysicalEntities(const BBPhysicBody* InStartingValues, size_t InSizes);
		void AddPhysicalEntity(const BBPhysicBody& InStartingValue);
		BBPhysicBody* GetPhysicalEntity(BBPhysicBodyID InID);
		void MarkRemovePhysicalEntity(BBPhysicBodyID InID);

		std::weak_ptr<BBSolver> AddPhysicSolver(std::shared_ptr<BBSolver>&& InSolver);

		void RemovePhysicSolver(std::weak_ptr<BBSolver> InSolver);

		void ClearAllPhysicSolvers();
		inline void SetPhysicContext(const BBPhysicContext& InContext) { m_physicContext = InContext; };
		void UpdatePhysic(const sf::Time& InGameTime, const sf::Time& InDeltaTime);

	public:
		static BBPhysicManager* Get(const BB::IBBSceneObject* InSceneObject);
	public:
		BBPhysicManager(BB::IBBSceneObject* InParentObject);
		BBPhysicManager(const BBPhysicManager&) = delete;
		BBPhysicManager& operator=(const BBPhysicManager&) = delete;

		bool Init();
		void Shutdown();
	public:
		std::vector<const BBPhysicBody*> FindOverlappedEntities(const BBColliderBody& InCollidersBody, const BBPhysicBodyID InBodyToIgnore = 0, const size_t InMaxReturnedObject = std::numeric_limits<size_t>::max()) const;
		inline std::vector<const BBPhysicBody*> FindOverlappedEntities(const BBPhysicBody& InPhysicBody, const size_t InMaxReturnedObject = std::numeric_limits<size_t>::max()) const
		{
			return FindOverlappedEntities(InPhysicBody.Colliders.GetWorld(), InPhysicBody.Id(), InMaxReturnedObject);
		}
		inline bool IsOverlappingAny(const BBColliderBody& InCollidersBody, const BBPhysicBodyID InBodyToIgnore) const { return FindOverlappedEntities(InCollidersBody, InBodyToIgnore, 1).size() > 0; }
		inline bool IsOverlappingAny(const BBPhysicBody& InPhysicBody) const { return FindOverlappedEntities(InPhysicBody, 1).size() > 0; }

#if BB_DEBUG_PHYSIC
		void draw(sf::RenderWindow* InWindow);
#endif
	protected:
		std::vector<std::pair<size_t, size_t>> ComputeOverlaping();

	private:

		void RemovePhysicSolver(std::vector<std::shared_ptr<BBSolver>>::iterator InIterSolver);

	private:
		friend class BBPhysicManager_Collisions;
		friend class BBPhysicManager_Dynamics;
		friend class BBSolver;
	};
}
