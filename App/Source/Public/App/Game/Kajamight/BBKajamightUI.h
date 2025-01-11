#pragma once
#include "App/Game/BBGameUI.h"
#include "App/GameSystem/BBTextureLibrary.h"

namespace BB
{
	class BBKajamightScene;
	class BBKajamightUI : public BBGameUI
	{
	private:
		bool m_initialized;
		BBKajamightScene* _kmScene;
		BBFontResult m_fontAsset;

		sf::Text m_currentLevelText;
		sf::Text m_scoreText;
		sf::FloatRect m_currentLevelRect;
		sf::FloatRect m_scoreRect;
	public:
		BBKajamightUI(BBKajamightScene* InSceneObject);
		bool Initialize();
		bool InitializeGraphics();
		virtual void ReportProgress(const BBGameProgress& InNewProgressState) override;
		virtual void Render(sf::RenderWindow* InWindow) override;
		bool DeInitialize();
		bool DeInitializeGraphics();

		void SetCurrentLevelRect(const sf::FloatRect& InCurrentLevelRect);
		void SetScoreRect(const sf::FloatRect& InScoreRect);
		R_VIRTUAL_IMPLICIT ~BBKajamightUI() R_OVERRIDE_IMPLICIT;
	};
};