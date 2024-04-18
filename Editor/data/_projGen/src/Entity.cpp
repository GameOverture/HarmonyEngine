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
	
	m_fTimelineFrameTime = 0.0f;
	m_uiTimelineFrame = 0;
	m_bTimelinePaused = false;
	
	m_fpTimelineUpdate();
	
	return true;
}

/*virtual*/ uint32 %HY_CLASS%::GetNumStates()
{
	return %HY_NUMSTATES%;
}

uint32 %HY_CLASS%::GetTimelineFrame() const
{
	return m_uiTimelineFrame;
}

float %HY_CLASS%::GetTimelineFrameDuration() const
{
	return m_fTIMELINE_FRAME_DURATION;
}
%HY_ACCESSORDEFINITION%

void %HY_CLASS%::SetTimelineFrame(uint32 uiFrameIndex)
{
	uiFrameIndex = HyMath::Min(uiFrameIndex, m_uiTimelineFinalFrame);

	// Extrapolate to frame 'uiFrameIndex'
	m_uiTimelineFrame = 0;
	bool bWasPaused = m_bTimelinePaused;
	m_bTimelinePaused = false;

	while(true)
	{
		if(m_uiTimelineFrame == uiFrameIndex)
		{
			m_fTimelineFrameTime = 0.0f;
			m_fpTimelineUpdate();
			break;
		}
		
		m_fTimelineFrameTime = 0.0f;
		m_fpTimelineUpdate();
		TimelineAdvance();
	}

	m_uiTimelineFrame = uiFrameIndex;
	m_bTimelinePaused = bWasPaused;
}

bool %HY_CLASS%::IsTimelinePaused() const
{
	return m_bTimelinePaused;
}

void %HY_CLASS%::SetTimelinePause(bool bPause)
{
	m_bTimelinePaused = bPause;
}

/*virtual*/ void %HY_CLASS%::Update() /*override*/
{
	m_fpTimelineUpdate();
	HyEntity2d::Update();
}

void %HY_CLASS%::TimelineAdvance()
{
%HY_TIMELINEADVANCEIMPL%
}

} // '%HY_NAMESPACE%' namespace
