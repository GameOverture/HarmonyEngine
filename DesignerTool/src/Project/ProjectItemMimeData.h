/**************************************************************************
*	ProjectItemMimeData.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef PROJECTITEMMIMEDATA_H
#define PROJECTITEMMIMEDATA_H

#include <QMimeData>

#define HYGUI_MIMETYPE "application/x-harmony"

class ProjectItem;

class ProjectItemMimeData : public QMimeData
{
	Q_OBJECT

	ProjectItem *           m_pProjItem;
	QByteArray              m_Data;

public:
	ProjectItemMimeData(ProjectItem *pProjItem);
	virtual ~ProjectItemMimeData();

	virtual bool hasFormat(const QString &sMimeType) const override;
	virtual QStringList formats() const override;

protected:
	virtual QVariant retrieveData(const QString &mimeType, QVariant::Type type) const override;
};

#endif // PROJECTITEMMIMEDATA_H
