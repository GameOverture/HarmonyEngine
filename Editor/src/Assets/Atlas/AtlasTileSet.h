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

#include "AtlasFrame.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class AtlasTileSet : public AtlasFrame
{
	Q_OBJECT

	QSize			m_TileSize;

public:
	AtlasTileSet(IManagerModel &modelRef,
			   QUuid uuid,
			   quint32 uiChecksum,
			   quint32 uiBankId,
			   QString sName,
			   HyTextureInfo texInfo,
			   quint16 uiW, quint16 uiH, quint16 uiX, quint16 uiY,
			   QSize tileSize,
			   int iTextureIndex,
			   uint uiErrors);
	~AtlasTileSet();

	QSize GetTileSize() const;

	virtual void InsertUniqueJson(QJsonObject &frameObj) override;
};

#endif // ATLASTILESET_H
