#pragma once
//#include "App/Game/Physics/BBCollisionResult.h"
#include <SFML/Graphics.hpp>

namespace BB
{
	struct BBPhysicBody;
	struct BBIDXsAndCollisionResult; ///*Is : ColbodyIDXsAndColResult*/
	class IBBPhysicBodyListener
	{
	public:
		virtual void OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) {}
		virtual void OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody) {}
		virtual void OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) {}
		
		virtual void OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const BBIDXsAndCollisionResult& CollisionResult) {}
	};
};