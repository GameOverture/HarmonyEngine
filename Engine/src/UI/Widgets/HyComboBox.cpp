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
	m_SubBtnPanel(this),
	m_fSubBtnSpacing(5.0f),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f)
{
}

HyComboBox::HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, pParent),
	m_SubBtnPanel(this),
	m_fSubBtnSpacing(5.0f),
	m_fElapsedExpandedTime(0.0f),
	m_fExpandedTimeout(0.0f)
{
	OnSetup();
}

HyComboBox::HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_SubBtnPanel(this),
	m_fSubBtnSpacing(5.0f),
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
	return InsertSubButton(initRef, sTextPrefix, sTextName, 0, 0, 0, 0, fpCallBack, pParam, sAudioPrefix, sAudioName);
}

uint32 HyComboBox::InsertSubButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/, std::string sAudioPrefix /*= ""*/, std::string sAudioName /*= ""*/)
{
	HyButton *pNewBtn = HY_NEW HyButton(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, this);
	pNewBtn->SetButtonClickedCallback(fpCallBack, pParam, sAudioPrefix, sAudioName);
	pNewBtn->SetAsEnabled(false);
	pNewBtn->alpha.Set(0.0f);
	pNewBtn->Load();

	m_SubBtnList.push_back(pNewBtn);

	return static_cast<uint32>(m_SubBtnList.size()) - 1;
}

void HyComboBox::ClearSubButtons()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		delete m_SubBtnList[i];

	m_SubBtnList.clear();
}

void HyComboBox::SetExpandType(HyOrientation eOrientation, bool bPositiveDirection, bool bAnimate)
{
	if(eOrientation == HYORIEN_Horizontal)
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

void HyComboBox::SetExpandPanel(std::string sSpritePrefix, std::string sSpriteName)
{
	m_SubBtnPanel.Setup(HyPanelInit(sSpritePrefix, sSpriteName));
}

void HyComboBox::SetExpandPanel(uint32 uiFrameSize, HyColor panelColor /*= HyColor(0x252526)*/, HyColor frameColor /*= HyColor(0x3F3F41)*/)
{
	m_SubBtnPanel.Setup(HyPanelInit(0, 0, uiFrameSize, panelColor, frameColor));
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
			if(m_uiAttribs & COMBOBOXATTRIB_IsInstantExpand)
			{
				pSubBtn->pos.Set(ptTweenDest[0], ptTweenDest[1]);
				pSubBtn->alpha.Set(1.0f);
			}
			else
			{
				pSubBtn->pos.Tween(ptTweenDest[0], ptTweenDest[1], 0.5f, HyTween::QuadOut);
				pSubBtn->alpha.Tween(1.0f, fTweenExpandDur * 0.5f);
			}

			ptTweenDest[iExpandIndex] += (pSubBtn->GetSizeHint()[iExpandIndex] + m_fSubBtnSpacing) * iExpandDir;
		}
		if(m_uiAttribs & COMBOBOXATTRIB_IsInstantExpand)
			m_SubBtnPanel.size.Set(ptTweenDest[0], ptTweenDest[1]);
		else
			m_SubBtnPanel.size.Tween(ptTweenDest[0], ptTweenDest[1], 0.5f, HyTween::QuadOut);
	}
	else
	{
		for(uint32 i = 0; i < static_cast<uint32>(m_SubBtnList.size()); ++i)
		{
			HyButton *pSubBtn = m_SubBtnList[i];
			
			pSubBtn->pos.Tween(0.0f, 0.0f, fTweenExpandDur * 0.5f, HyTween::Linear);
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

/*virtual*/ void HyComboBox::OnUpdate() /*override*/
{
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
				m_SubBtnList[i]->SetAsEnabled(true);

			ResetExpandedTimeout();
			m_uiAttribs &= ~(COMBOBOXATTRIB_IsTransition | COMBOBOXATTRIB_IsExpandMouseDwn);
			m_uiAttribs |= COMBOBOXATTRIB_IsExpanded;
		}
		break; }
	}
}

/*virtual*/ void HyComboBox::OnSetup() /*override*/
{
	HyButton::OnSetup();
	SetButtonClickedCallback(OnComboBoxClickedCallback, this);

	shape.SetAsBox(GetWidth(), GetHeight());
}

/*static*/ void HyComboBox::OnComboBoxClickedCallback(HyButton *pBtn, void *pData)
{
	HyComboBox *pThis = static_cast<HyComboBox *>(pData);
	pThis->ToggleExpanded();
}
