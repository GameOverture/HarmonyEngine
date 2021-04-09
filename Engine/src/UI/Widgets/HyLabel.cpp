/**************************************************************************
*	HyLabel.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Diagnostics/Console/IHyConsole.h"

HyLabel::PrimPanel::PrimPanel(float fWidth, float fHeight, float fStroke, HyEntity2d *pParent) :
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

HyLabel::HyLabel(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
}

HyLabel::HyLabel(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName);
}

HyLabel::HyLabel(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

HyLabel::HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName);
}

HyLabel::HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

/*virtual*/ HyLabel::~HyLabel()
{
	delete m_pPrimPanel;
}

void HyLabel::Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_uiInfoPanelAttribs |= INFOPANELATTRIB_IsPrimitive;

	m_SpritePanel.Uninit();
	delete m_pPrimPanel;
	m_pPrimPanel = HY_NEW PrimPanel(fWidth, fHeight, fStroke, this);

	OnSetup("", "", sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

void HyLabel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_IsPrimitive;

	delete m_pPrimPanel;
	m_pPrimPanel = nullptr;
	m_SpritePanel.Init(sPanelPrefix, sPanelName, this);

	OnSetup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

float HyLabel::GetPanelWidth()
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

float HyLabel::GetPanelHeight()
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

uint32 HyLabel::GetSpriteState() const
{
	return m_SpritePanel.GetState();
}

/*virtual*/ void HyLabel::SetSpriteState(uint32 uiStateIndex)
{
	if(m_SpritePanel.IsLoadDataValid() == false)
	{
		HyLogWarning("HyLabel::SetSpriteState was invoked with an invalid panel sprite");
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

/*virtual*/ std::string HyLabel::GetText() const
{
	return m_Text.GetText();
}

/*virtual*/ void HyLabel::SetText(std::string sText)
{
	m_Text.SetText(sText);
}

/*virtual*/ void HyLabel::SetTextState(uint32 uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
}

/*virtual*/ void HyLabel::SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY)
{
	m_Text.SetTextAlignment(HYALIGN_HCenter);

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

/*virtual*/ void HyLabel::SetTextAlignment(HyAlignment eAlignment)
{
	m_Text.SetTextAlignment(eAlignment);
}

/*virtual*/ void HyLabel::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_Text.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

bool HyLabel::IsPrimitivePanel() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsPrimitive) != 0;
}

bool HyLabel::IsDisabled() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsDisabled) != 0;
}

/*virtual*/ void HyLabel::SetAsDisabled(bool bIsDisabled)
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

bool HyLabel::IsHighlighted() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsHighlighted) != 0;
}

/*virtual*/ void HyLabel::SetAsHighlighted(bool bIsHighlighted)
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

bool HyLabel::IsHideDisabled() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_HideDisabled) != 0;
}

void HyLabel::SetHideDisabled(bool bIsHideDisabled)
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

HyEntity2d *HyLabel::GetPrimitiveNode()
{
	return	m_pPrimPanel;
}

HySprite2d &HyLabel::GetSpriteNode()
{
	return m_SpritePanel;
}

HyText2d &HyLabel::GetTextNode()
{
	return m_Text;
}

/*virtual*/ void HyLabel::OnSetup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_Text.Init(sTextPrefix, sTextName, this);

	SetTextLocation(iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
	SetAsDisabled(IsDisabled());
	SetAsHighlighted(IsHighlighted());

	// Determine the m_vUiSizeHint
	if(m_pPrimPanel)
	{
		glm::vec2 vCachedScale = m_pPrimPanel->scale.Get();
		m_pPrimPanel->scale.Set(1.0f, 1.0f);

		auto &aabb = m_pPrimPanel->GetSceneAABB();
		if(aabb.IsValid())
			HySetVec(m_vUiSizeHint, aabb.GetExtents().x * 2.0f, aabb.GetExtents().y * 2.0f);
	}
	else if(m_SpritePanel.IsLoadDataValid())
		HySetVec(m_vUiSizeHint, m_SpritePanel.GetCurFrameWidth(false), m_SpritePanel.GetCurFrameHeight(false));
	else if(m_Text.IsLoadDataValid())
		HySetVec(m_vUiSizeHint, m_Text.GetTextWidth(false), m_Text.GetTextHeight(false));
}

/*virtual*/ glm::vec2 HyLabel::GetPosOffset() /*override*/
{
	b2Vec2 ptLowerBound(0.0f, 0.0f);
	if(m_pPrimPanel)
		ptLowerBound = m_pPrimPanel->GetSceneAABB().lowerBound;
	else if(m_SpritePanel.IsLoadDataValid())
		ptLowerBound = m_SpritePanel.GetSceneAABB().lowerBound;
	else if(m_Text.IsLoadDataValid())
		ptLowerBound = m_Text.GetSceneAABB().lowerBound;

	return glm::vec2(pos.Get() - glm::vec2(ptLowerBound.x, ptLowerBound.y));
}

/*virtual*/ void HyLabel::OnResize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
	if(m_pPrimPanel)
	{
		m_pPrimPanel->scale.Set(1.0f, 1.0f);

		if(m_vUiSizeHint.x != 0.0f)
			m_pPrimPanel->scale.X((m_pPrimPanel->scale.X() * iNewWidth) / m_vUiSizeHint.x);
		if(m_vUiSizeHint.y != 0.0f)
			m_pPrimPanel->scale.Y((m_pPrimPanel->scale.Y() * iNewHeight) / m_vUiSizeHint.y);
	}
	else if(m_SpritePanel.IsLoadDataValid())
	{
		m_SpritePanel.scale.Set(1.0f, 1.0f);

		if(m_vUiSizeHint.x != 0.0f)
			m_SpritePanel.scale.X((m_SpritePanel.scale.X() * iNewWidth) / m_vUiSizeHint.x);
		if(m_vUiSizeHint.y != 0.0f)
			m_SpritePanel.scale.Y((m_SpritePanel.scale.Y() * iNewHeight) / m_vUiSizeHint.y);
	}
	else if(m_Text.IsLoadDataValid())
	{
		m_Text.scale.Set(1.0f, 1.0f);

		if(m_vUiSizeHint.x != 0.0f)
			m_Text.scale.X((m_Text.scale.X() * iNewWidth) / m_vUiSizeHint.x);
		if(m_vUiSizeHint.y != 0.0f)
			m_Text.scale.Y((m_Text.scale.Y() * iNewHeight) / m_vUiSizeHint.y);
	}


	//scale.Set(1.0f, 1.0f);

	////auto &aabb = GetSceneAABB();
	////float fCurWidth = aabb.GetExtents().x * 2.0f;
	////float fCurHeight = aabb.GetExtents().y * 2.0f;

	////scale.Set((scale.X() * iNewWidth) / fCurWidth, (scale.Y() * iNewHeight) / fCurHeight);

	//glm::ivec2 vSizeHint = GetSizeHint();
	//if(vSizeHint.x != 0.0f)
	//	scale.X((scale.X() * iNewWidth) / vSizeHint.x);
	//if(vSizeHint.y != 0.0f)
	//	scale.Y((scale.Y() * iNewHeight) / vSizeHint.y);
}
