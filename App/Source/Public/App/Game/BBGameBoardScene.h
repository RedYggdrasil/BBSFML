#pragma once
#include "App/Game/BBGameBoard.h"
#include "App/GameSystem/BBScene.h"
#include "App/Game/BBUserInputs.h"


namespace BB
{
	class BBGameBoardScene : public BBScene
	{
	protected:
		sf::IntRect m_gameBoardRect;
		std::shared_ptr<BBGameBoard> m_gameBoard;
		std::unique_ptr <BBUserInput> m_bbUserInputs;
	protected:
		virtual std::shared_ptr<BBGameBoard> CreateGameBoard(const bool bInIsEditor) const;
		virtual std::unique_ptr<BBUserInput> CreateUserInputs() const { return std::make_unique<BBUserInput>((BBGameBoardScene*)this); }
	public:
		BBGameBoardScene(IBBProgramObject* InParentObject);

		virtual void Start() override;
		virtual void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void Render(sf::RenderWindow* InWindow) override;
		virtual void Shutdown() override;

	public:
		virtual void RenderUnderBoard(sf::RenderWindow* InWindow);
		virtual void RenderOverBoard(sf::RenderWindow* InWindow);

	public:
		R_VIRTUAL_IMPLICIT ~BBGameBoardScene() R_OVERRIDE_IMPLICIT;

	};
};