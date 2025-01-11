#include "App/Game/BBBorders.h"
#include "App/Game/Physics/BBCollisionResult.h"
#include "App/Game/Physics/BBPhysicBody.h"
#include "App/Game/Physics/BBPhysicManager.h"
#include "App/GameSystem/BBProgramInstance.h"

using namespace BB;

const std::string BORDER_BALL_IMPACTS[] =
{
	"Sounds/Impacts/2h_macehit_mtl_woodbody_01.ogg",
	"Sounds/Impacts/2h_macehit_mtl_woodbody_02.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_03.ogg",
	"Sounds/Impacts/2h_macehit_mtl_woodbody_04.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_05.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_06.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_07.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_08.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_09.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_10.ogg",
	//"Sounds/Impacts/2h_macehit_mtl_woodbody_11.ogg"
};

BBBorders::BBBorders(IBBGameBoardObject* InParentObject)
	:IBBGameBoardObject(InParentObject), IBBPhysicBodyListener(), m_bInitialized(false), m_thisWPtr(), m_borders()
{
}

void BBBorders::Initialize(const std::weak_ptr<BBBorders> InThisWPtr, const sf::FloatRect& InBoardRect)
{
	bool bAllSucessfull = true;
	m_thisWPtr = InThisWPtr;
	InitializePhysicalBody(InBoardRect);
	bAllSucessfull = InitializeSounds() && bAllSucessfull;
}
bool BB::BBBorders::InitializeSounds()
{
	bool bAllSucessfull = true;
	BBTextureLibrary* library = m_programInstance->GetTextureLibrary();

	BBSoundPlayerData useSoundData = BBSoundPlayerData::DEFAULT;
	useSoundData.Volume = 25.f;

	bAllSucessfull = library->GetAssetsToVector<sf::SoundBuffer>(BORDER_BALL_IMPACTS, _countof(BORDER_BALL_IMPACTS), useSoundData, /*InOut*/m_borderBallSounds) && bAllSucessfull;

	return bAllSucessfull;
}
bool BB::BBBorders::DeInitialize()
{
	bool bAllSucessfull = true;
	bAllSucessfull = DeInitializeSounds() && bAllSucessfull;
	bAllSucessfull = DeInitializePhysicalBody() && bAllSucessfull;
	m_thisWPtr.reset();
	return bAllSucessfull;
}
bool BB::BBBorders::DeInitializeSounds()
{
	bool bAllSucessfull = true;
	BBTextureLibrary* library = m_programInstance->GetTextureLibrary();
	if (library)
	{
		library->ReportAssetUsageEndedFromVector(/*InOut*/m_borderBallSounds, true);
	}
	else
	{
		bAllSucessfull = false;
	}
	return bAllSucessfull;
}
BBBorders::~BBBorders()
{
	DeInitialize();
}
void BBBorders::InitializePhysicalBody(const sf::FloatRect& InBoardRect)
{
	constexpr int TOP_MARGIN = 1024;
	constexpr int BOTTOM_MARGIN = TOP_MARGIN;
	constexpr int BORDER_MARGIN = 1024;
	///Top Border
	m_borders[0] = BBBorderData
	{
		.Size = sf::IntRect
		(
			/*Pos__*/sf::Vector2i{(int)InBoardRect.position.x - BORDER_MARGIN, (int)InBoardRect.position.y - TOP_MARGIN},
			/*Scale*/sf::Vector2i{(int)InBoardRect.size.x + (BORDER_MARGIN * 2), TOP_MARGIN }
		),
		.PhysIndex = 0,
		.bIsDeadZone = false
	};

	///Left Border
	m_borders[1] = BBBorderData
	{
		.Size = sf::IntRect
		(
			/*Pos__*/sf::Vector2i{(int)InBoardRect.position.x - BORDER_MARGIN, (int)InBoardRect.position.y},
			/*Scale*/sf::Vector2i{BORDER_MARGIN, (int)InBoardRect.size.y }
		),
		.PhysIndex = 0,
		.bIsDeadZone = false
	};
	///Right Border
	m_borders[2] = BBBorderData
	{
		.Size = sf::IntRect{
			/*Pos__*/sf::Vector2i{(int)InBoardRect.position.x + (int)InBoardRect.size.x, (int)InBoardRect.position.y},
			/*Scale*/sf::Vector2i{BORDER_MARGIN, (int)InBoardRect.size.y }
		},
		.PhysIndex = 0,
		.bIsDeadZone = false
	};
	///Bottom DeadZoneBorder
	m_borders[3] = BBBorderData
	{
		.Size = sf::IntRect{
			/*Pos__*/sf::Vector2i{(int)InBoardRect.position.x + -BORDER_MARGIN, (int)InBoardRect.position.y + (int)InBoardRect.size.y},
			/*Scale*/sf::Vector2i{(int)InBoardRect.size.x + (BORDER_MARGIN * 2), BOTTOM_MARGIN }
		},
		.PhysIndex = 0,
		.bIsDeadZone = true
	};

	for (BBBorderData& BorderData : m_borders)
	{
		CreatePhysicBorder(BorderData);
	}
}

