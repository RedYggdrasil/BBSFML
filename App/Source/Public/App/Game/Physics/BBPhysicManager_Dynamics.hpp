#pragma once
#include "RSFML/Tools/REnum.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/RLog.h"
#include "App/Defines/pch.h"
#include "App/Game/Physics/BBPhysicManager.h"

#include <algorithm>
#include <format>

namespace BB
{
	class BBPhysicManager_Dynamics
	{
	public:
		friend BBPhysicManager;
		STATIC_CLASS_R5(BBPhysicManager_Dynamics);
	private:

		static void ComputeDynamics(BBPhysicManager& pm, const sf::Time& InDeltaTime)
		{
			float fInDeltaTime = InDeltaTime.asSeconds();
			if (fInDeltaTime < std::numeric_limits<float>::epsilon())
			{
				return;
			}
			sf::Vector2f lGobalGravity = sf::Vector2f(0.f, 0.f);// pm.m_physicContext.GlobalGravity;

			size_t count = pm.m_physicBodies.Values.size();

			for (size_t i = 0; i < count; ++i)
			{
				BBPhysicBody& physicalBody = pm.m_physicBodies.Values[i];
				if (physicalBody.Dynamics.bIsDynamic)
				{
					BBDynamics& dynamics = physicalBody.Dynamics;
					sf::Vector2f lForce = dynamics.Force;
					sf::Vector2f lVelocity = dynamics.Velocity;
					sf::Vector2f lPosition = RSfml::GetTransformPosition(physicalBody.Transform);

					lForce += (lGobalGravity * dynamics.ComputeMass());
					lVelocity += (lForce * (dynamics.InverseMass * fInDeltaTime));
					lPosition += (lVelocity * fInDeltaTime);

					dynamics.Force = sf::Vector2f { 0.f, 0.f };
					dynamics.Velocity = lVelocity;
					RSfml::SetTransformPosition(/*InOut*/physicalBody.Transform, lPosition);

				}
			}
		}

		static void ApplyGravityToForce(BBPhysicManager& pm, const sf::Time& InDeltaTime)
		{
			float fInDeltaTime = InDeltaTime.asSeconds();
			if (fInDeltaTime < std::numeric_limits<float>::epsilon())
			{
				return;
			}
			sf::Vector2f lGobalGravity = pm.m_physicContext.GlobalGravity;

			size_t count = pm.m_physicBodies.Values.size();

			for (size_t i = 0; i < count; ++i)
			{
				BBPhysicBody& physicalBody = pm.m_physicBodies.Values[i];
				if (physicalBody.Dynamics.bIsDynamic)
				{
					BBDynamics& dynamics = physicalBody.Dynamics;
					sf::Vector2f lForce = dynamics.Force;
					
					lForce += (RSfml::mul(lGobalGravity, dynamics.ScaledGravityInfluence) * 0.f * dynamics.ComputeMass());
					dynamics.Force = lForce;

				}
			}
		}

		/// <summary>
		/// Apply all forces to velocities and positions, reset forces for this frame
		/// </summary>
		/// <param name="pm"></param>
		/// <param name="InDeltaTime"></param>
		static void ApplyForcesToObjets(BBPhysicManager& pm, const sf::Time& InDeltaTime)
		{
			float fInDeltaTime = InDeltaTime.asSeconds();
			if (fInDeltaTime < std::numeric_limits<float>::epsilon())
			{
				return;
			}

			size_t count = pm.m_physicBodies.Values.size();

			for (size_t i = 0; i < count; ++i)
			{
				BBPhysicBody& physicalBody = pm.m_physicBodies.Values[i];
				if (physicalBody.Dynamics.bIsDynamic)
				{
					BBDynamics& dynamics = physicalBody.Dynamics;
					sf::Vector2f lForce = dynamics.Force;
					sf::Vector2f lVelocity = dynamics.Velocity;
					sf::Vector2f lPosition = RSfml::GetTransformPosition(physicalBody.Transform);

					lVelocity += (lForce * (dynamics.InverseMass * fInDeltaTime));
					lPosition += (lVelocity * fInDeltaTime);

					dynamics.Force = sf::Vector2f{ 0.f, 0.f };
					dynamics.Velocity = lVelocity;
					RSfml::SetTransformPosition(/*InOut*/physicalBody.Transform, lPosition);

				}
			}
		}
	};
};