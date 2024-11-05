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

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

using AutoTileHandle = uint32_t;
using PhysicsLayerHandle = uint32_t;

struct TileData;

class AtlasTileSet : public AtlasFrame
{
	Q_OBJECT

	FileDataPair				m_FileDataPair;
	bool						m_bExistencePendingSave;
	//QUndoStack *				m_pUndoStack;
	//QAction *					m_pActionUndo;
	//QAction *					m_pActionRedo;

	QSize						m_TileSize;

	struct Autotile
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
		QColor					m_Color;
	};
	QList<Autotile>				m_AutotileList;

	struct PhysicsLayer
	{
		PhysicsLayerHandle		m_hId;
		QString					m_sName;
		QColor					m_Color;
	};
	QList<PhysicsLayer>			m_PhysicsLayerList;

	QMap<QPoint, TileData *>	m_TileMap;		// QPoint key is the user/meta location, not the atlas

public:
	AtlasTileSet(IManagerModel &modelRef,
				 QUuid uuid,
				 quint32 uiChecksum,
				 quint32 uiBankId,
				 QString sName,
				 HyTextureInfo texInfo,
				 quint16 uiW, quint16 uiH, quint16 uiX, quint16 uiY,
				 int iTextureIndex,
				 const FileDataPair &initFileDataRef,
				 bool bIsPendingSave,
				 uint uiErrors);
	~AtlasTileSet();

	int GetNumTiles() const;
	QSize GetTileSize() const;
	QString GetTileSetInfo() const;

	void GetLatestFileData(FileDataPair &fileDataPairOut) const;
	void GetSavedFileData(FileDataPair &fileDataPairOut) const;
	bool Save(bool bWriteToDisk);
	bool IsExistencePendingSave() const;
	bool IsSaveClean() const;
	void DiscardChanges();

	virtual void InsertUniqueJson(QJsonObject &frameObj) override;
};

#endif // ATLASTILESET_H
