/**************************************************************************
 *	TreeModelItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TreeModelItem.h"

TreeModelItem::TreeModelItem(const QVector<QVariant> &data, TreeModelItem *pParent /*= nullptr*/) :
	m_pParentItem(pParent),
	m_DataVec(data)
{ }

TreeModelItem::~TreeModelItem()
{
	qDeleteAll(m_ChildList);
}

TreeModelItem *TreeModelItem::GetParent()
{
	return m_pParentItem;
}

int TreeModelItem::GetIndex() const
{
	if(m_pParentItem)
		return m_pParentItem->m_ChildList.indexOf(const_cast<TreeModelItem*>(this));

	return 0;
}

TreeModelItem *TreeModelItem::GetChild(int iIndex)
{
	return m_ChildList.value(iIndex);
}

QVector<TreeModelItem *>TreeModelItem::GetChildren()
{
	QStack<TreeModelItem *> treeItemStack;
	for(int i = 0; i < GetNumChildren(); ++i)
		treeItemStack.push(GetChild(i));

	QVector<TreeModelItem *> returnVec;
	while(!treeItemStack.isEmpty())
	{
		TreeModelItem *pItem = treeItemStack.pop();
		returnVec.push_back(pItem);

		for(int i = 0; i < pItem->GetNumChildren(); ++i)
			treeItemStack.push(pItem->GetChild(i));
	}

	return returnVec;
}

int TreeModelItem::GetNumChildren() const
{
	return m_ChildList.count();
}

int TreeModelItem::columnCount() const
{
	return m_DataVec.count();
}

QVariant TreeModelItem::data(int iColumn) const
{
	return m_DataVec.value(iColumn);
}

bool TreeModelItem::SetData(int iColumn, const QVariant &valueRef)
{
	if(iColumn < 0 || iColumn >= m_DataVec.size() || m_DataVec[iColumn] == valueRef)
		return false;

	m_DataVec[iColumn] = valueRef;
	return true;
}

bool TreeModelItem::InsertChildren(int iPosition, int iCount, int iColumns)
{
	if(iPosition < 0 || iPosition > m_ChildList.size())
		return false;

	for(int i = 0; i < iCount; ++i)
	{
		QVector<QVariant> data(iColumns);
		TreeModelItem *pNewItem = new TreeModelItem(data, this);
		m_ChildList.insert(iPosition, pNewItem);
	}
	return true;
}

bool TreeModelItem::InsertColumns(int iPosition, int iColumns)
{
	if(iPosition < 0 || iPosition > m_DataVec.size())
		return false;

	for(int i = 0; i < iColumns; ++i)
		m_DataVec.insert(iPosition, QVariant());

	for(int i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->InsertColumns(iPosition, iColumns);

	return true;
}

bool TreeModelItem::IsRemoveValid(int iPosition, int iCount)
{
	if(iPosition < 0 || iPosition + iCount > m_ChildList.size())
		return false;
	return true;
}
bool TreeModelItem::DeleteChildren(int iPosition, int iCount)
{
	if(IsRemoveValid(iPosition, iCount) == false)
		return false;

	for(int i = 0; i < iCount; ++i)
		delete m_ChildList.takeAt(iPosition);

	return true;
}

bool TreeModelItem::RemoveColumns(int iPosition, int iColumns)
{
	if(iPosition < 0 || iPosition + iColumns > m_DataVec.size())
		return false;

	for(int i = 0; i < iColumns; ++i)
		m_DataVec.remove(iPosition);

	for(int i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->RemoveColumns(iPosition, iColumns);

	return true;
}

bool TreeModelItem::MoveChild(int iPosition, TreeModelItem *pNewParent, int iNewPostion)
{
	if(iPosition < 0 || iPosition >= m_ChildList.size() || pNewParent == nullptr)
		return false;

	TreeModelItem *pChild = m_ChildList.takeAt(iPosition);
	pChild->m_pParentItem = pNewParent;
	pNewParent->m_ChildList.insert(iNewPostion, pChild);
}
