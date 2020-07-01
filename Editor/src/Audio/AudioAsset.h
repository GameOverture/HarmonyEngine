/**************************************************************************
 *	AudioAsset.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AudioAsset_H
#define AudioAsset_H

#include "IAssetItemData.h"
#include "ProjectItemData.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class AudioAsset : public AssetItemData
{
	Q_OBJECT

public:
	AudioAsset(IManagerModel &modelRef, HyGuiItemType eType, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sFormat, uint uiErrors);
	~AudioAsset();

	void ReplaceAudio(QString sName, uint32 uiChecksum);

	virtual void InsertUniqueJson(QJsonObject &frameObj) override;
};

#endif // AudioAsset_H
