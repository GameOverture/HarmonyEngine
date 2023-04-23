/**************************************************************************
 *	ExplorerItemData.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERITEMDATA_H
#define EXPLORERITEMDATA_H

#include <QObject>
#include <QTreeWidget>

#include "TreeModelItemData.h"
#include "Global.h"

class ExplorerItemData : public TreeModelItemData
{
	Q_OBJECT
	friend class ExplorerModel;

protected:
	Project *				m_pProject;
	
	ExplorerItemData(Project &projectRef, ItemType eType, const QUuid &uuid, const QString sName);
public:
	virtual ~ExplorerItemData();

	Project &GetProject() const;
	
	virtual QString GetName(bool bWithPrefix) const;
	QString GetPrefix() const;

	void Rename(QString sNewName);
	void Rename(QString sNewPrefix, QString sNewName);
	virtual void DeleteFromProject();

	QString GetToolTip() const;
};

#endif // EXPLORERITEMDATA_H
