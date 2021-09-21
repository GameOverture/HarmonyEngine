/**************************************************************************
*	HyProgressBar.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyProgressBar.h"

HyProgressBar::HyProgressBar(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pFill(nullptr)
{
}

HyProgressBar::HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, iStroke+1, iStroke+1, iStroke+1, iStroke+1, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pFill(nullptr)
{
	OnSetup();
}

HyProgressBar::HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pFill(nullptr)
{
	OnSetup();
}

HyProgressBar::HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pFill(nullptr)
{
	OnSetup();
}

HyProgressBar::HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pFill(nullptr)
{
	OnSetup();
}

/*virtual*/ HyProgressBar::~HyProgressBar()
{
	delete m_pFill;
}

void HyProgressBar::SetFillMargins(const HyRectangle<int32> &fillMarginsRef)
{
	SetFillMargins(fillMarginsRef.left, fillMarginsRef.bottom, fillMarginsRef.right, fillMarginsRef.top);
}

void HyProgressBar::SetFillMargins(int32 iFillMarginLeft, int32 iFillMarginBottom, int32 iFillMarginRight, int32 iFillMarginTop)
{
	m_FillMargins.Set(static_cast<float>(iFillMarginLeft), static_cast<float>(iFillMarginBottom), static_cast<float>(iFillMarginRight), static_cast<float>(iFillMarginTop));

	if(m_pFill->GetType() == HYTYPE_Primitive)
	{
		glm::vec2 vTotalFillArea(GetPanelWidth() - (m_FillMargins.left + m_FillMargins.right), GetPanelHeight() - (m_FillMargins.top + m_FillMargins.bottom));
		static_cast<HyPrimitive2d *>(m_pFill)->SetAsBox(vTotalFillArea.x, vTotalFillArea.y);
	}

	AdjustProgress();
}

void HyProgressBar::Reset()
{
	m_iMinimum = m_iMaximum = m_iValue = 0;
	m_pFill->scale.SetX(0.0f);

	AdjustProgress();
}

void HyProgressBar::SetMinimum(int32 iMinimum)
{
	if(m_iMinimum == iMinimum)
		return;

	m_iMinimum = iMinimum;
	m_iMaximum = HyMax(m_iMaximum, m_iMinimum);
	m_iValue = HyClamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress();
}

void HyProgressBar::SetMaximum(int32 iMaximum)
{
	if(m_iMaximum == iMaximum)
		return;

	m_iMaximum = iMaximum;
	m_iMinimum = HyMin(m_iMinimum, m_iMaximum);
	m_iValue = HyClamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress();
}

void HyProgressBar::SetRange(int32 iMinimum, int32 iMaximum)
{
	if(m_iMinimum == iMinimum && m_iMaximum == iMaximum)
		return;

	SetMinimum(iMinimum);
	SetMaximum(iMaximum);
	AdjustProgress();
}

void HyProgressBar::SetValue(int32 iValue)
{
	if(m_iValue == iValue)
		return;

	m_iValue = HyClamp(iValue, m_iMinimum, m_iMaximum);
	AdjustProgress();
}

HyNumberFormat HyProgressBar::GetNumFormat() const
{
	return m_NumberFormat;
}

void HyProgressBar::SetNumFormat(HyNumberFormat format)
{
	m_NumberFormat = format;
	AdjustProgress();
}

/*virtual*/ void HyProgressBar::OnSetup() /*override*/
{
	m_FillMargins = m_TextMargins;

	delete m_pFill;
	if(m_SpritePanel.IsLoadDataValid() && m_SpritePanel.GetNumStates() > 1)
	{
		m_pFill = HY_NEW HySprite2d(m_SpritePanel.GetPrefix(), m_SpritePanel.GetName(), this);
		m_pFill->SetState(1);
	}
	else
	{
		m_pFill = HY_NEW HyPrimitive2d(this);

		glm::vec2 vTotalFillArea(GetPanelWidth() - (m_FillMargins.left + m_FillMargins.right), GetPanelHeight() - (m_FillMargins.top + m_FillMargins.bottom));
		static_cast<HyPrimitive2d *>(m_pFill)->SetAsBox(vTotalFillArea.x, vTotalFillArea.y);
	}

	m_NumberFormat.SetFractionPrecision(0, 1);
	
	AdjustProgress();
}

void HyProgressBar::AdjustProgress()
{
	if(IsLoaded() == false || m_pFill == nullptr)
		return;

	// Position fill
	m_pFill->pos.Set(m_FillMargins.left, m_FillMargins.bottom);

	// Scale fill
	float fProgress = 0.0f;
	if((m_iMaximum - m_iMinimum) != 0)
		fProgress = 1.0f - static_cast<float>(m_iValue - m_iMinimum) / static_cast<float>(m_iMaximum - m_iMinimum);
	m_pFill->scale.Tween(fProgress, 1.0f, 0.2f, HyTween::QuadInOut);

	SetText(HyLocale::Percent_Format(fProgress * 100.0, m_NumberFormat));
}
