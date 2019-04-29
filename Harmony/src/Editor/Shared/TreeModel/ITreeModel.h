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
