/********************************************************************************
** Auto generated class by Harmony Engine - Editor Tool
**
** WARNING! All changes made in this file will be lost when resaving the entity!
********************************************************************************/
#ifndef %HY_FILENAME%_h__
#define %HY_FILENAME%_h__

#include "pch.h"
%HY_INCLUDES%
namespace %HY_NAMESPACE% {

%HY_STATEENUMS%
class %HY_CLASS%%HY_BASECLASSDECL%
{
protected:
	const float				m_fTIMELINE_FRAME_DURATION;
	std::function<void()>	m_fpTimelineUpdate;
	float					m_fTimelineFrameTime;
	uint32					m_uiTimelineFrame;
	bool					m_bTimelinePaused;
	uint32					m_uiTimelineFinalFrame;

%HY_MEMBERVARIABLES%
public:
	%HY_CLASS%(%HY_CLASSCTORSIG%);
	virtual ~%HY_CLASS%();
	
	using IHyLoadable::GetState;
	virtual bool SetState(uint32 uiStateIndex) override;
	virtual uint32 GetNumStates() override;
	
	uint32 GetTimelineFrame() const;
	float GetTimelineFrameDuration() const;
%HY_ACCESSORDECL%
	
	void SetTimelineFrame(uint32 uiFrameIndex);
	
	bool IsTimelinePaused() const;
	void SetTimelinePause(bool bPause);
	
protected:
	virtual void Update() override;
	
	void TimelineAdvance();
};

} // '%HY_NAMESPACE%' namespace

#endif // %HY_FILENAME%_h__
