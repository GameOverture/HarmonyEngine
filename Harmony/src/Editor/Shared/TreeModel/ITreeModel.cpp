/**************************************************************************
*	ITreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "ITreeModel.h"

ITreeModel::ITreeModel(TreeModelItem *pRootItem, QObject *parent) :
	QAbstractItemModel(parent),
	m_pRootItem(pRootItem)
{
	if(m_pRootItem == nullptr)
		HyGuiLog("ITreeModel was constructed with a nullptr root item", LOGTYPE_Error);
}

/*virtual*/ ITreeModel::~ITreeModel()
{
	delete m_pRootItem;
}

QModelIndex ITreeModel::GetIndex(TreeModelItem *pItem)
{
	return createIndex(pItem->GetRow(), 0, pItem);
}

void ITreeModel::RemoveItem(TreeModelItem *pItem)
{
	if(pItem == nullptr)
		return;

	QModelIndex parentIndex = pItem->GetParent() ? createIndex(pItem->GetParent()->GetRow(), 0, pItem->GetParent()) : QModelIndex();
	beginRemoveRows(parentIndex, pItem->GetRow(), pItem->GetRow());
	RecursiveRemoveItem(pItem);
	endRemoveRows();
}


/*virtual*/ QModelIndex ITreeModel::index(int iRow, int iColumn, const QModelIndex &parent) const /*override*/
{
	if(hasIndex(iRow, iColumn, parent) == false)
		return QModelIndex();

	const TreeModelItem *pParentItem;

	if(parent.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<TreeModelItem *>(parent.internalPointer());

	TreeModelItem *pChildItem = static_cast<TreeModelItem *>(pParentItem->GetChild(iRow));
	if(pChildItem)
		return createIndex(iRow, iColumn, pChildItem);
	else
		return QModelIndex();
}

/*virtual*/ QModelIndex ITreeModel::parent(const QModelIndex &index) const /*override*/
{
	if(index.isValid() == false)
		return QModelIndex();

	TreeModelItem *pChildItem = static_cast<TreeModelItem *>(index.internalPointer());
	TreeModelItem *pParentItem = static_cast<TreeModelItem *>(pChildItem->GetParent());

	if(pChildItem == m_pRootItem)// || pParentItem == m_pRootItem)
		return QModelIndex();

	return createIndex(pParentItem->GetRow(), 0, pParentItem);
}

/*virtual*/ int ITreeModel::rowCount(const QModelIndex &parentIndex /*= QModelIndex()*/) const /*override*/
{
	const TreeModelItem *pParentItem;
	if(parentIndex.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<TreeModelItem *>(parentIndex.internalPointer());

	return pParentItem->GetNumChildren();
}

void ITreeModel::InsertItem(int iRow, TreeModelItem *pItem, TreeModelItem *pParentItem)
{
	QList<TreeModelItem *> itemList;
	itemList << pItem;
	InsertItems(iRow, itemList, pParentItem);
}

void ITreeModel::InsertItems(int iRow, QList<TreeModelItem *> itemList, TreeModelItem *pParentItem)
{
	QModelIndex parentIndex = pParentItem ? createIndex(pParentItem->GetRow(), 0, pParentItem) : QModelIndex();

	TreeModelItem *pParent;
	if(parentIndex.isValid() == false)
		pParent = m_pRootItem;
	else
		pParent = static_cast<TreeModelItem *>(parentIndex.internalPointer());

	iRow = HyClamp(iRow, 0, pParent->GetNumChildren());

	beginInsertRows(parentIndex, iRow, iRow + itemList.size() - 1);

	for(int i = 0; i < itemList.size(); ++i)
		pParent->InsertChild(iRow + i, itemList[i]);

	endInsertRows();
}

bool ITreeModel::IsRoot(const QModelIndex &index) const
{
	return m_pRootItem == static_cast<TreeModelItem *>(index.internalPointer());
}

void ITreeModel::RecursiveRemoveItem(TreeModelItem *pItem)
{
	for(int i = 0; i < pItem->GetNumChildren(); ++i)
		RemoveItem(pItem->GetChild(i));

	// All children are taken care of at this point, safe to delete
	pItem->GetParent()->RemoveChild(pItem->GetRow());
	delete pItem;
}
