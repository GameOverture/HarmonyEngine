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

#define HYGUI_MIMETYPE_ITEM "application/x-harmonyitem"
#define HYGUI_MIMETYPE_ASSET "application/x-harmonyasset"

class AssetItemData;
class ProjectItemData;

class IMimeData : public QMimeData
{
protected:
	const MimeType		m_eMIME_TYPE;
	QByteArray			m_Data;

public:
	IMimeData(MimeType eMimeType);
	virtual ~IMimeData();

	virtual bool hasFormat(const QString &sMimeType) const override;
	virtual QStringList formats() const override;

protected:
	virtual QVariant retrieveData(const QString &sMimeType, QVariant::Type type) const override;

	QJsonObject MakeAssetJsonObj(AssetItemData &assetDataRef);
};

#endif // IMIMEDATA_H
