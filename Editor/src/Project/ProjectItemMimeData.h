/**************************************************************************
*	ProjectItemMimeData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ProjectItemMimeData_H
#define ProjectItemMimeData_H

#include <QMimeData>

#define HYGUI_MIMETYPE "application/x-harmony"

class ExplorerItemData;
class ProjectItemData;

class ProjectItemMimeData : public QMimeData
{
	QByteArray				m_Data;

public:
	ProjectItemMimeData(QList<ExplorerItemData *> &itemListRef);
	ProjectItemMimeData(const QVariant &data);
	virtual ~ProjectItemMimeData();

	virtual bool hasFormat(const QString &sMimeType) const override;
	virtual QStringList formats() const override;

protected:
	virtual QVariant retrieveData(const QString &mimeType, QVariant::Type type) const override;
	QJsonArray GetAssetsArray(HyGuiItemType eManagerType, ProjectItemData *pProjectItem);
};

#endif // ProjectItemMimeData_H
