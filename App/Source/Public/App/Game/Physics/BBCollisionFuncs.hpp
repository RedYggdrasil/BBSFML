#pragma once

#include "RSFML/Defines/RSFML.h"
#include "RSFML/Tools/REnum.h"
#include "RSFML/Tools/RLog.h"
#include "RSFML/Tools/RMath.h"
#include "App/Game/Physics/BBCollider.h"
#include "App/Game/Physics/BBCollisionResult.h"

#include <functional>

using namespace BB;

namespace BBCols
{
	auto ReserveInit = [](const size_t InReserveSize) ->std::vector<sf::Vector2f> { std::vector<sf::Vector2f> vec; vec.reserve(InReserveSize); return vec; };

	static thread_local std::vector<sf::Vector2f> s_axes = ReserveInit(10);
	static thread_local std::vector<sf::Vector2f> s_verts1 = ReserveInit(15);
	static thread_local std::vector<sf::Vector2f> s_verts2 = ReserveInit(15);

	using FindContactFunc = std::function<BBCollisionResult(const BBCollider*, const BBCollider*)>;


	static BBCollisionResult SphereSphereCol(const BBCollider* InLeftSphere, const BBCollider* InRightSphere);
	static BBCollisionResult SatSatCol(const BBCollider* InLeftSAT, const BBCollider* InRightSAT);
	static BBCollisionResult SphereSatCol(const BBCollider* InLeftSphere, const BBCollider* InRightSAT);

	static const FindContactFunc contactFuncs[2][2] =
	{
		//				Sphere				OriBox             
		/*Sphere*/ { SphereSphereCol	, SphereSatCol	},
		/*OriBox*/ { nullptr			, SatSatCol	}
	};

	static BBCollisionResult ResolveCollision(const BBCollider* InLeftCollider, const BBCollider* InRightCollider)
	{
		bool bNeedSwapping = InLeftCollider->Type > InRightCollider->Type;

		if (bNeedSwapping)
		{
			std::swap(InLeftCollider, InRightCollider);
		}

		BBCollisionResult result = contactFuncs[RSfml::UT_cast(InLeftCollider->Type)][RSfml::UT_cast(InRightCollider->Type)](InLeftCollider, InRightCollider);
		if (bNeedSwapping)
		{
			result.InvertPointsSelf();
		}
		return result;
	}

