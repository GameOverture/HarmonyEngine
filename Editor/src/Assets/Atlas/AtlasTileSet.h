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
#define NUM_ROWS_TILESET(numTiles, numCols) (static_cast<int>(numTiles) + static_cast<int>(numCols) - 1) / static_cast<int>(numCols);

bool operator<(const QPoint &a, const QPoint &b);

class TileData;

class AtlasTileSet : public AtlasFrame
{
	Q_OBJECT

	FileDataPair				m_TileSetDataPair;				// The currently 'saved to disk' data of the TileSet
	bool						m_bExistencePendingSave;
	
	QUndoStack *				m_pUndoStack;
	QAction						m_ActionSave;
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

	QMap<QPoint, TileData *>				m_TileDataMap;			// Map of all imported TileData objects in this tile set. Each key is a grid meta-location that is presented to the user, and not its location on the sub-atlas texture

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

	QMap<QPoint, TileData *> GetTileDataMap() const;

	TileSetScene *GetGfxScene();

	// Cmd functions are the only functions that change the data (via Undo/Redo)
	QList<QPair<QPoint, TileData *>> Cmd_AppendNewTiles(QSize vTileSize, const QMap<QPoint, QPixmap> &importBatchMap, Qt::Edge eAppendEdge);
	QList<QPair<QPoint, TileData *>> Cmd_RemoveTiles(QVector<TileData *> tileDataList);
	void Cmd_ReaddTiles(QList<QPair<QPoint, TileData *>> tileDataList);

	QAction *GetSaveAction();
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
	void on_actionSave_triggered();
};

#endif // ATLASTILESET_H
