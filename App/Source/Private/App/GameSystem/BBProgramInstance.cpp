#include "App/GameSystem/BBProgramInstance.h"
#include "Gen_App/Config/AppConfig.h"
#include "RSFML/Tools/RLog.h"
#include "App/Game/Kajamight/BBKajamightScene.h"
#include "App/GameSystem/BBScene.h"
#include "App/GameSystem/BBPaths.h"

#include <iostream>
#include <string>
#include <string_view>
#include <libloaderapi.h>
#include <Tracy.hpp>


using namespace BB;

static const std::string WINDOW_PREFIX = "Brick Breaker SFLM";
constexpr bool B_START_IN_FULLSCREEN = false;

static std::string WindowNameFromSceneName(const std::string_view InSceneName)
{
    static const std::string WINDOW_PREFIX_APPEND = WINDOW_PREFIX + " : ";
    std::string result = WINDOW_PREFIX;
    result.append(InSceneName);
    return result;
}

void BBTime::StartTimer()
{
    m_bIsPaused = false;
    this->m_programClock.start();
    this->m_timeAtLastPause = sf::seconds(0.f);

    this->CurrentTime = this->m_timeAtLastPause + this->m_programClock.getElapsedTime();
    this->CurrentElipsedTime = this->CurrentTime;
}

void BB::BBTime::PauseTimer()
{
    if (!m_bIsPaused)
    {
        m_bIsPaused = true;
        m_timeAtLastPause = CurrentTime;
        m_programClock.reset();
    }
}

void BB::BBTime::ResumeTimer()
{
    if (m_bIsPaused)
    {
        m_bIsPaused = false;
        m_programClock.restart();
    }
}

void BBTime::TickTimer()
{
    sf::Time previousFrameTime = this->CurrentTime;
    this->CurrentTime = this->m_timeAtLastPause + this->m_programClock.getElapsedTime();
    this->CurrentElipsedTime = this->CurrentTime - previousFrameTime;
}

static sf::RenderWindow GetStartRenderWindow()
{
    if (B_START_IN_FULLSCREEN)
    {
        return sf::RenderWindow(sf::VideoMode::getDesktopMode(), WINDOW_PREFIX, sf::Style::None, sf::State::Fullscreen);
    }
    else
    {
        return sf::RenderWindow(sf::VideoMode(sf::Vector2u{ 1280u, 720u }), WINDOW_PREFIX);
    }
}


BBProgramInstance::BBProgramInstance()
    : IBBProgramObject(),
    bUsingFullscreenMode(B_START_IN_FULLSCREEN), m_bShouldRunEditor(false), m_window(GetStartRenderWindow()), m_time(), m_textureLibrary(this), m_levelJsonReader(this), m_soundPlayer(this)
{
}

static bool GetUseEditor(const size_t argc, char** argv)
{
    for (size_t i = 0; i < argc; ++i)
    {
        if (std::strcmp(argv[i], RUN_EDITOR_COMMAND) == 0)
        {
            return true;
        }
    }
    return false;
}

void BB::BBProgramInstance::Run(int argc, char** argv)
{
    ZoneScopedN("BBProgramInstance Run");
    this->m_bShouldRunEditor = GetUseEditor((size_t)argc, argv);
    Init();
    bool bQuitRequested = false;
    while (m_window.isOpen())
    {
        {
            ZoneScopedN("Window Events !");
            while (const std::optional event = m_window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                {
                    bQuitRequested = true;
                }
                else if (const auto* focusLost = event->getIf<sf::Event::FocusLost>())
                {
                    RLog::Log(LOG_DISPLAY, "Focus Lost at time : '{}'", m_time.CurrentTime.asSeconds());
                }
                else if (const auto* focusResumed = event->getIf<sf::Event::FocusGained>())
                {
                    RLog::Log(LOG_DISPLAY, "Focus Gain at time : '{}'", m_time.CurrentTime.asSeconds());
                }
                else if (const auto* resized = event->getIf<sf::Event::Resized>())
                {
                    m_time.PauseTimer();
                    this->OnWindowResized(resized->size);
                    m_time.ResumeTimer();
                }
                else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    {
                        bQuitRequested = true;
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::F11)
                    {
                        ApplyWindowFullscreenMode(!bUsingFullscreenMode);
                    }
                }
            }
        }
        if (bQuitRequested)
        {
            break;
        }
        m_time.TickTimer();
        
        m_soundPlayer.Tick(m_time.CurrentTime, m_time.CurrentElipsedTime);
        {
            ZoneScopedN("Scenes Tick !");
            for (std::shared_ptr<BBScene>& activeScene : m_activeScenes)
            {
                activeScene->Tick(m_time.CurrentTime, m_time.CurrentElipsedTime);
            }
        }
        {
            ZoneScopedN("Window Clear !");
            m_window.clear();
        }
        {
            ZoneScopedN("Window Render !");
            for (std::shared_ptr<BBScene>& activeScene : m_activeScenes)
            {
                m_window.setView(activeScene->GetView());
                activeScene->Render(&m_window);
            }
        }
        {
            ZoneScopedN("Window Display !");
            m_window.display();
        }

    }

    {
        ZoneScopedN("BBProgramInstance Shutdown !");
        Shutdown();
    }
}

