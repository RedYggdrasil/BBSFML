#pragma once
#include "App/Defines/pch.h"
#include "App/Game/Physics/BBPhysicBody.h"
#include <memory>

namespace BB
{
	class BBProgramInstance;
	class BBPhysicManager;
	class BBScene;
	namespace Debug
	{
		class BBDebugSAT : public IBBPhysicBodyListener
		{
		private:
			std::weak_ptr<BBDebugSAT> m_thisWPtr;
			BBProgramInstance* m_programInstance;
			BBPhysicManager* m_physicManager;
			BBScene* m_scene;
		private:
			sf::Angle m_debugAngle = sf::degrees(0.f);
			sf::Vector2f m_debugMousePos = sf::Vector2f();
			sf::Vector2f m_secondPos = sf::Vector2f();
			sf::Angle m_secondAngle = sf::degrees(0.f);
			BBPhysicBodyID m_movablePBID;
			BBPhysicBodyID m_secondPBID;
			BBPhysicBodyID m_movableCirclePBID;
			BBPhysicBodyID m_secondCirclePBID;

			sf::RectangleShape m_movableShape;
			sf::CircleShape m_movableCircleShape;
			sf::RectangleShape m_secondShape;
			sf::CircleShape m_secondCircleShape;
		public:
			void Start(std::weak_ptr<BBDebugSAT> InThisWPtr, BBProgramInstance* InContext, BBPhysicManager* InPhysicManager, BBScene* InScene);
			void Tick(const sf::Time InDeltaTime, sf::View InView);
			void Draw(sf::RenderWindow* InWindow);
			~BBDebugSAT();
		public : 
			//IBBPhysicBodyListener
			virtual void OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody) override;

			virtual void OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
			virtual void OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		};
	}
}