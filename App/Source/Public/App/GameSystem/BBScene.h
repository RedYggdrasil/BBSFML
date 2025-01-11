#pragma once
#include "App/Defines/pch.h"
#include "App/GameSystem/BBMusicPlayerData.h"
#include "App/Memory/IBBSceneObject.h"

namespace BB
{
	class BBPhysicManager;
	class BBScene : public IBBSceneObject
	{
		bool b_isRunning;
		sf::View m_view;
	protected:
		std::unique_ptr<BBPhysicManager> m_bbPhysicManager;
		BBMusicPlayerData m_musicPlayerData;
		sf::Vector2u m_sceneMinimalView{ 2560, 1440 };
		sf::Vector2u m_sceneTargetView{ 3840, 2160 };
		sf::Vector2u m_sceneMaximalView{ 3840 + (256*2), 2160 + (256 * 2) };
		float m_scalePrioritizeX = 1.f;
		inline sf::Vector2f ScalePriorityVector() const
		{ return sf::Vector2f{ m_scalePrioritizeX, 1.f - m_scalePrioritizeX }; }
	public:
		inline const BBMusicPlayerData& GetMusicPlayerData() const { return m_musicPlayerData; }
		inline BBPhysicManager* GetPhysicManager() const { return m_bbPhysicManager.get(); }
		sf::Vector2u GetSceneMinimal() const { return m_sceneMinimalView; }
		bool GetIsRunning() const { return b_isRunning; }
		const sf::View& GetView() const { return m_view; }

		virtual void SetViewData(const sf::View& InView);
		virtual void OnWindowResized(const sf::Vector2u InNewSize);
	public:
		BBScene(IBBProgramObject* InParentObject);
		BBScene(IBBProgramObject* InParentObject, std::unique_ptr<BBPhysicManager>&& InPhysicManager);

		virtual void Start();
		virtual void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime);
		virtual void Render(sf::RenderWindow* InWindow);
		virtual void Shutdown();
		virtual void DrawDebug(sf::RenderWindow* InWindow);

		R_VIRTUAL_IMPLICIT ~BBScene() R_OVERRIDE_IMPLICIT;
	};
};