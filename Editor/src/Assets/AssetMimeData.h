/**************************************************************************
*	AssetMimeData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef AssetMimeData_H
#define AssetMimeData_H

#include "Global.h"
#include <QMimeData>

#define HYGUI_MIMETYPE_ASSET "application/x-harmonyasset"

class Project;
class TreeModelItemData;

class AssetMimeData : public QMimeData
{
	QByteArray				m_Data;

public:
	AssetMimeData(Project &projRef, AssetType eManagerType, QList<TreeModelItemData *> &itemListRef);
	AssetMimeData(const QVariant &data);
	virtual ~AssetMimeData();

	virtual bool hasFormat(const QString &sMimeType) const override;
	virtual QStringList formats() const override;

protected:
	virtual QVariant retrieveData(const QString &mimeType, QVariant::Type type) const override;
};

#endif // AssetMimeData_H
