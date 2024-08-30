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
	m_SubBtnPanel(this),
	m_fSubBtnSpacing(5.0f),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f),
	m_ExpandAnimVec(*this, 0)
{
	SetButtonClickedCallback(OnComboBoxClickedCallback, this);
	//m_Shape.SetAsBox(HyRect(GetWidth(), GetHeight()));
}

HyComboBox::HyComboBox(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textNodePath, pParent),
	m_Shape(this),
	m_SubBtnPanel(this),
	m_fSubBtnSpacing(5.0f),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f),
	m_ExpandAnimVec(*this, 0)
{
	SetButtonClickedCallback(OnComboBoxClickedCallback, this);
	//m_Shape.SetAsBox(HyRect(GetWidth(), GetHeight()));
}

HyComboBox::HyComboBox(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textNodePath, textMargins, pParent),
	m_Shape(this),
	m_SubBtnPanel(this),
	m_fSubBtnSpacing(5.0f),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f),
	m_ExpandAnimVec(*this, 0)
{
	SetButtonClickedCallback(OnComboBoxClickedCallback, this);
	//m_Shape.SetAsBox(HyRect(GetWidth(), GetHeight()));
}

/*virtual*/ HyComboBox::~HyComboBox()
{
	ClearSubButtons();
}

uint32 HyComboBox::InsertSubButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/, const HyNodePath &audioNodePath /*= HyNodePath()*/)
{
	return InsertSubButton(panelInit, textNodePath, HyMargins<float>(), fpCallBack, pParam, audioNodePath);
}

uint32 HyComboBox::InsertSubButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/, const HyNodePath &audioNodePath /*= HyNodePath()*/)
{
	HyButton *pNewBtn = HY_NEW HyButton(panelInit, textNodePath, textMargins, this);
	pNewBtn->SetButtonClickedCallback(fpCallBack, pParam, audioNodePath);
	pNewBtn->SetAsEnabled(false);
	pNewBtn->pos.Set(pNewBtn->GetPosOffset());
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
		m_SubBtnList[uiSubBtnIndex]->SetAsEnabled(true);
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

void HyComboBox::SetExpandPanel(const HyPanelInit &panelInit, HyOrientation eOrientation, bool bPositiveDirection, bool bAnimate)
{
	m_SubBtnPanel.Setup(panelInit);

	if(eOrientation == HYORIENT_Horizontal)
		m_uiAttribs |= COMBOBOXATTRIB_IsHorzExpand;
	else
		m_uiAttribs &= ~COMBOBOXATTRIB_IsHorzExpand;

	if(bPositiveDirection)
		m_uiAttribs |= COMBOBOXATTRIB_IsPositiveExpand;
	else
		m_uiAttribs &= ~COMBOBOXATTRIB_IsPositiveExpand;

	if(bAnimate)
		m_uiAttribs &= ~COMBOBOXATTRIB_IsInstantExpand;
	else
		m_uiAttribs |= COMBOBOXATTRIB_IsInstantExpand;
}

bool HyComboBox::IsExpanded() const
{
	return m_uiAttribs & COMBOBOXATTRIB_IsExpanded;
}

bool HyComboBox::IsTransition() const
{
	return m_uiAttribs & COMBOBOXATTRIB_IsTransition;
}

void HyComboBox::ToggleExpanded()
{
	if(IsTransition())
		return;

	float fTweenExpandDur = 0.5f;

	if(IsExpanded() == false) // Is collapsed
	{
		glm::vec2 ptTweenDest(0.0f, 0.0f);
		int32 iExpandIndex = (m_uiAttribs & COMBOBOXATTRIB_IsHorzExpand) ? 0 : 1;
		int32 iExpandDir = (m_uiAttribs & COMBOBOXATTRIB_IsPositiveExpand) ? 1 : -1;

		ptTweenDest[iExpandIndex] += (GetSizeHint()[iExpandIndex] + m_fSubBtnSpacing) * iExpandDir;
		for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		{
			HyButton *pSubBtn = m_SubBtnList[i];
			glm::vec2 vOffset = pSubBtn->GetPosOffset();
			if(m_uiAttribs & COMBOBOXATTRIB_IsInstantExpand)
			{
				pSubBtn->pos.Set(ptTweenDest[0] + vOffset.x, ptTweenDest[1] + vOffset.y);
				pSubBtn->alpha.Set(1.0f);
			}
			else
			{
				pSubBtn->pos.Tween(ptTweenDest[0] + vOffset.x, ptTweenDest[1] + vOffset.y, 0.5f, HyTween::QuadOut);
				pSubBtn->alpha.Tween(1.0f, fTweenExpandDur * 0.5f);
			}

			ptTweenDest[iExpandIndex] += (pSubBtn->GetSizeHint()[iExpandIndex] + m_fSubBtnSpacing) * iExpandDir;
		}

		if(m_uiAttribs & COMBOBOXATTRIB_IsInstantExpand)
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

			glm::vec2 vOffset = pSubBtn->GetPosOffset();
			pSubBtn->pos.Tween(vOffset.x, vOffset.y, fTweenExpandDur * 0.5f, HyTween::Linear);
			pSubBtn->alpha.Tween(0.0f, fTweenExpandDur * 0.5f);
			pSubBtn->SetAsEnabled(false);
		}
	}
	
	m_uiAttribs |= COMBOBOXATTRIB_IsTransition;
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

	switch(m_uiAttribs & COMBOBOXATTRIB_STATEMASK)
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
			m_uiAttribs &= ~COMBOBOXATTRIB_STATEMASK;
		break; }

	case COMBOBOXATTRIB_IsExpanded:
		if(m_uiAttribs & COMBOBOXATTRIB_NeedsRetracting)
		{
			ToggleExpanded();
			m_uiAttribs &= ~COMBOBOXATTRIB_NeedsRetracting;
		}
		else
		{
			if((m_uiAttribs & COMBOBOXATTRIB_IsExpandMouseDwn) == 0 && HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft))
				m_uiAttribs |= COMBOBOXATTRIB_IsExpandMouseDwn;

			m_fElapsedExpandedTime += HyEngine::DeltaTime();
			if(((m_uiAttribs & COMBOBOXATTRIB_IsExpandMouseDwn) && HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft) == false) ||
				(m_fExpandedTimeout != 0.0f && m_fElapsedExpandedTime >= m_fExpandedTimeout))
			{
				m_uiAttribs |= COMBOBOXATTRIB_NeedsRetracting;
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
				m_SubBtnList[i]->SetAsEnabled(m_SubBtnEnabledMap[m_SubBtnList[i]]);

			ResetExpandedTimeout();
			m_uiAttribs &= ~(COMBOBOXATTRIB_IsTransition | COMBOBOXATTRIB_IsExpandMouseDwn);
			m_uiAttribs |= COMBOBOXATTRIB_IsExpanded;
		}
		break; }
	}
}

/*static*/ void HyComboBox::OnComboBoxClickedCallback(HyButton *pBtn, void *pData)
{
	HyComboBox *pThis = static_cast<HyComboBox *>(pData);
	pThis->ToggleExpanded();
}
