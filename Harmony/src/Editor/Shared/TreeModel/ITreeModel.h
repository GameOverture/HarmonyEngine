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
#include "Shared/TreeModel/ITreeModelItem.h"

#include <QAbstractItemModel>

class ITreeModel : public QAbstractItemModel
{
	Q_OBJECT

protected:
	ITreeModelItem *		m_pRootItem;

public:
	ITreeModel(ITreeModelItem *pRootItem, QObject *parent);
	virtual ~ITreeModel();

	QModelIndex GetIndex(ITreeModelItem *pItem);
	void RemoveItem(ITreeModelItem *pItem);

	virtual QModelIndex index(int iRow, int iColumn, const QModelIndex &parent) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual int rowCount(const QModelIndex &parentIndex = QModelIndex()) const override;

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
	virtual QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override = 0;

protected:
	void InsertItem(int iRow, ITreeModelItem *pItem, ITreeModelItem *pParentItem);
	void InsertItems(int iRow, QList<ITreeModelItem *> itemList, ITreeModelItem *pParentItem);

	bool IsRoot(const QModelIndex &index) const;

private:
	void RecursiveRemoveItem(ITreeModelItem *pItem);
};

#endif // ITREEMODEL_H
