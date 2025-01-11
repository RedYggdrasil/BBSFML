#pragma once
#include "RSFML/Defines/RSFML.h"
#include "RSFML/Tools/REnum.h"
#include "RSFML/Tools/RMath.h"
#include "App/Game/Physics/BBCollider.h"
#include <functional>

using namespace BB;
namespace BBOvrlps
{
	auto ReserveInit = [](const size_t InReserveSize) ->std::vector<sf::Vector2f> { std::vector<sf::Vector2f> vec; vec.reserve(InReserveSize); return vec; };

	static thread_local std::vector<sf::Vector2f> s_axes = ReserveInit(10);
	static thread_local std::vector<sf::Vector2f> s_verts1 = ReserveInit(15);
	static thread_local std::vector<sf::Vector2f> s_verts2 = ReserveInit(15);

	using FindOverlapFunc = std::function<bool(const BBCollider*, const BBCollider*)>;

	static bool SphereSphereOvrlp(const BBCollider* InLeftSphere, const BBCollider* InRightSphere);

	static bool SatSatOvrlp(const BBCollider* InLeftSAT, const BBCollider* InRightSAT);
	static bool SphereSatOvrlp(const BBCollider* InLeftSphere, const BBCollider* InRightSAT);
	static const FindOverlapFunc overlapFuncs[2][2] =
	{
		//					Sphere             OriBox
		/*Sphere*/ { SphereSphereOvrlp	, SphereSatOvrlp },
		/*OriBox*/ { nullptr			, SatSatOvrlp	 }
	};
	static bool ResolveOverlap(const BBCollider* InLeftCollider, const BBCollider* InRightCollider)
	{
		bool bNeedSwapping = InLeftCollider->Type > InRightCollider->Type;

		if (bNeedSwapping)
		{
			std::swap(InLeftCollider, InRightCollider);
		}
		return overlapFuncs[RSfml::UT_cast(InLeftCollider->Type)][RSfml::UT_cast(InRightCollider->Type)](InLeftCollider, InRightCollider);
	}


	bool SphereSphereOvrlp(const BBCollider* InLeftSphere, const BBCollider* InRightSphere)
	{
		float distSqured = (InRightSphere->SphereData.Position - InLeftSphere->SphereData.Position).lengthSquared();
		float radSum = InLeftSphere->SphereData.Radius + InRightSphere->SphereData.Radius;
		return distSqured <= (radSum * radSum);
	}

	bool BBOvrlps::SatSatOvrlp(const BBCollider* InLeftSAT, const BBCollider* InRightSAT)
	{
		
		auto FindMinMaxOnAxis = [](const sf::Vector2f& InAxis,  const std::vector<sf::Vector2f>& InVertices, float& OutMin, float& OutMax) -> void 
			{
				OutMin = InAxis.dot(InVertices[0]);
				OutMax = OutMin;
				for (auto it = InVertices.begin() + 1; it != InVertices.end(); ++it)
				{
					const sf::Vector2f& v1 = *it;
					float projectedDot = InAxis.dot(v1);
					OutMin = std::min(OutMin, projectedDot);
					OutMax = std::max(OutMax, projectedDot);
				}
			};

		s_axes.clear();
		s_verts1.clear();
		s_verts2.clear();


		InLeftSAT->FillSATAxesAndVertices(/*InOut*/ s_axes, /*InOut*/ s_verts1);
		InRightSAT->FillSATAxesAndVertices(/*InOut*/ s_axes, /*InOut*/ s_verts2);

		if (s_verts1.size() < 1 || s_verts2.size() < 1)
		{
			return false;
		}

		for (const sf::Vector2f& axis : s_axes)
		{
			float p1Min; 
			float p1Max; 
			FindMinMaxOnAxis(axis, s_verts1, p1Min, p1Max);
			float p2Min;
			float p2Max;
			FindMinMaxOnAxis(axis, s_verts2, p2Min, p2Max);

			if ((p1Min - p2Max > 0) || (p2Min - p1Max > 0))
			{	//There is at least 1 separating axis
				return false;
			}
		}
		//There no separating axis
		return true;
	}


	bool BBOvrlps::SphereSatOvrlp(const BBCollider* InLeftSphere, const BBCollider* InRightSAT)
	{

		auto ReserveInit = [](const size_t InReserveSize) ->std::vector<sf::Vector2f> { std::vector<sf::Vector2f> vec; vec.reserve(InReserveSize); return vec; };

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
		auto FindMinMaxOnAxis = [](const sf::Vector2f& InAxis, const std::vector<sf::Vector2f>& InVertices, float& OutMin, float& OutMax) -> void
			{
				OutMin = InAxis.dot(InVertices[0]);
				OutMax = OutMin;
				for (auto it = InVertices.begin() + 1; it != InVertices.end(); ++it)
				{
					const sf::Vector2f& v1 = *it;
					float projectedDot = InAxis.dot(v1);
					OutMin = std::min(OutMin, projectedDot);
					OutMax = std::max(OutMax, projectedDot);
				}
			};


		s_axes.clear();
		s_verts2.clear();


		InRightSAT->FillSATAxesAndVertices(/*InOut*/ s_axes, /*InOut*/ s_verts2);

		if (s_verts2.size() < 1)
		{
			return false;
		}

		size_t closestPointIndex;
		float squaredClosestDistance;

		const sf::Vector2f& sphereCenter = InLeftSphere->SphereData.Position;
		float sphereRadius = InLeftSphere->SphereData.Radius;

		FindClosestPointToSphereCenter(sphereCenter, s_verts2, /*Out*/closestPointIndex, /*Out*/squaredClosestDistance);
		//We use directly the vector, not it's .perpendicular(), source : https://youtu.be/Vgq9M8JOgwY?si=8Mfd8BBOJmnjVTOu&t=450
		sf::Vector2f sphereAxis = (s_verts2[closestPointIndex] - sphereCenter).normalized();
		
		s_axes.push_back(sphereAxis);
		for (const sf::Vector2f& axis : s_axes)
		{

			float projectedDot = axis.dot(sphereCenter);

			float p1Min = projectedDot - sphereRadius;
			float p1Max = projectedDot + sphereRadius;
			float p2Min;
			float p2Max;
			FindMinMaxOnAxis(axis, s_verts2, p2Min, p2Max);

			if ((p1Min - p2Max > 0) || (p2Min - p1Max > 0))
			{	//There is at least 1 separating axis
				return false;
			}
		}
		return true;
	}
}