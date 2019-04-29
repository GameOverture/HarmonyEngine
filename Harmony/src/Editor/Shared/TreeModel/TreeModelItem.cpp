/**************************************************************************
 *	ITreeModelItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TreeModelItem.h"

TreeModelItem::TreeModelItem() :
	QObject(nullptr),
	m_pParentItem(nullptr)
{ }

/*virtual*/ TreeModelItem::~TreeModelItem()
{ }

TreeModelItem *TreeModelItem::GetParent()
{
	return m_pParentItem;
}

TreeModelItem *TreeModelItem::GetChild(int iRow) const
{
	if(iRow >= m_ChildList.size() || iRow < 0)
		return nullptr;

	return m_ChildList[iRow];
}

QList<TreeModelItem *> TreeModelItem::GetChildren(bool bRecusively)
{
	if(bRecusively == false)
		return m_ChildList;

	QStack<TreeModelItem *> itemStack;
	for(int i = 0; i < m_ChildList.size(); ++i)
		itemStack.push(m_ChildList[i]);

	QList<TreeModelItem *> returnList;
	while(!itemStack.isEmpty())
	{
		TreeModelItem *pItem = itemStack.pop();
		returnList.append(pItem);

		for(int i = 0; i < pItem->GetNumChildren(); ++i)
			itemStack.push(pItem->GetChild(i));
	}

	return returnList;
}

void TreeModelItem::AppendChild(TreeModelItem *pChild)
{
	InsertChild(m_ChildList.size(), pChild);
}

void TreeModelItem::InsertChild(int iIndex, TreeModelItem *pChild)
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

void TreeModelItem::RemoveChild(int iIndex)
{
	m_ChildList[iIndex]->m_pParentItem = nullptr;
	m_ChildList.removeAt(iIndex);
}

int TreeModelItem::GetNumChildren() const
{
	return m_ChildList.size();
}

int TreeModelItem::GetRow() const
{
	if(m_pParentItem)
		return m_pParentItem->m_ChildList.indexOf(const_cast<TreeModelItem *>(this));

	return 0;
}
