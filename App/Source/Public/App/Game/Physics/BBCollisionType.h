#pragma once
#include "RSFML/Tools/REnum.h"

namespace BB
{
	enum class BBCollisionType : uint8_t
	{
		RENUM_MIN_VAL(None, 0),
		Overlap = 1,
		RENUM_MAX_VAL(Collide, 2)
	};
	RS_DEFINE_ENUM(BBCollisionType);
};