	BBCollisionResult BBCols::SphereSphereCol(const BBCollider* InLeftSphere, const BBCollider* InRightSphere)
	{
		sf::Vector2f s1TowardS2 = (InRightSphere->SphereData.Position - InLeftSphere->SphereData.Position);
		float distSqured = s1TowardS2.lengthSquared();
		float radSum = InLeftSphere->SphereData.Radius + InRightSphere->SphereData.Radius;
		if (distSqured > (radSum * radSum))
		{
			return BBCollisionResult::NO_COLLISION;
		}
		if (s1TowardS2 == sf::Vector2f())
		{
			//We 2 sphere have the same center, default to normal up (down, then up later)
			//This will push left sphere up
			s1TowardS2 = sf::Vector2f(0.f, 1.f);
		}
		else
		{
			s1TowardS2 = s1TowardS2.normalized();
		}
		BBCollisionResult result;
		result.P1 = InLeftSphere->SphereData.Position + (s1TowardS2 * InLeftSphere->SphereData.Radius);
		result.P2 = InRightSphere->SphereData.Position + (s1TowardS2 * InRightSphere->SphereData.Radius * -1.f);

		result.Depth = (result.P2 - result.P1).length();
		result.Normal = -s1TowardS2;
		result.bDidCollide = true;

		return result;
	}
	BBCollisionResult BBCols::SatSatCol(const BBCollider* InLeftSAT, const BBCollider* InRightSAT)
	{
		auto FindMinMaxOnAxisAndMinMaxVertex = [](const sf::Vector2f& InAxis, const std::vector<sf::Vector2f>& InVertices, float& OutMin, float& OutMax, size_t& OutMinVertexProjectedID, size_t& OutMaxVertexProjectedID) -> void
			{
				OutMin = InAxis.dot(InVertices[0]);
				OutMinVertexProjectedID = OutMaxVertexProjectedID = 0;
				OutMax = OutMin;
				for (size_t i = 1; i < InVertices.size(); ++i)
				{
					const sf::Vector2f& v1 = InVertices[i];
					float projectedDot = InAxis.dot(v1);
					if (projectedDot < OutMin)
					{
						//Finding the vertex corresponding to the MVT 
						// https://youtu.be/Vgq9M8JOgwY?si=Mh_-FIqsdG-677HF&t=163
						OutMin = projectedDot;
						OutMinVertexProjectedID = i;
					}
					if (projectedDot > OutMax)
					{
						//Finding the vertex corresponding to the MVT 
						// https://youtu.be/Vgq9M8JOgwY?si=Mh_-FIqsdG-677HF&t=163
						OutMax = projectedDot;
						OutMaxVertexProjectedID = i;
					}
				}
			};

		BBCollisionResult result = BBCollisionResult::NO_COLLISION;

		s_axes.clear();
		s_verts1.clear();
		s_verts2.clear();

		InLeftSAT->FillSATAxesAndVertices(/*InOut*/ s_axes, /*InOut*/ s_verts1);
		size_t shape1AxesCount = s_axes.size();
		InRightSAT->FillSATAxesAndVertices(/*InOut*/ s_axes, /*InOut*/ s_verts2);

		if (s_verts1.size() < 1 || s_verts2.size() < 1)
		{
			return BBCollisionResult::NO_COLLISION;
		}
		//Searching for the Minimum Translation Vector (MTV)
		size_t mtvAxesIndex = std::numeric_limits<size_t>::max();
		float mtvAxesLength = std::numeric_limits<float>::infinity();


		constexpr size_t OBJECT_1_INDEX = 0;
		constexpr size_t OBJECT_2_INDEX = 1;

		//Object witch hold the vertex that has "collided" with an edge of the other object
		//Thinking than it is awayus a vertex to edge collision is a simplification we are doing.
		size_t vertexObject = std::numeric_limits<size_t>::max();
		sf::Vector2f contactVertexValue = sf::Vector2f();


		bool bDidInverse = false;
		const size_t axesCount = s_axes.size();
		for (size_t i = 0; i < axesCount; ++i)
		{
			const sf::Vector2f& currentAxisReadonly = s_axes[i];

			float p1Min;
			float p1Max;
			size_t v1minVertexProjectedID;
			size_t v1maxVertexProjectedID;
			FindMinMaxOnAxisAndMinMaxVertex(currentAxisReadonly, s_verts1, /*Out*/p1Min, /*Out*/p1Max, /*Out*/v1minVertexProjectedID, /*Out*/v1maxVertexProjectedID);

			float p2Min;
			float p2Max;
			size_t v2minVertexProjectedID;
			size_t v2maxVertexProjectedID;
			FindMinMaxOnAxisAndMinMaxVertex(currentAxisReadonly, s_verts2, /*Out*/p2Min, /*Out*/p2Max, /*Out*/v2minVertexProjectedID, /*Out*/v2maxVertexProjectedID);

			float overlapMin = std::max(p1Min, p2Min);
			float overlapMax = std::min(p1Max, p2Max);


			if (overlapMax < overlapMin)
			{	//There is at least 1 separating axis
				return BBCollisionResult::NO_COLLISION;
			}
			else
			{
				sf::Vector2f currentAxis = currentAxisReadonly;
				float newOverlap = overlapMax - overlapMin;

				if (newOverlap < mtvAxesLength)
				{
					//before advenced checking and computation, newoverlap is a potential mtv
					//We fill MVT search data

					bool bEdgeIsShape1 = i < shape1AxesCount;
					//Since Edge of shape "bEdgeIsShape1" collided with something, we assume that edge collided with a vertex of the other shape (simplification/assuption, this could be edge-edge)
					vertexObject = bEdgeIsShape1 ? OBJECT_2_INDEX : OBJECT_1_INDEX;
					std::vector<sf::Vector2f>* verticeObjectVertices = bEdgeIsShape1 ? &s_verts2 : &s_verts1;
					size_t verticeObjectMin = bEdgeIsShape1 ? v2minVertexProjectedID : v1minVertexProjectedID;
					size_t verticeObjectMax = bEdgeIsShape1 ? v2maxVertexProjectedID : v1maxVertexProjectedID;

					//First we search for a containement sitation (on projection is inside another)
					if
						(
							(p1Max < p2Max && p1Min > p2Min)
							||
							(p2Max < p1Max && p2Min > p1Min)
						)
					{

						float minsLength = std::abs(p1Min - p2Min);
						float maxsLength = std::abs(p1Max - p2Max);
						if (minsLength < maxsLength)
						{
							newOverlap += minsLength;
						}
						else
						{
							newOverlap += maxsLength;
							std::swap(verticeObjectMin, verticeObjectMax);
							currentAxis *= -1.f;
						}

					}
					if (newOverlap < mtvAxesLength)
					{
						//The new overlap is a mtv candidate even after potential containment
						mtvAxesIndex = i;
						result.Depth = mtvAxesLength = newOverlap;

						//Shape Vertex is colliding inside an edge of shape Edge, we select the normal from edge of shape Edge toward shape Vertex
						//For Axis of shape Edge we select the normal pointing from shape Edge to shape Vertex
						if
							(
								(bEdgeIsShape1 && (p1Max > p2Max))
								||
								(!bEdgeIsShape1 && (p1Max < p2Max))
							)
						{
							result.Normal = currentAxis * -1.f;
							contactVertexValue = verticeObjectVertices->operator[](verticeObjectMax);
							bDidInverse = true;
						}
						else
						{
							result.Normal = currentAxis;
							contactVertexValue = verticeObjectVertices->operator[](verticeObjectMin);
							bDidInverse = false;
						}
					}
				}
			}
		}
		//sf::Vector2f selectedAxisValue = s_axes[mtvAxesIndex];
		//
		//constexpr size_t logEach = 10;
		//static size_t nextlog = 0;
		//if (nextlog >= logEach)
		//{
		//	RLog::Log
		//	(
		//		LOG_DISPLAY, "Sp '{}' | vert '{}' | axis '{}' | inv '{}' ",
		//		vertexObject, RSfml::_tostring(contactVertexValue), RSfml::_tostring(selectedAxisValue), bDidInverse
		//	);
		//	nextlog = 0;
		//}
		//else
		//{
		//	++nextlog;
		//}

		//There no separating axis
		result.bDidCollide = true;
		result.P1 = contactVertexValue;
		result.P2 = contactVertexValue + (result.Normal * result.Depth);
		if (vertexObject == OBJECT_2_INDEX)
		{	//Vertice object is object 2 or right
			std::swap(result.P1, result.P2);
			result.Normal *= -1.f;
		}
		return result;
	}
	

