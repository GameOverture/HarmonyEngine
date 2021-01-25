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

extern HyInput &Hy_Input();

#define HYDIAG_WIDTH 600.0f

HyDiagOutput::HyDiagOutput() :
	m_dFrameTime_Low(9999.0),
	m_dFrameTime_High(0.0),
	m_dFrameTime_Cumulative(0.0),
	m_uiFrameCount(0),
	m_uiShowFlags(HYDIAG_NONE)
{
	m_txtLastFrameTime.Init(HY_SYSTEM_FONT, this);
	m_txtAvgFrame.Init(HY_SYSTEM_FONT, this);
	m_txtAvgFrameLow.Init(HY_SYSTEM_FONT, this);
	m_txtAvgFrameHigh.Init(HY_SYSTEM_FONT, this);
	m_txtFps.Init(HY_SYSTEM_FONT, this);
	m_txtMouse.Init(HY_SYSTEM_FONT, this);

	SetDisplayOrder(HY_SYSTEM_FONT_DISPLAYORDER);
	SetShowFlags(HYDIAG_NONE);
}

HyDiagOutput::~HyDiagOutput()
{
}

void HyDiagOutput::SetShowFlags(uint32 uiDiagFlags)
{
	m_uiShowFlags = uiDiagFlags;
	SetVisible(m_uiShowFlags != 0);
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

	if(IsVisible())
	{
		float fOffsetY = -HY_SYSTEM_FONT_SIZE;

		std::stringstream ss;
		ss.precision(3);

		if((m_uiShowFlags & HYDIAG_LastFrameTime) != 0)
		{
			ss << "Cur : " << dTimeDelta << "ms";
			m_txtLastFrameTime.SetText(ss.str());

			m_txtLastFrameTime.pos.Y(fOffsetY);
			fOffsetY -= HY_SYSTEM_FONT_SIZE;
		}

		if(m_dFrameTime_Cumulative >= 1.0)
		{
			if((m_uiShowFlags & HYDIAG_AvgFrameTimes) != 0)
			{
				ss.str("");
				ss.clear();
				ss << "Avg : " << static_cast<double>(m_dFrameTime_Cumulative / m_uiFrameCount) * 1000.0 << "ms";
				m_txtAvgFrame.SetText(ss.str());
				m_txtAvgFrame.pos.Y(fOffsetY);
				fOffsetY -= HY_SYSTEM_FONT_SIZE;

				ss.str("");
				ss.clear();
				ss << "Low : " << m_dFrameTime_Low << "ms";
				m_txtAvgFrameLow.SetText(ss.str());
				m_txtAvgFrameLow.pos.Y(fOffsetY);
				fOffsetY -= HY_SYSTEM_FONT_SIZE;

				ss.str("");
				ss.clear();
				ss << "High: " << m_dFrameTime_High << "ms";
				m_txtAvgFrameHigh.SetText(ss.str());
				m_txtAvgFrameHigh.pos.Y(fOffsetY);
				fOffsetY -= HY_SYSTEM_FONT_SIZE;
			}

			if((m_uiShowFlags & HYDIAG_Fps) != 0)
			{
				ss.str("");
				ss.clear();
				ss << "FPS : " << m_uiFrameCount;
				m_txtFps.SetText(ss.str());
				m_txtFps.pos.Y(fOffsetY);
				fOffsetY -= HY_SYSTEM_FONT_SIZE;
			}

			// Collect a new sample
			m_dFrameTime_Low = 9999.0;	// Any large value that should be greater than any single frame time
			m_dFrameTime_High = 0.0;
			m_dFrameTime_Cumulative = 0.0;
			m_uiFrameCount = 0;
		}

		if((m_uiShowFlags & HYDIAG_Mouse) != 0)
		{
			HyInput &inputRef = Hy_Input();
			glm::vec2 ptMousePos = inputRef.GetWorldMousePos();
			if(inputRef.IsMouseBtnDown(HYMOUSE_BtnLeft))
				m_txtMouse.SetText("MOUSE DOWN X:" + std::to_string(ptMousePos.x) + " Y:" + std::to_string(ptMousePos.y));
			else
				m_txtMouse.SetText("MOUSE UP X:" + std::to_string(ptMousePos.x) + " Y:" + std::to_string(ptMousePos.y));

			m_txtMouse.pos.Y(fOffsetY);
			fOffsetY -= HY_SYSTEM_FONT_SIZE;
		}
	}
}
