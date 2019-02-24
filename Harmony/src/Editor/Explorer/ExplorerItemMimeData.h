/**************************************************************************
*	ExplorerItemMimeData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ExplorerItemMimeData_H
#define ExplorerItemMimeData_H

#include <QMimeData>

#define HYGUI_MIMETYPE "application/x-harmony"

class ExplorerItem;

class ExplorerItemMimeData : public QMimeData
{
	ExplorerItem *			m_pExplorerItem;
	QByteArray				m_Data;

public:
	ExplorerItemMimeData(ExplorerItem *pExplorerItem);
	virtual ~ExplorerItemMimeData();

	HyGuiItemType GetType() const;
	ExplorerItem *GetItem() const;

	virtual bool hasFormat(const QString &sMimeType) const override;
	virtual QStringList formats() const override;

protected:
	virtual QVariant retrieveData(const QString &mimeType, QVariant::Type type) const override;
};

#endif // ExplorerItemMimeData_H
