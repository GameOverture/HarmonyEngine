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

ITreeModel::ITreeModel(const QStringList &sHeaderList, QObject *pParent /*= nullptr*/) :
	QAbstractItemModel(pParent)
{
	QVector<QVariant> rootData;
	for(int i = 0; i < sHeaderList.size(); ++i)
		rootData << sHeaderList[i];

	m_pRootItem = new TreeModelItem(rootData);
}

/*virtual*/ ITreeModel::~ITreeModel()
{
	delete m_pRootItem;
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

	TreeModelItem *pChildItem = pParentItem->child(iRow);
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
	TreeModelItem *pParentItem = pChildItem->parent();

	if(pParentItem == m_pRootItem)
		return QModelIndex();

	return createIndex(pParentItem->childNumber(), 0, pParentItem);
}

/*virtual*/ int ITreeModel::rowCount(const QModelIndex &parentRef /*= QModelIndex()*/) const /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	return pParentItem->childCount();
}

/*virtual*/ int ITreeModel::columnCount(const QModelIndex &parentRef /*= QModelIndex()*/) const /*override*/
{
	return m_pRootItem->columnCount();
}

/*virtual*/ bool ITreeModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(iRole != Qt::EditRole)
		return false;

	TreeModelItem *pItem = GetItem(indexRef);
	bool bResult = pItem->setData(indexRef.column(), valueRef);
	if(bResult)
		Q_EMIT dataChanged(indexRef, indexRef, {iRole});

	return bResult;
}

/*virtual*/ bool ITreeModel::setHeaderData(int iSection, Qt::Orientation eOrientation, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(iRole != Qt::EditRole || eOrientation != Qt::Horizontal)
		return false;

	bool bResult = m_pRootItem->setData(iSection, valueRef);
	if(bResult)
		Q_EMIT headerDataChanged(eOrientation, iSection, iSection);

	return bResult;
}

/*virtual*/ bool ITreeModel::insertRows(int iPosition, int iRows, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	bool bSuccess = false;

	beginInsertRows(parentRef, iPosition, iPosition + iRows - 1);
	bSuccess = pParentItem->insertChildren(iPosition, iRows, m_pRootItem->columnCount());
	endInsertRows();

	return bSuccess;
}

/*virtual*/ bool ITreeModel::removeRows(int iPosition, int iRows, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	bool bSuccess = true;

	beginRemoveRows(parentRef, iPosition, iPosition + iRows - 1);
	bSuccess = pParentItem->removeChildren(iPosition, iRows);
	endRemoveRows();

	return bSuccess;
}

/*virtual*/ bool ITreeModel::insertColumns(int iPosition, int iColumns, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	bool bSuccess = false;

	beginInsertColumns(parentRef, iPosition, iPosition + iColumns - 1);
	bSuccess = m_pRootItem->insertColumns(iPosition, iColumns);
	endInsertColumns();

	return bSuccess;
}

/*virtual*/ bool ITreeModel::removeColumns(int iPosition, int iColumns, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	bool bSuccess = false;

	beginRemoveColumns(parentRef, iPosition, iPosition + iColumns - 1);
	bSuccess = m_pRootItem->removeColumns(iPosition, iColumns);
	endRemoveColumns();

	if(m_pRootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return bSuccess;
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
