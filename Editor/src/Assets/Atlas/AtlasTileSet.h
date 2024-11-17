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

struct TileData;

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

	// Row major order: (Y * NumColumns) + X
	struct MetaLocation
	{
		int iX;
		int iY;

		MetaLocation(int x, int y) : iX(x), iY(y) { }
		bool operator==(const MetaLocation &rhs) const
		{
			return iX == rhs.iX && iY == rhs.iY;
		}
		bool operator!=(const MetaLocation &rhs) const
		{
			return this->operator==(rhs) == false;
		}
		bool operator<(const MetaLocation &rhs) const
		{
			if(iX < rhs.iX)
				return true;
			else if(iX == rhs.iX)
				return iY < rhs.iY;
			return false;
		}
	};
	QMap<MetaLocation, TileData *>	m_TileDataMap;		// QPoint key is the user/meta location, not the atlas. TileData * may be an AlternateTile

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

	void GetLatestFileData(FileDataPair &fileDataPairOut) const;
	void GetSavedFileData(FileDataPair &fileDataPairOut) const;
	bool Save(bool bWriteToDisk);
	bool IsExistencePendingSave() const;
	bool IsSaveClean() const;
	void DiscardChanges();

	virtual void InsertUniqueJson(QJsonObject &frameObj) override;

private Q_SLOTS:
	void on_undoStack_cleanChanged(bool bClean);
	void on_undoStack_indexChanged(int iIndex);
};

#endif // ATLASTILESET_H
