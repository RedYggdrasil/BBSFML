#pragma once
#include "App/Defines/pch.h"
#include "App/Memory/IBBSceneObject.h"
#include <array>
//#include <memory>

namespace BB
{
	class BBUserInput : public IBBSceneObject
	{
	protected:
		bool m_bIsUsingMouse = false;

		std::array<sf::Keyboard::Scancode, 4> m_leftScancodes 
		{ sf::Keyboard::Scancode::A, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown };
	
		std::array<sf::Keyboard::Scancode, 4> m_rightScancodes
		{ sf::Keyboard::Scancode::D, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown };

		float m_currentXAxisTarget = 0.f;
	public:
		bool GetIsUsingMouse() const { return m_bIsUsingMouse; }
		const std::array<sf::Keyboard::Scancode, 4>& getLeftScanKeys() const { return m_leftScancodes; }
		const std::array<sf::Keyboard::Scancode, 4>& getRightScanKeys() const { return m_rightScancodes; }

		void SetIsUsingMouse(bool bInUseMouse) { m_bIsUsingMouse = bInUseMouse; }
		void SetLeftScanKeys(const std::array<sf::Keyboard::Scancode, 4>& InLeftKeys) { m_leftScancodes = InLeftKeys; }
		void SetRightScanKeys(const std::array<sf::Keyboard::Scancode, 4>& InRightKeys) { m_rightScancodes = InRightKeys; }

		inline void SetLeftScanKey(sf::Keyboard::Scancode InLeftKey)
		{ SetLeftScanKeys({ InLeftKey, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown }); }
		inline void SetRightScanKey(sf::Keyboard::Scancode InRightKey)
		{ SetRightScanKeys({ InRightKey, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown, sf::Keyboard::Scancode::Unknown }); }
		
		float UpdateState();
		inline float GetPaddleTarget() const { return m_currentXAxisTarget; }

	public:
		BBUserInput(IBBSceneObject* InSceneObject);
		R_VIRTUAL_IMPLICIT ~BBUserInput() R_OVERRIDE_IMPLICIT;
	};
};