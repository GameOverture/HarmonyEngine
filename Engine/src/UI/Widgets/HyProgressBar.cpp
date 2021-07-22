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
	m_NumFormat.SetFractionPrecision(0, 1);
}

HyProgressBar::HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, "", "", 0, iStroke+1, 0, iStroke+1, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0)
{
	m_NumFormat.SetFractionPrecision(0, 1);
}

HyProgressBar::HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, 0, iStroke+1, 0, iStroke+1, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0)
{
	m_NumFormat.SetFractionPrecision(0, 1);
}

/*virtual*/ HyProgressBar::~HyProgressBar()
{
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

HyNumberFormat &HyProgressBar::SetPercentFormat()
{
	return m_NumFormat;
}

void HyProgressBar::AdjustProgress()
{
	if(m_pPrimPanel)
	{
		float fProgress = 0.0f;
		if((m_iMaximum - m_iMinimum) != 0)
			fProgress = 1.0f - static_cast<float>(m_iValue - m_iMinimum) / static_cast<float>(m_iMaximum - m_iMinimum);

		m_pPrimPanel->m_Fill.scale.SetX(fProgress);

		SetText(HyLocale::Percent_Format(fProgress * 100.0, m_NumFormat));
	}
}
