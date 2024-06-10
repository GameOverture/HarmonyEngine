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
	
	m_uiTimelineFrame = 0;
	m_fpTimelineUpdate();
	
	m_fTimelineFrameTime = -m_fTIMELINE_FRAME_DURATION;
	
	return true;
}

/*virtual*/ uint32 %HY_CLASS%::GetNumStates()
{
	return %HY_NUMSTATES%;
}

float %HY_CLASS%::GetTimelineFrameDuration() const
{
	return m_fTIMELINE_FRAME_DURATION;
}

uint32 %HY_CLASS%::GetTimelineFrame() const
{
	return m_uiTimelineFrame;
}

void %HY_CLASS%::SetTimelineFrame(uint32 uiFrameIndex)
{
	m_uiTimelineFrame = HyMath::Min(uiFrameIndex, m_uiTimelineFinalFrame);
	m_fTimelineFrameTime = -m_fTIMELINE_FRAME_DURATION;
}

void %HY_CLASS%::ExtrapolateTimelineFrame(uint32 uiFrameIndex)
{
	uiFrameIndex = HyMath::Min(uiFrameIndex, m_uiTimelineFinalFrame);

	// Extrapolate to frame 'uiFrameIndex'
	m_uiTimelineFrame = 0;
	while(true)
	{
		m_fpTimelineUpdate();
		if(m_uiTimelineFrame == uiFrameIndex)
			break;
		
		// Advance timeline by 1 frame
%HY_TIMELINEADVANCEIMPL%
		
		m_uiTimelineFrame++;
	}
	
	m_fTimelineFrameTime = -m_fTIMELINE_FRAME_DURATION;
}

bool %HY_CLASS%::IsTimelinePaused() const
{
	return m_bTimelinePaused;
}

bool %HY_CLASS%::IsTimelineFinished() const
{
	return m_uiTimelineFrame == m_uiTimelineFinalFrame;
}

void %HY_CLASS%::SetTimelinePause(bool bPause)
{
	m_bTimelinePaused = bPause;
}

%HY_ACCESSORDEFINITION%

/*virtual*/ void %HY_CLASS%::Update() /*override*/
{
	if(m_bTimelinePaused == false && m_uiTimelineFrame < m_uiTimelineFinalFrame)
	{
		m_fTimelineFrameTime += HyEngine::DeltaTime();
		while(m_fTimelineFrameTime >= 0.0f && m_uiTimelineFrame < m_uiTimelineFinalFrame)
		{
			m_uiTimelineFrame++;
			m_fpTimelineUpdate();
			
			m_fTimelineFrameTime -= m_fTIMELINE_FRAME_DURATION;
		}
	}
	
	HyEntity2d::Update();
}

} // '%HY_NAMESPACE%' namespace
