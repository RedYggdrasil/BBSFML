#pragma once
#include "App/Memory/IBBSceneObject.h"
//#include "App/Game/BBGameBoard.h"
namespace BB
{
	class BBScene;
	class BBGameBoard;
	class IBBGameBoardObject : public IBBSceneObject
	{
	protected:
		BBGameBoard* m_gameBoard;
	protected:
		IBBGameBoardObject(IBBGameBoardObject* InGameBoardObject);
	private:
		IBBGameBoardObject(IBBSceneObject* InSceneObject);
		friend BBGameBoard;
	public:
		R_VIRTUAL_IMPLICIT ~IBBGameBoardObject() R_OVERRIDE_IMPLICIT;
	};
};