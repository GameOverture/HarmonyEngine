/**************************************************************************
*	HyWidget.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyWidget.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Diagnostics/Console/IHyConsole.h"

HyWidget::PrimPanel::PrimPanel(float fWidth, float fHeight, float fStroke, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_Fill(this),
	m_Stroke(this)
{
	m_Fill.SetAsBox(fWidth, fHeight);
	m_Fill.SetTint(0.5f, 0.5f, 0.5f);
	
	m_Stroke.SetAsBox(fWidth, fHeight);
	m_Stroke.SetTint(0.3f, 0.3f, 0.3f);
	m_Stroke.SetWireframe(true);
	m_Stroke.SetLineThickness(fStroke);

	Load();
}

HyWidget::HyWidget(HyEntity2d *pParent /*= nullptr*/) :
	IHyLayoutItem(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
}

HyWidget::HyWidget(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyLayoutItem(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName);
}

HyWidget::HyWidget(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	IHyLayoutItem(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

HyWidget::HyWidget(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyLayoutItem(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName);
}

HyWidget::HyWidget(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	IHyLayoutItem(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

/*virtual*/ HyWidget::~HyWidget()
{
	delete m_pPrimPanel;
}

void HyWidget::Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyWidget::Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_uiInfoPanelAttribs |= INFOPANELATTRIB_IsPrimitive;

	m_SpritePanel.Uninit();
	delete m_pPrimPanel;
	m_pPrimPanel = HY_NEW PrimPanel(fWidth, fHeight, fStroke, this);
	m_Text.Init(sTextPrefix, sTextName, this);

	OnSetup("", "", sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

void HyWidget::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyWidget::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_IsPrimitive;

	delete m_pPrimPanel;
	m_pPrimPanel = nullptr;
	m_SpritePanel.Init(sPanelPrefix, sPanelName, this);
	m_Text.Init(sTextPrefix, sTextName, this);

	OnSetup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);

	auto aabb = GetSceneAABB();
	if(aabb.IsValid())
		HySetVec(m_vSizeHint, aabb.GetExtents().x * 2.0f, aabb.GetExtents().y * 2.0f);
}

float HyWidget::GetPanelWidth()
{
	if(m_pPrimPanel)
	{
		b2AABB tmpAABB;
		b2Transform tmpTransform;
		tmpTransform.SetIdentity();
		m_pPrimPanel->m_Fill.GetLocalBoundingVolume().GetB2Shape()->ComputeAABB(&tmpAABB, tmpTransform, 0);
		return tmpAABB.upperBound.x - tmpAABB.lowerBound.x;
	}
	else
		return m_SpritePanel.GetCurFrameWidth(true);
}

float HyWidget::GetPanelHeight()
{
	if(m_pPrimPanel)
	{
		b2AABB tmpAABB;
		b2Transform tmpTransform;
		tmpTransform.SetIdentity();
		m_pPrimPanel->m_Fill.GetLocalBoundingVolume().GetB2Shape()->ComputeAABB(&tmpAABB, tmpTransform, 0);
		return tmpAABB.upperBound.y - tmpAABB.lowerBound.y;
	}
	else
		return m_SpritePanel.GetCurFrameHeight(true);
}

uint32 HyWidget::GetSpriteState() const
{
	return m_SpritePanel.GetState();
}

/*virtual*/ void HyWidget::SetSpriteState(uint32 uiStateIndex)
{
	if(m_SpritePanel.IsLoadDataValid() == false)
	{
		HyLogWarning("HyWidget::SetSpriteState was invoked with an invalid panel sprite");
		return;
	}

	// Calculate the original text offset specified from an earlier call to Setup()
	glm::ivec2 vOrigTextOffset = m_Text.pos.Get();
	glm::ivec2 vPanelOffset(0);
	const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_SpritePanel.AcquireData());
	if(pPanelData)
	{
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_SpritePanel.GetState(), m_SpritePanel.GetFrame());
		vPanelOffset = frameRef.vOFFSET;
	}
	vOrigTextOffset.x -= vPanelOffset.x;
	vOrigTextOffset.y -= vPanelOffset.y;

	// Now set new sprite state, so below SetTextLocation() can offset properly onto it
	m_SpritePanel.SetState(uiStateIndex);

	// Realign text on new panel sprite state
	SetTextLocation(static_cast<int32>(m_Text.GetTextBox().x), static_cast<int32>(m_Text.GetTextBox().y), vOrigTextOffset.x, vOrigTextOffset.y);
}

/*virtual*/ std::string HyWidget::GetText() const
{
	return m_Text.GetText();
}

/*virtual*/ void HyWidget::SetText(std::string sText)
{
	m_Text.SetText(sText);
}

/*virtual*/ void HyWidget::SetTextState(uint32 uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
}

/*virtual*/ void HyWidget::SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY)
{
	m_Text.SetTextAlignment(HYALIGN_Center);

	glm::ivec2 vPanelOffset(0);
	if(m_pPrimPanel == nullptr)
	{
		const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_SpritePanel.AcquireData());
		if(pPanelData)
		{
			const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_SpritePanel.GetState(), m_SpritePanel.GetFrame());
			vPanelOffset = frameRef.vOFFSET;
		}
	}
	
	m_Text.pos.Set(iOffsetX + vPanelOffset.x, iOffsetY + vPanelOffset.y);

	if(iWidth > 0 && iHeight > 0)
		m_Text.SetAsScaleBox(static_cast<float>(iWidth), static_cast<float>(iHeight), true);
	else
		m_Text.SetAsScaleBox(GetPanelWidth(), GetPanelHeight(), true);
}

