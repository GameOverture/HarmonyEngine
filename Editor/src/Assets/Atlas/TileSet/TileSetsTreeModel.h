/**************************************************************************
 *	TileSetsTreeModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILESETSTREEMODEL_H
#define TILESETSTREEMODEL_H

#include "AtlasTileSet.h"
#include "PropertiesTreeModel.h"

class TileSetsTreeModel : public QAbstractItemModel
{
	Q_OBJECT

	TreeModelItem *					m_pRootItem;		// Not visible in the tree view. Used internally.
	QList<AtlasTileSet *>			m_TileSetsList;

public:
	//enum Column
	//{
	//	COLUMN_Name = 0,
	//	COLUMN_Info,

	//	NUMCOLUMNS
	//};

public:
	TileSetsTreeModel(QObject *pParent);
	virtual ~TileSetsTreeModel();

	bool Add(AtlasTileSet *pTileSet);
	void Remove(AtlasTileSet *pTileSet);
	bool Contains(QString sTileSetName) const;
	AtlasTileSet *GetTileSet(QString sTileSetName) const;
	AtlasTileSet *GetTileSet(const QModelIndex &index) const;
	QUuid GetTerrainUuid(const QModelIndex &index) const;

	bool IsIndexTileSet(const QModelIndex &index) const;
	bool IsIndexTerrainSet(const QModelIndex &index) const;
	bool IsIndexTerrain(const QModelIndex &index) const;

	virtual QModelIndex index(int iRow, int iColumn, const QModelIndex &parentRef = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &child) const override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) override;
	virtual QVariant headerData(int iIndex, Qt::Orientation eOrientation, int iRole = Qt::DisplayRole) const override;

	virtual bool insertRows(int iPosition, int iRows, const QModelIndex &parentRef = QModelIndex()) override;
	virtual bool removeRows(int iPosition, int iRows, const QModelIndex &parentRef = QModelIndex()) override;
	virtual bool insertColumns(int iPosition, int iColumns, const QModelIndex &parentRef = QModelIndex()) override;
	virtual bool removeColumns(int iPosition, int iColumns, const QModelIndex &parentRef = QModelIndex()) override;

	QJsonArray GetTileSetsMetaArray() const;

protected:
	TreeModelItem *GetItem(const QModelIndex &indexRef) const;
};

#endif // TILESETSTREEMODEL_H
