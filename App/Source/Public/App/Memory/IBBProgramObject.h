#pragma once

namespace BB
{
	class BBProgramInstance;
	class IBBProgramObject
	{
	protected:
		BBProgramInstance* m_programInstance;
	protected:
		IBBProgramObject(IBBProgramObject* InProgramInstance);
	private:
		IBBProgramObject();
		friend BBProgramInstance;
	public:
		virtual ~IBBProgramObject();
	};
};