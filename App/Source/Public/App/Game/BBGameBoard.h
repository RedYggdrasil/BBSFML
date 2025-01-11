#pragma once
#include "App/Defines/pch.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
#include "App/Game/BBBricks.h"
#include "App/Game/BBGameProgress.h"
#include "App/Game/Physics/BBBallProjectileSolver.h"
#include "App/Game/Physics/IBBPhysicBodyListener.h"
#include "App/Game/IBBGameBoardObject.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include <memory>
#include <string>
#include <string_view>

namespace BB
{
	class BBGameBoardScene;
	class BBBall;
	class BBBorders;
	class BBPaddle;
	class BBGameBoard : public IBBGameBoardObject, public IBBPhysicBodyListener
	{
	protected:
		enum class EGameBoardState : uint8_t
		{
			Unknown = 0,
			InLevelTransition = 1,
			PlayingALevel = 2
		};
		struct GameBoardState
		{
		protected:
			EGameBoardState m_eGameBoardState;
			bool m_bPlayerLost;
			sf::Time m_lastTranslationTime;
		public:
			EGameBoardState GetGameBoardState() const { return m_eGameBoardState; }
			bool GetPlayerLost() const { return m_bPlayerLost; }
			sf::Time GetLastTranslationTime() const { return m_lastTranslationTime; }
		public:
			void MoveToLevelTransition(const sf::Time InTime, const bool bInHasLost);
			void MoveToPlayLevel(const sf::Time InTime);
		public:
			static GameBoardState CreateDefault();
		};
	protected:
		bool b_isRunning;
		std::weak_ptr<BBGameBoard> m_thisWPtr;
		GameBoardState m_gameBoardState;
		BBGameProgress m_gameProgress;

		BBAtlasResult m_gameBoardAtlasData;
		BBTextureResult m_gameBoardTexturePack;

		//sf::RectangleShape m_paddle;
		//sf::Sprite m_paddleSprite;
		std::shared_ptr<BBBricks> m_BBBricks;
		std::vector<std::shared_ptr<BBBall>> m_BBBalls;
		std::shared_ptr<BBBorders> m_BBBorders;
		std::vector<std::shared_ptr<BBPaddle>> m_paddles;
		//BBBallProjectileSolver m_BBBallProjectileSolver;
	protected:
		float m_paddleMin;
		float m_paddleMax;
		float m_currentPaddleHalfWidth;
		float m_paddleSpeed = 100.f;
	public:
		bool GetIsRunning() const { return b_isRunning; }
		const BBGameProgress& GetProgress() const { return m_gameProgress; }
	public:
		BBGameBoard(BBGameBoardScene* InParentSceneObject);

		virtual void Initialize(std::weak_ptr<BBGameBoard> InThisWPtr, const std::string_view InSpritesheetRelativePath);
		virtual bool DeInitialize();
		/// <summary>
		/// To Start the Game
		/// </summary>
		virtual void Start();
		/// <summary>
		/// To Stop the Game
		/// </summary>
		virtual void Stop();
		virtual void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const float InPaddleDirection, const bool bInDirectInput);
		void UpdatePaddle(const sf::Time InDeltaTime, const float InPaddleDirection, const bool bInDirectInput);
		virtual void Render(sf::RenderWindow* InWindow);
		virtual void Shutdown();

		void ReportDestroyedBall(std::weak_ptr<BBBall> InBallWPtr);
		void ReportBrick(size_t InAddedScore, size_t RemainingBricks, size_t OriginalBricksNumber);
		void OnLevelFailed();
		void OnLevelCleared();


		R_VIRTUAL_IMPLICIT ~BBGameBoard() R_OVERRIDE_IMPLICIT;
	protected:
		bool InitTextureByPath(const std::string_view InSpritesheetRelativePath);
		bool InitSprites();
		bool DeInitializeSprites();
		bool DeInitializeTexture();
	protected:
		void StartLevel(const size_t InLevelID);
		void ClearCurrentLevel(const bool bInReasonIsPlayerLost);

		size_t CreateNewBall(const sf::Vector2f InLocation);
		size_t CreateNewBall(const sf::Vector2f InLocation, const sf::Vector2f InDirection);

		void RemoveBall(const size_t InBallIndex);
		void ClearBalls();

		size_t CreateNewPaddle();
		size_t CreateNewPaddle(const sf::Vector2f InLocation);

		void RemovePaddle(size_t InPaddleIndex);
		void ClearPaddles();

		template <class TScene = BBGameBoardScene>
		TScene* SceneAs() const
		{
			return static_cast<TScene*>(this->m_scene);
		}
#if _DEBUG
	protected:
		std::unique_ptr<class BBGameBoard_Debug> m_pGBDebug;
		sf::FloatRect DEBUG_GetDebugLocation() const;
		BBBall* GetBallByPhysicID(const BBPhysicBodyID InPhysicBodyID) const;
		friend BBGameBoard_Debug;
		friend class BallDebugRenderData;
#endif
	};
};