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
#include "UI/Components/HyScrollBar.h"
#include "HyEngine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PageControl class
HyScrollBar::PageControl::PageControl(HyOrientation eOrientation, uint32 uiLength, uint32 uiDiameter, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_uiDiameter(uiDiameter),
	m_Panel(this),
	m_Slider(this),
	m_eDragState(DRAG_None)
{
	EnableMouseInput();
	SetMetrics(eOrientation, uiLength, uiDiameter, 1.0f);
}

uint32 HyScrollBar::PageControl::GetDiameter() const
{
	return m_uiDiameter;
}

void HyScrollBar::PageControl::SetMetrics(HyOrientation eOrientation, uint32 uiLength, uint32 uiDiameter, float fSliderPercent)
{
	m_uiDiameter = uiDiameter;

	if(eOrientation == HYORIEN_Vertical)
	{
		m_Panel.shape.SetAsBox(static_cast<int32>(m_uiDiameter), static_cast<int32>(uiLength));
		m_Slider.shape.SetAsBox(static_cast<float>(m_uiDiameter) * 0.5f, (uiLength - (m_uiDiameter * 2)) * fSliderPercent);
	}
	else
	{
		m_Panel.shape.SetAsBox(static_cast<int32>(uiLength), static_cast<int32>(m_uiDiameter));
		m_Slider.shape.SetAsBox((uiLength - (m_uiDiameter * 2)) * fSliderPercent, static_cast<float>(m_uiDiameter) /** 0.5f*/);
	}
}

void HyScrollBar::PageControl::SetSliderPos(HyOrientation eOrientation, float fAnimScrollPos, float fClientTotalSize, float fClientShownSize)
{
	if(eOrientation == HYORIEN_Vertical)
		m_Slider.pos.Set(static_cast<float>(m_uiDiameter) * 0.25f, ((static_cast<float>(fClientTotalSize - fClientShownSize) - fAnimScrollPos) * (m_Panel.GetSceneHeight() - m_Slider.GetSceneHeight())) / static_cast<float>(fClientTotalSize - fClientShownSize));
	else
		m_Slider.pos.Set((fAnimScrollPos * (m_Panel.GetSceneWidth() - m_Slider.GetSceneWidth())) / static_cast<float>(fClientTotalSize - fClientShownSize), static_cast<float>(m_uiDiameter) * 0.25f);
}

void HyScrollBar::PageControl::SetColor(HyColor color)
{
	m_Panel.SetTint(color);
	if(color.IsLight())
		m_Slider.SetTint(color.Darken());
	else
		m_Slider.SetTint(color.Lighten());
}

/*virtual*/ void HyScrollBar::PageControl::OnUpdate() /*override*/
{
	if(m_eDragState == DRAG_MouseHeld)
	{
		if(HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft) == false)
		{
			m_eDragState = DRAG_None;
			return;
		}

		glm::vec2 ptMousePos;
		if(GetCoordinateSystem() >= 0)
		{
			if(HyEngine::Input().GetMouseWindowIndex() == GetCoordinateSystem())
				ptMousePos = HyEngine::Input().GetMousePos();
			else
				return;
		}
		else
			HyEngine::Input().GetWorldMousePos(ptMousePos);

		glm::vec2 vDist = ptMousePos - m_ptDragPos;

		HyScrollBar *pScrollBar = static_cast<HyScrollBar *>(m_pParent);
		if(pScrollBar->GetOrientation() == HYORIEN_Vertical)
			pScrollBar->OffsetSlider(vDist.y);
		else
			pScrollBar->OffsetSlider(vDist.x);

		m_ptDragPos = ptMousePos;
	}
}

