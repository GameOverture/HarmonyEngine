/**************************************************************************
 *	ExplorerItem.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERITEM_H
#define EXPLORERITEM_H

#include <QObject>
#include <QTreeWidget>

#include "Global.h"

class ExplorerItem : public QObject
{
	Q_OBJECT

protected:
	const HyGuiItemType		m_eTYPE;
	QString					m_sName;

	Project *				m_pProject;
	bool					m_bIsProjectItem;
	
public:
	ExplorerItem();
	ExplorerItem(Project &projectRef, HyGuiItemType eType, const QString sName);
	virtual ~ExplorerItem();

	HyGuiItemType GetType() const;
	Project &GetProject() const;

	bool IsProjectItem() const;
	
	virtual QString GetName(bool bWithPrefix) const;
	QString GetPrefix() const;
	QIcon GetIcon(SubIcon eSubIcon) const;

	void Rename(QString sNewName);
	void Rename(QString sNewPrefix, QString sNewName);
	virtual void DeleteFromProject();

	QString GetToolTip() const;
};
Q_DECLARE_METATYPE(ExplorerItem *)

QDataStream &operator<<(QDataStream &out, ExplorerItem *const &rhs);
QDataStream &operator>>(QDataStream &in, ExplorerItem *rhs);

#endif // EXPLORERITEM_H