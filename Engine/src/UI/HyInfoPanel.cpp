/**************************************************************************
*	HyInfoPanel.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyInfoPanel.h"
#include "Assets/Nodes/HySprite2dData.h"

HyInfoPanel::HyInfoPanel()
{
}

HyInfoPanel::HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName);
}

HyInfoPanel::HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY);
}

HyInfoPanel::HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

/*virtual*/ HyInfoPanel::~HyInfoPanel()
{
}

/*virtual*/ void HyInfoPanel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0);
}

/*virtual*/ void HyInfoPanel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, 0, 0);
}

/*virtual*/ void HyInfoPanel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_Panel.Init(sPanelPrefix, sPanelName, this);
	m_Text.Init(sTextPrefix, sTextName, this);
	
	m_Text.SetTextAlignment(HYALIGN_Center);
	SetTextLocation(iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

HySprite2d &HyInfoPanel::GetPanel()
{
	return m_Panel;
}

HyText2d &HyInfoPanel::GetText()
{
	return m_Text;
}

/*virtual*/ void HyInfoPanel::SetPanelState(uint32 uiAnimIndex, bool bResetAnim)
{
	m_Panel.SetState(uiAnimIndex);

	if(bResetAnim)
		m_Panel.SetFrame(0);

	SetTextLocation(static_cast<int32>(m_Text.GetTextBox().x), static_cast<int32>(m_Text.GetTextBox().y), m_vTextOffset.x, m_vTextOffset.y);
}

float HyInfoPanel::GetPanelWidth()
{
	return m_Panel.GetCurFrameWidth(true);
}

float HyInfoPanel::GetPanelHeight()
{
	return m_Panel.GetCurFrameHeight(true);
}

/*virtual*/ std::string HyInfoPanel::GetStr()
{
	return m_Text.GetText();
}

/*virtual*/ void HyInfoPanel::SetStr(std::string sText)
{
	m_Text.SetText(sText);
}

/*virtual*/ void HyInfoPanel::SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY)
{
	m_vTextOffset.x = iOffsetX;
	m_vTextOffset.y = iOffsetY;

	glm::ivec2 vPanelOffset(0);
	const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_Panel.AcquireData());
	if(pPanelData)
	{
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_Panel.GetState(), m_Panel.GetFrame());
		vPanelOffset = frameRef.vOFFSET;
	}
	
	m_Text.pos.Set(m_vTextOffset.x + vPanelOffset.x, m_vTextOffset.y + vPanelOffset.y);
	m_Text.SetAsScaleBox(static_cast<float>(iWidth), static_cast<float>(iHeight), true);
}

/*virtual*/ void HyInfoPanel::SetTextAlignment(HyTextAlign eAlignment)
{
	m_Text.SetTextAlignment(eAlignment);
}

/*virtual*/ glm::vec2 HyInfoPanel::GetTextScaleBox()
{
	return m_Text.GetTextBox();
}
