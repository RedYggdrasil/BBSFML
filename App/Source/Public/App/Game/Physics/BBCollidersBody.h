#pragma once
#include "RSFML/Tools/REnum.h"
#include "App/Defines/pch.h"
#include "App/Game/Physics/BBColBehavior.h"
#include "App/Game/Physics/BBCollider.h"
#include "App/Game/Physics/BBCollisionType.h"
#include "App/Game/Physics/BBCollisionResult.h"

namespace BB
{
	class BBColliderBody
	{
	public:
		BBColBehavior Behavior;
		sf::FloatRect AABB;
		std::vector<BBCollider> Colliders;
	public:
		void ComputeAABB();
	public:
		static void ComputeWorldFromLocal(const sf::Transform& InOuterWorldMatrix, const BBColliderBody& InLocal, BBColliderBody& OutWorld);
		static void ComputeLocalFromWorld(const sf::Transform& InOuterWorldMatrix, const BBColliderBody& InWorld, BBColliderBody& OutLocal);

	public:
		bool Intersects(const BBColliderBody& InOther) const { return BBColliderBody::Intersects(*this, InOther); }
		static bool Intersects(const BBColliderBody& InLeft, const BBColliderBody& InRight);

		BBCollisionType FillCollidesOrOverlaps(const BBColliderBody& InOther, std::vector<ColIDXsAndColResult>& OutIDXCollisionResults, bool bFillOnlyDeepestCollision) const { return BBColliderBody::FillCollidesOrOverlaps(*this, InOther, OutIDXCollisionResults, bFillOnlyDeepestCollision); }
		static BBCollisionType FillCollidesOrOverlaps(const BBColliderBody& InLeft, const BBColliderBody& InRight, std::vector<ColIDXsAndColResult>& OutIDXCollisionResults, bool bFillOnlyDeepestCollision);

		static bool TestCollidersForOneOverlap(const BBColliderBody& InLeft, const BBColliderBody& InRight);
		static ColIDXsAndColResult TestCollidersForDeepestCollision(const BBColliderBody& InLeft, const BBColliderBody& InRight);
		static size_t TestCollidersForCollisions(const BBColliderBody& InLeft, const BBColliderBody& InRight, std::vector<ColIDXsAndColResult>& InOutColResults);
	public:
		BBColliderBody() = default;
		BBColliderBody(const BBColBehavior& InColBehavior);
		BBColliderBody(const BBColliderBody& InOther);
		BBColliderBody& operator=(const BBColliderBody& InOther);
	};

	struct BBColliderBodyLWPair
	{
	protected:
		alignas(16) BBColliderBody m_localPB;
		alignas(16) BBColliderBody m_worldPB;
		bool m_bLocalToWorldDirty = false;
	public:
		inline BBColliderBody& DirtyGetLocal() { DirtyLtoW(); return m_localPB; }
		inline const BBColliderBody& GetLocal() const { return m_localPB; }

		inline const BBColliderBody& GetWorld() const { return m_worldPB; }

		inline void DirtyLtoW() { m_bLocalToWorldDirty = true; }
		inline bool IsLtoWDirty() { return m_bLocalToWorldDirty; }

		void RecomputeWorld(const sf::Transform& InOuterWorldMatrix, bool bInForceDirty = false);
	public:
		BBColliderBodyLWPair() = default;
		BBColliderBodyLWPair(const BBColBehavior& InColBehavior);
		static BBColliderBodyLWPair FromLocalCollidersBody(const BBColliderBody& InLocalColliders, const sf::Transform& InOuterWorldMatrix);
	};
};