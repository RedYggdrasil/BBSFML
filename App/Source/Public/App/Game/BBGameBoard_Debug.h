#pragma once
#if _DEBUG

#include "App/Game/BBGameBoard.h"
#include "App/Game/Physics/BBPhysicBodyID.h"

namespace BB
{
	class BBGameBoard_Debug;
	class DebugRenderData
	{
	protected:
		BBGameBoard_Debug* m_gbDebug;
	public:
		sf::FloatRect InfoRect;
	public:
		virtual bool Initialize(const sf::Vector2f InTopLeftPosition);
		virtual void UpdateDatas(const sf::Time InGameTime, const sf::Time InDeltaTime, BBGameBoard* InBBGameBoard) = 0;
		virtual void Render(sf::RenderWindow* InWindow) = 0;
		sf::Vector2f PositionBelow() const;
	protected:
		DebugRenderData(BBGameBoard_Debug* InGameBoardDebug);
	public:
		virtual ~DebugRenderData();
	};
	class BallDebugRenderData :  public DebugRenderData
	{
	public:
		BBPhysicBodyID Ball_ID;
		sf::Vector2f Velocity;
		sf::Text m_speedText;

	public:
		BallDebugRenderData(BBGameBoard_Debug* InGameBoardDebug, const BBPhysicBodyID InBall_ID);
		virtual bool Initialize(const sf::Vector2f InTopLeftPosition) override;
		virtual void UpdateDatas(const sf::Time InGameTime, const sf::Time InDeltaTime, BBGameBoard* InBBGameBoard) override;
		virtual void Render(sf::RenderWindow* InWindow) override;
	public:
		R_VIRTUAL_IMPLICIT ~BallDebugRenderData() R_OVERRIDE_IMPLICIT;
	};
	class BBGameBoard_Debug : public IBBGameBoardObject
	{
	public:
		BBFontResult m_fontAsset;
		std::vector<std::unique_ptr<DebugRenderData>> m_renderDatas;
		sf::FloatRect m_avalableSpace;
	public:
		bool OnBallsChanged();
	public:
		BBGameBoard_Debug(BBGameBoard* InGameBoard);
		bool Initialize(class BBTextureLibrary* InLibrary);
		bool InitializeAssets(class BBTextureLibrary* InLibrary);
		bool InitializeElements();
		void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime);
		void Render(sf::RenderWindow* InWindow);
		bool DeInitialize(BBTextureLibrary* InLibrary);
		bool DeInitializeAssets(BBTextureLibrary* InLibrary);
		bool DeInitializeElements();
		R_VIRTUAL_IMPLICIT ~BBGameBoard_Debug() R_OVERRIDE_IMPLICIT;
	private:
		sf::Vector2f GetNextDebugLocation() const;
	};
}
#endif