BBProgramInstance::~BBProgramInstance()
{

}

void BB::BBProgramInstance::Init()
{
    bool bAllSucessfull = true;
    InitializePaths();
    bAllSucessfull = m_levelJsonReader.Initialize() && bAllSucessfull;
    m_window.setVerticalSyncEnabled(true);
    m_activeScenes.push_back(std::make_shared<BBKajamightScene>(this));
    BBKajamightScene* kajaScene = static_cast<BBKajamightScene*>(m_activeScenes.begin()->get());
   
    bAllSucessfull = m_soundPlayer.Initialize() && bAllSucessfull;

    m_soundPlayer.SetMusicPlayerData(kajaScene->GetMusicPlayerData());

    sf::Vector2f size = (sf::Vector2f)m_window.getSize();
    sf::View view(sf::Vector2f(0.f, 0.f), size);
    kajaScene->SetViewData(view);
    kajaScene->Start();


    OnWindowResized(m_window.getSize());

    m_time.StartTimer();
}

void BB::BBProgramInstance::Shutdown()
{
    bool bAllSucessfull = true;

    for (auto it = m_activeScenes.rbegin(); it != m_activeScenes.rend();)
    {
        if (it->get()->GetIsRunning())
        {
            it->get()->Shutdown();
        }
        it = std::vector<std::shared_ptr<BBScene>>::reverse_iterator(m_activeScenes.erase((it + 1).base()));
    }

    bAllSucessfull = m_soundPlayer.DeInitialize() && bAllSucessfull;

    bAllSucessfull = m_levelJsonReader.DeInitialize() && bAllSucessfull;

    if (m_window.isOpen())
    {
        m_window.close();
    }
}

void BB::BBProgramInstance::OnWindowResized(const sf::Vector2u InNewSize)
{
    for (auto it = m_activeScenes.begin(); it != m_activeScenes.end();++it)
    {
        it->get()->OnWindowResized(InNewSize);
        //sf::View view = scene->GetView();
        //sf::FloatRect rect = view.getViewport();
        //
        //sf::Vector2f avalablePixelView = sf::Vector2f(rect.size.x * (float)InNewSize.x, rect.size.y * (float)InNewSize.y);
        //view.setSize(avalablePixelView * 4.f);
        //view.setCenter(sf::Vector2f());
        //scene->SetViewData(view);
    }
}

void BB::BBProgramInstance::ApplyWindowFullscreenMode(const bool bInNewWindowMode)
{
    if (bUsingFullscreenMode != bInNewWindowMode)
    {
        m_time.PauseTimer();
        m_window.clear();
        m_window.close();
        bUsingFullscreenMode = bInNewWindowMode;
        if (bUsingFullscreenMode)
        {
            m_window = sf::RenderWindow(sf::VideoMode::getDesktopMode(), WINDOW_PREFIX, sf::Style::None, sf::State::Fullscreen);
        }
        else
        {
            m_window = sf::RenderWindow(sf::VideoMode(sf::Vector2u{ 1280u, 720u }), WINDOW_PREFIX);
        }
        OnWindowResized(m_window.getSize());

        m_time.ResumeTimer();
    }

}

bool BB::BBProgramInstance::InitializePaths() const
{
    constexpr size_t maxPath = 260;
    wchar_t moduleFileName[maxPath];
    GetModuleFileNameW(nullptr, moduleFileName, maxPath);

    BBPaths::m_runnablePath = moduleFileName;
    BBPaths::m_runnablePath.remove_filename();
    BBPaths::m_runnablePath_str = BBPaths::m_runnablePath.string();
    BBPaths::m_runnablePath_view = BBPaths::m_runnablePath_str;

#if    USE_App_PROJECT_ASSETS
    BBPaths::m_assetsDiskPath = App_PROJECT_ASSET_PATH;
#else
    BBPaths::m_assetsDiskPath = BBPaths::m_runnablePath / App_LOCAL_ASSET_PATH / "";
    //BBPaths::m_assetsDiskPath 
#endif
    BBPaths::m_assetsDiskPath_str = BBPaths::m_assetsDiskPath.string();
    BBPaths::m_assetsDiskPath_view = BBPaths::m_assetsDiskPath_str;



    char separator = std::filesystem::path::preferred_separator;
    std::string separatorString(1, separator);

    std::string assetFolderAndSeparatorConstruction = separatorString;
    assetFolderAndSeparatorConstruction.append(App_LOCAL_ASSET_PATH).append(separatorString);
    BBPaths::m_assetFolderAndSeparator = assetFolderAndSeparatorConstruction;
    BBPaths::m_assetFolderAndSeparator_str = BBPaths::m_assetFolderAndSeparator.string();

    return true;
}
