#include "App/Game/IBBGameBoardObject.h"
#include "App/GameSystem/BBScene.h"
#include "App/Game/BBGameBoard.h"

using namespace BB;

using Super = IBBSceneObject;
IBBGameBoardObject::IBBGameBoardObject(IBBGameBoardObject* InGameBoardObject)
	: Super(InGameBoardObject), m_gameBoard(InGameBoardObject->m_gameBoard)
{
}

IBBGameBoardObject::IBBGameBoardObject(IBBSceneObject* InSceneObject)
	: Super(InSceneObject), m_gameBoard(static_cast<BBGameBoard*>(this))
{
}

BB::IBBGameBoardObject::~IBBGameBoardObject()
{

}