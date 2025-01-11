#pragma once
#include "RSFML/Tools/Textures/RGDXTPSprite.h"
#include "App/Editor/BBDrawableGrid.h"
#include "App/Game/BBGameBoard.h"
#include "App/Game/Physics/BBPhysicBodyID.h"

#include "App/Game/BBBrickData.h"
#include <vector>
#include <array>
#include <functional>
#include <filesystem>


namespace BB
{
	class BBGameBoardEditor : public BBGameBoard
	{
		struct BGClick
		{
		public:
			sf::Time m_timeout;
			sf::Time m_lastInputStarted;
			std::function<bool()> m_getIsHeldInput;
		public:
			bool m_bClick;
			void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime);
			void ResetFrameValues();
			BGClick(std::function<bool()> InGetDownState);
			inline bool GetIsClick() const { return m_bClick; }
		};
		struct GBEditorInputs
		{
		public:
			std::array<BGClick, 9> m_clicks;
		public:

			bool m_bSave;
			sf::Vector2f m_mouseWorldPosition;

			void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const sf::RenderWindow& InWindow, const sf::View& InView);
			void ResetFrameValues();

		public:
			bool GetLeftClick() const { return m_clicks[0].GetIsClick(); };
			bool GetRightClick() const { return m_clicks[1].GetIsClick(); };
			bool GetSaveClick() const { return m_clicks[2].GetIsClick(); };

			bool GetToggleGridClick() const { return m_clicks[3].GetIsClick(); };
			bool GetIncreaseGridClick() const { return m_clicks[4].GetIsClick(); };
			bool GetDecreaseGridClick() const { return m_clicks[5].GetIsClick(); };

			bool GetToggleRotClick() const { return m_clicks[6].GetIsClick(); };
			bool GetIncreaseRotClick() const { return m_clicks[7].GetIsClick(); };
			bool GetDecreaseRotClick() const { return m_clicks[8].GetIsClick(); };
			GBEditorInputs();
		};

		struct GBEditorBrushData
		{
		public:
			struct PublicBrushData
			{
			public:
				size_t m_currentBrushIndex;
				sf::Vector2f m_brushRawPosition;
				sf::Angle m_brushRawRotation;
			};
		public:
			PublicBrushData m_rawBrushData;
		public:
			PublicBrushData m_correctedBrushData;
			bool bSnapPosToGrid;
			bool bSnapRotToGrid;
			sf::Vector2f CurrentGridScale;
			sf::Angle CurrentAngleSnapScale;

			void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const GBEditorInputs& InFrameInputs);
			void ComputeCorrectedBrushData();

		public:
			const PublicBrushData& GetRaw() const { return m_rawBrushData; }
			const PublicBrushData& GetCorrected() const { return m_correctedBrushData; }

		};
	private:
		BBFontResult m_fontAsset;
		sf::Text m_commandInstruction;

		BBDrawableGrid m_drawableGrid;
		BBPhysicBodyID m_cursorPBID;
		GBEditorInputs m_inputs;
		std::vector<BBBrickData> m_currentBrushes;
		GBEditorBrushData m_currentBrushData;

		RSfml::RGDXTPSprite m_brushPreview;

		BBPhysicBodyID m_currentlyOverlappingBrick;
		std::filesystem::path m_levelPath;
	public:
		void UpdateBrushPreviewSprite();
	public:
		BBGameBoardEditor(BBGameBoardScene* InParentSceneObject);

		virtual void Initialize(std::weak_ptr<BBGameBoard> InThisWPtr, const std::string_view InSpritesheetRelativePath) override;
		virtual bool DeInitialize() override;

		bool InitializePhysics();
		bool DeInitializePhysics();

		bool InitializeTexts();
		bool DeInitializeTexts();

		void UpdateInstructions();
		virtual void Start();
		virtual void Stop();

		virtual void Tick(const sf::Time InGameTime, const sf::Time InDeltaTime, const float InPaddleDirection, const bool bInDirectInput);
		
		virtual void Render(sf::RenderWindow* InWindow);
		virtual void Shutdown();

		R_VIRTUAL_IMPLICIT ~BBGameBoardEditor() R_OVERRIDE_IMPLICIT;
	public:
		//IBBPhysicBodyListener
		virtual void OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody) override;

		virtual void OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime) override;
		virtual void OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const BBIDXsAndCollisionResult& InCollisionResult) override;

		public : 
			//Control Grid

			void ToggleGrid();
			bool GetGridToggleState();
			void SetGridToggleState(const bool InbNewGridState, const bool InbUpdateInstruction = true);
			void ChangeGridSize(const bool bIsIncrease);

			sf::Vector2f GetGridSnapSize() const;
			void SetGridSnapSize(const sf::Vector2f InNewSize);

		public:
			//Control Rot

			void ToggleRot();
			bool GetRotToggleState();
			void SetRotToggleState(const bool InbNewGridState, const bool InbUpdateInstruction = true);
			void ChangeRotSize(const bool bIsIncrease);

			sf::Angle GetRotSnapSize() const;
			void SetRotSnapSize(const sf::Angle InAngle);
	};
}