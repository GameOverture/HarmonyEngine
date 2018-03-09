/**************************************************************************
 *	ExplorerTreeItem.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERTREEITEM_H
#define EXPLORERTREEITEM_H

#include <QObject>
#include <QTreeWidget>

#include "Global.h"
#include "Harmony/HyEngine.h"

class HyGuiDependencies;
class WidgetRenderer;
class AtlasFrame;

class ExplorerTreeItem : public QObject
{
	Q_OBJECT

protected:
	const HyGuiItemType		m_eTYPE;
	QString					m_sPath;

	bool					m_bIsProjectItem;
	
	QTreeWidgetItem *		m_pTreeItemPtr;
	
public:
	ExplorerTreeItem(HyGuiItemType eType, const QString sPath, QTreeWidgetItem *pParentTreeItem);
	virtual ~ExplorerTreeItem();

	HyGuiItemType GetType() const;
	QTreeWidgetItem *GetTreeItem() const;

	bool IsProjectItem() const;
	
	QString GetName(bool bWithPrefix) const;
	QString GetPrefix() const;				// Ends with a '/'
	QIcon GetIcon(SubIcon eSubIcon) const;

	virtual void Rename(QString sNewName);
	void SetTreeItemSubIcon(SubIcon eSubIcon);
};
Q_DECLARE_METATYPE(ExplorerTreeItem *)

QDataStream &operator<<(QDataStream &out, ExplorerTreeItem *const &rhs);
QDataStream &operator>>(QDataStream &in, ExplorerTreeItem *rhs);

#endif // EXPLORERTREEITEM_H
