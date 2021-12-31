/**************************************************************************
*	HyPanel.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Components/HyPanel.h"

HyPanelInit::HyPanelInit() :
	m_uiWidth(0),
	m_uiHeight(0),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0x252526)),
	m_FrameColor(HyColor(0x3F3F41)),
	m_ePanelType(PANELTYPE_Null)
{
}

HyPanelInit::HyPanelInit(std::string sSpritePrefix, std::string sSpriteName) :
	m_sSpritePrefix(sSpritePrefix),
	m_sSpriteName(sSpriteName),
	m_uiWidth(0),
	m_uiHeight(0),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0x252526)),
	m_FrameColor(HyColor(0x3F3F41)),
	m_ePanelType(PANELTYPE_Sprite)
{
}

HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize /*= 4*/, HyColor panelColor /*= HyColor(0x252526)*/, HyColor frameColor /*= HyColor(0x3F3F41)*/) :
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_uiFrameSize(uiFrameSize),
	m_PanelColor(panelColor),
	m_FrameColor(frameColor),
	m_ePanelType(PANELTYPE_Primitive)
{ }

HyPanel::HyPanel(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_Panel(this),
	m_Frame1(this),
	m_Frame2(this)
{
	Setup(HyPanelInit());
}

HyPanel::HyPanel(const HyPanelInit &initRef, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_Panel(this),
	m_Frame1(this),
	m_Frame2(this)
{
	Setup(initRef);
}

/*virtual*/ HyPanel::~HyPanel()
{
}

void HyPanel::Setup(const HyPanelInit &initRef)
{
	m_ePanelType = initRef.m_ePanelType;

	switch(m_ePanelType)
	{
	case HyPanelInit::PANELTYPE_Null:
		SetAsNull();
		break;

	case HyPanelInit::PANELTYPE_Sprite:
		m_SpritePanel.Init(initRef.m_sSpritePrefix, initRef.m_sSpriteName, this);
		if(m_SpritePanel.IsLoadDataValid() == false)
			SetAsNull();
		break;

	case HyPanelInit::PANELTYPE_Primitive:
		m_SpritePanel.Uninit();
		
		HySetVec(m_vSize, initRef.m_uiWidth, initRef.m_uiHeight);
		m_uiFrameSize = initRef.m_uiFrameSize;
		ConstructPrimitives();
		SetPanelColor(initRef.m_PanelColor);
		SetFrameColor(initRef.m_FrameColor);
		break;
	}
}

void HyPanel::SetAsNull()
{
	m_SpritePanel.Uninit();

	m_Panel.SetAsNothing();
	m_Frame1.SetAsNothing();
	m_Frame2.SetAsNothing();

	m_ePanelType = HyPanelInit::PANELTYPE_Null;
}

bool HyPanel::IsValid()
{
	return m_ePanelType != HyPanelInit::PANELTYPE_Null;
}

bool HyPanel::IsPrimitive() const
{
	return m_ePanelType == HyPanelInit::PANELTYPE_Primitive;
}

bool HyPanel::IsSprite()
{
	return m_ePanelType == HyPanelInit::PANELTYPE_Sprite;
}

HySprite2d &HyPanel::GetSprite()
{
	return m_SpritePanel;
}

uint32 HyPanel::GetSpriteState() const
{
	return m_SpritePanel.GetState();
}

void HyPanel::SetSpriteState(uint32 uiStateIndex)
{
	m_SpritePanel.SetState(uiStateIndex);
}

glm::ivec2 HyPanel::GetSizeHint()
{
	if(IsPrimitive())
		return GetSize();
	else if(IsSprite())
	{
		return glm::ivec2(m_SpritePanel.GetStateMaxWidth(m_SpritePanel.GetState(), false),
						  m_SpritePanel.GetStateMaxHeight(m_SpritePanel.GetState(), false));
	}
	
	return m_vSize;
}

uint32 HyPanel::GetWidth()
{
	if(IsSprite())
		return m_SpritePanel.GetStateMaxWidth(m_SpritePanel.GetState(), true);
	else
	{
		glm::vec3 vScale;
		glm::quat quatRot;
		glm::vec3 ptTranslation;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(this->GetSceneTransform(), vScale, quatRot, ptTranslation, vSkew, vPerspective);

		return m_vSize.x * vScale.x;
	}
}

uint32 HyPanel::GetHeight()
{
	if(IsSprite())
		return m_SpritePanel.GetStateMaxHeight(m_SpritePanel.GetState(), true);
	else
	{
		glm::vec3 vScale;
		glm::quat quatRot;
		glm::vec3 ptTranslation;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(this->GetSceneTransform(), vScale, quatRot, ptTranslation, vSkew, vPerspective);

		return m_vSize.y * vScale.y;
	}
}

glm::ivec2 HyPanel::GetSize()
{
	return glm::ivec2(GetWidth(), GetHeight());
}

void HyPanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	HySetVec(m_vSize, uiWidth, uiHeight);

	if(IsPrimitive())
		ConstructPrimitives();
	else if(IsSprite())
	{
		auto vUiSizeHint = GetSizeHint();
		scale.X(static_cast<float>(uiWidth) / vUiSizeHint.x);
		scale.Y(static_cast<float>(uiHeight) / vUiSizeHint.y);
	}
}

uint32 HyPanel::GetFrameSize() const
{
	return m_uiFrameSize;
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(IsPrimitive())
		return glm::vec2(0, 0);
	else if(m_SpritePanel.IsLoadDataValid())
	{
		glm::vec2 vPanelDimensions = GetSize();

		const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_SpritePanel.AcquireData());
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_SpritePanel.GetState(), m_SpritePanel.GetFrame());

		auto vUiSizeHint = GetSizeHint();
		return -glm::vec2(frameRef.vOFFSET.x * (vPanelDimensions.x / vUiSizeHint.x), frameRef.vOFFSET.y * (vPanelDimensions.y / vUiSizeHint.y));
	}

	return glm::vec2(0.0f, 0.0f);
}

