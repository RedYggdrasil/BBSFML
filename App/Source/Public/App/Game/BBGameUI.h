#pragma once
#include "RSFML/Tools/Textures/RGDXTPSprite.h"
#include "App/Memory/IBBSceneObject.h"
#include "App/Game/BBGameProgress.h"


namespace BB
{
	class BBGameUI : public IBBSceneObject
	{
	protected:
		BBGameProgress m_lastReportedProgress;
	protected:
		BBGameUI(IBBSceneObject* InSceneObject);
	public:
		virtual void ReportProgress(const BBGameProgress& InNewProgressState);
		virtual void Render(sf::RenderWindow* InWindow);
		R_VIRTUAL_IMPLICIT ~BBGameUI() R_OVERRIDE_IMPLICIT;
	};
};