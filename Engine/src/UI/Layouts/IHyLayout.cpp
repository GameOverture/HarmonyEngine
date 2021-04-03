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

IHyLayout::IHyLayout(HyEntity2d *pParent /*= nullptr*/) :
	IHyLayoutItem(pParent)
{
}

/*virtual*/ IHyLayout::~IHyLayout()
{
}

void IHyLayout::AddItem(IHyLayoutItem *pItem)
{
	if(pItem)
		ChildAppend(*pItem);
	OnDoLayout();
}

void IHyLayout::SetSize(int32 iWidth, int32 iHeight)
{
	HySetVec(m_vSize, iWidth, iHeight);
	OnDoLayout();
}

void IHyLayout::SetMargins(int32 iLeft, int32 iTop, int32 iRight, int32 iBottom, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY)
{
	m_Margins.Set(iLeft, iTop, iRight, iBottom);
	m_Margins.iTag = uiWidgetSpacingX | (uiWidgetSpacingY << 16);
	OnDoLayout();
}

uint16 IHyLayout::GetHorizontalSpacing()
{
	return m_Margins.iTag & 0x0000FFFF;
}

uint16 IHyLayout::GetVerticalSpacing()
{
	return (m_Margins.iTag & 0xFFFF0000) >> 16;
}
