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
	m_PageControl(eOrientation, uiDiameter, uiDiameter, this),
	m_Slider(eOrientation, uiDiameter, uiDiameter, this),
	m_PosBtn(m_eORIENTATION, true, uiDiameter, this),
	m_NegBtn(m_eORIENTATION, false, uiDiameter, this),
	m_fLineScrollAmt(20.0f),
	m_bIsValidMetrics(false),
	m_fScrollPos(0.0f),
	m_AnimScrollPos(m_fScrollPos, *this, 0),
	m_fpCallback(nullptr),
	m_pCallbackData(nullptr)
{
	m_PosBtn.SetButtonClickedCallback(OnArrowBtnPressed, this);
	m_PosBtn.SetAsEnabled(true);
	m_NegBtn.SetButtonClickedCallback(OnArrowBtnPressed, this);
	m_NegBtn.SetAsEnabled(true);
}

HyOrientation HyScrollBar::GetOrientation() const
{
	return m_eORIENTATION;
}

void HyScrollBar::SetColor(HyColor color)
{
	m_PageControl.SetColor(color);
	if(color.IsLight())
		m_Slider.SetColor(color.Darken());
	else
		m_Slider.SetColor(color.Lighten());
	m_PosBtn.SetColor(color);
	m_NegBtn.SetColor(color);
}

void HyScrollBar::SetMetrics(uint32 uiLength, uint32 uiDiameter, float fClientTotalSize, float fClientShownSize)
{
	if(fClientTotalSize <= 0.0f || fClientTotalSize <= fClientShownSize)
	{
		m_bIsValidMetrics = false;
		return;
	}
	m_bIsValidMetrics = true;

	m_fClientTotalSize = fClientTotalSize;
	m_fClientShownSize = fClientShownSize;
	float fPercent = m_fClientShownSize / m_fClientTotalSize;

	m_PageControl.SetMetrics(m_eORIENTATION, uiDiameter, (uiLength - (uiDiameter * 2)));
	m_Slider.SetMetrics(m_eORIENTATION, uiDiameter, (uiLength - (uiDiameter * 2)) * fPercent);
	m_PosBtn.SetMetrics(m_eORIENTATION, true, uiDiameter);
	m_NegBtn.SetMetrics(m_eORIENTATION, false, uiDiameter);

	if(m_eORIENTATION == HYORIEN_Vertical)
	{
		m_PageControl.pos.Set(0, uiDiameter);
		m_PosBtn.pos.Set(0, uiLength - uiDiameter);
		m_NegBtn.pos.Set(0, 0);
	}
	else // HYORIEN_Horizontal
	{
		m_PageControl.pos.Set(uiDiameter, 0);
		m_PosBtn.pos.Set(uiLength - uiDiameter, 0);
		m_NegBtn.pos.Set(0, 0);
	}

	m_AnimScrollPos.Set(0.0f);
	InvokeOnScrollCallback();
}

bool HyScrollBar::IsValidMetrics() const
{
	return m_bIsValidMetrics;
}

void HyScrollBar::SetOnScrollCallback(HyScrollBarCallback fpCallback, void *pData)
{
	m_fpCallback = fpCallback;
	m_pCallbackData = pData;
}

void HyScrollBar::DoLineScroll(int32 iLinesOffset)
{
	if(iLinesOffset == 0)
		return;

	m_AnimScrollPos.Tween(m_AnimScrollPos.GetAnimDestination() + (iLinesOffset * m_fLineScrollAmt), 0.15f);
}

void HyScrollBar::DoPageScroll(int32 iPagesOffset)
{
	if(iPagesOffset == 0)
		return;

	m_AnimScrollPos.Tween(m_AnimScrollPos.GetAnimDestination() + (iPagesOffset * m_fClientShownSize), 0.15f);
}

/*virtual*/ void HyScrollBar::OnUpdate() /*override*/
{
	if(m_AnimScrollPos.IsAnimating() && m_fpCallback)
		InvokeOnScrollCallback();
}

/*static*/ void HyScrollBar::OnArrowBtnPressed(HyButton *pBtn, void *pData)
{
	HyScrollBar *pThis = static_cast<HyScrollBar *>(pData);

	if(pBtn == &pThis->m_PosBtn)
		pThis->m_AnimScrollPos.Offset(pThis->m_fLineScrollAmt);
	else // m_NegBtn
		pThis->m_AnimScrollPos.Offset(-pThis->m_fLineScrollAmt);

	pThis->InvokeOnScrollCallback();
}

void HyScrollBar::InvokeOnScrollCallback()
{
	// Clamp scroll - Will stop any anim/tween if .Set() is called
	if(m_AnimScrollPos.Get() < 0.0f)
		m_AnimScrollPos.Set(0.0f);
	else if(m_AnimScrollPos.Get() > static_cast<float>(m_fClientTotalSize - m_fClientShownSize))
		m_AnimScrollPos.Set(static_cast<float>(m_fClientTotalSize - m_fClientShownSize));

	if(m_eORIENTATION == HYORIEN_Vertical)
		m_Slider.pos.Set(0.0f, m_Slider.GetSceneWidth() + (m_AnimScrollPos.Get() * (m_PageControl.GetSceneHeight() - m_Slider.GetSceneHeight())) / static_cast<float>(m_fClientTotalSize - m_fClientShownSize));
	else
		m_Slider.pos.Set(m_Slider.GetSceneHeight() + (m_AnimScrollPos.Get() * (m_PageControl.GetSceneWidth() - m_Slider.GetSceneWidth())) / static_cast<float>(m_fClientTotalSize - m_fClientShownSize), 0.0f);

	if(m_fpCallback)
		m_fpCallback(this, static_cast<uint32>(m_AnimScrollPos.Get()), m_pCallbackData);
}