/*virtual*/ void HyWidget::SetTextAlignment(HyTextAlign eAlignment)
{
	m_Text.SetTextAlignment(eAlignment);
}

/*virtual*/ void HyWidget::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_Text.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

bool HyWidget::IsPrimitivePanel() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsPrimitive) != 0;
}

bool HyWidget::IsDisabled() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsDisabled) != 0;
}

/*virtual*/ void HyWidget::SetAsDisabled(bool bIsDisabled)
{
	if(bIsDisabled)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_IsDisabled;

		if(IsHideDisabled() == false)
			SetTint(0.3f, 0.3f, 0.3f);
	}
	else
	{
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_IsDisabled;
		SetTint(1.0f, 1.0f, 1.0f);
	}
}

bool HyWidget::IsHighlighted() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsHighlighted) != 0;
}

/*virtual*/ void HyWidget::SetAsHighlighted(bool bIsHighlighted)
{
	if(bIsHighlighted == IsHighlighted())
		return;

	if(bIsHighlighted)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_IsHighlighted;
		if(m_pPrimPanel)
		{
			m_pPrimPanel->m_Stroke.SetTint(0.0f, 0.0f, 1.0f);
			m_pPrimPanel->m_Stroke.SetLineThickness(m_pPrimPanel->m_Stroke.GetLineThickness() * 2.0f);
		}
	}
	else
	{
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_IsHighlighted;
		if(m_pPrimPanel)
		{
			m_pPrimPanel->m_Stroke.SetTint(0.3f, 0.3f, 0.3f);
			m_pPrimPanel->m_Stroke.SetLineThickness(m_pPrimPanel->m_Stroke.GetLineThickness() / 2.0f);
		}
	}
}

bool HyWidget::IsHideDisabled() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_HideDisabled) != 0;
}

void HyWidget::SetHideDisabled(bool bIsHideDisabled)
{
	if(bIsHideDisabled)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_HideDisabled;
		SetTint(1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_HideDisabled;
		SetAsDisabled(IsDisabled());
	}
}

HyEntity2d *HyWidget::GetPrimitiveNode()
{
	return	m_pPrimPanel;
}

HySprite2d &HyWidget::GetSpriteNode()
{
	return m_SpritePanel;
}

HyText2d &HyWidget::GetTextNode()
{
	return m_Text;
}

/*virtual*/ void HyWidget::OnSetup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	SetTextLocation(iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
	SetAsDisabled(IsDisabled());
	SetAsHighlighted(IsHighlighted());
}

/*virtual*/ void HyWidget::OnResize(int32 iNewWidth, int32 iNewHeight)
{
	auto aabb = GetSceneAABB();
	float fCurWidth = aabb.GetExtents().x * 2.0f;
	float fCurHeight = aabb.GetExtents().y * 2.0f;

	scale.Set((scale.X() * iNewWidth) / fCurWidth, (scale.Y() * iNewHeight) / fCurHeight);
}
