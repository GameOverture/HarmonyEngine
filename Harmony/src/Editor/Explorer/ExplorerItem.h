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

//class ExplorerTreeWidget;
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class ExplorerTreeItem : public QTreeWidgetItem
//{
//public:
//	ExplorerTreeItem(int type = Type);
//	ExplorerTreeItem(ExplorerTreeWidget *pView, int type = Type);
//	ExplorerTreeItem(QTreeWidgetItem *parent, int type = Type);
//
//	bool operator<(const QTreeWidgetItem& other) const;
//};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExplorerItem : public QObject
{
	Q_OBJECT

protected:
	const HyGuiItemType		m_eTYPE;
	QString					m_sPath;
	ExplorerItem *			m_pParent;

	Project &				m_ProjectRef;
	bool					m_bIsProjectItem;
	
public:
	ExplorerItem(Project &projectRef, HyGuiItemType eType, const QString sPath, ExplorerItem *pParentItem);
	virtual ~ExplorerItem();

	HyGuiItemType GetType() const;
	Project &GetProject() const;

	bool IsProjectItem() const;
	
	QString GetName(bool bWithPrefix) const;
	QString GetPrefix() const;				// Ends with a '/'
	QIcon GetIcon(SubIcon eSubIcon) const;

	virtual void Rename(QString sNewName);
	void SetTreeItemSubIcon(SubIcon eSubIcon);
};
Q_DECLARE_METATYPE(ExplorerItem *)

QDataStream &operator<<(QDataStream &out, ExplorerItem *const &rhs);
QDataStream &operator>>(QDataStream &in, ExplorerItem *rhs);

#endif // EXPLORERITEM_H
