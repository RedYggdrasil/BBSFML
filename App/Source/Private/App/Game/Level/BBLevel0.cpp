#include "App/Game/Level/BBLevel0.h"
#include "App/Defines/BBGameBoardDef.h"

using namespace BB;


static std::vector<BBBrickData> CreateLevel0Vector();
static const std::vector<BBBrickData> BB::Level::Level0 = CreateLevel0Vector();

static constexpr size_t NB_X_ROWS = 5;
static constexpr size_t NB_Y_ROWS = 2;

constexpr std::array<std::array<BBBrickData, NB_X_ROWS>, NB_Y_ROWS> CreateLevel0()
{
	constexpr sf::Vector2f DEFAULT_BRICK_SIZE = sf::Vector2f(400.f, 128.f) / 2.f;

	constexpr float START_X = 0.f;//F_BOARD_RECT.position.x ;
	constexpr float END_X = START_X + F_BOARD_RECT.size.x;

	constexpr float STEP_X = (END_X - START_X) / (float)(NB_X_ROWS);

	constexpr float DEFAULT_TILE_HALF_WIDTH = 100.f;

	constexpr int START_Y = I_BOARD_RECT.position.y + 100;
	constexpr int STEP_Y = 200;



	BBBrickData A = BBBrickData
	{
		.m_size = sf::Vector2f(400 * 1, 128 * 1) / 2.f,
		.m_pos = sf::Vector2f(0.f, 0.f),
		.m_rotDegree = 0.f,
		.TextureName = "RectBrick0",
		.LifePoints = 1,
		.PhysIndex = 0,
		.GraphIndex = 0
	};

	BBBrickData B = BBBrickData
	{
		.m_size = sf::Vector2f(200 * 1, 128 * 1) / 2.f,
		.m_pos = sf::Vector2f(0.f, 0.f),
		.m_rotDegree = 0.f,
		.TextureName = "RectBrick0",
		.LifePoints = 1,
		.PhysIndex = 0,
		.GraphIndex = 0
	};

	std::array<std::array<BBBrickData, NB_X_ROWS>, NB_Y_ROWS> arr =
	{
		std::array<BBBrickData, NB_X_ROWS>{ A, A, B, A, A},//, A, A, A, A, A },
		std::array<BBBrickData, NB_X_ROWS>{ A, B, A, B, A},// A, A, A, A, A }
	};

	std::array<std::array<BBBrickData, NB_X_ROWS>, NB_Y_ROWS> arr2 =
	{
		std::array<BBBrickData, NB_X_ROWS>{ A, A, A, A, A},//, A, A, A, A, A },
		std::array<BBBrickData, NB_X_ROWS>{ A, A, A, A, A},// A, A, A, A, A }
	};
	
	for (size_t y = 0; y < arr.size(); ++y)
	{
		for (size_t x = 0; x < arr[y].size(); ++x)
		{
			arr[y][x].m_pos = sf::Vector2f
			(
				(float)((float)START_X + DEFAULT_TILE_HALF_WIDTH + ((float)(STEP_X * (int)x) + (arr[y][x].m_size.x * 0.5f))),
				(float)((float)START_Y + ((float)(STEP_Y * (int)y) + (arr[y][x].m_size.y * 0.5f)))
			);
		}
	}

	return arr;
}

std::vector<BBBrickData> CreateLevel0Vector()
{
	static const std::array<std::array<BBBrickData, NB_X_ROWS>, NB_Y_ROWS> level0Arr =  CreateLevel0();

	std::vector<BBBrickData> bricks;
	bricks.reserve(level0Arr.size() * level0Arr[0].size());
	for (size_t x = 0; x < level0Arr.size(); ++x)
	{
		for (size_t y = 0; y < level0Arr[x].size(); ++y)
		{
			bricks.push_back(level0Arr[x][y]);
		}
	}
	return bricks;
}
