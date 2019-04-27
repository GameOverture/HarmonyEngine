/**************************************************************************
*	ITreeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ITREEMODEL_H
#define ITREEMODEL_H

#include "Global.h"
#include "Shared/IModelTreeItem.h"

#include <QAbstractItemModel>

class ITreeModel : public QAbstractItemModel
{
protected:
	IModelTreeItem *		m_pRootItem;

public:
	ITreeModel(IModelTreeItem *pRootItem, QObject *parent);
	virtual ~ITreeModel();

	virtual QModelIndex index(int iRow, int iColumn, const QModelIndex &parent) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual int rowCount(const QModelIndex &parentIndex) const override;
	//virtual bool insertRows(int iRow, int iCount, const QModelIndex &parentIndex = QModelIndex()) override;
	//virtual bool removeRows(int iRow, int iCount, const QModelIndex &parentIndex = QModelIndex()) override;

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const = 0;
	virtual QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const = 0;

protected:
	void InsertItem(int iRow, IModelTreeItem *pItem, IModelTreeItem *pParentItem);
	void InsertItems(int iRow, QList<IModelTreeItem *> itemList, IModelTreeItem *pParentItem);

	bool IsRoot(const QModelIndex &index) const;
};

#endif // ITREEMODEL_H
