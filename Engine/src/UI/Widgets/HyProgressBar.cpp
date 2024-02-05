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
#include "Diagnostics/Console/IHyConsole.h"

#define HYPROGRESSBAR_DEFAULT_ADJUST_DUR 0.2f

HyProgressBar::HyProgressBar(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_Bar(HyPanelInit(), this),
	m_BarMask(this),
	m_fBarProgressAmt(0.0f),
	m_BarProgressAmt(m_fBarProgressAmt, *this, 0),
	m_fBarAdjustDuration(HYPROGRESSBAR_DEFAULT_ADJUST_DUR)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
}

HyProgressBar::HyProgressBar(const HyPanelInit &panelInit, const HyPanelInit &barInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_Bar(barInit, this),
	m_BarMask(this),
	m_fBarProgressAmt(0.0f),
	m_BarProgressAmt(m_fBarProgressAmt, *this, 0),
	m_fBarAdjustDuration(HYPROGRESSBAR_DEFAULT_ADJUST_DUR)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
	Setup(panelInit, barInit);
}

HyProgressBar::HyProgressBar(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_Bar(barInit, this),
	m_BarMask(this),
	m_fBarProgressAmt(0.0f),
	m_BarProgressAmt(m_fBarProgressAmt, *this, 0),
	m_fBarAdjustDuration(HYPROGRESSBAR_DEFAULT_ADJUST_DUR)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
	Setup(panelInit, barInit, textNodePath, 0, 0, 0, 0);
}

