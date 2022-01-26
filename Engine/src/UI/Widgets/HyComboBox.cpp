/**************************************************************************
*	HyComboBox.h
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
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f)
{
}

HyComboBox::HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, pParent),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f)
{
	OnSetup();
}

HyComboBox::HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f)
{
	OnSetup();
}

/*virtual*/ HyComboBox::~HyComboBox()
{
	ClearSubButtons();
}

uint32 HyComboBox::InsertSubButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/, std::string sAudioPrefix /*= ""*/, std::string sAudioName /*= ""*/)
{
	HyButton *pNewBtn = HY_NEW HyButton(initRef, sTextPrefix, sTextName, this);
	pNewBtn->SetButtonClickedCallback(fpCallBack, pParam, sAudioPrefix, sAudioName);
	pNewBtn->alpha.Set(0.0f);

	m_SubBtnList.push_back(pNewBtn);
	return static_cast<uint32>(m_SubBtnList.size()) - 1;
}

void HyComboBox::ClearSubButtons()
{
	for(int32 i = 0; i < static_cast<int32>(m_SubBtnList.size()); ++i)
		delete m_SubBtnList[i];

	m_SubBtnList.clear();
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

		ptTweenDest[iExpandIndex] += (GetSizeHint()[iExpandIndex] * iExpandDir);
		for(uint32 i = 0; i < m_SubBtnList.size(); ++i)
		{
			m_SubBtnList[i]->pos.Tween(ptTweenDest[0], ptTweenDest[1], 0.5f, HyTween::QuadOut);
			m_SubBtnList[i]->alpha.Tween(1.0f, fTweenExpandDur * 0.5f);
			ptTweenDest[iExpandIndex] += (m_SubBtnList[i]->GetSizeHint()[iExpandIndex] * iExpandDir);
		}
		m_uiAttribs |= COMBOBOXATTRIB_IsTransition;
	}
	else
	{
		for(uint32 i = 0; i < m_SubBtnList.size(); ++i)
		{
			m_SubBtnList[i]->pos.Tween(0.0f, 0.0f, fTweenExpandDur * 0.5f, HyTween::Linear);
			m_SubBtnList[i]->alpha.Tween(1.0f, fTweenExpandDur * 0.5f);
		}

		m_uiAttribs &= ~COMBOBOXATTRIB_IsExpanded;
		m_uiAttribs |= COMBOBOXATTRIB_IsTransition;
	}
}

void HyComboBox::SetExpandedTimeout(float fTimeoutDuration)
{
	m_fExpandedTimeout = fTimeoutDuration;
}

void HyComboBox::ResetExpandedTimeout()
{
	m_fElapsedExpandedTime = 0.0f;
}

HyButton *HyComboBox::GetSubBtn(uint32 uiIndex)
{
	if(uiIndex >= m_SubBtnList.size())
		return nullptr;

	return m_SubBtnList[uiIndex];
}

/*virtual*/ void HyComboBox::OnUpdate() /*override*/
{
	switch(m_uiAttribs & COMBOBOXATTRIB_STATEMASK)
	{
	case 0:
		break;

	case COMBOBOXATTRIB_IsTransition: { // Is expanding (from closed)
		bool bAllFinished = true;
		for(uint32 i = 0; i < m_SubBtnList.size(); ++i)
		{
			if(m_SubBtnList[i]->pos.IsAnimating())
			{
				bAllFinished = false;
				break;
			}
		}

		if(bAllFinished)
		{
			ResetExpandedTimeout();
			m_uiAttribs &= ~COMBOBOXATTRIB_IsTransition;
			m_uiAttribs |= COMBOBOXATTRIB_IsExpanded;
		}
		break; }

	case COMBOBOXATTRIB_IsExpanded:
		m_fElapsedExpandedTime += HyEngine::DeltaTime();
		if(m_fExpandedTimeout != 0.0f && m_fElapsedExpandedTime >= m_fExpandedTimeout)
			ToggleExpanded();
		break;

	case COMBOBOXATTRIB_IsExpanded | COMBOBOXATTRIB_IsTransition: { // Is Collapsing
		bool bAllFinished = true;
		for(uint32 i = 0; i < m_SubBtnList.size(); ++i)
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
	}
}

/*virtual*/ void HyComboBox::OnSetup() /*override*/
{
	HyButton::OnSetup();
	//SetButtonClickedCallback(
}
