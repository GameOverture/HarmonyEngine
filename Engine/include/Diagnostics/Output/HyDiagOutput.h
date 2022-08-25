/**************************************************************************
*	HyDiagOutput.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyDiagOutput_h__
#define HyDiagOutput_h__

#include "Afx/HyStdAfx.h"
#include "Diagnostics/Output/HyProfiler.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

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

	uint32					m_uiShowFlags;

public:
	HyDiagOutput();
	virtual ~HyDiagOutput();

	void InitText(std::string sPrefix, std::string sName, uint32 uiTextState);

	void SetShowFlags(uint32 uiDiagFlags);
	uint32 GetShowFlags();

	void ApplyTimeDelta(double dTimeDelta);
};

#endif /* HyDiagOutput_h__ */
