/**************************************************************************
*	HyComboBox.cpp
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyComboBox.h"
#include "HyEngine.h"

HyComboBox::HyComboBox(HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent),
	m_Shape(this),
	m_SubBtnPanel(HyUiPanelInit(), this),
	m_fSubBtnSpacing(5.0f),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f),
	m_ExpandAnimVec(*this, 0)
{
	SetButtonClickedCallback([this](HyButton *pThis) { ToggleExpanded(); });
}

HyComboBox::HyComboBox(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textInit, pParent),
	m_Shape(this),
	m_SubBtnPanel(HyUiPanelInit(),this),
	m_fSubBtnSpacing(5.0f),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f),
	m_ExpandAnimVec(*this, 0)
{
	SetButtonClickedCallback([this](HyButton *pThis) { ToggleExpanded(); });
}

/*virtual*/ HyComboBox::~HyComboBox()
{
	ClearSubButtons();
}

uint32 HyComboBox::InsertSubButton(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, std::function<void(HyButton *)> fpCallBack, const HyNodePath &audioNodePath /*= HyNodePath()*/)
{
	HyButton *pNewBtn = HY_NEW HyButton(panelInit, textInit, this);
	pNewBtn->SetButtonClickedCallback(fpCallBack, audioNodePath);
	pNewBtn->SetEnabled(false);
	pNewBtn->alpha.Set(0.0f);
	pNewBtn->Load();

	m_SubBtnList.push_back(pNewBtn);
	m_SubBtnEnabledMap.insert(std::pair<HyButton *, bool>(pNewBtn, true));

	return static_cast<uint32>(m_SubBtnList.size()) - 1;
}

void HyComboBox::SetSubButtonEnabled(uint32 uiSubBtnIndex, bool bEnabled)
{
	if(uiSubBtnIndex >= m_SubBtnList.size())
		return;

	m_SubBtnEnabledMap[m_SubBtnList[uiSubBtnIndex]] = bEnabled;

	if(IsExpanded() && bEnabled)
		m_SubBtnList[uiSubBtnIndex]->SetEnabled(true);
}

void HyComboBox::RemoveSubButton(uint32 uiSubBtnIndex)
{
	uint32 uiIndexCount = 0;
	for(auto it = m_SubBtnList.begin(); it != m_SubBtnList.end(); ++it, ++uiIndexCount)
	{
		if(uiIndexCount == uiSubBtnIndex)
		{
			HyButton *pSubBtn = *it;
			m_SubBtnEnabledMap.erase(pSubBtn);
			m_SubBtnList.erase(it);
			delete pSubBtn;
			return;
		}
	}

	HyLogWarning("HyComboBox::RemoveSubButton was passed invalid index");
}

void HyComboBox::ClearSubButtons()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		delete m_SubBtnList[i];

	m_SubBtnList.clear();
	m_SubBtnEnabledMap.clear();
}

void HyComboBox::SetExpandPanel(const HyUiPanelInit &panelInit, HyOrientation eOrientation, bool bPositiveDirection, bool bAnimate)
{
	m_SubBtnPanel.Setup(panelInit, this);

	if(eOrientation == HYORIENT_Horizontal)
		m_uiEntityAttribs |= COMBOBOXATTRIB_IsHorzExpand;
	else
		m_uiEntityAttribs &= ~COMBOBOXATTRIB_IsHorzExpand;

	if(bPositiveDirection)
		m_uiEntityAttribs |= COMBOBOXATTRIB_IsPositiveExpand;
	else
		m_uiEntityAttribs &= ~COMBOBOXATTRIB_IsPositiveExpand;

	if(bAnimate)
		m_uiEntityAttribs &= ~COMBOBOXATTRIB_IsInstantExpand;
	else
		m_uiEntityAttribs |= COMBOBOXATTRIB_IsInstantExpand;
}

bool HyComboBox::IsExpanded() const
{
	return m_uiEntityAttribs & COMBOBOXATTRIB_IsExpanded;
}

bool HyComboBox::IsTransition() const
{
	return m_uiEntityAttribs & COMBOBOXATTRIB_IsTransition;
}

