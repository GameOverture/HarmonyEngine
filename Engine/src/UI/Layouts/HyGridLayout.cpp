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
#include "UI/Widgets/HySpacer.h"

HyGridLayout::HyGridLayout(HyEntity2d *pParent /*= nullptr*/) :
	IHyLayout(HYLAYOUT_Grid, pParent)
{
}

/*virtual*/ HyGridLayout::~HyGridLayout()
{
	for(uint32 i = 0; i < m_SpacerList.size(); ++i)
		delete m_SpacerList[i];
}

void HyGridLayout::InsertItem(int32 iX, int32 iY, HyEntityUi *pItem)
{
	if(pItem)
		ChildAppend(*pItem);
	else
	{
		// Create "null" layout item to fill index location (utilized by grid layouts)
		HySpacer *pNullItem = HY_NEW HySpacer;
		m_SpacerList.push_back(pNullItem);
		ChildAppend(*pNullItem);
	}
	OnSetLayoutItems();
}

void HyGridLayout::Clear()
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	for(uint32 i = 0; i < m_SpacerList.size(); ++i)
		delete m_SpacerList[i];

	m_GridSize.x = m_GridSize.y = 0;
}

/*virtual*/ void HyGridLayout::OnSetLayoutItems() /*override*/
{
	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	SetLayoutItems(m_GridSize.x, m_GridSize.y);
}
