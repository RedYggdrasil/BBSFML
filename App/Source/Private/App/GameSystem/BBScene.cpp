#include "App/GameSystem/BBScene.h"
#include "RSFML/Tools/RMath.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/GameSystem/BBProgramInstance.h"

using namespace BB;

void BB::BBScene::SetViewData(const sf::View& InView)
{
	m_view = InView;
}

void BB::BBScene::OnWindowResized(const sf::Vector2u InNewSize)
{
    bool bHasOutOfBounds = false;
    float resultFactor = 1.f;

    sf::FloatRect viewRect = this->m_view.getViewport();
    sf::Vector2f fRectPixelSize = RSfml::mul(viewRect.size, (sf::Vector2f)InNewSize);


    sf::Vector2f minFactorVec = RSfml::div((sf::Vector2f)m_sceneMinimalView, fRectPixelSize);
    float minFactor = std::max(minFactorVec.x, minFactorVec.y);

    sf::Vector2f maxFactorVec = RSfml::div((sf::Vector2f)m_sceneMaximalView, fRectPixelSize);
    float maxFactor = std::min(maxFactorVec.x, maxFactorVec.y);

    if (minFactor >= maxFactor)
    {
        resultFactor = minFactor;
        bHasOutOfBounds = maxFactor != minFactor;
    }
    else
    {
        sf::Vector2f targetFactorVec = RSfml::div((sf::Vector2f)m_sceneTargetView, fRectPixelSize);

        //Mulsum (dot)
        targetFactorVec = RSfml::mul(targetFactorVec, ScalePriorityVector());
        float targetFactor = (targetFactorVec.x + targetFactorVec.y);
        
        
        resultFactor = std::clamp(targetFactor, minFactor, maxFactor);
        bHasOutOfBounds = false;
    }
    m_view.setSize(fRectPixelSize * resultFactor);

    //sf::Vector2u realViewSize = sf::Vector2u(viewRect.size.x * InNewSize.x, viewRect.size.y * InNewSize.y);
    //
    //sf::Vector2f scalePriority = ScalePriorityVector();
    //
    //sf::Vector2f scaleFactorXY = RSfml::div((sf::Vector2f)m_sceneTargetView, (sf::Vector2f)realViewSize);
    //
    //float scaleFactor = scaleFactorXY.x * m_scalePrioritizeX + scaleFactorXY.y * (1 - m_scalePrioritizeX);
    //
    //sf::Vector2f newSize = ((sf::Vector2f)realViewSize) * scaleFactor;
    //
    //
    //float newWidth = realViewSize.x * scaleFactor;
    //float newHeight = realViewSize.y * scaleFactor;
    //
    //static int count = 1;

    //m_view.zoo(sf::Vector2f{ (float)count, (float)count });
    //
    //sf::Vector2f avalablePixelView = sf::Vector2f(rect.size.x * (float)InNewSize.x, rect.size.y * (float)InNewSize.y);
    //view.setSize(avalablePixelView * 4.f);
    //view.setCenter(sf::Vector2f());
    //scene->SetViewData(view);
}

BBScene::BBScene(IBBProgramObject* InParentObject)
: BBScene(InParentObject, std::move(std::make_unique<BBPhysicManager>(this)))
{

}
BBScene::BBScene(IBBProgramObject* InParentObject, std::unique_ptr<BBPhysicManager>&& InPhysicManager)
    : IBBSceneObject((IBBProgramObject*)InParentObject), b_isRunning(false), 
    m_bbPhysicManager(std::move(InPhysicManager)), m_musicPlayerData(BBMusicPlayerData::DEFAULT)
{

}

void BBScene::Start()
{
	b_isRunning = true;
    m_bbPhysicManager->Init();
}

void BBScene::Tick(const sf::Time InGameTime, const sf::Time InDeltaTime)
{
    m_bbPhysicManager->UpdatePhysic(InGameTime, InDeltaTime);
}

void BB::BBScene::Render(sf::RenderWindow* InWindow)
{
}

void BBScene::Shutdown()
{
    m_bbPhysicManager->Shutdown();
	b_isRunning = false;
}

void BB::BBScene::DrawDebug(sf::RenderWindow* InWindow)
{
#if BB_DEBUG_PHYSIC
    m_bbPhysicManager->draw(InWindow);
#endif
}

BBScene::~BBScene()
{

}