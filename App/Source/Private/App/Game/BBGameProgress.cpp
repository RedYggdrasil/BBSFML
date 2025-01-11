#include "App/Game/BBGameProgress.h"

using namespace BB;

void BB::BBGameProgress::LevelCleared()
{
	++ClearedLevelsNumber;
	ClearedLevelsScore += ScoreThisLevel;
	ClearedLevelsBrickDestroyed += BrickDestroyedThisLevel;

	++CurrentThisLevel;
	ScoreThisLevel = 0;
	BrickDestroyedThisLevel = 0;
}

BBGameProgress BBGameProgress::CreateEmptyProgress()
{
	return BBGameProgress
	{
		.ClearedLevelsNumber = 0,
		.ClearedLevelsScore = 0,
		.ClearedLevelsBrickDestroyed = 0,

		.CurrentThisLevel = 1,
		.ScoreThisLevel = 0,
		.BrickDestroyedThisLevel = 0
	};
}
