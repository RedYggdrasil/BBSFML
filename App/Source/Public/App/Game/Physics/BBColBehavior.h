#pragma once
#include "App/Defines/pch.h"
#include "RSFML/Tools/REnum.h"

namespace BB
{

	enum class EBBColMask : uint64_t
	{
		NONE				= 0,			// 0000 0000 0000	0x00		000  
		WORLD_STATIC = 1 << 0,				// 0000 0000 0001	0x01		001
		WORLD_DYNAMIC = 1 << 1,				// 0000 0000 0010	0x02		002
		BRICK = 1 << 2,						// 0000 0000 0100	0x04		004
		BALL = 1 << 3,						// 0000 0000 1000	0x08		008
		PLAYER_PROJECTILE = 1 << 4,			// 0000 0001 0000	0x10		016
		PADDLE = 1 << 5,					// 0000 0010 0000	0x20		032
		PADDLE_ITEM = 1 << 6,				// 0000 0100 0000	0x40		064
		GAMEBOARD_BORDER = 1 << 7,			// 0000 1000 0000	0x80		128	
		GAMEBOARD_DEADZONE = 1 << 8,		// 0001 0000 0000  0x100		256
		/*EXEMPLE_10 = 1 << 9,				// 0010 0000 0000  0x200		512
		EXEMPLE_11 = 1 << 10,				// 0100 0000 0000  0x400		1024
		EXEMPLE_12 = 1 << 11,				// 1000 0000 0000  0x800		2048*/

		ALL = std::numeric_limits<uint64_t>::max()	// 1111 1111	0xFFFFFFFFFFFFFFFF
	};
	RS_DEFINE_ENUM_FLAG(BB::EBBColMask);

	struct BBColBehavior
	{
	public:
		EBBColMask SelfMask = EBBColMask::NONE;
		EBBColMask InteractMask = EBBColMask::ALL;
		EBBColMask CollideMask = EBBColMask::ALL;
	public:
		inline bool Interact(const BBColBehavior& InOther) const
		{
			return
				((uint64_t)this->SelfMask & (uint64_t)InOther.InteractMask) != 0
				&&
				((uint64_t)this->InteractMask & (uint64_t)InOther.SelfMask) != 0;
		}
		inline bool InteractWithMask(const EBBColMask& InOtherSelf) const
		{
			return ((uint64_t)this->InteractMask & (uint64_t)InOtherSelf) != 0;
		}
		inline bool Collide(const BBColBehavior& InOther) const
		{
			return
				((uint64_t)this->SelfMask & (uint64_t)InOther.CollideMask) != 0
				&&
				((uint64_t)this->CollideMask & (uint64_t)InOther.SelfMask) != 0;
		}
		inline bool CollideWithMask(const EBBColMask& InOtherSelf) const
		{
			return ((uint64_t)this->CollideMask & (uint64_t)InOtherSelf) != 0;
		}
		inline bool IsA(const EBBColMask& InMask) const
		{ return ((uint64_t)this->SelfMask & (uint64_t)InMask); }
		inline bool IsExactly(const EBBColMask& InMask) const
		{ return (this->SelfMask == InMask); }
	};

	constexpr BBColBehavior WORLD_STATIC =
	{
		.SelfMask = EBBColMask::WORLD_STATIC,
		.InteractMask = EBBColMask::ALL ^ (EBBColMask::WORLD_STATIC/* | OTHERS*/),
		.CollideMask = EBBColMask::ALL ^ (EBBColMask::WORLD_STATIC/* | OTHERS*/)
	};

	constexpr BBColBehavior WORLD_DYNAMIC =
	{
		.SelfMask = EBBColMask::WORLD_DYNAMIC,
		.InteractMask = EBBColMask::ALL,
		.CollideMask = EBBColMask::ALL /*^ (EBBColMask::WORLD_STATIC | OTHERS)*/
	};

	constexpr BBColBehavior BRICK =
	{
		.SelfMask = EBBColMask::BRICK,
		.InteractMask = EBBColMask::ALL,
		.CollideMask = EBBColMask::BALL | EBBColMask::PLAYER_PROJECTILE
	};

	constexpr BBColBehavior BALL =
	{
		.SelfMask = EBBColMask::BALL,
		.InteractMask = EBBColMask::ALL^ (EBBColMask::BRICK /* | OTHERS */ ),
		.CollideMask = EBBColMask::ALL ^ (EBBColMask::NONE/*EBBColMask::BRICK  | OTHERS */)//Ball SAT collision not yet implemented // EBBColMask::BALL | EBBColMask::BRICK | EBBColMask::WORLD_STATIC | EBBColMask::WORLD_DYNAMIC | EBBColMask::PADDLE
	};

	constexpr BBColBehavior PLAYER_PROJECTILE =
	{
		.SelfMask = EBBColMask::PLAYER_PROJECTILE,
		.InteractMask = EBBColMask::WORLD_STATIC | EBBColMask::WORLD_DYNAMIC | EBBColMask::BRICK,
		.CollideMask = EBBColMask::WORLD_STATIC | EBBColMask::WORLD_DYNAMIC | EBBColMask::BRICK
	};

	constexpr BBColBehavior PADDLE =
	{
		.SelfMask = EBBColMask::PADDLE,
		.InteractMask = EBBColMask::BALL | EBBColMask::PADDLE_ITEM,
		.CollideMask = EBBColMask::BALL | EBBColMask::PADDLE_ITEM
	};

	constexpr BBColBehavior PADDLE_ITEM =
	{
		.SelfMask = EBBColMask::PADDLE_ITEM,
		.InteractMask = EBBColMask::PADDLE,
		.CollideMask = EBBColMask::PADDLE
	};
	constexpr BBColBehavior GAMEBOARD_BORDER =
	{
		.SelfMask = EBBColMask::GAMEBOARD_BORDER,
		.InteractMask = EBBColMask::NONE,
		.CollideMask = EBBColMask::BALL
	};

	constexpr BBColBehavior GAMEBOARD_DEADZONE =
	{
		.SelfMask = EBBColMask::GAMEBOARD_DEADZONE,
		.InteractMask = EBBColMask::BALL | EBBColMask::PADDLE_ITEM,
		.CollideMask = EBBColMask::NONE
	};
#if DEBUG_PHYSIC
	constexpr EBBColMask DEBUG_PHYSIC_MASK =
		EBBColMask::NONE
#if DEBUG_PHYSIC_WORLD_STATIC
		| EBBColMask::WORLD_STATIC
#endif
#if DEBUG_PHYSIC_WORLD_DYNAMIC
		| EBBColMask::WORLD_DYNAMIC
#endif
#if DEBUG_BRICK
		| EBBColMask::BRICK
#endif
#if DEBUG_BALL
		| EBBColMask::BALL
#endif
#if DEBUG_PLAYER_PROJECTILE
		| EBBColMask::PLAYER_PROJECTILE
#endif
#if DEBUG_PADDLE
		| EBBColMask::PADDLE
#endif
#if DEBUG_PADDLE_ITEM
		| EBBColMask::PADDLE_ITEM
#endif
#if DEBUG_GAMEBOARD_BORDER
		| EBBColMask::GAMEBOARD_BORDER
#endif
		;
#endif
};