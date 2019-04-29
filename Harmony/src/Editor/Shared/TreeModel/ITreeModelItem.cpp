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
#include "ITreeModelItem.h"

ITreeModelItem::ITreeModelItem() :
	QObject(nullptr),
	m_pParentItem(nullptr)
{ }

/*virtual*/ ITreeModelItem::~ITreeModelItem()
{ }

ITreeModelItem *ITreeModelItem::GetParent()
{
	return m_pParentItem;
}

ITreeModelItem *ITreeModelItem::GetChild(int iRow) const
{
	if(iRow >= m_ChildList.size() || iRow < 0)
		return nullptr;

	return m_ChildList[iRow];
}

QList<ITreeModelItem *> ITreeModelItem::GetChildren(bool bRecusively)
{
	if(bRecusively == false)
		return m_ChildList;

	QStack<ITreeModelItem *> itemStack;
	for(int i = 0; i < m_ChildList.size(); ++i)
		itemStack.push(m_ChildList[i]);

	QList<ITreeModelItem *> returnList;
	while(!itemStack.isEmpty())
	{
		ITreeModelItem *pItem = itemStack.pop();
		returnList.append(pItem);

		for(int i = 0; i < pItem->GetNumChildren(); ++i)
			itemStack.push(pItem->GetChild(i));
	}

	return returnList;
}

void ITreeModelItem::AppendChild(ITreeModelItem *pChild)
{
	InsertChild(m_ChildList.size(), pChild);
}

void ITreeModelItem::InsertChild(int iIndex, ITreeModelItem *pChild)
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

void ITreeModelItem::RemoveChild(int iIndex)
{
	m_ChildList[iIndex]->m_pParentItem = nullptr;
	m_ChildList.removeAt(iIndex);
}

int ITreeModelItem::GetNumChildren() const
{
	return m_ChildList.size();
}

int ITreeModelItem::GetRow() const
{
	if(m_pParentItem)
		return m_pParentItem->m_ChildList.indexOf(const_cast<ITreeModelItem *>(this));

	return 0;
}
