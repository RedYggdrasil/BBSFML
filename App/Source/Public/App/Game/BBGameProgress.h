#pragma once

namespace BB
{
	struct BBGameProgress
	{
	public:
		size_t ClearedLevelsNumber;
		size_t ClearedLevelsScore;
		size_t ClearedLevelsBrickDestroyed;

		size_t CurrentThisLevel;
		size_t ScoreThisLevel;
		size_t BrickDestroyedThisLevel;
	public:
		void LevelCleared();
		static BBGameProgress CreateEmptyProgress();

	};
}