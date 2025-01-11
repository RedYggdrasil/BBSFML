#include "App/Memory/IBBProgramObject.h"
#include "App/GameSystem/BBProgramInstance.h"

using namespace BB;

IBBProgramObject::IBBProgramObject(IBBProgramObject* InProgramInstance)
: m_programInstance(InProgramInstance->m_programInstance)
{
}
IBBProgramObject::IBBProgramObject()
	: m_programInstance(static_cast<BBProgramInstance*>(this))
{
}

BB::IBBProgramObject::~IBBProgramObject()
{

}
