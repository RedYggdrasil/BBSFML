#pragma once
#include "App/Defines/pch.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"
#include "App/Game/IBBGameBoardObject.h"
#include "App/Game/BBBrickData.h"
#include "App/Game/Physics/IBBPhysicBodyListener.h"
#include "App/GameSystem/BBSoundPlayerData.h"
#include "App/GameSystem/BBTextureLibrary.h"
#include <memory>
#include <vector>
#include <utility>

namespace BB
{
	struct BBBricks : public IBBGameBoardObject, public sf::Drawable, public sf::Transformable, public IBBPhysicBodyListener
	{
	protected:
		size_t m_currentLevelID;
		std::weak_ptr<BBBricks> m_thisWPtr;
		size_t m_originalBrickCount;
		std::vector<BBBrickData> m_bricks;
		sf::VertexArray m_vertices;
		std::shared_ptr<sf::Texture> m_gameBoardTexturePack;
		std::shared_ptr<RSfml::RGDXTPAtlasData> m_gameBoardAtlasData;
		std::vector<std::pair<BBSoundBufferResult, BBSoundPlayerData>> m_brickSounds;
	public:
		BBBricks(IBBGameBoardObject* InGameBoardObject);
		void Initialize(std::weak_ptr<BBBricks> InThisWPtr, std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas);
		
		bool DeInitialize();
		R_VIRTUAL_IMPLICIT ~BBBricks() R_OVERRIDE_IMPLICIT;

	public:
		bool IsCurrentlyInALevel() const { return (bool)m_currentLevelID; }
		void InitializeLevel(size_t InLevelID);
		void InitializeLevel(std::vector<BBBrickData>&& InBricks, const bool bPos0IsTopLeft = true);

		void ClearCurrentLevel();
	protected:
		void SetGameboardTexture(std::shared_ptr<sf::Texture> InTexture, std::shared_ptr<RSfml::RGDXTPAtlasData> InTextureAtlas);
		void CreateVertexArrayFromBricks();
		void CreatePhysicData(bool bInOverrideExistingID = true);

	public:
		//IBBPhysicBodyListener
		virtual void OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody) override;

		virtual void OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const BBIDXsAndCollisionResult& InCollisionResult) override;
	protected:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const R_PURE_OVERRIDE;
		void HandleBrickTouched(const size_t InBrickIndex, const BBPhysicBody& InBrickPhysicBody, const BBPhysicBody& InBallPhysicBody, const BBIDXsAndCollisionResult& InCollisionResult);
		void RemoveBrick(const size_t InBrickIndex);

	private:
		friend class BBGameBoardEditor;

		void EDITOR_AddBrick(const BBBrickData& InNewBrick, bool bPositionRelativeToBoard = true);
		void EDITOR_RemoveBrick(const BBPhysicBodyID& InIDToRemove);
		inline const std::vector<BBBrickData>& EDITOR_GetBricks() const { return m_bricks; }
	};
};