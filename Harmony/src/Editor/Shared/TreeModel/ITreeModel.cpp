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

ITreeModel::ITreeModel(IModelTreeItem *pRootItem, QObject *parent) :
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

/*virtual*/ QModelIndex ITreeModel::index(int iRow, int iColumn, const QModelIndex &parent) const /*override*/
{
	if(hasIndex(iRow, iColumn, parent) == false)
		return QModelIndex();

	const IModelTreeItem *pParentItem;

	if(parent.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<IModelTreeItem *>(parent.internalPointer());

	IModelTreeItem *pChildItem = static_cast<IModelTreeItem *>(pParentItem->GetChild(iRow));
	if(pChildItem)
		return createIndex(iRow, iColumn, pChildItem);
	else
		return QModelIndex();
}

/*virtual*/ QModelIndex ITreeModel::parent(const QModelIndex &index) const /*override*/
{
	if(index.isValid() == false)
		return QModelIndex();

	IModelTreeItem *pChildItem = static_cast<IModelTreeItem *>(index.internalPointer());
	IModelTreeItem *pParentItem = static_cast<IModelTreeItem *>(pChildItem->GetParent());

	if(pParentItem == m_pRootItem)
		return QModelIndex();

	return createIndex(pParentItem->GetRow(), 0, pParentItem);
}

/*virtual*/ int ITreeModel::rowCount(const QModelIndex &parentIndex /*= QModelIndex()*/) const /*override*/
{
	const IModelTreeItem *pParentItem;
	if(parentIndex.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<IModelTreeItem *>(parentIndex.internalPointer());

	return pParentItem->GetNumChildren();
}

//// An insertRows() implementation must call beginInsertRows() before inserting new rows into the data structure, and endInsertRows() immediately afterwards.
///*virtual*/ bool ITreeModel::insertRows(int iRow, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
//{
//	beginInsertRows(parentRef, iRow, iRow + iCount - 1);
//	endInsertRows();
//
//	return false;
//}
//
//// A removeRows() implementation must call beginRemoveRows() before the rows are removed from the data structure, and endRemoveRows() immediately afterwards.
///*virtual*/ bool ITreeModel::removeRows(int iRow, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
//{
//	beginRemoveRows(parentRef, iRow, iRow + iCount);
//	endRemoveRows();
//
//	return false;
//}

//// An insertColumns() implementation must call beginInsertColumns() before inserting new columns into the data structure, and endInsertColumns() immediately afterwards.
///*virtual*/ bool ExplorerModel::insertColumns(int iColumn, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
//{
//}
//
//// A removeColumns() implementation must call beginRemoveColumns() before the columns are removed from the data structure, and endRemoveColumns() immediately afterwards.
///*virtual*/ bool ExplorerModel::removeColumns(int iColumn, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
//{
//}

void ITreeModel::InsertItem(int iRow, IModelTreeItem *pItem, IModelTreeItem *pParentItem)
{
	QList<IModelTreeItem *> itemList;
	itemList << pItem;
	InsertItems(iRow, itemList, pParentItem);
}

void ITreeModel::InsertItems(int iRow, QList<IModelTreeItem *> itemList, IModelTreeItem *pParentItem)
{
	QModelIndex parentIndex = pParentItem ? createIndex(pParentItem->GetRow(), 0, pParentItem) : QModelIndex();

	IModelTreeItem *pParent;
	if(parentIndex.isValid() == false)
		pParent = m_pRootItem;
	else
		pParent = static_cast<IModelTreeItem *>(parentIndex.internalPointer());

	iRow = HyClamp(iRow, 0, pParent->GetNumChildren());

	beginInsertRows(parentIndex, iRow, iRow + itemList.size() - 1);

	for(int i = 0; i < itemList.size(); ++i)
		pParent->InsertChild(iRow + i, itemList[i]);

	endInsertRows();
}

bool ITreeModel::IsRoot(const QModelIndex &index) const
{
	return m_pRootItem == static_cast<IModelTreeItem *>(index.internalPointer());
}
