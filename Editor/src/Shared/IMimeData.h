/**************************************************************************
 *	IMimeData.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IMIMEDATA_H
#define IMIMEDATA_H

#include "Global.h"

#include <QMimeData>

class IAssetItemData;
class TreeModelItemData;

class IMimeData : public QMimeData
{
protected:
	const MimeType		m_eMIME_TYPE;
	QByteArray			m_Data;

public:
	IMimeData(MimeType eMimeType);
	virtual ~IMimeData();

	MimeType GetMimeType() const;

	virtual bool hasFormat(const QString &sMimeType) const override;
	virtual QStringList formats() const override;

protected:
	virtual QVariant retrieveData(const QString &sMimeType, QMetaType type) const override;

	QJsonArray MakeAssetJsonArray(Project &projRef, QList<TreeModelItemData *> assetList, AssetManagerType eAssetType);
};

#endif // IMIMEDATA_H
