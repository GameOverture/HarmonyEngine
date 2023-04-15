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
#include "TreeModelItem.h"
#include "TreeModelItemData.h"

ITreeModel::ITreeModel(int iNumColumns, const QStringList &sHeaderList, QObject *pParent /*= nullptr*/) :
	QAbstractItemModel(pParent)
{
	if(iNumColumns <= 0)
		HyGuiLog("Cannot create a ITreeModel with columns <= 0", LOGTYPE_Error);

	QVector<QVariant> rootData;
	for(int i = 0; i < iNumColumns; ++i)
	{
		if(i < sHeaderList.size())
			rootData << sHeaderList[i];
		else
			rootData << "";
	}

	m_pRootItem = new TreeModelItem(rootData);
}

/*virtual*/ ITreeModel::~ITreeModel()
{
	delete m_pRootItem;
}

bool ITreeModel::InsertTreeItem(TreeModelItemData *pNewItemData, TreeModelItem *pParentTreeItem, int iRow /*= -1*/)
{
	QModelIndex parentIndex;
	if(pParentTreeItem)
	{
		parentIndex = FindIndex<TreeModelItemData *>(pParentTreeItem->data(0).value<TreeModelItemData *>(), 0);
		iRow = (iRow < 0 ? pParentTreeItem->GetNumChildren() : iRow);
	}
	else
		iRow = 0;

	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("ExplorerModel::InsertNewItem() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	QVariant v;
	v.setValue<TreeModelItemData *>(pNewItemData);
	if(setData(index(iRow, 0, parentIndex), v, Qt::UserRole) == false)
		HyGuiLog("IManagerModel::InsertNewItem() - setData failed", LOGTYPE_Error);

	return true;
}

QList<TreeModelItemData *> ITreeModel::GetItemsRecursively(const QModelIndex &indexRef) const
{
	QList<TreeModelItemData *> returnList;

	QStack<TreeModelItem *> treeItemStack;
	treeItemStack.push(static_cast<TreeModelItem *>(indexRef.internalPointer()));
	while(!treeItemStack.isEmpty())
	{
		TreeModelItem *pItem = treeItemStack.pop();
		returnList.push_back(pItem->data(0).value<TreeModelItemData *>());

		for(int i = 0; i < pItem->GetNumChildren(); ++i)
			treeItemStack.push(pItem->GetChild(i));
	}

	return returnList;
}

QModelIndexList ITreeModel::GetAllIndices() const
{
	QModelIndexList returnList;
	QStack<QModelIndex> indexStack;

	// Top level indices
	for(int i = 0; i < rowCount(); ++i)
		indexStack.push(index(i, 0));

	while(!indexStack.isEmpty())
	{
		QModelIndex curIndex = indexStack.pop();
		returnList.push_back(curIndex);

		for(int i = 0; i < rowCount(curIndex); ++i)
			indexStack.push(index(i, 0, curIndex));
	}

	return returnList;
}

void ITreeModel::MoveTreeItem(TreeModelItemData *pSourceItemData, TreeModelItemData *pDestinationParent, int32 iDestRow)
{
	QModelIndex sourceIndex = FindIndex<TreeModelItemData *>(pSourceItemData, 0);
	if(sourceIndex.isValid() == false)
	{
		HyGuiLog("ITreeModel::MoveTreeItem was passed an invalid pSourceItemData", LOGTYPE_Error);
		return;
	}
	TreeModelItem *pSourceTreeItem = GetItem(sourceIndex);

	QModelIndex destParentIndex = FindIndex<TreeModelItemData *>(pDestinationParent, 0);
	if(destParentIndex.isValid() == false)
	{
		HyGuiLog("ITreeModel::MoveTreeItem was passed an invalid pDestinationParent", LOGTYPE_Error);
		return;
	}
	TreeModelItem *pDestParentTreeItem = GetItem(destParentIndex);

	int iSourceIndex = pSourceTreeItem->GetIndex();

	// Appease the stupid Qt API - if sourceParent and destinationParent are the same, you must ensure
	// that the destinationChild is not within the range of sourceFirst and sourceLast + 1
	int iQtDestRow = iDestRow;
	if(sourceIndex.parent() == destParentIndex && (iSourceIndex + 1) == iDestRow)
		iQtDestRow = iDestRow + 1;

	// Move tree item to new location
	bool bValidMove = beginMoveRows(sourceIndex.parent(), iSourceIndex, iSourceIndex, destParentIndex, iQtDestRow);
	if(bValidMove == false)
	{
		HyGuiLog("ITreeModel::MoveTreeItem - beginMoveRows returned false", LOGTYPE_Error);
		return;
	}
	pSourceTreeItem->GetParent()->MoveChild(iSourceIndex, pDestParentTreeItem, iDestRow);
	endMoveRows();
}

/*virtual*/ QVariant ITreeModel::headerData(int iSection, Qt::Orientation orientation, int iRole /*= Qt::DisplayRole*/) const /*override*/
{
	if(iRole == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	if(orientation == Qt::Horizontal && iRole == Qt::DisplayRole)
		return m_pRootItem->data(iSection);

	return QVariant();
}

/*virtual*/ QModelIndex ITreeModel::index(int iRow, int iColumn, const QModelIndex &parentRef /*= QModelIndex()*/) const /*override*/
{
	if(parentRef.isValid() && parentRef.column() != 0)
		return QModelIndex();

	TreeModelItem *pParentItem = GetItem(parentRef);

	TreeModelItem *pChildItem = pParentItem->GetChild(iRow);
	if(pChildItem)
		return createIndex(iRow, iColumn, pChildItem);
	else
		return QModelIndex();
}

/*virtual*/ QModelIndex ITreeModel::parent(const QModelIndex &indexRef) const /*override*/
{
	if(indexRef.isValid() == false)
		return QModelIndex();

	TreeModelItem *pChildItem = GetItem(indexRef);
	TreeModelItem *pParentItem = pChildItem->GetParent();

	if(pParentItem == m_pRootItem)
		return QModelIndex();

	return createIndex(pParentItem->GetIndex(), 0, pParentItem);
}

/*virtual*/ int ITreeModel::rowCount(const QModelIndex &parentRef /*= QModelIndex()*/) const /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	return pParentItem->GetNumChildren();
}

/*virtual*/ int ITreeModel::columnCount(const QModelIndex &parentRef /*= QModelIndex()*/) const /*override*/
{
	return m_pRootItem->columnCount();
}

/*virtual*/ bool ITreeModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(indexRef.isValid() == false || iRole != Qt::UserRole)
	{
		HyLogError("Default ITreeModel::setData() was invoked with invalid index or role. Other role types should be implemented in derived classes");
		return false;
	}

	TreeModelItem *pItem = GetItem(indexRef);
	bool bResult = pItem->SetData(indexRef.column(), valueRef);
	if(bResult)
		Q_EMIT dataChanged(indexRef, indexRef, {iRole});

	return true;
}

