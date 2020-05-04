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

HyInfoPanel::HyInfoPanel(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_pPanel(nullptr),
	m_pText(nullptr)
{
}

HyInfoPanel::HyInfoPanel(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent) :
	HyEntity2d(nullptr),
	m_pPanel(nullptr),
	m_pText(nullptr)
{
	Init(szPanelPrefix, szPanelName, pParent);
}

HyInfoPanel::HyInfoPanel(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyEntity2d(nullptr),
	m_pPanel(nullptr),
	m_pText(nullptr)
{
	Init(szTextPrefix, szTextName, iTextDimensionsX, iTextDimensionsY, pParent);
}

HyInfoPanel::HyInfoPanel(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyEntity2d(nullptr),
	m_pPanel(nullptr),
	m_pText(nullptr)
{
	Init(szPanelPrefix, szPanelName, szTextPrefix, szTextName, iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY, pParent);
}

HyInfoPanel::~HyInfoPanel()
{
	delete m_pPanel;
	delete m_pText;
}

/*virtual*/ void HyInfoPanel::Init(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent)
{
	Init(szPanelPrefix, szPanelName, nullptr, nullptr, 0, 0, 0, 0, pParent);
}

/*virtual*/ void HyInfoPanel::Init(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent)
{
	Init(nullptr, nullptr, szTextPrefix, szTextName, 0, 0, iTextDimensionsX, iTextDimensionsY, pParent);
}

/*virtual*/ void HyInfoPanel::Init(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent)
{
	delete m_pPanel;
	delete m_pText;

	m_pPanel = szPanelName != nullptr ? HY_NEW HySprite2d(szPanelPrefix, szPanelName, this) : nullptr;
	m_pText = szTextName != nullptr ? HY_NEW HyText2d(szTextPrefix, szTextName, this) : nullptr;

	if(m_pText)
	{
		m_pText->TextSetAlignment(HYALIGN_Center);
		SetTextLocation(iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY);
	}

	Reinitialize("", "", pParent);
}

HySprite2d *HyInfoPanel::GetPanelPtr()
{
	return m_pPanel;
}

HyText2d *HyInfoPanel::GetTextPtr()
{
	return m_pText;
}

/*virtual*/ void HyInfoPanel::SetPanelState(uint32 uiAnimIndex, bool bResetAnim)
{
	if(m_pPanel)
	{
		m_pPanel->AnimSetState(uiAnimIndex);

		if(bResetAnim)
			m_pPanel->AnimSetFrame(0);

		if(m_pText)
			SetTextLocation(m_vTextOffset.x, m_vTextOffset.y, static_cast<int32>(m_pText->TextGetBox().x), static_cast<int32>(m_pText->TextGetBox().y));
	}
}

float HyInfoPanel::GetPanelWidth()
{
	if(m_pPanel)
		return m_pPanel->AnimGetCurFrameWidth(true);
	
	return 0.0f;
}

float HyInfoPanel::GetPanelHeight()
{
	if(m_pPanel)
		return m_pPanel->AnimGetCurFrameHeight(true);

	return 0.0f;
}

/*virtual*/ std::string HyInfoPanel::GetStr()
{
	if(m_pText)
		return m_pText->TextGet();

	return std::string();
}

/*virtual*/ void HyInfoPanel::SetStr(std::string sText)
{
	if(m_pText)
		m_pText->TextSet(sText);
}

/*virtual*/ void HyInfoPanel::SetTextLocation(int32 iOffsetX, int32 iOffsetY, int32 iWidth, int32 iHeight)
{
	if(m_pText == nullptr)
		return;

	m_vTextOffset.x = iOffsetX;
	m_vTextOffset.y = iOffsetY;

	glm::ivec2 vPanelOffset(0);
	if(m_pPanel)
	{
		const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_pPanel->AcquireData());
		HyAssert(pPanelData, "HyInfoPanel could not acquire data for its panel");
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_pPanel->AnimGetState(), m_pPanel->AnimGetFrame());
		vPanelOffset = frameRef.vOFFSET;
	}
	
	m_pText->pos.Set(m_vTextOffset.x + vPanelOffset.x, m_vTextOffset.y + vPanelOffset.y);
	m_pText->SetAsScaleBox(static_cast<float>(iWidth), static_cast<float>(iHeight), true);
}

/*virtual*/ void HyInfoPanel::SetTextAlignment(HyTextAlign eAlignment)
{
	if(m_pText)
		m_pText->TextSetAlignment(eAlignment);
}

/*virtual*/ glm::vec2 HyInfoPanel::GetTextScaleBox()
{
	if(m_pText)
		return m_pText->TextGetBox();

	return glm::vec2(0.0f);
}
