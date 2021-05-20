/**************************************************************************
*	IHyLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/IHyLayout.h"
#include "UI/Widgets/IHyWidget.h"

IHyLayout::IHyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent /*= nullptr*/) :
	HyEntityUi(Ui_Layout, pParent),
	m_eLAYOUT_TYPE(eLayoutType),
	m_vSize(0, 0),
	m_vPreferredSize(0, 0)
{
}

/*virtual*/ IHyLayout::~IHyLayout()
{
}

glm::ivec2 IHyLayout::GetSize() const
{
	return m_vSize;
}

void IHyLayout::SetMargins(int32 iLeft, int32 iBottom, int32 iRight, int32 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY)
{
	m_Margins.Set(iLeft, iBottom, iRight, iTop);
	m_Margins.iTag = uiWidgetSpacingX | (uiWidgetSpacingY << 16);
	OnSetLayoutItems();
}

uint16 IHyLayout::GetHorizontalSpacing()
{
	return m_Margins.iTag & 0x0000FFFF;
}

uint16 IHyLayout::GetVerticalSpacing()
{
	return (m_Margins.iTag & 0xFFFF0000) >> 16;
}

glm::ivec2 IHyLayout::GetSpacing()
{
	return glm::ivec2(GetHorizontalSpacing(), GetVerticalSpacing());
}

glm::ivec2 IHyLayout::GetPreferredSize() const
{
	return m_vPreferredSize;
}

void IHyLayout::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	HySetVec(m_vSize, iNewWidth, iNewHeight);
	OnSetLayoutItems();
}
