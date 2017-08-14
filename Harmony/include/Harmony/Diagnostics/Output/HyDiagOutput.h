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
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Scene/Nodes/Leafs/Draws/HyText2d.h"
#include "Scene/Nodes/Leafs/Draws/HyPrimitive2d.h"

#define HY_SYSTEM_FONT "Lg", "Default"
#define HY_SYSTEM_FONT_SIZE 25.0f
#define HY_SYSTEM_FONT_STATE 0
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

	//HyProfiler				m_Profiler;

public:
	HyDiagOutput();
	virtual ~HyDiagOutput();

	void SetShowFlags(uint32 uiDiagFlags);

	void ApplyTimeDelta(double dTimeDelta);

	void ProfileBegin(const char *szName);
	void ProfileEnd();
};

#endif /* HyDiagOutput_h__ */