bool BB::BBBorders::DeInitializePhysicalBody()
{
	BBPhysicManager* pm = nullptr;
	const size_t initialSize = m_borders.size();
	for (size_t i = initialSize; i > 0; --i)
	{
		BBBorderData* data = &m_borders[i - 1];
		if ((bool)data->PhysIndex)
		{
			if (!pm)
			{
				pm = BBPhysicManager::Get(this);
			}
			pm->MarkRemovePhysicalEntity(data->PhysIndex);
			*data = BBBorderData();
		}
	}
	return true;
}

void BBBorders::OnOverlapWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody)
{
	if (InOtherPhysicBody.GetBehavior().IsA(EBBColMask::BALL))
	{
		//InOtherPhysicBody.lis
	}
	//const size_t initialSize = m_borders.size();
	//for (size_t i = initialSize; i > 0; --i)
	//{
	//	BBBorderData* data = &m_borders[i - 1];
	//
	//	if (InThisPhysicBody.Id() == data->PhysIndex)
	//	{
	//	}
	//}
}

void BBBorders::OnPhysicalPrePass(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
}

void BBBorders::OnPhysicPassEnded(BBPhysicBody& InOutThisPhysicBody, const sf::Time InGameTime, const sf::Time InDeltaTime)
{
}

void BB::BBBorders::OnCollideWith(const BBPhysicBody& InThisPhysicBody, const BBPhysicBody& InOtherPhysicBody, const ColbodyIDXsAndColResult& CollisionResult)
{
	if (/*InThisPhysicBody.Id() == m_physicBodyID &&*/ InOtherPhysicBody.GetBehavior().IsA(EBBColMask::BALL))
	{
		m_programInstance->PlaySound(m_borderBallSounds);
	}
}

void BBBorders::CreatePhysicBorder(BBBorderData& InOutBorderData)
{
    BBPhysicManager* pm = BBPhysicManager::Get(this);
    if ((bool)InOutBorderData.PhysIndex)
    {
		//override is handled by AddPhysicalEntity
        //pm->RemovePhysicalEntity(InOutBorderData.PhysIndex);
    }

    sf::Transform borderTrs;
	borderTrs.translate((sf::Vector2f)InOutBorderData.Size.position + ((sf::Vector2f)(InOutBorderData.Size.size) * 0.5f));
    BBColliderBody localColliderBody = BBColliderBody(InOutBorderData.bIsDeadZone ? GAMEBOARD_DEADZONE : GAMEBOARD_BORDER);

	localColliderBody.Colliders.push_back
	(
		BBCollider
		(
			OrientedBox(sf::Vector2f(), (sf::Vector2f)InOutBorderData.Size.size * 0.5f, sf::Angle())
        )
    );
    localColliderBody.ComputeAABB();

    BBColliderBodyLWPair ColliderBodyPair = BBColliderBodyLWPair::FromLocalCollidersBody(localColliderBody, borderTrs);

	BBPhysicBody Body =
    (
        BBPhysicBody::CreatePhysicBodyWithID
        (
            std::move(borderTrs),
            std::move(ColliderBodyPair),
            std::move(NoDynamics(10000.f)),
            m_thisWPtr
        )
    );
	InOutBorderData.PhysIndex = Body.Id();
	pm->AddPhysicalEntity(Body);
}
