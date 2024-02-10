/********************************************************************************
** Auto generated class by Harmony Engine - Editor Tool
**
** WARNING! All changes made in this file will be lost when resaving the entity!
********************************************************************************/
#include "pch.h"
#include "%HY_FILENAME%.h"

namespace %HY_NAMESPACE% {

%HY_CLASS%::%HY_CLASS%(%HY_CLASSCTORSIG%)%HY_CLASSMEMBERINITIALIZERLIST%
{
	%HY_CTORIMPL%
}

/*virtual*/ %HY_CLASS%::~%HY_CLASS%()
{
}

/*virtual*/ bool %HY_CLASS%::SetState(uint32 uiStateIndex)
{
	if(%HY_BASECLASS%::SetState(uiStateIndex) == false || uiStateIndex >= GetNumStates())
		return false;
	
	%HY_SETSTATEIMPL%
	
	m_fElapsedFrameTime = 0.0f;
	m_uiCurFrame = 0;
	
	return true;
}

/*virtual*/ uint32 %HY_CLASS%::GetNumStates()
{
	return %HY_NUMSTATES%;
}

/*virtual*/ void %HY_CLASS%::Update() /*override*/
{
	m_fpUpdateFunc();
	HyEntity2d::Update();
}

} // '%HY_NAMESPACE%' namespace
