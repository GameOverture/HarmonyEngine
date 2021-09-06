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
	m_iValue(0)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
}

HyProgressBar::HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, "", "", 0, iStroke+1, 0, iStroke+1, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
}

HyProgressBar::HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, 0, iStroke+1, 0, iStroke+1, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
}

HyProgressBar::HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sPanelFillPrefix, std::string sPanelFillName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d* pParent /*= nullptr*/) :
	m_sprFill(sPanelFillPrefix, sPanelFillName, this),
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0)
{
	m_sprFill.UseWindowCoordinates(0);
	m_sprFill.scale.Set(0.0f, 1.0f);
	ChildInsert(this->GetSpriteNode(), m_sprFill);
	m_NumberFormat.SetFractionPrecision(0, 1);

	if (m_pPrimPanel)
		m_pPrimPanel->m_Fill.scale.SetX(0.0f);
}

/*virtual*/ HyProgressBar::~HyProgressBar()
{
}

HySprite2d& HyProgressBar::GetFill()
{
	return m_sprFill;
}

void HyProgressBar::Reset()
{
	m_iMinimum = m_iMaximum = m_iValue = 0;
	AdjustProgress();
}

void HyProgressBar::SetMinimum(int32 iMinimum)
{
	m_iMinimum = iMinimum;
	m_iMaximum = HyMax(m_iMaximum, m_iMinimum);
	m_iValue = HyClamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress();
}

void HyProgressBar::SetMaximum(int32 iMaximum)
{
	m_iMaximum = iMaximum;
	m_iMinimum = HyMin(m_iMinimum, m_iMaximum);
	m_iValue = HyClamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress();
}

void HyProgressBar::SetRange(int32 iMinimum, int32 iMaximum)
{
	SetMinimum(iMinimum);
	SetMaximum(iMaximum);
	AdjustProgress();
}

void HyProgressBar::SetValue(int32 iValue)
{
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

void HyProgressBar::AdjustProgress()
{
	if (IsLoaded() == false)
		return;

	if (m_iValue > 0)
	{
		float fProgress = 0.0f;
		if ((m_iMaximum - m_iMinimum) != 0)
			fProgress = 1.0f - static_cast<float>(m_iValue - m_iMinimum) / static_cast<float>(m_iMaximum - m_iMinimum);

		if (m_sprFill.IsLoadDataValid() == true)
			m_sprFill.scale.Tween(fProgress, 1.0f, 0.2f, HyTween::QuadInOut);
		else if (m_pPrimPanel)
			m_pPrimPanel->m_Fill.scale.Tween(fProgress, 1.0f, 0.2f, HyTween::QuadInOut);

	//	SetText(HyLocale::Percent_Format(fProgress * 100.0, m_NumberFormat));
	}
	else
	{
		if (m_sprFill.IsLoadDataValid() == true)
			m_sprFill.scale.SetX(0.0f);
		else if (m_pPrimPanel)
			m_pPrimPanel->m_Fill.scale.SetX(0.0f);
	}
}
