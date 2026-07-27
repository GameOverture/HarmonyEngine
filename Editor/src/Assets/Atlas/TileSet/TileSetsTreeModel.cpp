/**************************************************************************
 *	TileSetsTreeModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileSetsTreeModel.h"

TileSetsTreeModel::TileSetsTreeModel(QObject *pParent) :
	QAbstractItemModel(pParent)
{
	QStringList sHeaderList;
	sHeaderList << "Tile Sets";// << "# Tiles";
	QVector<QVariant> rootData;
	for(int i = 0; i < 1/*NUMCOLUMNS*/; ++i)
		rootData << sHeaderList[i];

	m_pRootItem = new TreeModelItem(rootData);
}

/*virtual*/ TileSetsTreeModel::~TileSetsTreeModel()
{
	delete m_pRootItem;
}

bool TileSetsTreeModel::Add(AtlasTileSet *pTileSet)
{
	if(Contains(pTileSet->GetName()))
	{
		HyGuiLog("TileSetsTreeModel::Add() - Attempting to add a duplicate tile set name: " % pTileSet->GetName(), LOGTYPE_Error);
		return false;
	}

	int iRow = m_pRootItem->GetNumChildren();
	if(insertRow(iRow, QModelIndex()) == false)
	{
		HyGuiLog("TileSetsTreeModel::Add() - insertRow failed", LOGTYPE_Error);
		return false;
	}
	m_TileSetsList.append(pTileSet);

	QVariant v;
	v.setValue(pTileSet);
	if(setData(index(iRow, 0, QModelIndex()), v, Qt::UserRole) == false)
		HyGuiLog("TileSetsTreeModel::Add() - setData failed", LOGTYPE_Error);

	if(pTileSet->HasTerrainSets())
	{
		QModelIndex tileSetIndex = index(iRow, 0, QModelIndex());

		QVector<QJsonObject> terrainSets = pTileSet->GetTerrainSets();
		for(int i = 0; i < terrainSets.size(); ++i)
		{
			if(insertRow(i, tileSetIndex) == false)
			{
				HyGuiLog("TileSetsTreeModel::Add() - insertRow failed", LOGTYPE_Error);
				return false;
			}
			QVariant v;
			v.setValue(terrainSets[i]);
			if(setData(index(i, 0, tileSetIndex), v, Qt::UserRole) == false)
				HyGuiLog("TileSetsTreeModel::Add() - setData failed", LOGTYPE_Error);

			QJsonArray terrainArray = terrainSets[i]["terrains"].toArray();
			for(int j = 0; j < terrainArray.size(); ++j)
			{
				if(insertRow(j, index(i, 0, tileSetIndex)) == false)
				{
					HyGuiLog("TileSetsTreeModel::Add() - insertRow failed", LOGTYPE_Error);
					return false;
				}
				QVariant v;
				v.setValue(terrainArray[j].toObject());
				if(setData(index(j, 0, index(i, 0, tileSetIndex)), v, Qt::UserRole) == false)
					HyGuiLog("TileSetsTreeModel::Add() - setData failed", LOGTYPE_Error);
			}
		}
	}

	return true;
}

void TileSetsTreeModel::Remove(AtlasTileSet *pTileSet)
{
	int iRemoveIndex = m_TileSetsList.indexOf(pTileSet);
	if(iRemoveIndex == -1)
	{
		HyGuiLog("TileSetsTreeModel::Remove() - Attempting to remove a tile set that doesn't exist: " % pTileSet->GetName(), LOGTYPE_Error);
		return;
	}

	removeRow(iRemoveIndex, QModelIndex());
	m_TileSetsList.removeOne(pTileSet);
}

bool TileSetsTreeModel::Contains(QString sTileSetName) const
{
	for(AtlasTileSet *pTileSet : m_TileSetsList)
	{
		if(pTileSet->GetName().compare(sTileSetName, Qt::CaseInsensitive) == 0)
			return true;
	}

	return false;
}

AtlasTileSet *TileSetsTreeModel::GetTileSet(QString sTileSetName) const
{
	for(AtlasTileSet *pTileSet : m_TileSetsList)
	{
		if(pTileSet->GetName().compare(sTileSetName, Qt::CaseInsensitive) == 0)
			return pTileSet;
	}

	return nullptr;
}