HyColor HyPanel::GetPanelColor() const
{
	return HyColor(m_Panel.topColor.X(), m_Panel.topColor.Y(), m_Panel.topColor.Z());
}

void HyPanel::SetPanelColor(HyColor color)
{
	m_Panel.SetTint(color);
}

HyColor HyPanel::GetFrameColor() const
{
	return HyColor(m_Frame1.topColor.X(), m_Frame1.topColor.Y(), m_Frame1.topColor.Z());
}

void HyPanel::SetFrameColor(HyColor color)
{
	m_Frame1.SetTint(color);
	if(color.IsDark())
		m_Frame2.SetTint(color.Lighten());
	else
		m_Frame2.SetTint(color.Darken());
}

void HyPanel::ConstructPrimitives()
{
	m_Panel.SetAsBox(m_vSize.x - (m_uiFrameSize * 2), m_vSize.y - (m_uiFrameSize * 2));
	m_Panel.pos.Set(static_cast<int32>(m_uiFrameSize), static_cast<int32>(m_uiFrameSize));

	if(m_uiFrameSize > 0)
	{
		m_Frame1.SetAsBox(m_vSize.x, m_vSize.y);

		uint32 uiHalfFrameSize = m_uiFrameSize / 3;
		if(uiHalfFrameSize > 0)
		{
			m_Frame2.SetAsBox(m_vSize.x - (m_uiFrameSize * 2) + (uiHalfFrameSize * 2), m_vSize.y - (m_uiFrameSize * 2) + (uiHalfFrameSize * 2));
			m_Frame2.pos.Set(static_cast<int32>(m_uiFrameSize - uiHalfFrameSize), static_cast<int32>(m_uiFrameSize - uiHalfFrameSize));
		}
		else
			m_Frame2.SetAsNothing();
	}
	else
	{
		m_Frame1.SetAsNothing();
		m_Frame2.SetAsNothing();
	}
}
