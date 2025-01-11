#include "App/Game/Physics/BBPhysicManager.h"

#include "RSFML/Tools/RLog.h"
#include "App/GameSystem/BBScene.h"
#include "App/Game/Physics/BBPhysicManager_Dynamics.hpp"
#include "App/Game/Physics/BBPhysicManager_Collisions.hpp"

using namespace BB;

BBPhysicManager* BB::BBPhysicManager::Get(const BB::IBBSceneObject* InSceneObject)
{
	BBScene* scene = InSceneObject->GetScene<BBScene>();
	return scene->GetPhysicManager();
}

BBPhysicManager::BBPhysicManager(IBBSceneObject* InParentObject)
	:IBBSceneObject(InParentObject), m_currentlyResolvingPhysics(false), m_physicContext(PhysicContext::DEFAULT_EARTH),
	m_physicBodies(), m_physicalSolvers()
{
}

bool BB::BBPhysicManager::Init()
{
	return true;
}

void BB::BBPhysicManager::Shutdown()
{
	ClearAllPhysicSolvers();
	if (m_physicBodies.size() > 0)
	{
		RLog::Log(LOG_ERROR, "Physic Body still have values on Shutdown '{}' !", m_physicBodies.size());
		m_physicBodies.clear();
	}
}

std::vector<const BBPhysicBody*> BB::BBPhysicManager::FindOverlappedEntities(const BBColliderBody& InCollidersBody, const BBPhysicBodyID InBodyToIgnore, const size_t InMaxReturnedObject) const
{
	bool bHasIgnore = InBodyToIgnore > 0;

	std::vector<const BBPhysicBody*> overlapped;


	for (const BBPhysicBody& physicalBody : m_physicBodies.Values)
	{
		if (physicalBody.Id() == InBodyToIgnore)
		{
			continue;
		}

		if (InCollidersBody.Intersects(physicalBody.Colliders.GetWorld()))
		{
			overlapped.push_back(&physicalBody);
			if (overlapped.size() >= InMaxReturnedObject)
			{
				break;
			}
		}
	}
	return overlapped;
}

#if BB_DEBUG_PHYSIC
void BB::BBPhysicManager::draw(sf::RenderWindow* InWindow)
{
	sf::Vertex lineP1P2[2] =
	{
		sf::Vertex(),
		sf::Vertex()
	};

	sf::Vertex lineP2Normal[2] =
	{
		sf::Vertex(),
		sf::Vertex()
	};

	for (size_t arrIndex = 0; arrIndex < DEBUG_lastCollisions.size(); ++arrIndex)
	{
		for (size_t i = 0; i < DEBUG_lastCollisions[arrIndex].size(); ++i)
		{
			const ColbodyIDXsAndColResult& col = DEBUG_lastCollisions[arrIndex][i];
			const BBCollisionResult& result = col.CollisionResult;

			lineP2Normal[0] = sf::Vertex(result.P1, sf::Color::White);
			lineP2Normal[1] = sf::Vertex(result.P1 + (result.Normal * 100.f), sf::Color::White);
			InWindow->draw(lineP2Normal, 2, sf::PrimitiveType::Lines);

			lineP1P2[0] = sf::Vertex(result.P1, sf::Color::Blue);
			lineP1P2[1] = sf::Vertex(result.P2, sf::Color::Green);

			InWindow->draw(lineP1P2, 2, sf::PrimitiveType::Lines);
		}
	}
}
#endif

std::vector<std::pair<size_t, size_t>> BB::BBPhysicManager::ComputeOverlaping()
{
	std::vector<std::pair<size_t, size_t>> overlapingObjects = std::vector<std::pair<size_t, size_t>>();

	for (size_t i = 0, length = m_physicBodies.Values.size(); i < length; ++i)
	{
		BBPhysicBody& pBOne = m_physicBodies.Values[i];
		const BBColliderBody& e1CBody = pBOne.Colliders.GetWorld();
		for (size_t j = i + 1; j < length; ++j)
		{
			BBPhysicBody& pBTwo = m_physicBodies.Values[j];
			const BBColliderBody& e2CBody = pBTwo.Colliders.GetWorld();

			if (e1CBody.Intersects(e2CBody))
			{
				overlapingObjects.push_back({ i, j });
			}
		}
	}

	return overlapingObjects;
}

void BB::BBPhysicManager::RemovePhysicSolver(std::vector<std::shared_ptr<BBSolver>>::iterator InIterSolver)
{
	if (InIterSolver != m_physicalSolvers.end())
	{
		m_physicalSolvers.erase(InIterSolver);
	}
}

void BBPhysicManager::AddPhysicalEntities(const BBPhysicBody* InStartingValues, size_t InSizes)
{
	if (InSizes > 0)
	{
		m_physicBodies.reserve_over_current_size(InSizes);
		for (size_t i = 0; i < InSizes; ++i)
		{
			AddPhysicalEntity(InStartingValues[i]);
		}
	}
}
void BBPhysicManager::AddPhysicalEntity(const BBPhysicBody& InStartingValue)
{
	m_physicBodies.insert_or_assign(InStartingValue.Id(), InStartingValue);
}

BBPhysicBody* BBPhysicManager::GetPhysicalEntity(BBPhysicBodyID InID)
{
	if (m_physicBodies.contains(InID))
	{
		return &m_physicBodies[InID];
	}
	return nullptr;
}

void BBPhysicManager::MarkRemovePhysicalEntity(BBPhysicBodyID InID)
{
	if (m_currentlyResolvingPhysics)
	{
		m_physicBodies.mark_for_deletion(InID);
	}
	else
	{
		m_physicBodies.remove(InID);
	}
}