AtlasTileSet *TileSetsTreeModel::GetTileSet(const QModelIndex &index) const
{
	if(index.isValid())
	{
		if(IsIndexTileSet(index))
			return m_TileSetsList[index.row()];

		// If it's a terrain set or terrain, get the parent tile set
		if(IsIndexTerrainSet(index) || IsIndexTerrain(index))
		{
			QModelIndex tileSetIndex = index;
			while(IsIndexTileSet(tileSetIndex) == false)
				tileSetIndex = parent(tileSetIndex);
			return m_TileSetsList[tileSetIndex.row()];
		}
	}

	HyGuiLog("TileSetsTreeModel::GetTileSet() - Invalid index passed in", LOGTYPE_Error);
	return nullptr;
}

QUuid TileSetsTreeModel::GetTerrainUuid(const QModelIndex &index) const
{
	if(IsIndexTerrain(index))
	{
		QJsonObject terrainObj = GetItem(index)->data(0).toJsonObject();
		return QUuid(terrainObj["UUID"].toString());
	}

	return QUuid();
}

bool TileSetsTreeModel::IsIndexTileSet(const QModelIndex &index) const
{
	return parent(index).isValid() == false;
}

bool TileSetsTreeModel::IsIndexTerrainSet(const QModelIndex &index) const
{
	return IsIndexTileSet(index) == false && parent(parent(index)).isValid() == false;
}

bool TileSetsTreeModel::IsIndexTerrain(const QModelIndex &index) const
{
	return IsIndexTileSet(index) == false && IsIndexTerrainSet(index) == false;
}

/*virtual*/ QModelIndex TileSetsTreeModel::index(int iRow, int iColumn, const QModelIndex &parentRef /*= QModelIndex()*/) const /*override*/
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

/*virtual*/ QModelIndex TileSetsTreeModel::parent(const QModelIndex &child) const /*override*/
{
	if(child.isValid() == false)
		return QModelIndex();

	TreeModelItem *pChildItem = GetItem(child);
	TreeModelItem *pParentItem = pChildItem->GetParent();

	if(pParentItem == m_pRootItem)
		return QModelIndex();

	return createIndex(pParentItem->GetIndex(), 0, pParentItem);
}

/*virtual*/ int TileSetsTreeModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	if(parent.isValid() == false)
		return m_TileSetsList.size();

	TreeModelItem *pParentItem = GetItem(parent);
	return pParentItem->GetNumChildren();
}

/*virtual*/ int TileSetsTreeModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return 1;//NUMCOLUMNS;
}

/*virtual*/ QVariant TileSetsTreeModel::data(const QModelIndex &index, int iRole /*= Qt::DisplayRole*/) const /*override*/
{
	TreeModelItem *pItem = GetItem(index);

	bool bIsTileSet = IsIndexTileSet(index);
	bool bIsTerrainSet = IsIndexTerrainSet(index);
	bool bIsTerrain = IsIndexTerrain(index);

	switch(iRole)
	{
	case Qt::TextAlignmentRole:
		if(index.column() != 0)
			return Qt::AlignCenter;
		break;

	case Qt::DisplayRole:
		if(bIsTileSet)
			return m_TileSetsList[index.row()]->GetName();
		else if(bIsTerrainSet)
		{
			QString sDisplay = "Terrain Set " % QString::number(index.row());
			QJsonObject terrainSetObj = pItem->data(0).toJsonObject();
			int iTerrainSetMode = terrainSetObj["terrainSetMode"].toInt();
			switch(iTerrainSetMode)
			{
			case AUTOTILETYPE_MatchCornerSides:
				sDisplay += " (Match Corners & Sides)";
				break;
			case AUTOTILETYPE_MatchCorner:
				sDisplay += " (Match Corners)";
				break;
			case AUTOTILETYPE_MatchSides:
				sDisplay += " (Match Sides)";
				break;
			default:
				HyGuiLog("TileSetsTreeModel::data() - Invalid terrain set mode: " % QString::number(iTerrainSetMode), LOGTYPE_Error);
				break;
			}
			return sDisplay;
		}
		else // bIsTerrain
			return pItem->data(0).toJsonObject()["name"].toString();
		break;

	case Qt::DecorationRole:
		if(bIsTileSet)
			return m_TileSetsList[index.row()]->GetTileSetIcon();
		else if(bIsTerrainSet)
		{
			QJsonObject terrainSetObj = pItem->data(0).toJsonObject();
			int iTerrainSetMode = terrainSetObj["terrainSetMode"].toInt();
			switch(iTerrainSetMode)
			{
			case AUTOTILETYPE_MatchCornerSides:
				return QVariant(QIcon(":/icons16x16/tileset-terrainset-cornersandsides.png"));
			case AUTOTILETYPE_MatchCorner:
				return QVariant(QIcon(":/icons16x16/tileset-terrainset-corners.png"));
			case AUTOTILETYPE_MatchSides:
				return QVariant(QIcon(":/icons16x16/tileset-terrainset-sides.png"));
			default:
				HyGuiLog("TileSetsTreeModel::data() - Invalid terrain set mode: " % QString::number(iTerrainSetMode), LOGTYPE_Error);
				break;
			}
		}
		else // bIsTerrain
		{
			QJsonObject terrainObj = pItem->data(0).toJsonObject();
			// TODO: Get first tile in terrain to use as icon
			return QVariant();
		}
		break;
	}

	return QVariant();
}

