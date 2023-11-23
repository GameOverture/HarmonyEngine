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

class %HY_CLASS%%HY_BASECLASSDECL%
{
protected:
	const float				m_fFRAME_DURATION;
	std::function<void()>	m_fpUpdateFunc;
	float					m_fElapsedFrameTime;
	uint32					m_uiCurFrame;

%HY_MEMBERVARIABLES%
public:
	%HY_CLASS%(%HY_CLASSCTORSIG%);
	virtual ~%HY_CLASS%();
	
	virtual bool SetState(uint32 uiStateIndex) override;
	virtual uint32 GetNumStates() override;
	
protected:
	virtual void Update() override;
};

} // '%HY_NAMESPACE%' namespace

#endif // %HY_FILENAME%_h__
