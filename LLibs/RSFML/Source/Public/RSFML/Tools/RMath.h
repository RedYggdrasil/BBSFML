#pragma once
#include <SFML/Graphics.hpp>

namespace RSfml
{
	namespace tmplts
	{
		template <typename TVector2>
		TVector2 mul2(const TVector2& InLeft, const TVector2& InRight)
		{
			return TVector2{ InLeft.x * InRight.x, InLeft.y * InRight.y };
		}
		template <typename TVector2>
		TVector2 div2(const TVector2& InLeft, const TVector2& InRight)
		{
			return TVector2{ InLeft.x / InRight.x, InLeft.y / InRight.y };
		}
		template <typename TVector2>
		TVector2 add2(const TVector2& InLeft, const TVector2& InRight)
		{
			return TVector2{ InLeft.x + InRight.x, InLeft.y + InRight.y };
		}
		template <typename TVector2>
		TVector2 sub2(const TVector2& InLeft, const TVector2& InRight)
		{
			return TVector2{ InLeft.x - InRight.x, InLeft.y - InRight.y };
		}

		template <typename TVector3>
		TVector3 mul3(const TVector3& InLeft, const TVector3& InRight)
		{
			return TVector3{ InLeft.x * InRight.x, InLeft.y * InRight.y, InLeft.z * InRight.z };
		}
		template <typename TVector3>
		TVector3 div3(const TVector3& InLeft, const TVector3& InRight)
		{
			return TVector3{ InLeft.x / InRight.x, InLeft.y / InRight.y, InLeft.z / InRight.z };
		}
		template <typename TVector3>
		TVector3 add3(const TVector3& InLeft, const TVector3& InRight)
		{
			return TVector3{ InLeft.x + InRight.x, InLeft.y + InRight.y, InLeft.z + InRight.z };
		}
		template <typename TVector3>
		TVector3 sub3(const TVector3& InLeft, const TVector3& InRight)
		{
			return TVector3{ InLeft.x - InRight.x, InLeft.y - InRight.y, InLeft.z - InRight.z };
		}

		// Helper struct to differentiate between 2D and 3D vectors 
		template<typename T> struct is_vector2 : std::false_type {};
		template<typename T> struct is_vector3 : std::false_type {};
		template<> struct is_vector2<sf::Vector2f> : std::true_type {};
		template<> struct is_vector2<sf::Vector2i> : std::true_type {};
		template<> struct is_vector2<sf::Vector2u> : std::true_type {};
		template<> struct is_vector3<sf::Vector3f> : std::true_type {};
		template<> struct is_vector3<sf::Vector3i> : std::true_type {};
	}

	template <typename TVector>
	TVector mul(const TVector& InLeft, const TVector& InRight)
	{
		if constexpr (tmplts::is_vector2<TVector>::value)
		{
			return tmplts::mul2(InLeft, InRight);
		}
		else if constexpr (tmplts::is_vector3<TVector>::value)
		{
			return tmplts::mul3(InLeft, InRight);
		}
	}

	template <typename TVector>
	TVector div(const TVector& InLeft, const TVector& InRight)
	{
		if constexpr (tmplts::is_vector2<TVector>::value)
		{
			return tmplts::div2(InLeft, InRight);
		}
		else if constexpr (tmplts::is_vector3<TVector>::value)
		{
			return tmplts::div3(InLeft, InRight);
		}
	}

	template <typename TVector>
	TVector add(const TVector& InLeft, const TVector& InRight)
	{
		if constexpr (tmplts::is_vector2<TVector>::value)
		{
			return tmplts::add2(InLeft, InRight);
		}
		else if constexpr (tmplts::is_vector3<TVector>::value)
		{
			return tmplts::add3(InLeft, InRight);
		}
	}
	template <typename TVector>
	TVector sub(const TVector& InLeft, const TVector& InRight)
	{
		if constexpr (tmplts::is_vector2<TVector>::value)
		{
			return tmplts::sub2(InLeft, InRight);
		}
		else if constexpr (tmplts::is_vector3<TVector>::value)
		{
			return tmplts::sub3(InLeft, InRight);
		}
	}

	template <typename T> 
	sf::Vector2<T> RectTopLeft(const sf::Rect<T>& rect) 
	{ 
		return rect.position;
	}
	template <typename T>
	sf::Vector2<T> RectTopRight(const sf::Rect<T>& rect)
	{
		return sf::Vector2<T>(rect.position.x + rect.size.x, rect.position.y);
	}
	template <typename T>
	sf::Vector2<T> RectBottomLeft(const sf::Rect<T>& rect)
	{
		return sf::Vector2<T>(rect.position.x, rect.position.y + rect.size.y);
	}
	template <typename T>
	sf::Vector2<T> RectBottomRight(const sf::Rect<T>& rect)
	{
		return rect.position + rect.size;
	}

