#include "App/Game/BBUserInputs.h"
#include "App/GameSystem/BBProgramInstance.h"
#include "App/GameSystem/BBScene.h"

using namespace BB;

float BBUserInput::UpdateState()
{
	if (m_bIsUsingMouse)
	{
		const auto& window = m_programInstance->GetWindow();
		const auto& sceneView = m_scene->GetView();
		sf::Vector2i pos = sf::Mouse::getPosition(window);
		sf::Vector2f worldPos = window.mapPixelToCoords(pos, sceneView);
		m_currentXAxisTarget = worldPos.x;
	}
	else
	{
		float currentDirection = 0.f;
		for (const auto scancode : m_leftScancodes)
		{
			if (scancode != sf::Keyboard::Scancode::Unknown)
			{
				if (sf::Keyboard::isKeyPressed(scancode))
				{
					currentDirection = currentDirection - 1.f;
					break;
				}
			}
		}
		for (const auto scancode : m_rightScancodes)
		{
			if (scancode != sf::Keyboard::Scancode::Unknown)
			{
				if (sf::Keyboard::isKeyPressed(scancode))
				{
					currentDirection = currentDirection + 1.f;
					break;
				}
			}
		}
		if (std::abs(currentDirection) > std::numeric_limits<float>::epsilon())
		{
			currentDirection = currentDirection * std::numeric_limits<float>::infinity();
		}
		else
		{
			currentDirection = 0.f;
		}
		m_currentXAxisTarget = currentDirection;
	}
	return GetPaddleTarget();
}

BBUserInput::BBUserInput(IBBSceneObject* InSceneObject)
: IBBSceneObject(InSceneObject)
{
}
BBUserInput::~BBUserInput()
{
}