void HyComboBox::ToggleExpanded()
{
	if(IsTransition())
		return;

	float fTweenExpandDur = 0.5f;

	if(IsExpanded() == false) // Is collapsed
	{
		glm::vec2 ptTweenDest(0.0f, 0.0f);
		int32 iExpandIndex = (m_uiEntityAttribs & COMBOBOXATTRIB_IsHorzExpand) ? 0 : 1;
		int32 iExpandDir = (m_uiEntityAttribs & COMBOBOXATTRIB_IsPositiveExpand) ? 1 : -1;

		ptTweenDest[iExpandIndex] += (GetPreferredSize()[iExpandIndex] + m_fSubBtnSpacing) * iExpandDir;
		for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		{
			HyButton *pSubBtn = m_SubBtnList[i];
			if(m_uiEntityAttribs & COMBOBOXATTRIB_IsInstantExpand)
			{
				pSubBtn->pos.Set(ptTweenDest[0], ptTweenDest[1]);
				pSubBtn->alpha.Set(1.0f);
			}
			else
			{
				pSubBtn->pos.Tween(ptTweenDest[0], ptTweenDest[1], 0.5f, HyTween::QuadOut);
				pSubBtn->alpha.Tween(1.0f, fTweenExpandDur * 0.5f);
			}

			ptTweenDest[iExpandIndex] += (pSubBtn->GetPreferredSize()[iExpandIndex] + m_fSubBtnSpacing) * iExpandDir;
		}

		if(m_uiEntityAttribs & COMBOBOXATTRIB_IsInstantExpand)
			m_SubBtnPanel.SetSize(static_cast<uint32>(ptTweenDest[0]), static_cast<uint32>(ptTweenDest[1]));
		else
		{
			m_ExpandAnimVec.Set(0.0f, 0.0f);
			m_ExpandAnimVec.Tween(ptTweenDest[0], ptTweenDest[1], 0.5f, HyTween::QuadOut, 0.0f, [this](IHyNode *pThis) { m_SubBtnPanel.SetSize(static_cast<uint32>(m_ExpandAnimVec.X()), static_cast<uint32>(m_ExpandAnimVec.Y())); });
		}
	}
	else
	{
		for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		{
			HyButton *pSubBtn = m_SubBtnList[i];
			pSubBtn->alpha.Tween(0.0f, fTweenExpandDur * 0.5f);
			pSubBtn->SetEnabled(false);
		}
	}
	
	m_uiEntityAttribs |= COMBOBOXATTRIB_IsTransition;
}

void HyComboBox::SetExpandedTimeout(float fTimeoutDuration)
{
	m_fExpandedTimeout = fTimeoutDuration;
}

void HyComboBox::ResetExpandedTimeout()
{
	m_fElapsedExpandedTime = 0.0f;
}

/*virtual*/ void HyComboBox::Update() /*override*/
{
	HyButton::Update();

	if(m_ExpandAnimVec.IsAnimating())
		m_SubBtnPanel.SetSize(static_cast<uint32>(m_ExpandAnimVec.X()), static_cast<uint32>(m_ExpandAnimVec.Y()));

	switch(m_uiEntityAttribs & COMBOBOXATTRIB_STATEMASK)
	{
	case 0:
		break;

	case COMBOBOXATTRIB_IsExpanded | COMBOBOXATTRIB_IsTransition: { // Is collapsing
		bool bAllFinished = true;
		for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		{
			if(m_SubBtnList[i]->pos.IsAnimating())
			{
				bAllFinished = false;
				break;
			}
		}
		if(bAllFinished)
			m_uiEntityAttribs &= ~COMBOBOXATTRIB_STATEMASK;
		break; }

	case COMBOBOXATTRIB_IsExpanded:
		if(m_uiEntityAttribs & COMBOBOXATTRIB_NeedsRetracting)
		{
			ToggleExpanded();
			m_uiEntityAttribs &= ~COMBOBOXATTRIB_NeedsRetracting;
		}
		else
		{
			if((m_uiEntityAttribs & COMBOBOXATTRIB_IsExpandMouseDwn) == 0 && HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft))
				m_uiEntityAttribs |= COMBOBOXATTRIB_IsExpandMouseDwn;

			if(IsMouseHover())
				ResetExpandedTimeout();

			m_fElapsedExpandedTime += HyEngine::DeltaTime();
			if(((m_uiEntityAttribs & COMBOBOXATTRIB_IsExpandMouseDwn) && HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft) == false) ||
				(m_fExpandedTimeout != 0.0f && m_fElapsedExpandedTime >= m_fExpandedTimeout))
			{
				m_uiEntityAttribs |= COMBOBOXATTRIB_NeedsRetracting;
			}
		}
		break;

	case COMBOBOXATTRIB_IsTransition: { // Is Expanding
		bool bAllFinished = true;
		for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		{
			if(m_SubBtnList[i]->pos.IsAnimating())
			{
				bAllFinished = false;
				break;
			}
		}
		if(bAllFinished)
		{
			for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
				m_SubBtnList[i]->SetEnabled(m_SubBtnEnabledMap[m_SubBtnList[i]]);

			ResetExpandedTimeout();
			m_uiEntityAttribs &= ~(COMBOBOXATTRIB_IsTransition | COMBOBOXATTRIB_IsExpandMouseDwn);
			m_uiEntityAttribs |= COMBOBOXATTRIB_IsExpanded;
		}
		break; }
	}
}
