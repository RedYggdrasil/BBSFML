#pragma once
#include <SFML/Graphics.hpp>

namespace BB
{
	struct BBDynamics
	{
	public:
		sf::Vector2f Velocity;
		sf::Vector2f Force;
		sf::Vector2f ScaledGravityInfluence;

		float Bounciness;
		float InverseMass;
		bool bIsDynamic;
	public:
		static inline constexpr float InvertMass(const float InMass)
		{
#if (_DEBUG)
			float result = (InMass > std::numeric_limits<float>::epsilon()) ? (1.f / InMass) : 0.f;
			assert(result > std::numeric_limits<float>::epsilon() && "Error: invertMass cannot be inferior to espilon !");
			return result;
#else
			return (InMass > std::numeric_limits<float>::epsilon()) ? (1.f / InMass) : 0.f;
#endif
		}
		static inline constexpr float RevertInvMass(const float InInverseMass)
		{
			if (InInverseMass > std::numeric_limits<float>::epsilon()) { return 1.f / InInverseMass; }
			return std::numeric_limits<float>::infinity();
		}
		inline constexpr float ComputeMass() const { return BBDynamics::RevertInvMass(this->InverseMass); }
		inline constexpr bool InvMassFiniteNonZero() const
		{ return this->InverseMass > std::numeric_limits<float>::epsilon() && this->InverseMass < std::numeric_limits<float>::max(); }
		inline void SetMass(const float InMass) { this->InverseMass = BBDynamics::InvertMass(InMass); }
	};
	inline BBDynamics NoDynamics(float InMass = 0.f, const float InBounciness = 1.f)
	{
		return {
			.Velocity = sf::Vector2f(),
			.Force = sf::Vector2f(),
			.ScaledGravityInfluence = sf::Vector2f(0.f, 0.f),
			.Bounciness = InBounciness,
			.InverseMass	= BBDynamics::InvertMass(InMass),
			.bIsDynamic		= false
		};
	}
	inline BBDynamics Dynamics(float InMass = 0.f, const sf::Vector2f InScaledGravityInfluence = sf::Vector2f(1.f, 1.f), const float InBounciness = 1.f)
	{
		return {
			.Velocity = sf::Vector2f(),
			.Force = sf::Vector2f(),
			.ScaledGravityInfluence = InScaledGravityInfluence,
			.Bounciness = InBounciness,
			.InverseMass = BBDynamics::InvertMass(InMass),
			.bIsDynamic = true
		};
	}

}