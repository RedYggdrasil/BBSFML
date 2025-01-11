#pragma once
#include "App/Game/Physics/BBCollidersBody.h"
#include "App/Game/Physics/BBDynamics.h"
#include "App/Game/Physics/BBPhysicBodyID.h"
#include "App/Game/Physics/IBBPhysicBodyListener.h"

namespace BB
{
	struct BBPhysicBody
	{
	private:
		static BBPhysicBodyID s_nextIndex;
	public:
		BBPhysicBodyID m_INIT_ONLYphysicBodyID;
		sf::Transform Transform;
		BBColliderBodyLWPair Colliders;
		BBDynamics Dynamics;
		std::weak_ptr<IBBPhysicBodyListener> Listener;
	public:
		inline const BBColBehavior& GetBehavior(bool InbGetWorldBehavior = true) const
		{
			if (InbGetWorldBehavior)
			{
				return Colliders.GetWorld().Behavior;
			} else {
				return Colliders.GetLocal().Behavior;
			}
		}
		BBPhysicBodyID Id() const { return m_INIT_ONLYphysicBodyID; }
		static BBPhysicBodyID GetUseNextID()
		{
			BBPhysicBodyID usedID = s_nextIndex; ++s_nextIndex;
			return usedID;
		}
		static BBPhysicBodyID LookOnlyNextID() { return s_nextIndex; }

	public:
		inline static BBPhysicBody CreatePhysicBodyWithID(sf::Transform&& InTransform, BBColliderBodyLWPair&& InColliders, BBDynamics&& InDynamics, std::weak_ptr<IBBPhysicBodyListener> InListener)
		{
			return BBPhysicBody
			{
				.m_INIT_ONLYphysicBodyID = BBPhysicBody::GetUseNextID(),
				.Transform = std::move(InTransform),
				.Colliders = std::move(InColliders),
				.Dynamics = std::move(InDynamics),
				.Listener = InListener
			};
		}

		inline static BBPhysicBody CreatePhysicBodyWithID(sf::Transform&& InTransform, BBColliderBodyLWPair&& InColliders, BBDynamics&& InDynamics)
		{
			return BBPhysicBody
			{
				.m_INIT_ONLYphysicBodyID = BBPhysicBody::GetUseNextID(),
				.Transform = std::move(InTransform),
				.Colliders = std::move(InColliders),
				.Dynamics = std::move(InDynamics),
				.Listener = std::weak_ptr<IBBPhysicBodyListener>()
			};
		}
	};
};