	//We have a bug where shere inside cube cause normal and point flip, should not be hard to debug
	BBCollisionResult BBCols::SphereSatCol(const BBCollider* InLeftSphere, const BBCollider* InRightSAT)
	{
		auto FindClosestPointToSphereCenter = [](const sf::Vector2f& InSphereCenter, const std::vector<sf::Vector2f>& InVertices, size_t& OutClosestPointIndex, float& OutSquaredDistance) -> void
			{
				OutClosestPointIndex = 0;
				OutSquaredDistance = (InVertices[0] - InSphereCenter).lengthSquared();
				for (size_t i = 1; i < InVertices.size(); ++i)
				{
					const sf::Vector2f& v1 = InVertices[i];
					float loopSquaredDistance = (v1 - InSphereCenter).lengthSquared();
					if (loopSquaredDistance < OutSquaredDistance)
					{
						OutSquaredDistance = loopSquaredDistance;
						OutClosestPointIndex = i;
					}
				}
			};

		auto FindMinMaxOnAxisAndMinMaxVertex = [](const sf::Vector2f& InAxis, const std::vector<sf::Vector2f>& InVertices, float& OutMin, float& OutMax, size_t& OutMinVertexProjectedID, size_t& OutMaxVertexProjectedID) -> void
			{
				OutMin = InAxis.dot(InVertices[0]);
				OutMinVertexProjectedID = OutMaxVertexProjectedID = 0;
				OutMax = OutMin;
				for (size_t i = 1; i < InVertices.size(); ++i)
				{
					const sf::Vector2f& v1 = InVertices[i];
					float projectedDot = InAxis.dot(v1);
					//Finding the vertex corresponding to the MVT 
					// https://youtu.be/Vgq9M8JOgwY?si=Mh_-FIqsdG-677HF&t=163
					if (projectedDot < OutMin)
					{
						OutMin = projectedDot;
						OutMinVertexProjectedID = i;
					}
					if (projectedDot > OutMax)
					{
						OutMax = projectedDot;
						OutMaxVertexProjectedID = i;
					}
				}
			};

		BBCollisionResult result = BBCollisionResult::NO_COLLISION;

		s_axes.clear();
		s_verts2.clear();

		InRightSAT->FillSATAxesAndVertices(/*InOut*/ s_axes, /*InOut*/ s_verts2);
		size_t shapeSATAxesCount = s_axes.size();

		if (s_verts2.size() < 1)
		{
			return BBCollisionResult::NO_COLLISION;
		}

		const sf::Vector2f& sphereCenter = InLeftSphere->SphereData.Position;
		float sphereRadius = InLeftSphere->SphereData.Radius;

		size_t closestPointIndex;
		float squaredClosestDistance;
		FindClosestPointToSphereCenter(sphereCenter, s_verts2, /*Out*/closestPointIndex, /*Out*/squaredClosestDistance);

		sf::Vector2f sphereAxis = (s_verts2[closestPointIndex] - sphereCenter);
		if (sphereAxis.lengthSquared() <= std::numeric_limits<float>::epsilon())
		{
			sphereAxis = sf::Vector2f(sf::Vector2f(0.f, -1.f)); //Default up
		}
		else
		{
			sphereAxis = sphereAxis.normalized();
		}

		s_axes.reserve(s_axes.size() + 1);
		s_axes.push_back(sphereAxis);

		//Searching for the Minimum Translation Vector (MTV)
		size_t mtvAxesIndex = std::numeric_limits<size_t>::max();
		float mtvAxesLength = std::numeric_limits<float>::infinity();


		constexpr size_t OBJECT_1_INDEX = 0;
		constexpr size_t OBJECT_2_INDEX = 1;

		//Object witch hold the vertex that has "collided" with an edge of the other object
		//Thinking than it is always a vertex to edge collision is a simplification we are doing.
		size_t vertexObject = std::numeric_limits<size_t>::max();
		sf::Vector2f contactVertexValue = sf::Vector2f();


		bool bDidInverse = false;
		const size_t axesCount = s_axes.size();
		for (size_t i = 0; i < axesCount; ++i)
		{
			const sf::Vector2f& currentAxisReadonly = s_axes[i];


			float projectedDot = currentAxisReadonly.dot(sphereCenter);

			float p1Min = projectedDot - sphereRadius;
			float p1Max = projectedDot + sphereRadius;


			float p2Min;
			float p2Max;
			size_t v2minVertexProjectedID;
			size_t v2maxVertexProjectedID;
			FindMinMaxOnAxisAndMinMaxVertex(currentAxisReadonly, s_verts2, /*Out*/p2Min, /*Out*/p2Max, /*Out*/v2minVertexProjectedID, /*Out*/v2maxVertexProjectedID);

			float overlapMin = std::max(p1Min, p2Min);
			float overlapMax = std::min(p1Max, p2Max);


			if (overlapMax < overlapMin)
			{	//There is at least 1 separating axis
				return BBCollisionResult::NO_COLLISION;
			}
			else
			{
				sf::Vector2f currentAxis = currentAxisReadonly;
				float newOverlap = overlapMax - overlapMin;

				if (newOverlap < mtvAxesLength)
				{
					sf::Vector2f sphereVertices[2] = { (sphereCenter - (currentAxisReadonly * sphereRadius)), (sphereCenter + (currentAxisReadonly * sphereRadius)) };
					size_t sphereMinVertexProjectedID = 0;
					size_t sphereMaxVertexProjectedID = 1;
					//before advenced checking and computation, newoverlap is a potential mtv
					//We fill MVT search data

					//sphere as only 1 axis, so should be true when i == to shapeSATAxesCount
					bool bEdgeIsLeftSphere = (i >= shapeSATAxesCount);

					//Since Edge of shape "bEdgeIsShape1" collided with something, we assume that edge collided with a vertex of the other shape (simplification/assuption, this could be edge-edge)
					vertexObject = bEdgeIsLeftSphere ? OBJECT_2_INDEX : OBJECT_1_INDEX;

					sf::Vector2f* verticeObjectVertices = bEdgeIsLeftSphere ? s_verts2.data() : sphereVertices;
					size_t verticeObjectMin = bEdgeIsLeftSphere ? v2minVertexProjectedID : sphereMinVertexProjectedID;
					size_t verticeObjectMax = bEdgeIsLeftSphere ? v2maxVertexProjectedID : sphereMaxVertexProjectedID;

					//First we search for a containement sitation (on projection is inside another)
					if
						(
							(p1Max < p2Max && p1Min > p2Min)
							||
							(p2Max < p1Max && p2Min > p1Min)
							)
					{

						float minsLength = std::abs(p1Min - p2Min);
						float maxsLength = std::abs(p1Max - p2Max);
						if (minsLength < maxsLength)
						{
							newOverlap += minsLength;
						}
						else
						{
							newOverlap += maxsLength;
							std::swap(verticeObjectMin, verticeObjectMax);
							currentAxis *= -1.f;
						}

					}
					if (newOverlap < mtvAxesLength)
					{
						//The new overlap is a mtv candidate even after potential containment
						mtvAxesIndex = i;
						result.Depth = mtvAxesLength = newOverlap;

						//Shape Vertex is colliding inside an edge of shape Edge, we select the normal from edge of shape Edge toward shape Vertex
						//For Axis of shape Edge we select the normal pointing from shape Edge to shape Vertex
						if
							(
								(bEdgeIsLeftSphere && (p1Max > p2Max))
								||
								(!bEdgeIsLeftSphere && (p1Max < p2Max))
								)
						{
							result.Normal = currentAxis * -1.f;
							contactVertexValue = verticeObjectVertices[verticeObjectMax];
							bDidInverse = true;
						}
						else
						{
							result.Normal = currentAxis;
							contactVertexValue = verticeObjectVertices[verticeObjectMin];
							bDidInverse = false;
						}
					}
				}
			}
		}
		//sf::Vector2f selectedAxisValue = s_axes[mtvAxesIndex];
		//
		//constexpr size_t logEach = 10;
		//static size_t nextlog = 0;
		//if (nextlog >= logEach)
		//{
		//	RLog::Log
		//	(
		//		LOG_DISPLAY, "Sp '{}' | vert '{}' | axis '{}' | inv '{}' ",
		//		vertexObject, RSfml::_tostring(contactVertexValue), RSfml::_tostring(selectedAxisValue), bDidInverse
		//	);
		//	nextlog = 0;
		//}
		//else
		//{
		//	++nextlog;
		//}

		//There no separating axis
		result.bDidCollide = true;
		result.P1 = contactVertexValue;
		result.P2 = contactVertexValue + (result.Normal * result.Depth);
		if (vertexObject == OBJECT_2_INDEX)
		{	//Vertice object is object 2 or right
			std::swap(result.P1, result.P2);
			result.Normal *= -1.f;
		}
		return result;
	}
};