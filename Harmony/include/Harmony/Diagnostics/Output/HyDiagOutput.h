/**************************************************************************
*	HyDiagOutput.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyDiagOutput_h__
#define HyDiagOutput_h__

#include "Afx/HyStdAfx.h"
#include "Diagnostics/Output/HyProfiler.h"
#include "Scene/Nodes/Draws/Entities/HyEntity2d.h"
#include "Scene/Nodes/Draws/Instances/HyText2d.h"
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"

#define HY_SYSTEM_FONT "+Hy", "+HyFont"
#define HY_SYSTEM_FONT_SIZE 16.0f
#define HY_SYSTEM_FONT_DISPLAYORDER 0x7FFFFF00

class HyDiagOutput : public HyEntity2d
{
	double					m_dFrameTime_Low;
	double					m_dFrameTime_High;
	double					m_dFrameTime_Cumulative;
	uint32					m_uiFrameCount;

	HyText2d				m_txtLastFrameTime;
	HyText2d				m_txtAvgFrame;
	HyText2d				m_txtAvgFrameLow;
	HyText2d				m_txtAvgFrameHigh;
	HyText2d				m_txtFps;

	HyText2d				m_txtMouse;

	//HyProfiler				m_Profiler;
	uint32					m_uiShowFlags;

public:
	HyDiagOutput();
	virtual ~HyDiagOutput();

	void SetShowFlags(uint32 uiDiagFlags);
	uint32 GetShowFlags();

	void ApplyTimeDelta(double dTimeDelta);

	void ProfileBegin(const char *szName);
	void ProfileEnd();
};

#endif /* HyDiagOutput_h__ */
