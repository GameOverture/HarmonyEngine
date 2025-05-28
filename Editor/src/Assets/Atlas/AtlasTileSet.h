/**************************************************************************
 *	AtlasTileSet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASTILESET_H
#define ATLASTILESET_H

#include "Global.h"
#include "AtlasFrame.h"
#include "TileSetScene.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QUndoStack>

using AutoTileHandle = uint32_t;
using PhysicsLayerHandle = uint32_t;

#define NUM_COLS_TILESET(numTiles) static_cast<int>(std::floor(std::sqrt(numTiles)))
#define NUM_ROWS_TILESET(numTiles) static_cast<int>(std::ceil(static_cast<double>(numTiles) / NUM_COLS_TILESET(numTiles)))

class TileData;

class AtlasTileSet : public AtlasFrame
{
	Q_OBJECT

	FileDataPair				m_TileSetDataPair;				// The currently 'saved to disk' data of the TileSet
	bool						m_bExistencePendingSave;
	
	QUndoStack *				m_pUndoStack;
	QAction *					m_pActionUndo;
	QAction *					m_pActionRedo;
	
	TileSetScene				m_GfxScene;

	QSize						m_TileSize;

	struct AutoTile
	{
		AutoTileHandle			m_hId;
		enum Type
		{
			AUTOTILE_MatchCornerSides,
			AUTOTILE_MatchCorner,
			AUTOTILE_MatchSides
		};
		int						m_iType;
		QString					m_sName;
		HyColor					m_Color;

		AutoTile(const QJsonObject &initObj)
		{
			m_hId = initObj["id"].toInt();
			m_iType = initObj["type"].toInt();
			m_sName = initObj["name"].toString();
			m_Color = HyColor(initObj["color"].toVariant().toLongLong());
		}
	};
	QList<AutoTile>				m_AutotileList;

	struct PhysicsLayer
	{
		PhysicsLayerHandle		m_hId;
		QString					m_sName;
		HyColor					m_Color;

		PhysicsLayer(const QJsonObject &initObj)
		{
			m_hId = initObj["id"].toInt();
			m_sName = initObj["name"].toString();
			m_Color = HyColor(initObj["color"].toVariant().toLongLong());
		}
	};
	QList<PhysicsLayer>			m_PhysicsLayerList;

	//struct TileAnimation
	//{
	//	quint32					m_uiId;
	//	QString					m_sName;
	//	HyColor					m_Color;
	//	TileData *m_pStartTile;
	//};


	QList<TileData *>				m_TileDataList;			// List of all TileData objects in this tile set, including AlternateTiles. It's ordered by the index in the sub-atlas texture
	//QList<QPair<int, TileData *>>	m_RemovedTileDataList;	// List of TileData objects that have been removed from the tile set, and not deleted. This is used for undo/redo operations.

	//struct MetaLocation
	//{
	//	int iX;
	//	int iY;
	//	MetaLocation(int x, int y) : iX(x), iY(y) { }
	//	bool operator==(const MetaLocation &rhs) const
	//	{
	//		return iX == rhs.iX && iY == rhs.iY;
	//	}
	//	bool operator!=(const MetaLocation &rhs) const
	//	{
	//		return this->operator==(rhs) == false;
	//	}
	//	bool operator<(const MetaLocation &rhs) const
	//	{
	//		// TODO: This seems wrong, should check Y first?
	//		if(iX < rhs.iX)
	//			return true;
	//		else if(iX == rhs.iX)
	//			return iY < rhs.iY;
	//		return false;
	//	}
	//};
	//QMap<MetaLocation, TileData *>	m_MetaTileDataMap;	// QPoint key is the user/meta location, not the atlas



public:
	AtlasTileSet(IManagerModel &modelRef,
				 QUuid uuid,
				 quint32 uiChecksum,
				 quint32 uiBankId,
				 QString sName,
				 HyTextureInfo texInfo,
				 quint16 uiW, quint16 uiH, quint16 uiX, quint16 uiY,
				 int iTextureIndex,
				 const FileDataPair &tileSetDataPair,
				 bool bIsPendingSave,
				 uint uiErrors);
	~AtlasTileSet();

	int GetNumTiles() const;
	QSize GetTileSize() const;
	void SetTileSize(QSize size);

	QString GetTileSetInfo() const;
	QIcon GetTileSetIcon() const;

	TileSetScene *GetGfxScene();

	// Cmd functions are the only functions that change the data (via Undo/Redo)
	QList<QPair<int, TileData *>> Cmd_AppendNewTiles(QSize vTileSize, const QVector<QPixmap> &pixmapList, Qt::Edge eAppendEdge);
	QList<QPair<int, TileData *>> Cmd_RemoveTiles(QVector<TileData *> tileDataList);
	void Cmd_ReaddTiles(QList<QPair<int, TileData *>> tileDataList);

	QUndoStack *GetUndoStack();
	QAction *GetUndoAction();
	QAction *GetRedoAction();

	void GetLatestFileData(FileDataPair &fileDataPairOut) const;
	void GetSavedFileData(FileDataPair &fileDataPairOut) const;
	bool Save(bool bWriteToDisk);
	bool IsExistencePendingSave() const;
	bool IsSaveClean() const;
	void DiscardChanges();

	virtual void InsertUniqueJson(QJsonObject &frameObj) override;

protected:
	void RegenerateSubAtlas(); // Assumes m_TileDataList is up to date. This will regenerate the sub-atlas texture and update each TileData

private Q_SLOTS:
	void on_undoStack_cleanChanged(bool bClean);
	void on_undoStack_indexChanged(int iIndex);
};

#endif // ATLASTILESET_H
