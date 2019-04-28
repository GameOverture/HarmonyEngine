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
	Q_OBJECT

protected:
	IModelTreeItem *		m_pRootItem;

public:
	ITreeModel(IModelTreeItem *pRootItem, QObject *parent);
	virtual ~ITreeModel();

	QModelIndex GetIndex(IModelTreeItem *pItem);
	void RemoveItem(IModelTreeItem *pItem);

	virtual QModelIndex index(int iRow, int iColumn, const QModelIndex &parent) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual int rowCount(const QModelIndex &parentIndex = QModelIndex()) const override;

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const = 0;
	virtual QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const = 0;

protected:
	void InsertItem(int iRow, IModelTreeItem *pItem, IModelTreeItem *pParentItem);
	void InsertItems(int iRow, QList<IModelTreeItem *> itemList, IModelTreeItem *pParentItem);

	bool IsRoot(const QModelIndex &index) const;

private:
	void RecursiveRemoveItem(IModelTreeItem *pItem);
};

#endif // ITREEMODEL_H