/*virtual*/ void HyScrollBar::PageControl::OnMouseDown() /*override*/
{
	HyScrollBar *pScrollBar = static_cast<HyScrollBar *>(m_pParent);

	glm::vec2 ptMousePos;
	if(GetCoordinateSystem() >= 0)
		ptMousePos = HyEngine::Input().GetMousePos();
	else
		HyEngine::Input().GetWorldMousePos(ptMousePos);

	b2AABB sliderAABB = m_Slider.GetSceneAABB();
	if(pScrollBar->GetOrientation() == HYORIEN_Vertical)
	{
		sliderAABB.lowerBound.x -= static_cast<float>(m_uiDiameter) * 0.5f;
		sliderAABB.upperBound.x += static_cast<float>(m_uiDiameter) * 0.5f;
	}
	else
	{
		sliderAABB.lowerBound.y -= static_cast<float>(m_uiDiameter) * 0.5f;
		sliderAABB.upperBound.y += static_cast<float>(m_uiDiameter) * 0.5f;
	}
	
	if(HyTestPointAABB(sliderAABB, ptMousePos))
	{
		m_eDragState = DRAG_MouseHeld;
		m_ptDragPos = ptMousePos;
	}
	else
	{
		
		if(pScrollBar->GetOrientation() == HYORIEN_Vertical)
		{
			if(ptMousePos.y >= sliderAABB.GetCenter().y)
				pScrollBar->DoPageScroll(-1);
			else
				pScrollBar->DoPageScroll(1);
		}
		else // HYORIEN_Horizontal
		{
			if(ptMousePos.x >= sliderAABB.GetCenter().x)
				pScrollBar->DoPageScroll(1);
			else
				pScrollBar->DoPageScroll(-1);
		}
	}
}

/*virtual*/ void HyScrollBar::PageControl::OnMouseClicked() /*override*/
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Button class
HyScrollBar::Button::Button(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter, HyEntity2d *pParent) :
	HyButton(pParent),
	m_Panel(this),
	m_Decal(this)
{
	SetMetrics(eOrientation, bPositive, uiDiameter);
}

void HyScrollBar::Button::SetColor(HyColor color)
{
	m_Panel.SetTint(color);
	if(color.IsLight())
		m_Decal.SetTint(color.Darken());
	else
		m_Decal.SetTint(color.Lighten());
}

