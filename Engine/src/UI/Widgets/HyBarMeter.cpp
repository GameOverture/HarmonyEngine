/**************************************************************************
*	HyBarMeter.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyBarMeter.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"

HyBarMeter::HyBarMeter(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_BarMask(this),
	m_Bar(HyUiPanelInit(), this),
	m_fBarProgressAmt(0.0f),
	m_BarProgressAmt(m_fBarProgressAmt, *this, 0)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
}

HyBarMeter::HyBarMeter(const HyUiPanelInit &panelInit, const HyUiPanelInit &barInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_BarMask(this),
	m_Bar(),
	m_fBarProgressAmt(0.0f),
	m_BarProgressAmt(m_fBarProgressAmt, *this, 0)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
	Setup(panelInit, barInit);
}

HyBarMeter::HyBarMeter(const HyUiPanelInit &panelInit, const HyUiPanelInit &barInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_iMinimum(0),
	m_iMaximum(0),
	m_iValue(0),
	m_BarMask(this),
	m_Bar(),
	m_fBarProgressAmt(0.0f),
	m_BarProgressAmt(m_fBarProgressAmt, *this, 0)
{
	m_NumberFormat.SetFractionPrecision(0, 1);
	Setup(panelInit, barInit, textInit);
}

//HyBarMeter::HyBarMeter(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
//	HyLabel(pParent),
//	m_iMinimum(0),
//	m_iMaximum(0),
//	m_iValue(0),
//	m_BarMask(this),
//	m_Bar(this),
//	m_fBarProgressAmt(0.0f),
//	m_BarProgressAmt(m_fBarProgressAmt, *this, 0)
//{
//	m_NumberFormat.SetFractionPrecision(0, 1);
//	Setup(panelInit, barInit, textNodePath, HyMargins<float>());
//}
//
//HyBarMeter::HyBarMeter(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent /*= nullptr*/) :
//	HyLabel(pParent),
//	m_iMinimum(0),
//	m_iMaximum(0),
//	m_iValue(0),
//	m_BarMask(this),
//	m_Bar(this),
//	m_fBarProgressAmt(0.0f),
//	m_BarProgressAmt(m_fBarProgressAmt, *this, 0)
//{
//	m_NumberFormat.SetFractionPrecision(0, 1);
//	Setup(panelInit, barInit, textNodePath, textMargins);
//}

/*virtual*/ HyBarMeter::~HyBarMeter()
{
}

/*virtual*/ void HyBarMeter::SetText(const std::string &sUtf8Text) /*override*/
{
	if(sUtf8Text.empty())
		m_uiEntityAttribs &= ~BARMETERATTRIB_IsTextOverride;
	else
		m_uiEntityAttribs |= BARMETERATTRIB_IsTextOverride;

	HyLabel::SetText(sUtf8Text);
}

void HyBarMeter::Setup(const HyUiPanelInit &panelInit, const HyUiPanelInit &barInit)
{
	m_Bar.Setup(barInit, this);
	HyLabel::Setup(panelInit);
}

void HyBarMeter::Setup(const HyUiPanelInit &panelInit, const HyUiPanelInit &barInit, const HyUiTextInit &textInit)
{
	m_Bar.Setup(barInit, this);
	HyLabel::Setup(panelInit, textInit);
}

void HyBarMeter::SetupBar(const HyUiPanelInit &barInit)
{
	m_Bar.Setup(barInit, this);
	
	SetEnabled(IsEnabled());
	SetAssembleNeeded();
	OnSetup();
}

glm::vec2 HyBarMeter::GetBarOffset() const
{
	return m_vBarOffset;
}

void HyBarMeter::SetBarOffset(const glm::ivec2 &barOffset)
{
	SetBarOffset(barOffset.x, barOffset.y);
}

void HyBarMeter::SetBarOffset(int32 iBarOffsetX, int32 iBarOffsetY)
{
	HySetVec(m_vBarOffset, static_cast<float>(iBarOffsetX), static_cast<float>(iBarOffsetY));
	SetAssembleNeeded();
}

bool HyBarMeter::SetBarState(uint32 uiStateIndex)
{
	bool bSuccess = m_Bar.SetPanelState(uiStateIndex);
	SetAssembleNeeded();
	return bSuccess;
}

bool HyBarMeter::IsVertical() const
{
	return (m_uiEntityAttribs & BARMETERATTRIB_IsVertical) != 0;
}

