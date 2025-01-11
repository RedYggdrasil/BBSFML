#include "App/Game/BBGameUI.h"

using namespace BB;


BBGameUI::BBGameUI(IBBSceneObject* InSceneObject)
: IBBSceneObject(InSceneObject), m_lastReportedProgress(BBGameProgress::CreateEmptyProgress())
{
}

void BBGameUI::ReportProgress(const BBGameProgress& InNewProgressState)
{
	m_lastReportedProgress = InNewProgressState;
}

void BBGameUI::Render(sf::RenderWindow* InWindow)
{
}

BBGameUI::~BBGameUI()
{
}
