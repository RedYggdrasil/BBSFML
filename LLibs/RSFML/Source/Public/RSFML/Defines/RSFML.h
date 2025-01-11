#pragma once
#include "RCB/Defines/RCB.h"
#include <type_traits>

namespace RSfml
{
	template<typename T>
	using UT = std::underlying_type<T>::type;

	template <typename T>
	constexpr typename std::underlying_type<T>::type UT_cast(T e) noexcept {
		return static_cast<typename std::underlying_type<T>::type>(e);
	}

	template<typename TWeakPtr>
	inline constexpr bool WPtrEqual(const TWeakPtr& InLeft, const TWeakPtr& InRight)
	{
		return !InLeft.owner_before(InRight) && !InRight.owner_before(InLeft);
	}
};