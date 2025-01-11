#pragma once
#include <SFML/Graphics.hpp>
#include "App/Game/Physics/BBDynamics.h"

namespace BB
{

	class IBBProjectile
	{
	protected:
		BBDynamics m_dynamics;
	};
};