HyProgressBar::HyProgressBar(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_Bar(barInit, this),
	m_BarMask(this),
	m_fBarProgressAmt(0.0f),
	m_BarProgressAmt(m_fBarProgressAmt, *this, 0),
	m_fBarAdjustDuration(HYPROGRESSBAR_DEFAULT_ADJUST_DUR)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
	Setup(panelInit, barInit, textNodePath, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

/*virtual*/ HyProgressBar::~HyProgressBar()
{
}

/*virtual*/ void HyProgressBar::SetText(const std::string &sUtf8Text) /*override*/
{
	if(sUtf8Text.empty())
		m_uiAttribs &= ~PROGBARATTRIB_IsTextOverride;
	else
		m_uiAttribs |= PROGBARATTRIB_IsTextOverride;

	HyLabel::SetText(sUtf8Text);
}

void HyProgressBar::Setup(const HyPanelInit &panelInit, const HyPanelInit &barInit)
{
	m_Bar.Setup(barInit);
	HyLabel::Setup(panelInit);
}

void HyProgressBar::Setup(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath)
{
	m_Bar.Setup(barInit);
	HyLabel::Setup(panelInit, textNodePath);
}

void HyProgressBar::Setup(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	m_Bar.Setup(barInit);
	HyLabel::Setup(panelInit, textNodePath, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

glm::vec2 HyProgressBar::GetBarOffset() const
{
	return m_vBarOffset;
}

void HyProgressBar::SetBarOffset(const glm::ivec2 &barOffset)
{
	SetBarOffset(barOffset.x, barOffset.y);
}

void HyProgressBar::SetBarOffset(int32 iBarOffsetX, int32 iBarOffsetY)
{
	HySetVec(m_vBarOffset, iBarOffsetX, iBarOffsetY);
	OnSetup();
}

bool HyProgressBar::IsVertical() const
{
	return (m_uiAttribs & PROGBARATTRIB_IsVertical) != 0;
}

void HyProgressBar::SetVertical(bool bIsVertical)
{
	if(bIsVertical)
		m_uiAttribs |= PROGBARATTRIB_IsVertical;
	else
		m_uiAttribs &= ~PROGBARATTRIB_IsVertical;

	OnSetup();
}

bool HyProgressBar::IsInverted() const
{
	return (m_uiAttribs & PROGBARATTRIB_IsInverted) != 0;
}

void HyProgressBar::SetInverted(bool bIsInverted)
{
	if(bIsInverted)
		m_uiAttribs |= PROGBARATTRIB_IsInverted;
	else
		m_uiAttribs &= ~PROGBARATTRIB_IsInverted;

	OnSetup();
}

bool HyProgressBar::IsBarStretched() const
{
	return (m_uiAttribs & PROGBARATTRIB_IsBarStretched) != 0;
}

void HyProgressBar::SetBarStreteched(bool bIsBarStretched)
{
	if(bIsBarStretched)
		m_uiAttribs |= PROGBARATTRIB_IsBarStretched;
	else
	{
		m_Bar.scale.Set(1.0f, 1.0f);
		m_uiAttribs &= ~PROGBARATTRIB_IsBarStretched;
	}

	OnSetup();
}

bool HyProgressBar::IsBarUnderPanel() const
{
	return (m_uiAttribs & PROGBARATTRIB_IsBarUnderPanel) != 0;
}

void HyProgressBar::SetBarUnderPanel(bool bIsBarUnderPanel)
{
	if(bIsBarUnderPanel && IsBarUnderPanel() == false)
	{
		ChildRemove(&m_Bar);
		ChildInsert(m_Panel, m_Bar);
		m_uiAttribs |= PROGBARATTRIB_IsBarUnderPanel;
	}
	else if(bIsBarUnderPanel == false && IsBarUnderPanel())
	{
		ChildRemove(&m_Bar);
		ChildInsert(m_Text, m_Bar);
		m_uiAttribs &= ~PROGBARATTRIB_IsBarUnderPanel;
	}
	else
		return;
}

void HyProgressBar::SetMinimum(int32 iMinimum)
{
	if(m_iMinimum == iMinimum)
		return;

	m_iMinimum = iMinimum;
	m_iMaximum = HyMath::Max(m_iMaximum, m_iMinimum);
	m_iValue = HyMath::Clamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress(0.0f);
}

void HyProgressBar::SetMaximum(int32 iMaximum)
{
	if(m_iMaximum == iMaximum)
		return;

	m_iMaximum = iMaximum;
	m_iMinimum = HyMath::Min(m_iMinimum, m_iMaximum);
	m_iValue = HyMath::Clamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress(0.0f);
}

void HyProgressBar::SetRange(int32 iMinimum, int32 iMaximum)
{
	if(m_iMinimum == iMinimum && m_iMaximum == iMaximum)
		return;

	SetMinimum(iMinimum);
	SetMaximum(iMaximum);
	AdjustProgress(0.0f);
}

void HyProgressBar::SetValue(int32 iValue)
{
	if(m_iValue == iValue)
		return;

	m_iValue = HyMath::Clamp(iValue, m_iMinimum, m_iMaximum);
	AdjustProgress(m_fBarAdjustDuration);
}

HyNumberFormat HyProgressBar::GetNumFormat() const
{
	return m_NumberFormat;
}

void HyProgressBar::SetNumFormat(HyNumberFormat format)
{
	m_NumberFormat = format;
	AdjustProgress(m_fBarAdjustDuration);
}

/*virtual*/ void HyProgressBar::OnUiUpdate() /*override*/
{
	if(m_BarProgressAmt.IsAnimating())
		ApplyProgress();
}

/*virtual*/ void HyProgressBar::OnSetup() /*override*/
{
	if(IsInverted())
	{
		if(IsVertical())
		{
			m_Bar.scale_pivot.Set(0.0f, m_Bar.GetHeight(1.0f));
			m_BarMask.scale_pivot.Set(0.0f, m_Bar.GetHeight(1.0f));
		}
		else
		{
			m_Bar.scale_pivot.Set(m_Bar.GetWidth(1.0f), 0.0f);
			m_BarMask.scale_pivot.Set(m_Bar.GetWidth(1.0f), 0.0f);
		}
	}
	else
	{
		m_Bar.scale_pivot.Set(0.0f, 0.0f);
		m_BarMask.scale_pivot.Set(0.0f, 0.0f);
	}

	if(IsBarStretched())
		m_Bar.SetStencil(nullptr);
	else
	{
		m_BarMask.SetAsBox(m_Bar.GetWidth(), m_Bar.GetHeight());
		m_BarStencil.AddMask(m_BarMask);
		m_Bar.SetStencil(&m_BarStencil);
		
		m_Bar.scale.Set(1.0f, 1.0f);
	}

	AdjustProgress(0.0f);
}

void HyProgressBar::AdjustProgress(float fDuration)
{
	float fProgress = 0.0f;
	if((m_iMaximum - m_iMinimum) != 0)
		fProgress = 1.0f - static_cast<float>(m_iValue - m_iMinimum) / static_cast<float>(m_iMaximum - m_iMinimum);

	if(fProgress != m_BarProgressAmt.GetAnimDestination())
		m_BarProgressAmt.Tween(fProgress, fDuration, HyTween::QuadInOut, 0.0f, [this](IHyNode *pThis) { ApplyProgress(); });

	if((m_uiAttribs & PROGBARATTRIB_IsTextOverride) == 0)
		HyLabel::SetText(HyLocale::Percent_Format(fProgress * 100.0, m_NumberFormat)); // NOTE: Don't use this class's SetText() so m_uiAttribs doesn't get set
}

void HyProgressBar::ApplyProgress()
{
	if(IsVertical())
	{
		if(IsBarStretched())
			m_Bar.scale.SetY(m_BarProgressAmt.Get());
		else
			m_BarMask.scale.SetY(m_BarProgressAmt.Get());
	}
	else
	{
		if(IsBarStretched())
			m_Bar.scale.SetX(m_BarProgressAmt.Get());
		else
			m_BarMask.scale.SetX(m_BarProgressAmt.Get());
	}
}
