/**************************************************************************
*	GfxTextModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "GfxTextModel.h"

GfxTextModel::GfxTextModel(HyColor color)
{
	//SetData(color, eShape, floatList);
}

/*virtual*/ GfxTextModel::~GfxTextModel()
{
	//ClearFixtures();
}

void GfxTextModel::Refresh(HyText2d *pTextNode, bool bShowOutline)
{
	HyColor color = HyColor::Red;
	float fOutlineAlpha = bShowOutline ? 1.0f : 0.0f;

	// SetAsLine
	if(pTextNode->IsLine())
	{
		switch(pTextNode->GetAlignment())
		{
		case HYALIGN_Left:
		case HYALIGN_Justify:
			m_BoundingVolume.SetAsLineSegment(glm::vec2(0.0f), glm::vec2(pTextNode->GetWidth(), 0.0f));
			break;

		case HYALIGN_Center:
			m_BoundingVolume.SetAsLineSegment(glm::vec2(pTextNode->GetWidth(-0.5f), 0.0f), glm::vec2(pTextNode->GetWidth(0.5f), 0.0f));
			break;

		case HYALIGN_Right:
			m_BoundingVolume.SetAsLineSegment(glm::vec2(-pTextNode->GetWidth(), 0.0f), glm::vec2(0.0f, 0.0f));
			break;
		}
	}
	else if(pTextNode->IsScaleBox())
	{
		m_BoundingVolume.SetAsBox(pTextNode->GetTextBoxDimensions().x, pTextNode->GetTextBoxDimensions().y);
	}
	else if(pTextNode->IsColumn())
	{
		glm::vec2 ptCenter(pTextNode->GetTextBoxDimensions().x * 0.5f, pTextNode->GetHeight() * -0.5f);
		m_BoundingVolume.SetAsBox(HyRect(pTextNode->GetTextBoxDimensions().x * 0.5f, pTextNode->GetHeight() * 0.5f, ptCenter, 0.0f));
	}
	else if(pTextNode->IsVertical())
	{
		m_BoundingVolume.SetAsLineSegment(glm::vec2(0.0f), glm::vec2(0.0f, -pTextNode->GetHeight()));
	}
	else
		HyError("ShapeCtrl::SetAsText - Unknown HyText2d text attributes");

	const glm::mat4 &mtxSceneRef = pTextNode->GetSceneTransform(0.0f);
	m_BoundingVolume.TransformSelf(mtxSceneRef);
}