std::weak_ptr<BBSolver> BB::BBPhysicManager::AddPhysicSolver(std::shared_ptr<BBSolver>&& InSolver)
{
	m_physicalSolvers.push_back(std::move(InSolver));
	return m_physicalSolvers.back();
}

void BB::BBPhysicManager::RemovePhysicSolver(std::weak_ptr<BBSolver> InSolver)
{
	if (InSolver.expired())
	{
		RLog::Log(LOG_ERROR, "[RSRPhysicManager::RemovePhysicSolver] Solver pointer already expired !");
		return;
	}
	std::shared_ptr<BBSolver> sptrInSolver = InSolver.lock();
	if (sptrInSolver)
	{
		auto it = std::find(m_physicalSolvers.begin(), m_physicalSolvers.end(), sptrInSolver);
		RemovePhysicSolver(it);
	}
}

void BB::BBPhysicManager::ClearAllPhysicSolvers()
{
	while (m_physicalSolvers.size() > 0)
	{
		// Access the element using the index
		RemovePhysicSolver(m_physicalSolvers.end() - 1);
	}
}

void BB::BBPhysicManager::UpdatePhysic(const sf::Time& InGameTime, const sf::Time& InDeltaTime)
{
	m_currentlyResolvingPhysics = true;
	//Allow physical object to update their positons and states
	for (BBPhysicBody& physicalBody : m_physicBodies.Values)
	{
		if (std::shared_ptr<IBBPhysicBodyListener> entity = physicalBody.Listener.lock())
		{
			entity->OnPhysicalPrePass(/*InOut*/physicalBody, InGameTime, InDeltaTime);
		}
	}

	BBPhysicManager_Dynamics::ApplyGravityToForce(*this, InDeltaTime);


	//Inexact approch, but ensure collision takes precedence other overlaps
	// Issue is object may yet be pushed into other overlaps
	//If this ever cause issues, compute overlaps after physic solvers
	std::vector<std::pair<size_t, size_t>> overlapingObjects;
	std::vector<ColbodyIDXsAndColResult> collisionsObjects;
	BBPhysicManager_Collisions::ComputeCollisionsAndOverlaps(*this, InDeltaTime, /*Out*/collisionsObjects, /*Out*/overlapingObjects);


	for (size_t i = 0; i < m_physicalSolvers.size(); ++i)
	{
		if (m_physicalSolvers[i])
		{
			m_physicalSolvers[i]->Solve(*this, /*InOut*/collisionsObjects, InDeltaTime);
		}
	}
#if BB_DEBUG_PHYSIC
	DEBUG_lastCollisions[DEBUG_nextArrayToSet].clear();
	DEBUG_lastCollisions[DEBUG_nextArrayToSet].reserve(collisionsObjects.size());
	for (size_t i = 0; i < collisionsObjects.size(); ++i)
	{
		DEBUG_lastCollisions[DEBUG_nextArrayToSet].push_back(collisionsObjects[i]);
	}
	DEBUG_nextArrayToSet = (++DEBUG_nextArrayToSet % DEBUG_lastCollisions.size());
#endif

#if false && _DEBUG
	//Big spamming unoptimized log, to remove as soon as basic physic implemented 
	for (auto& colPr : collisionsObjects)
	{
		RSRLog::Log(LOG_DISPLAY, TEXT("Found collision with data '{}'"), mds::_tostring(colPr));
	}
#endif
	//std::string log = std::format("Found {} among {} entities !\n", overlapingObjects.size(), m_physicalBodies.size());
	//RSRLog::LogWarning(log);
	for (auto& OverlapPair : overlapingObjects)
	{
		const BBPhysicBody& firstBody = m_physicBodies.Values[OverlapPair.first];
		const BBPhysicBody& secondBody = m_physicBodies.Values[OverlapPair.second];
		std::shared_ptr<IBBPhysicBodyListener> firstEntity = firstBody.Listener.lock();
		std::shared_ptr<IBBPhysicBodyListener> secondEntity = secondBody.Listener.lock();
		if (firstEntity)
		{
			firstEntity->OnOverlapWith(firstBody, secondBody);
		}
		if (secondEntity)
		{
			secondEntity->OnOverlapWith(secondBody, firstBody);
		}
	}
	for (const auto& ColResult : collisionsObjects)
	{
		const BBPhysicBody& firstBody = m_physicBodies.Values[ColResult.FirstElementIndex];
		const BBPhysicBody& secondBody = m_physicBodies.Values[ColResult.SecondElementIndex];
		std::shared_ptr<IBBPhysicBodyListener> firstEntity = firstBody.Listener.lock();
		std::shared_ptr<IBBPhysicBodyListener> secondEntity = secondBody.Listener.lock();
		if (firstEntity)
		{
			firstEntity->OnCollideWith(firstBody, secondBody, ColResult);
		}
		if (secondEntity)
		{
			secondEntity->OnCollideWith(secondBody, firstBody, ColResult);
		}
	}


	BBPhysicManager_Dynamics::ApplyForcesToObjets(*this, InDeltaTime);

	for (BBPhysicBody& /*InOut*/physicalBody : m_physicBodies.Values)
	{
		sf::Transform lTrsMat = physicalBody.Transform;
		physicalBody.Colliders.RecomputeWorld(lTrsMat, true);

		std::shared_ptr<IBBPhysicBodyListener> entity = physicalBody.Listener.lock();
		if (!entity) { continue; }
		entity->OnPhysicPassEnded(/*InOut*/physicalBody, InGameTime, InDeltaTime);
	}

	m_currentlyResolvingPhysics = false;
	m_physicBodies.clear_marked_for_deletion();
}

