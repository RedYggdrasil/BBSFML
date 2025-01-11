#pragma once
#include <RSFML/Tools/RLog.h>
#include <SFML/Graphics.hpp>

namespace BB
{
	struct BBCollisionResult
	{
		static const BBCollisionResult NO_COLLISION;
	public:
		sf::Vector2f P1; //Point on left "1" shape, origin of normal
		sf::Vector2f P2; //Point on right "2" shape, target of normal
		sf::Vector2f Normal; // P2 - P1, normalized, so the normal from shape 1 point toward shape 2 point
		float Depth; // Length of P2 - P1
		bool bDidCollide; // Is the result a collision
	public:
		inline BBCollisionResult InvertPoints() const
		{
			BBCollisionResult result = *this;
			std::swap(result.P1, result.P2);
			result.Normal = -result.Normal;
			return result;
		}
		inline BBCollisionResult& InvertPointsSelf()
		{
			std::swap(this->P1, this->P2);
			this->Normal = -this->Normal;
			return *this;
		}

	};
	/// <summary>
	/// Generic structure for a collision between two elements reference by an index
	/// </summary>
	struct BBIDXsAndCollisionResult
	{
		size_t FirstElementIndex;
		size_t SecondElementIndex;
		BBCollisionResult CollisionResult;
	};

	/// <summary>
	/// a BBIDXsAndCollisionResult where IDXs represents BBCollider
	/// </summary>
	using ColIDXsAndColResult = BBIDXsAndCollisionResult;
	/// <summary>
	/// a BBIDXsAndCollisionResult where IDXs represent BBCollidersBody
	/// </summary>
	using ColbodyIDXsAndColResult = BBIDXsAndCollisionResult;
}
namespace RSfml
{
	inline std::string _tostring(const BB::BBCollisionResult& InCR)
	{
		return std::format("[didCol : {}, P1 : {}, P2 : {}, Norm : {}, Depth : {}]",
			InCR.bDidCollide, RSfml::_tostring(InCR.P1), RSfml::_tostring(InCR.P2), RSfml::_tostring(InCR.Normal), InCR.Depth);
	}
	inline std::string _tostring(const BB::BBIDXsAndCollisionResult& InIDXCR)
	{
		return std::format("{{ IDX1 : {}, IDX2 : {}, ColResult : {} }}",
			InIDXCR.FirstElementIndex, InIDXCR.SecondElementIndex, RSfml::_tostring(InIDXCR.CollisionResult));
	}
}