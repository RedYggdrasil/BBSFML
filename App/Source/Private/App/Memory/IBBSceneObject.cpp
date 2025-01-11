#include "App/Memory/IBBSceneObject.h"
#include "App/GameSystem/BBScene.h"

using namespace BB;

using Super = IBBProgramObject;
IBBSceneObject::IBBSceneObject(IBBSceneObject* InSceneObject)
: Super(InSceneObject), m_scene(InSceneObject->m_scene)
{
}

IBBSceneObject::IBBSceneObject(IBBProgramObject* InProgramObject)
: Super(InProgramObject), m_scene(static_cast<BBScene*>(this))
{
}

BB::IBBSceneObject::~IBBSceneObject()
{

}