void HyBarMeter::SetVertical(bool bIsVertical)
{
	if(bIsVertical)
		m_uiEntityAttribs |= BARMETERATTRIB_IsVertical;
	else
		m_uiEntityAttribs &= ~BARMETERATTRIB_IsVertical;

	SetAssembleNeeded();
}

bool HyBarMeter::IsInverted() const
{
	return (m_uiEntityAttribs & BARMETERATTRIB_IsInverted) != 0;
}

void HyBarMeter::SetInverted(bool bIsInverted)
{
	if(bIsInverted)
		m_uiEntityAttribs |= BARMETERATTRIB_IsInverted;
	else
		m_uiEntityAttribs &= ~BARMETERATTRIB_IsInverted;

	SetAssembleNeeded();
}

bool HyBarMeter::IsBarStretched() const
{
	return (m_uiEntityAttribs & BARMETERATTRIB_IsBarStretched) != 0;
}

void HyBarMeter::SetBarStreteched(bool bIsBarStretched)
{
	if(bIsBarStretched)
		m_uiEntityAttribs |= BARMETERATTRIB_IsBarStretched;
	else
	{
		if(m_Bar.GetPanelNode())
			m_Bar.GetPanelNode()->scale.Set(1.0f, 1.0f);
		m_uiEntityAttribs &= ~BARMETERATTRIB_IsBarStretched;
	}

	SetAssembleNeeded();
}

bool HyBarMeter::IsBarUnderPanel() const
{
	return (m_uiEntityAttribs & BARMETERATTRIB_IsBarUnderPanel) != 0;
}

//void HyBarMeter::SetBarUnderPanel(bool bIsBarUnderPanel)
//{
//	if(bIsBarUnderPanel && IsBarUnderPanel() == false)
//	{
//		ChildRemove(&m_Bar);
//		ChildInsert(m_Panel, m_Bar);
//		m_uiEntityAttribs |= BARMETERATTRIB_IsBarUnderPanel;
//	}
//	else if(bIsBarUnderPanel == false && IsBarUnderPanel())
//	{
//		ChildRemove(&m_Bar);
//		ChildInsert(m_Text, m_Bar);
//		m_uiEntityAttribs &= ~BARMETERATTRIB_IsBarUnderPanel;
//	}
//	else
//		return;
//}

int32 HyBarMeter::GetMinimum() const
{
	return m_iMinimum;
}

int32 HyBarMeter::GetMaximum() const
{
	return m_iMaximum;
}

void HyBarMeter::SetMinimum(int32 iMinimum)
{
	if(m_iMinimum == iMinimum)
		return;

	m_iMinimum = iMinimum;
	m_iMaximum = HyMath::Max(m_iMaximum, m_iMinimum);
	m_iValue = HyMath::Clamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress(0.0f);
}

void HyBarMeter::SetMaximum(int32 iMaximum)
{
	if(m_iMaximum == iMaximum)
		return;

	m_iMaximum = iMaximum;
	m_iMinimum = HyMath::Min(m_iMinimum, m_iMaximum);
	m_iValue = HyMath::Clamp(m_iValue, m_iMinimum, m_iMaximum);
	AdjustProgress(0.0f);
}

void HyBarMeter::SetRange(int32 iMinimum, int32 iMaximum)
{
	if(m_iMinimum == iMinimum && m_iMaximum == iMaximum)
		return;

	SetMinimum(iMinimum);
	SetMaximum(iMaximum);
	AdjustProgress(0.0f);
}

int32 HyBarMeter::GetValue() const
{
	return m_iValue;
}

void HyBarMeter::SetValue(int32 iValue, float fAdjustDuration)
{
	if(m_iValue == iValue)
		return;

	m_iValue = HyMath::Clamp(iValue, m_iMinimum, m_iMaximum);
	AdjustProgress(fAdjustDuration);
}

HyNumberFormat HyBarMeter::GetNumFormat() const
{
	return m_NumberFormat;
}

void HyBarMeter::SetNumFormat(HyNumberFormat format)
{
	m_NumberFormat = format;
	AdjustProgress(0.0f);
}

HyUiPanelInit HyBarMeter::CloneBarPanelInit() const
{
	return m_Bar.ClonePanelInit();
}

