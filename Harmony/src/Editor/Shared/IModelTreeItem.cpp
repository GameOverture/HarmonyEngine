/**************************************************************************
 *	IModelTreeItem.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IModelTreeItem.h"

IModelTreeItem::IModelTreeItem() : m_pParentItem(nullptr)
{ }

/*virtual*/ IModelTreeItem::~IModelTreeItem()
{ }

IModelTreeItem *IModelTreeItem::GetParent()
{
	return m_pParentItem;
}

IModelTreeItem *IModelTreeItem::GetChild(int iRow)
{
	if(iRow >= m_ChildList.size() || iRow < 0)
		return nullptr;

	return m_ChildList[iRow];
}

void IModelTreeItem::AppendChild(IModelTreeItem *pChild)
{
	InsertChild(m_ChildList.size(), pChild);
}

void IModelTreeItem::InsertChild(int iIndex, IModelTreeItem *pChild)
{
	if(pChild->m_pParentItem == this)
	{
		m_ChildList.move(m_ChildList.indexOf(pChild), iIndex);
		return;
	}
	else if(pChild->m_pParentItem)
		pChild->m_pParentItem->RemoveChild(pChild->GetRow());

	pChild->m_pParentItem = this;
	m_ChildList.insert(iIndex, pChild);
}

void IModelTreeItem::RemoveChild(int iIndex)
{
	m_ChildList[iIndex]->m_pParentItem = nullptr;
	m_ChildList.removeAt(iIndex);
}

int IModelTreeItem::GetNumChildren() const
{
	return m_ChildList.size();
}

int IModelTreeItem::GetRow() const
{
	if(m_pParentItem)
		return m_pParentItem->m_ChildList.indexOf(const_cast<IModelTreeItem *>(this));

	return 0;
}
