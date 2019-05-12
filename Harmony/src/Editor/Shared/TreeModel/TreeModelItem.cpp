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

TreeModelItem *TreeModelItem::parent()
{
	return m_pParentItem;
}

int TreeModelItem::childNumber() const
{
	if(m_pParentItem)
		return m_pParentItem->m_ChildList.indexOf(const_cast<TreeModelItem*>(this));

	return 0;
}

TreeModelItem *TreeModelItem::child(int iIndex)
{
	return m_ChildList.value(iIndex);
}

QVector<TreeModelItem *>TreeModelItem::GetChildren()
{
	QStack<TreeModelItem *> treeItemStack;
	for(int i = 0; i < childCount(); ++i)
		treeItemStack.push(child(i));

	QVector<TreeModelItem *> returnVec;
	while(!treeItemStack.isEmpty())
	{
		TreeModelItem *pItem = treeItemStack.pop();
		returnVec.push_back(pItem);

		for(int i = 0; i < pItem->childCount(); ++i)
			treeItemStack.push(pItem->child(i));
	}

	return returnVec;
}

int TreeModelItem::childCount() const
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

bool TreeModelItem::setData(int iColumn, const QVariant &valueRef)
{
	if(iColumn < 0 || iColumn >= m_DataVec.size() || m_DataVec[iColumn] == valueRef)
		return false;

	m_DataVec[iColumn] = valueRef;
	return true;
}

bool TreeModelItem::insertChildren(int iPosition, int iCount, int iColumns)
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

bool TreeModelItem::insertColumns(int iPosition, int iColumns)
{
	if(iPosition < 0 || iPosition > m_DataVec.size())
		return false;

	for(int i = 0; i < iColumns; ++i)
		m_DataVec.insert(iPosition, QVariant());

	for(int i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->insertColumns(iPosition, iColumns);

	return true;
}

bool TreeModelItem::isRemoveValid(int iPosition, int iCount)
{
	if(iPosition < 0 || iPosition + iCount > m_ChildList.size())
		return false;
	return true;
}
bool TreeModelItem::removeChildren(int iPosition, int iCount)
{
	if(isRemoveValid(iPosition, iCount) == false)
		return false;

	for(int i = 0; i < iCount; ++i)
		delete m_ChildList.takeAt(iPosition);

	return true;
}

bool TreeModelItem::removeColumns(int iPosition, int iColumns)
{
	if(iPosition < 0 || iPosition + iColumns > m_DataVec.size())
		return false;

	for(int i = 0; i < iColumns; ++i)
		m_DataVec.remove(iPosition);

	for(int i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->removeColumns(iPosition, iColumns);

	return true;
}
