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

#include <QAbstractItemModel>

class TreeModelItem;

class ITreeModel : public QAbstractItemModel
{
	Q_OBJECT

protected:
	TreeModelItem *		m_pRootItem;

public:
	ITreeModel(const QStringList &sHeaderList, QObject *pParent = nullptr);
	virtual ~ITreeModel();

	template<typename TYPE>
	QModelIndex FindIndex(TYPE *pData, int iColumn)
	{
		if(iColumn >= m_pRootItem->columnCount())
			return QModelIndex();

		QStack<TreeModelItem *> treeItemStack;
		for(int i = 0; i < m_pRootItem->childCount(); ++i)
			treeItemStack.push(m_pRootItem->child(i));

		while(!treeItemStack.isEmpty())
		{
			TreeModelItem *pItem = treeItemStack.pop();
			if(pItem->data(iColumn).value<TYPE *>() == pData)
				return createIndex(pItem->childNumber(), iColumn, &pItem);

			for(int i = 0; i < pItem->childCount(); ++i)
				treeItemStack.push(pItem->child(i));
		}

		return QModelIndex();
	}

	virtual QVariant headerData(int iSection, Qt::Orientation orientation, int iRole = Qt::DisplayRole) const override;

	virtual QModelIndex index(int iRow, int iColumn, const QModelIndex &parentRef = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &indexRef) const override;

	virtual int rowCount(const QModelIndex &parentRef = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parentRef = QModelIndex()) const override;
	
	virtual bool setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole = Qt::EditRole) override;
	virtual bool setHeaderData(int iSection, Qt::Orientation eOrientation, const QVariant &valueRef, int iRole = Qt::EditRole) override;

	virtual bool insertRows(int iPosition, int iRows, const QModelIndex &parentRef = QModelIndex()) override;
	virtual bool removeRows(int iPosition, int iRows, const QModelIndex &parentRef = QModelIndex()) override;
	virtual bool insertColumns(int iPosition, int iColumns, const QModelIndex &parentRef = QModelIndex()) override;
	virtual bool removeColumns(int iPosition, int iColumns, const QModelIndex &parentRef = QModelIndex()) override;

	virtual QVariant data(const QModelIndex &index, int role) const override = 0;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override = 0;

private:
	TreeModelItem *GetItem(const QModelIndex &indexRef) const;
};

#endif // ITREEMODEL_H
