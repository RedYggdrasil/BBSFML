#pragma once
#include "App/Defines/pch.h"
#include "App/Game/IBBGameBoardObject.h"
#include "App/Game/Physics/BBPhysicBodyID.h"
#include "App/Game/Physics/IBBPhysicBodyListener.h"

#include "App/GameSystem/BBSoundPlayerData.h"
#include "App/GameSystem/BBTextureLibrary.h"

#include <memory>

namespace RSfml
{
	struct RGDXTPAtlasData;
};
namespace BB
{
	class BBPaddle : public IBBGameBoardObject, public IBBPhysicBodyListener, public sf::Transformable, public sf::Drawable
	{
	private:
		bool m_bInitialized;
		sf::Vector2f m_currentSize;
		BBPhysicBodyID m_physicBodyID;
		sf::VertexArray m_paddleMesh;
		std::weak_ptr<BBPaddle> m_thisWPtr;
		std::shared_ptr<sf::Texture> m_texture;
		std::shared_ptr<RSfml::RGDXTPAtlasData> m_atlasData;
		std::vector<std::pair<BBSoundBufferResult, BBSoundPlayerData>> m_paddleBallSounds;
	public:
		BBPaddle(IBBGameBoardObject* InParentObject);
		void Initialize(const std::weak_ptr<BBPaddle> InThisWPtr, std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas);
		R_VIRTUAL_IMPLICIT ~BBPaddle() R_OVERRIDE_IMPLICIT;
		float GetCurrentWidth() const { return m_currentSize.x * getScale().x; }
		float GetCurrentHeight() const { return m_currentSize.y * getScale().y; }
	private:
		bool InitializeSounds();
		bool InitializeTexture(std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas);
		bool InitializePhysicalBody();

		bool DeInitialize();
		bool DeInitializePhysicalBody();
		bool DeInitializeTexture();
		bool DeInitializeSounds();

	public:
		//IBBPhysicBodyListener
		virtual void OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody) override;

		virtual void OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		
		virtual void OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const BBIDXsAndCollisionResult& CollisionResult) override;
		
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const R_PURE_OVERRIDE;

	};
};