/*virtual*/ bool ITreeModel::setHeaderData(int iSection, Qt::Orientation eOrientation, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(iRole != Qt::EditRole || eOrientation != Qt::Horizontal)
		return false;

	bool bResult = m_pRootItem->SetData(iSection, valueRef);
	if(bResult)
		Q_EMIT headerDataChanged(eOrientation, iSection, iSection);

	return bResult;
}

/*virtual*/ bool ITreeModel::insertRows(int iPosition, int iRows, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	bool bSuccess = false;

	beginInsertRows(parentRef, iPosition, iPosition + iRows - 1);
	bSuccess = pParentItem->InsertChildren(iPosition, iRows, m_pRootItem->columnCount());
	endInsertRows();

	return bSuccess;
}

/*virtual*/ bool ITreeModel::removeRows(int iPosition, int iRows, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	bool bSuccess = true;

	beginRemoveRows(parentRef, iPosition, iPosition + iRows - 1);

	if(pParentItem->IsRemoveValid(iPosition, iRows))
	{
		for(int i = 0; i < iRows; ++i)
			OnTreeModelItemRemoved(pParentItem->GetChild(iPosition + i));

		bSuccess = pParentItem->DeleteChildren(iPosition, iRows);
	}

	endRemoveRows();

	return bSuccess;
}

/*virtual*/ bool ITreeModel::insertColumns(int iPosition, int iColumns, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	bool bSuccess = false;

	beginInsertColumns(parentRef, iPosition, iPosition + iColumns - 1);
	bSuccess = m_pRootItem->InsertColumns(iPosition, iColumns);
	endInsertColumns();

	return bSuccess;
}

/*virtual*/ bool ITreeModel::removeColumns(int iPosition, int iColumns, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	bool bSuccess = false;

	beginRemoveColumns(parentRef, iPosition, iPosition + iColumns - 1);
	bSuccess = m_pRootItem->RemoveColumns(iPosition, iColumns);
	endRemoveColumns();

	if(m_pRootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return bSuccess;
}

/*virtual*/ QVariant ITreeModel::data(const QModelIndex &indexRef, int role) const /*override*/
{
	if(indexRef.isValid() == false)
		return QVariant();

	if(role != Qt::UserRole)
	{
		HyLogError("Default ITreeModel::data() was invoked with invalid role. Other role types should be implemented in derived classes");
		return QVariant();
	}

	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem)
		return pTreeItem->data(indexRef.column());
	
	return QVariant();
}

TreeModelItem *ITreeModel::GetItem(const QModelIndex &indexRef) const
{
	if(indexRef.isValid())
	{
		TreeModelItem *pItem = static_cast<TreeModelItem *>(indexRef.internalPointer());
		if(pItem)
			return pItem;
	}

	return m_pRootItem;
}

void ITreeModel::RemoveRedundantItems(HyGuiItemType eFolderType, QList<TreeModelItemData *> &itemListOut) const
{
	QList<TreeModelItemData *> folderList;
	for(auto item : itemListOut)
	{
		if(item->GetType() == eFolderType && folderList.contains(item) == false)
			folderList.append(item);
	}

	for(auto folder : folderList)
	{
		QList<TreeModelItemData *> nestedItemsList = GetItemsRecursively(FindIndex<TreeModelItemData *>(folder, 0));
		for(auto nestedItem : nestedItemsList)
		{
			if(folder == nestedItem)
				continue;

			for(auto iter = itemListOut.begin(); iter != itemListOut.end();)
			{
				if(*iter == nestedItem)
					iter = itemListOut.erase(iter);
				else
					++iter;
			}
		}
	}
}