/*virtual*/ bool TileSetsTreeModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(indexRef.isValid() == false || iRole != Qt::UserRole)
	{
		HyGuiLog("Default QAbstractItemModel::setData() was invoked with invalid index or role. Other role types should be implemented in derived classes", LOGTYPE_Error);
		return false;
	}

	TreeModelItem *pItem = GetItem(indexRef);
	bool bResult = pItem->SetData(indexRef.column(), valueRef);
	if(bResult)
		Q_EMIT dataChanged(indexRef, indexRef, {iRole});

	return true;
}

/*virtual*/ QVariant TileSetsTreeModel::headerData(int iIndex, Qt::Orientation eOrientation, int iRole /*= Qt::DisplayRole*/) const /*override*/
{
	if(iRole == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	if(eOrientation == Qt::Horizontal && iRole == Qt::DisplayRole)
		return m_pRootItem->data(iIndex);

	return QVariant();
}

/*virtual*/ bool TileSetsTreeModel::insertRows(int iPosition, int iRows, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	bool bSuccess = false;

	beginInsertRows(parentRef, iPosition, iPosition + iRows - 1);
	bSuccess = pParentItem->InsertChildren(iPosition, iRows, m_pRootItem->columnCount());
	endInsertRows();

	return bSuccess;
}

/*virtual*/ bool TileSetsTreeModel::removeRows(int iPosition, int iRows, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	TreeModelItem *pParentItem = GetItem(parentRef);
	bool bSuccess = false;

	if(pParentItem->IsRemoveValid(iPosition, iRows))
	{
		beginRemoveRows(parentRef, iPosition, iPosition + iRows - 1);
		bSuccess = pParentItem->DeleteChildren(iPosition, iRows);
		endRemoveRows();
	}

	return bSuccess;
}

/*virtual*/ bool TileSetsTreeModel::insertColumns(int iPosition, int iColumns, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	bool bSuccess = false;

	beginInsertColumns(parentRef, iPosition, iPosition + iColumns - 1);
	bSuccess = m_pRootItem->InsertColumns(iPosition, iColumns);
	endInsertColumns();

	return bSuccess;
}

/*virtual*/ bool TileSetsTreeModel::removeColumns(int iPosition, int iColumns, const QModelIndex &parentRef /*= QModelIndex()*/) /*override*/
{
	bool bSuccess = false;

	beginRemoveColumns(parentRef, iPosition, iPosition + iColumns - 1);
	bSuccess = m_pRootItem->RemoveColumns(iPosition, iColumns);
	endRemoveColumns();

	if(m_pRootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return bSuccess;
}

QJsonArray TileSetsTreeModel::GetTileSetsMetaArray() const
{
	QJsonArray tileSetArray;
	for(AtlasTileSet *pTileSet : m_TileSetsList)
		tileSetArray.append(pTileSet->GetSavedTileSetMeta());

	return tileSetArray;
}

TreeModelItem *TileSetsTreeModel::GetItem(const QModelIndex &indexRef) const
{
	if(indexRef.isValid())
	{
		TreeModelItem *pItem = static_cast<TreeModelItem *>(indexRef.internalPointer());
		if(pItem)
			return pItem;
	}

	return m_pRootItem;
}