#ifdef HY_PLATFORM_GUI
void HyBarMeter::GuiOverrideBarNodeData(HyType eNodeType, HyJsonObj itemDataObj, bool bUseGuiOverrideName /*= true*/)
{
	m_Bar.GuiOverridePanelNodeData(eNodeType, itemDataObj, bUseGuiOverrideName, this);
}
#endif

/*virtual*/ void HyBarMeter::Update() /*override*/
{
	HyLabel::Update();

	if(m_BarProgressAmt.IsAnimating())
		ApplyProgress();
}

/*virtual*/ void HyBarMeter::OnAssemble() /*override*/
{
	HyLabel::OnAssemble();

	m_BarMask.pos.Set(m_vBarOffset);
	if(m_Bar.GetPanelNode())
		m_Bar.GetPanelNode()->pos.Set(m_vBarOffset);

	if(IsInverted())
	{
		if(IsVertical())
		{
			m_BarMask.scale_pivot.Set(0.0f, m_Bar.GetHeight(1.0f));
			if(m_Bar.GetPanelNode())
				m_Bar.GetPanelNode()->scale_pivot.Set(0.0f, m_Bar.GetHeight(1.0f));
		}
		else
		{
			m_BarMask.scale_pivot.Set(m_Bar.GetWidth(1.0f), 0.0f);
			if(m_Bar.GetPanelNode())
				m_Bar.GetPanelNode()->scale_pivot.Set(m_Bar.GetWidth(1.0f), 0.0f);
		}
	}
	else
	{
		m_BarMask.scale_pivot.Set(0.0f, 0.0f);
		if(m_Bar.GetPanelNode())
			m_Bar.GetPanelNode()->scale_pivot.Set(0.0f, 0.0f);
	}

	if(IsBarStretched())
	{
		if(m_Bar.GetPanelNode())
			m_Bar.GetPanelNode()->SetStencil(nullptr);
	}
	else
	{
		glm::ivec2 vSpriteOffset(0.0f, 0.0f);
		if(m_Bar.IsItemForPanel() && m_Bar.GetPanelNode()->GetType() == HYTYPE_Sprite)
			vSpriteOffset = static_cast<HySprite2d *>(m_Bar.GetPanelNode())->GetCurFrameOffset();

		m_BarMask.SetAsBox(HyRect(m_Bar.GetWidth() + vSpriteOffset.x, m_Bar.GetHeight() + vSpriteOffset.x));
		m_BarMask.SetVisible(false);
		m_BarStencil.AddMask(m_BarMask);
		
		if(m_Bar.GetPanelNode())
		{
			m_Bar.GetPanelNode()->SetStencil(&m_BarStencil);
			m_Bar.GetPanelNode()->scale.Set(1.0f, 1.0f);
		}
	}

	AdjustProgress(0.0f);
	ApplyProgress();
}

void HyBarMeter::AdjustProgress(float fDuration)
{
	float fProgress = 0.0f;
	if((m_iMaximum - m_iMinimum) != 0)
		fProgress = static_cast<float>(m_iValue - m_iMinimum) / static_cast<float>(m_iMaximum - m_iMinimum);

	if(fProgress != m_BarProgressAmt.GetAnimDestination())
		m_BarProgressAmt.Tween(fProgress, fDuration, HyTween::QuadInOut, 0.0f, [this](IHyNode *pThis) { ApplyProgress(); });

	if((m_uiEntityAttribs & BARMETERATTRIB_IsTextOverride) == 0)
		HyLabel::SetText(HyLocale::Percent_Format(fProgress * 100.0, m_NumberFormat)); // NOTE: Don't use this class's SetText() so m_uiEntityAttribs doesn't get set
}

void HyBarMeter::ApplyProgress()
{
	if(IsVertical())
	{
		if(IsBarStretched())
		{
			if(m_Bar.GetPanelNode())
				m_Bar.GetPanelNode()->scale.SetY(m_BarProgressAmt.Get());
		}
		else if(m_Bar.GetPanelNode())
			m_Bar.GetPanelNode()->scale.SetY(m_BarProgressAmt.Get());
	}
	else
	{
		if(IsBarStretched())
		{
			if(m_Bar.GetPanelNode())
				m_Bar.GetPanelNode()->scale.SetX(m_BarProgressAmt.Get());
		}
		else
			m_BarMask.scale.SetX(m_BarProgressAmt.Get());
	}
}
