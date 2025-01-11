#pragma once

#include "RSFML/Tools/Textures/RGDXTPSprite.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include "App/Game/BBGameBoardScene.h"
#include "App/Game/Kajamight/BBKajamightUI.h"


namespace BB
{
	class BBKajamightScene : public BBGameBoardScene
	{
	protected:
		BBKajamightUI m_kmGameUI;
		BBAtlasResult m_atlasData;
		BBTextureResult m_texturePack;
		BBTextureResult m_gbTexture;
		BBTextureResult m_bgTexture;
	protected:
		RSfml::RGDXTPSprite m_gameboardBackgroundSprite;
		RSfml::RGDXTPSprite m_topCharacter;
		RSfml::RGDXTPMetaSprite m_gbFrame;
		sf::Sprite m_gbSprite;
		sf::Sprite m_bgSprite;
	public:
		BBKajamightScene(IBBProgramObject* InParentObject);

		virtual void Start() override;
		virtual void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void Shutdown() override;

		virtual void RenderUnderBoard(sf::RenderWindow* InWindow) override;
		virtual void RenderOverBoard(sf::RenderWindow* InWindow) override;

		R_VIRTUAL_IMPLICIT ~BBKajamightScene() R_OVERRIDE_IMPLICIT;
	private:
		bool InitTextureByPath(const std::string_view InSpritesheetRelativePath);
		bool DeInitializeTexture();

		bool InitializeSprites();
		bool DeInitializeSprites();

		bool InitializeBGFrame();
		bool DeInitializeBGFrame();
	};
};