	template <typename T>
	sf::Vector2<T> BoxTopLeft(const sf::Vector2<T>& InCenter, const sf::Vector2<T>& InExtends)
	{
		return InCenter - InExtends;
	}

	template <typename T>
	sf::Vector2<T> BoxTopRight(const sf::Vector2<T>& InCenter, const sf::Vector2<T>& InExtends)
	{
		return sf::Vector2f(InCenter.x + InExtends.x, InCenter.y - InExtends.y);
	}

	template <typename T>
	sf::Vector2<T> BoxBottomLeft(const sf::Vector2<T>& InCenter, const sf::Vector2<T>& InExtends)
	{
		return sf::Vector2f(InCenter.x - InExtends.x, InCenter.y + InExtends.y);
	}

	template <typename T>
	sf::Vector2<T> BoxBottomRight(const sf::Vector2<T>& InCenter, const sf::Vector2<T>& InExtends)
	{
		return InCenter + InExtends;
	}

	template <typename T>
	sf::Rect<T> RectFromTopLeftBottomRight(const sf::Vector2<T>& InTopLeft, const sf::Vector2<T>& InBottomRight)
	{
		return sf::Rect<T>(InTopLeft, InBottomRight - InTopLeft);
	}

	// Function to find the intersection point of two infinite lines
	bool LineLineIntersect(const sf::Vector2f InLine1A, const sf::Vector2f InLine1B, const sf::Vector2f InLine2A, const sf::Vector2f InLine2B, sf::Vector2f& OutResult);

	void ExtendAABB(sf::FloatRect& InOutAABB, const sf::Vector2f& InNewPoint);
	void ExtendAABB(sf::FloatRect& InOutAABB, const sf::Vector2f* InNewPoints, const size_t InPointsSize);
	void ExtendAABB(sf::FloatRect& InOutAABB, const sf::FloatRect& InOtherAABB);
	/// <summary>
	/// return the vector from an angle, normalized
	/// </summary>
	/// <param name="InAngle">The angle to test</param>
	/// <returns>the result (normalized - unit verctor)</returns>
	inline sf::Vector2f AngleToVector(const sf::Angle& InAngle)
	{
		float rad = InAngle.asRadians();
		return sf::Vector2f(std::cos(rad), std::sin(rad));
	}
	/// <summary>
	/// return the vector from an angle in radian, normalized
	/// </summary>
	/// <param name="InRandiantAngle">The angle in radian to test</param>
	/// <returns>the result (normalized - unit verctor)</returns>
	inline sf::Vector2f AngleRadToVector(float InRandianAngle)
	{
		return sf::Vector2f(std::cos(InRandianAngle), std::sin(InRandianAngle));
	}
	inline sf::Vector2f GetTransformPosition(const sf::Transform& InTransform)
	{
		const float* matrix = InTransform.getMatrix();
		float x = matrix[12];  // m02 in the matrix (third column, first row) 
		float y = matrix[13];  // m12 in the matrix (third column, second row) 
		return sf::Vector2f(x, y);
	}
	inline void SetTransformPosition(sf::Transform& InOutTransform, const sf::Vector2f& InPosition)
	{
#if false
		float* matrix = const_cast<float*>(InOutTransform.getMatrix()); 
		matrix[12] = InPosition.x; 
		matrix[13] = InPosition.y;
#else
		float tempmat[16];
		const float* oldMatrix = InOutTransform.getMatrix();
		std::copy(oldMatrix, oldMatrix + 16, tempmat);

		tempmat[12] = InPosition.x;
		tempmat[13] = InPosition.y;

		InOutTransform = sf::Transform
		(
			tempmat[0], tempmat[4], tempmat[12],
			tempmat[1], tempmat[5], tempmat[13],
			tempmat[3], tempmat[7], tempmat[15]
		);
#endif
	}


	template <typename TNumerical>
	constexpr TNumerical RSfmlAbs(const TNumerical InValue)
	{
		if (InValue < (TNumerical)0)
		{
			return -InValue;
		}
		return InValue;
	}

	template <typename TNumerical>
	constexpr TNumerical ScaleToFill(const sf::Vector2<TNumerical> InObjectSizeToScale, const sf::Vector2<TNumerical> InSizeToFill)
	{
		sf::Vector2<TNumerical> scaleVector = RSfml::div(InSizeToFill, InObjectSizeToScale);
		return std::max(scaleVector.x, scaleVector.y);
	}


	bool RMATH_UNIT_TEST();
}