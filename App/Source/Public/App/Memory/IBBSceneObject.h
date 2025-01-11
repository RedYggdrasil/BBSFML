#pragma once
#include "App/Memory/IBBProgramObject.h"

namespace BB
{
	class BBScene;
	class IBBSceneObject : public IBBProgramObject
	{
	protected:
		BBScene* m_scene;
	protected:
		IBBSceneObject(IBBSceneObject* InSceneObject);
	private:
		IBBSceneObject(IBBProgramObject* InProgramObject);
		friend BBScene;
	public:
		template <class TScene = BBScene>
		typename std::enable_if<std::is_base_of<BBScene, TScene>::value, TScene*>::type
		GetScene() const
		{
			return static_cast<TScene*>(m_scene);
		}
		R_VIRTUAL_IMPLICIT ~IBBSceneObject() R_OVERRIDE_IMPLICIT;
	};
};