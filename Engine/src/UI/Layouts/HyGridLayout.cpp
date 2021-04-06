/**************************************************************************
*	HyGridLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/HyGridLayout.h"

HyGridLayout::HyGridLayout(HyEntity2d *pParent /*= nullptr*/) :
	IHyLayout(pParent)
{
}

/*virtual*/ HyGridLayout::~HyGridLayout()
{
	for(uint32 i = 0; i < m_NullItemList.size(); ++i)
		delete m_NullItemList[i];
}

void HyGridLayout::InsertLayoutItem(int32 iX, int32 iY, HyUI *pItem)
{
	if(pItem)
		ChildAppend(*pItem);
	else
	{
		// Create "null" layout item to fill index location (utilized by grid layouts)
		HyUI *pNullItem = HY_NEW HyUI;
		m_NullItemList.push_back(pNullItem);
		ChildAppend(*pNullItem);
	}
	OnSetLayoutItems();
}

void HyGridLayout::Clear()
{
	while(m_ChildList.empty() == false)
		delete m_ChildList[m_ChildList.size() - 1];

	for(uint32 i = 0; i < m_NullItemList.size(); ++i)
		delete m_NullItemList[i];
}

/*virtual*/ void HyGridLayout::OnSetLayoutItems() /*override*/
{
	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	SetLayoutItems(m_GridSize.x, m_GridSize.y);
}