void HyScrollBar::Button::SetMetrics(HyOrientation eOrientation, bool bPositive, uint32 uiDiameter)
{
	m_Panel.shape.SetAsBox(static_cast<int32>(uiDiameter), static_cast<int32>(uiDiameter));

	if(eOrientation == HYORIEN_Vertical)
	{
		if(bPositive)
		{
			glm::vec2 ptDownArrow[3];
			HySetVec(ptDownArrow[0], uiDiameter * 0.2f, uiDiameter * 0.8f);
			HySetVec(ptDownArrow[1], uiDiameter * 0.5f, uiDiameter * 0.2f);
			HySetVec(ptDownArrow[2], uiDiameter * 0.8f, uiDiameter * 0.8f);
			m_Decal.shape.SetAsPolygon(ptDownArrow, 3);
		}
		else // negative
		{
			glm::vec2 ptUpArrow[3];
			HySetVec(ptUpArrow[0], uiDiameter * 0.2f, uiDiameter * 0.2f);
			HySetVec(ptUpArrow[1], uiDiameter * 0.5f, uiDiameter * 0.8f);
			HySetVec(ptUpArrow[2], uiDiameter * 0.8f, uiDiameter * 0.2f);
			m_Decal.shape.SetAsPolygon(ptUpArrow, 3);
		}
	}
	else // HYORIEN_Horizontal
	{
		if(bPositive)
		{
			glm::vec2 ptRightArrow[3];
			HySetVec(ptRightArrow[0], uiDiameter * 0.2f, uiDiameter * 0.8f);
			HySetVec(ptRightArrow[1], uiDiameter * 0.8f, uiDiameter * 0.5f);
			HySetVec(ptRightArrow[2], uiDiameter * 0.2f, uiDiameter * 0.2f);
			m_Decal.shape.SetAsPolygon(ptRightArrow, 3);
		}
		else // negative
		{
			glm::vec2 ptLeftArrow[3];
			HySetVec(ptLeftArrow[0], uiDiameter * 0.2f, uiDiameter * 0.5f);
			HySetVec(ptLeftArrow[1], uiDiameter * 0.8f, uiDiameter * 0.8f);
			HySetVec(ptLeftArrow[2], uiDiameter * 0.8f, uiDiameter * 0.2f);
			m_Decal.shape.SetAsPolygon(ptLeftArrow, 3);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HyScrollBar class
HyScrollBar::HyScrollBar(HyOrientation eOrientation, uint32 uiDiameter, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_eORIENTATION(eOrientation),
	m_PageControl(eOrientation, uiDiameter, uiDiameter, this),
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

uint32 HyScrollBar::GetDiameter() const
{
	return m_PageControl.GetDiameter();
}

float HyScrollBar::GetLineScrollAmt() const
{
	return m_fLineScrollAmt;
}

void HyScrollBar::SetLineScrollAmt(float fLineScrollAmt)
{
	m_fLineScrollAmt = fLineScrollAmt;
}

void HyScrollBar::SetColor(HyColor color)
{
	m_PageControl.SetColor(color);
	m_PosBtn.SetColor(color);
	m_NegBtn.SetColor(color);
}

void HyScrollBar::SetMetrics(uint32 uiLength, uint32 uiClientTotalSize, uint32 uiClientShownSize)
{
	uint32 uiDiameter = GetDiameter();

	if(uiClientTotalSize <= 0 || uiClientTotalSize <= uiClientShownSize)
	{
		m_bIsValidMetrics = false;
		return;
	}
	m_bIsValidMetrics = true;

	m_fClientTotalSize = static_cast<float>(uiClientTotalSize);
	m_fClientShownSize = static_cast<float>(uiClientShownSize);
	float fPercent = m_fClientShownSize / m_fClientTotalSize;

	m_PageControl.SetMetrics(m_eORIENTATION, uiLength - (uiDiameter * 2), uiDiameter, fPercent);
	m_PosBtn.SetMetrics(m_eORIENTATION, true, uiDiameter);
	m_NegBtn.SetMetrics(m_eORIENTATION, false, uiDiameter);

	if(m_eORIENTATION == HYORIEN_Vertical)
	{
		m_PageControl.pos.Set(0, uiDiameter);
		m_PosBtn.pos.Set(0, 0);
		m_NegBtn.pos.Set(0, uiLength - uiDiameter);
	}
	else // HYORIEN_Horizontal
	{
		m_PageControl.pos.Set(uiDiameter, 0);
		m_PosBtn.pos.Set(uiLength - uiDiameter, 0);
		m_NegBtn.pos.Set(0, 0);
	}

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

void HyScrollBar::OffsetSlider(float fPixels)
{
	// Convert fPixels to client sizing
	float fScrollBarLength = 0.0f;
	if(m_eORIENTATION == HYORIEN_Vertical)
	{
		fPixels *= -1.0f;
		fScrollBarLength = m_PageControl.GetSceneHeight();
	}
	else
		fScrollBarLength = m_PageControl.GetSceneWidth();

	m_AnimScrollPos.Offset((fPixels * m_fClientTotalSize) / fScrollBarLength);
	InvokeOnScrollCallback();
}

/*virtual*/ void HyScrollBar::OnUpdate() /*override*/
{
	if(m_AnimScrollPos.IsAnimating())
		InvokeOnScrollCallback();
}

/*static*/ void HyScrollBar::OnArrowBtnPressed(HyButton *pBtn, void *pData)
{
	HyScrollBar *pThis = static_cast<HyScrollBar *>(pData);

	if(pBtn == &pThis->m_PosBtn)
		pThis->m_AnimScrollPos.Offset(pThis->m_fLineScrollAmt);
	else // pBtn == &pThis->m_NegBtn
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

	m_PageControl.SetSliderPos(m_eORIENTATION, m_AnimScrollPos.Get(), m_fClientTotalSize, m_fClientShownSize);

	if(m_fpCallback)
		m_fpCallback(this, m_AnimScrollPos.Get(), m_fClientTotalSize - m_fClientShownSize, m_pCallbackData);
}
