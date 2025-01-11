#pragma once
#include "App/Defines/pch.h"
#include "App/Game/IBBGameBoardObject.h"
#include "App/Game/Physics/BBPhysicBody.h"
#include <memory>

namespace RSfml
{
	struct RGDXTPAtlasData;
};
namespace BB
{
	class BBBall : public IBBGameBoardObject,  public IBBPhysicBodyListener
	{
	private:
		bool m_bLoadedTextures;
		bool m_bCurrentlyDynamic;
		BBPhysicBodyID m_physicBodyID;
		sf::Sprite m_ballSprite;
		std::weak_ptr<BBBall> m_thisWPtr;
		std::shared_ptr<sf::Texture> m_texture;
		std::shared_ptr<RSfml::RGDXTPAtlasData> m_atlasData;

		float m_startSpeed;
		float m_minSpeed;
		float m_maxSpeed;
	public:

		inline float GetStartSpeed() const { return m_startSpeed; };
		inline float GetMinSpeed() const { return m_minSpeed; };
		inline float GetMaxSpeed() const { return m_maxSpeed; };
		inline BBPhysicBodyID GetPhysicBodyID() const { return m_physicBodyID; };
		inline bool GetCurrentlyDynamic() const { return m_bCurrentlyDynamic; };
	public:
		BBBall(IBBGameBoardObject* InParentObject);
		BBBall(IBBGameBoardObject* InParentObject, std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas);
		void Init(const std::weak_ptr<BBBall> InThisWPtr, const bool bInIsDynamic = true);
		void SetDynamic(const bool bInIsDynamic);
		R_VIRTUAL_IMPLICIT ~BBBall() R_OVERRIDE_IMPLICIT;


		bool DeInitialize();
		void SetPosition(const sf::Vector2f InPosition);
		void Launch(const sf::Vector2f InDirection, const float InLaunchSpeed);
		inline void LaunchAtStartSpeed(const sf::Vector2f InDirection) { Launch(InDirection, GetStartSpeed()); };
	public:
		void Render(sf::RenderWindow* InWindow) const;
		void SetGameboardTexture(std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas);
		bool ClearGameBoardTexture();
	private:
		void InitializePhysicalBody();
		bool DeInitializePhysicalBody();
	
	public:
		//IBBPhysicBodyListener
		virtual void OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody) override;

		virtual void OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;

	};
};