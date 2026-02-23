/**************************************************************************
*	HyProfiler.cpp
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Diagnostics/HyProfiler.h"
#include "HyEngine.h"

#define HY_DIAGNOSTICS_DISPLAYORDER 0x7FFFFF00

HyProfiler::HyProfiler() :
	HyGui(HYORIENT_Vertical, HyUiPanelInit()),
	m_dFrameTime_Low(9999.0),
	m_dFrameTime_High(0.0),
	m_dFrameTime_Cumulative(0.0),
	m_uiFrameCount(0),
	m_uiUpdateCount(0),
	m_dUpdateTimeStamp(0.0),
	m_dPrepTimeStamp(0.0),
	m_dRenderTimeStamp(0.0),
	m_uiShowFlags(HYDIAG_NONE)
{
	UseWindowCoordinates(0);
	SetDisplayOrder(HY_DIAGNOSTICS_DISPLAYORDER);
	SetShowFlags(HYDIAG_NONE);
}

/*virtual*/ HyProfiler::~HyProfiler()
{
}

void HyProfiler::InitText(const HyNodePath &nodePath, uint32 uiTextState)
{
	m_txtFps.Setup(HyUiPanelInit(), HyUiTextInit(nodePath));
	m_txtFps.SetTextState(uiTextState);

	m_txtFrameTimes.Setup(HyUiPanelInit(), HyUiTextInit(nodePath));
	m_txtFrameTimes.SetTextState(uiTextState);

	m_txtMouse.Setup(HyUiPanelInit(), HyUiTextInit(nodePath));
	m_txtMouse.SetTextState(uiTextState);

	m_txtMouseWorld.Setup(HyUiPanelInit(), HyUiTextInit(nodePath));
	m_txtMouseWorld.SetTextState(uiTextState);

	m_txtMouseBtns.Setup(HyUiPanelInit(), HyUiTextInit(nodePath));
	m_txtMouseBtns.SetTextState(uiTextState);
}

void HyProfiler::SetShowFlags(uint32 uiDiagFlags)
{
	m_uiShowFlags = uiDiagFlags;
	SetVisible(m_uiShowFlags != 0);

	ClearItems();

	InsertSpacer(HYSIZEPOLICY_Fixed, 50);

	m_txtFps.SetVisible(m_uiShowFlags & HYDIAG_Fps);
	if(m_uiShowFlags & HYDIAG_Fps)
		InsertWidget(m_txtFps);

	m_txtFrameTimes.SetVisible(m_uiShowFlags & HYDIAG_FrameTimes);
	if(m_uiShowFlags & HYDIAG_FrameTimes)
		InsertWidget(m_txtFrameTimes);

	m_Graph.SetVisible(m_uiShowFlags & HYDIAG_Graph);
	if(m_uiShowFlags & HYDIAG_Graph)
		InsertWidget(m_Graph);

	m_txtMouse.SetVisible(m_uiShowFlags & HYDIAG_Mouse);
	if(m_uiShowFlags & HYDIAG_Mouse)
		InsertWidget(m_txtMouse);

	m_txtMouseWorld.SetVisible(m_uiShowFlags & HYDIAG_MouseWorld);
	if(m_uiShowFlags & HYDIAG_MouseWorld)
		InsertWidget(m_txtMouseWorld);

	m_txtMouseBtns.SetVisible(m_uiShowFlags & HYDIAG_MouseButtons);
	if(m_uiShowFlags & HYDIAG_MouseButtons)
		InsertWidget(m_txtMouseBtns);

	InsertSpacer();
}

uint32 HyProfiler::GetShowFlags()
{
	return m_uiShowFlags;
}

void HyProfiler::BeginFrame(const HyTime &timeRef)
{
	m_uiFrameCount++;

	double dFrameDelta = timeRef.GetFrameDelta();
	m_dFrameTime_Cumulative += dFrameDelta;
	m_dFrameTime_Low = HyMath::Min(m_dFrameTime_Low, dFrameDelta);
	m_dFrameTime_High = HyMath::Max(m_dFrameTime_High, dFrameDelta);

	if(m_dFrameTime_Cumulative > 1.0)
	{
		m_txtFps.SetText(std::stringstream() << "FPS " << m_uiFrameCount << " (" << m_uiUpdateCount << ")");

		std::stringstream ss;
		ss.precision(3);
		ss << "AVG " << (m_dFrameTime_Cumulative / m_uiFrameCount) * 1000.0 << "ms";
		ss << " LOW " << m_dFrameTime_Low * 1000.0 << "ms";
		ss << " HI " << m_dFrameTime_High * 1000.0 << "ms";
		
		m_txtFrameTimes.SetText(ss);

		// Collect a new sample
		m_dFrameTime_Cumulative = 0.0;
		m_dFrameTime_Low = 9999.0;	// Any large value that should be greater than any single frame time
		m_dFrameTime_High = 0.0;
		m_uiFrameCount = 0;
		m_uiUpdateCount = 0;
		
		m_Graph.Flush();
	}

	// Submit differences in time stamps
	m_Graph.AddDeltas(m_dPrepTimeStamp - m_dUpdateTimeStamp, m_dRenderTimeStamp - m_dPrepTimeStamp, timeRef.GetTotalElapsedTime() - m_dRenderTimeStamp);

	// Start of Update while loop
	m_dUpdateTimeStamp = timeRef.GetTotalElapsedTime();
}

void HyProfiler::BeginUpdate(const HyTime &timeRef)
{
	m_uiUpdateCount++;
}

void HyProfiler::BeginRenderPrep(const HyTime &timeRef)
{
	// Start of Render prep
	m_dPrepTimeStamp = timeRef.GetTotalElapsedTime();
}

void HyProfiler::BeginRender(const HyTime &timeRef)
{
	// Start of Render
	m_dRenderTimeStamp = timeRef.GetTotalElapsedTime();

	if(m_uiShowFlags & HYDIAG_INPUT)
	{
		HyInput &inputRef = HyEngine::Input();

		glm::vec2 ptMousePos = inputRef.GetMousePos();
		m_txtMouse.SetText("MOUSE: " + std::to_string(static_cast<int32>(ptMousePos.x)) + " " + std::to_string(static_cast<int32>(ptMousePos.y)));

		glm::vec2 ptWorldMousePos;
		if(inputRef.GetWorldMousePos(ptWorldMousePos))
			m_txtMouseWorld.SetText("WORLD: " + std::to_string(static_cast<int32>(ptWorldMousePos.x)) + " " + std::to_string(static_cast<int32>(ptWorldMousePos.y)));
		else
			m_txtMouseWorld.SetText("WORLD MOUSE INVALID");

		if(m_uiShowFlags & HYDIAG_MouseButtons)
		{
			std::string sMouseBtns = "MOUSE BTNS: ";
			if(inputRef.IsMouseBtnDown(HYMOUSE_BtnLeft))
				sMouseBtns += "L ";
			if(inputRef.IsMouseBtnDown(HYMOUSE_BtnRight))
				sMouseBtns += "R ";
			if(inputRef.IsMouseBtnDown(HYMOUSE_BtnMiddle))
				sMouseBtns += "M ";
			if(inputRef.IsMouseBtnDown(HYMOUSE_Btn4))
				sMouseBtns += "4 ";
			if(inputRef.IsMouseBtnDown(HYMOUSE_Btn5))
				sMouseBtns += "5 ";
			m_txtMouseBtns.SetText(sMouseBtns);
		}
	}
}

