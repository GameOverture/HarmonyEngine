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
#include "UI/Containers/HyContainer.h"

IHyLayout::IHyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent /*= nullptr*/) :
	HyEntityUi(Ui_Layout, pParent),
	m_pContainerParent(nullptr),
	m_eLAYOUT_TYPE(eLayoutType),
	m_vSize(0, 0)
{
}

/*virtual*/ IHyLayout::~IHyLayout()
{
}

void IHyLayout::AppendItem(HyEntityUi &itemRef)
{
	ChildAppend(itemRef);
	SetLayoutItems();
}

/*virtual*/ void IHyLayout::ClearItems() /*override*/
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	OnClearItems();
	SetLayoutItems();
}

glm::ivec2 IHyLayout::GetSize()
{
	glm::ivec2 vCurSize = m_vSize;
	if(vCurSize.x == 0)
		vCurSize.x = GetSizeHint().x;
	if(vCurSize.y == 0)
		vCurSize.y = GetSizeHint().y;
	
	return vCurSize;
}

const HyRectangle<int32> &IHyLayout::GetMargins() const
{
	return m_Margins;
}

void IHyLayout::SetMargins(int32 iLeft, int32 iBottom, int32 iRight, int32 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY)
{
	m_Margins.Set(iLeft, iBottom, iRight, iTop);
	m_Margins.iTag = uiWidgetSpacingX | (uiWidgetSpacingY << 16);
	SetLayoutItems();
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

void IHyLayout::SetLayoutItems()
{
	OnSetLayoutItems();
	if(m_pContainerParent)
		m_pContainerParent->OnSetLayoutItems();
}

void IHyLayout::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	if(m_vSize.x == iNewWidth && m_vSize.y == iNewHeight)
		return;

	HySetVec(m_vSize, iNewWidth, iNewHeight);
	SetLayoutItems();
}

void IHyLayout::SetContainerParent(HyContainer *pContainerParent)
{
	m_pContainerParent = pContainerParent;
}

/*friend*/ void HyInternal_LayoutSetSize(IHyLayout &layoutRef, int32 iNewWidth, int32 iNewHeight)
{
	layoutRef.SetSize(iNewWidth, iNewHeight);
}
