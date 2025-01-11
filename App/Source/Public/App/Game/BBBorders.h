#pragma once
#include "App/Defines/pch.h"
#include "App/Game/IBBGameBoardObject.h"
#include "App/Game/Physics/BBPhysicBodyID.h"
#include "App/Game/Physics/IBBPhysicBodyListener.h"
//#include "App/Game/Physics/BBPhysicBody.h"
#include "App/GameSystem/BBSoundPlayerData.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include <memory>

namespace BB
{
	struct BBBorderData
	{
	public:
		sf::IntRect Size;
		BBPhysicBodyID PhysIndex;
		bool bIsDeadZone;
	};
	struct BBPhysicBody;
	class BBBorders : public IBBGameBoardObject, public IBBPhysicBodyListener
	{
	private:
		bool m_bInitialized;
		std::weak_ptr<BBBorders> m_thisWPtr;
		std::array<BBBorderData, 4> m_borders;
		std::vector<std::pair<BBSoundBufferResult, BBSoundPlayerData>> m_borderBallSounds;
	public:
		BBBorders(IBBGameBoardObject* InParentObject);
		void Initialize(const std::weak_ptr<BBBorders> InThisWPtr, const sf::FloatRect& InBoardRect);
		bool InitializeSounds();
		bool DeInitialize();
		bool DeInitializeSounds();
		R_VIRTUAL_IMPLICIT ~BBBorders() R_OVERRIDE_IMPLICIT;

	private:
		void InitializePhysicalBody(const sf::FloatRect& InBoardRect);
		bool DeInitializePhysicalBody();

	public:
		//IBBPhysicBodyListener
		virtual void OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody) override;

		virtual void OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const BBIDXsAndCollisionResult& CollisionResult) override;

	private:
		void CreatePhysicBorder(BBBorderData& InOutBorderData);
	};
};