/**************************************************************************
*	HyScrollBar.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Containers/HyScrollBar.h"

HyScrollBar::HyScrollBar(HyOrientation eOrientation, uint32 uiDiameter, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_eORIENTATION(eOrientation),
	m_Panel(this),
	m_Handle(uiDiameter, this),
	m_PosBtn(m_eORIENTATION, true, uiDiameter, this),
	m_NegBtn(m_eORIENTATION, false, uiDiameter, this),
	m_bIsValidMetrics(false)
{
	m_PosBtn.SetButtonClickedCallback(OnArrowBtnPressed, this);
	m_NegBtn.SetButtonClickedCallback(OnArrowBtnPressed, this);

	HyColor color;
	//m_Panel.SetTint(

	//m_Handle;
	//m_PosBtn;
	//m_NegBtn;
}

void HyScrollBar::SetMetrics(uint32 uiTotalLength, uint32 uiDiameter, uint32 uiClientTotalSize, uint32 uiClientShownSize)
{
	if(uiClientTotalSize == 0 || uiClientTotalSize <= uiClientShownSize)
	{
		m_bIsValidMetrics = false;
		return;
	}
	m_bIsValidMetrics = true;

	m_PosBtn.SetDiameter(uiDiameter);
	m_NegBtn.SetDiameter(uiDiameter);

	float fPercent = static_cast<float>(uiClientShownSize) / static_cast<float>(uiClientTotalSize);
	if(m_eORIENTATION == HYORIEN_Vertical)
	{
		m_Panel.SetAsBox(uiDiameter, uiTotalLength);
		m_Handle.panel.SetAsBox(static_cast<float>(uiDiameter), (uiTotalLength - (uiDiameter * 2.0f)) * fPercent);

		m_PosBtn.pos.Set(0, uiTotalLength - uiDiameter);
		m_NegBtn.pos.Set(0, 0);
	}
	else // HYORIEN_Horizontal
	{
		m_Panel.SetAsBox(uiTotalLength, uiDiameter);
		m_Handle.panel.SetAsBox((uiTotalLength - (uiDiameter * 2.0f)) * fPercent, static_cast<float>(uiDiameter));

		m_PosBtn.pos.Set(uiTotalLength - uiDiameter, 0);
		m_NegBtn.pos.Set(0, 0);
	}
}

bool HyScrollBar::IsValidMetrics() const
{
	return m_bIsValidMetrics;
}

/*static*/ void HyScrollBar::OnArrowBtnPressed(HyButton *pBtn, void *pData)
{
	HyScrollBar *pThis = static_cast<HyScrollBar *>(pData);

	if(pBtn == &pThis->m_PosBtn)
	{
	}
	else // m_NegBtn
	{
	}
}
