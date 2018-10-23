/**************************************************************************
*	HyDiagOutput.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Diagnostics/Output/HyDiagOutput.h"
#include "HyEngine.h"

#define HYDIAG_WIDTH 600.0f

HyDiagOutput::HyDiagOutput() :	m_dFrameTime_Low(9999.0),
								m_dFrameTime_High(0.0),
								m_dFrameTime_Cumulative(0.0),
								m_uiFrameCount(0),
								m_txtLastFrameTime(HY_SYSTEM_FONT, this),
								m_txtAvgFrame(HY_SYSTEM_FONT, this),
								m_txtAvgFrameLow(HY_SYSTEM_FONT, this),
								m_txtAvgFrameHigh(HY_SYSTEM_FONT, this),
								m_txtFps(HY_SYSTEM_FONT, this),
								m_txtMouse(HY_SYSTEM_FONT, this),
								m_uiShowFlags(HYDIAG_NONE)
{
	m_txtAvgFrame.pos.Y(-HY_SYSTEM_FONT_SIZE);
	m_txtAvgFrameLow.pos.Y(-HY_SYSTEM_FONT_SIZE * 2.0f);
	m_txtAvgFrameHigh.pos.Y(-HY_SYSTEM_FONT_SIZE * 3.0f);
	m_txtFps.pos.Y(-HY_SYSTEM_FONT_SIZE * 4.0f);
	
	m_txtMouse.pos.Y(-HY_SYSTEM_FONT_SIZE * 5.0f);

	SetDisplayOrder(HY_SYSTEM_FONT_DISPLAYORDER);
	SetShowFlags(HYDIAG_NONE);
}

HyDiagOutput::~HyDiagOutput()
{
}

void HyDiagOutput::SetShowFlags(uint32 uiDiagFlags)
{
	m_uiShowFlags = uiDiagFlags;
	SetEnabled(m_uiShowFlags != 0);
}

uint32 HyDiagOutput::GetShowFlags()
{
	return m_uiShowFlags;
}

void HyDiagOutput::ApplyTimeDelta(double dTimeDelta)
{
	m_dFrameTime_Cumulative += dTimeDelta;
	
	dTimeDelta *= 1000.0;
	m_dFrameTime_Low = HyMin(m_dFrameTime_Low, dTimeDelta);
	m_dFrameTime_High = HyMax(m_dFrameTime_High, dTimeDelta);
	
	m_uiFrameCount++;

	if(IsEnabled())
	{
		std::stringstream ss;
		ss.precision(3);
		ss << "Cur : " << dTimeDelta << "ms";
		m_txtLastFrameTime.TextSet(ss.str());

		if(m_dFrameTime_Cumulative >= 1.0)
		{
			ss.str("");
			ss.clear();
			ss << "Avg : " << static_cast<double>(m_dFrameTime_Cumulative / m_uiFrameCount) * 1000.0 << "ms";
			m_txtAvgFrame.TextSet(ss.str());

			ss.str("");
			ss.clear();
			ss << "Low : " << m_dFrameTime_Low << "ms";
			m_txtAvgFrameLow.TextSet(ss.str());

			ss.str("");
			ss.clear();
			ss << "High: " << m_dFrameTime_High << "ms";
			m_txtAvgFrameHigh.TextSet(ss.str());

			ss.str("");
			ss.clear();
			ss << "FPS : " << m_uiFrameCount;
			m_txtFps.TextSet(ss.str());

			// Collect a new sample
			m_dFrameTime_Low = 9999.0;	// Any large value that should be greater than any single frame time
			m_dFrameTime_High = 0.0;
			m_dFrameTime_Cumulative = 0.0;
			m_uiFrameCount = 0;
		}

		HyInput &inputRef = Hy_Input();
		glm::vec2 ptMousePos = inputRef.GetWorldMousePos();
		if(inputRef.IsMouseBtnDown(HYMOUSE_BtnLeft))
			m_txtMouse.TextSet("MOUSE DOWN\nX:" + std::to_string(ptMousePos.x) + " Y:" + std::to_string(ptMousePos.y));
		else
			m_txtMouse.TextSet("MOUSE UP\nX:" + std::to_string(ptMousePos.x) + " Y:" + std::to_string(ptMousePos.y));
	}
}
