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

HyProgressBar::HyProgressBar(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pBar(nullptr),
	m_fBarScissorAmt(0.0f),
	m_BarScissorAmt(m_fBarScissorAmt, *this, 0)
{
}

HyProgressBar::HyProgressBar(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, 0, 0, 0, 0, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pBar(nullptr),
	m_fBarScissorAmt(0.0f),
	m_BarScissorAmt(m_fBarScissorAmt, *this, 0)
{
	OnSetup();
}

HyProgressBar::HyProgressBar(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_pBar(nullptr),
	m_fBarScissorAmt(0.0f),
	m_BarScissorAmt(m_fBarScissorAmt, *this, 0)
{
	OnSetup();
}

/*virtual*/ HyProgressBar::~HyProgressBar()
{
	delete m_pBar;
}

const glm::ivec2 &HyProgressBar::GetBarOffset() const
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

	if(m_pBar)
	{
		if (m_pBar->GetType() == HYTYPE_Primitive)
		{
			glm::vec2 vTotalFillArea(m_Panel.size.X() - (m_vBarOffset.x * 2.0f), m_Panel.size.Y() - (m_vBarOffset.y * 2.0f));
			static_cast<HyPrimitive2d*>(m_pBar)->SetAsBox(vTotalFillArea.x, vTotalFillArea.y);
		}
		else
			m_pBar->pos.Offset(iBarOffsetX, iBarOffsetY);
	 }

	AdjustProgress();
}

void HyProgressBar::Reset()
{
	m_iMinimum = m_iMaximum = m_iValue = 0;
	AdjustProgress();
}

void HyProgressBar::SetMinimum(int32 iMinimum)
{
	if(m_iMinimum == iMinimum)
		return;

	m_iMinimum = iMinimum;
	m_iMaximum = HyMath::Max(m_iMaximum, m_iMinimum);
	m_iValue = HyMath::Clamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress();
}

void HyProgressBar::SetMaximum(int32 iMaximum)
{
	if(m_iMaximum == iMaximum)
		return;

	m_iMaximum = iMaximum;
	m_iMinimum = HyMath::Min(m_iMinimum, m_iMaximum);
	m_iValue = HyMath::Clamp(m_iValue, m_iMinimum, m_iMaximum);
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

	m_iValue = HyMath::Clamp(iValue, m_iMinimum, m_iMaximum);
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

/*virtual*/ void HyProgressBar::OnUiUpdate() /*override*/
{
	if(m_pBar)
		m_pBar->SetScissor(0, 0, static_cast<uint32>(m_BarScissorAmt.Get()), static_cast<uint32>(m_pBar->GetSceneHeight()));
}

/*virtual*/ void HyProgressBar::OnSetup() /*override*/
{
	delete m_pBar;
	if(m_Panel.IsSprite() && m_Panel.GetSprite().GetNumStates() > 1)
	{
		m_pBar = HY_NEW HySprite2d(m_Panel.GetSprite().GetPrefix(), m_Panel.GetSprite().GetName());
		m_pBar->SetState(1);
	}
	else
	{
		m_pBar = HY_NEW HyPrimitive2d();

		glm::vec2 vTotalFillArea(m_Panel.size.X() - (m_vBarOffset.x * 2.0f), m_Panel.size.Y() - (m_vBarOffset.y * 2.0f));
		static_cast<HyPrimitive2d *>(m_pBar)->SetAsBox(vTotalFillArea.x, vTotalFillArea.y);
	}
	m_pBar->Load();
	ChildInsert(m_Text, *m_pBar);

	m_NumberFormat.SetFractionPrecision(0, 1);
	
	AdjustProgress();
}

void HyProgressBar::AdjustProgress()
{
	if(IsLoaded() == false || m_pBar == nullptr)
		return;

	m_pBar->pos.Set(m_vBarOffset);

	float fProgress = 0.0f;
	if((m_iMaximum - m_iMinimum) != 0)
		fProgress = 1.0f - static_cast<float>(m_iValue - m_iMinimum) / static_cast<float>(m_iMaximum - m_iMinimum);
	
	m_BarScissorAmt.Tween(m_pBar->GetSceneWidth() * fProgress, 0.2f, HyTween::QuadInOut);

	SetText(HyLocale::Percent_Format(fProgress * 100.0, m_NumberFormat));
}
