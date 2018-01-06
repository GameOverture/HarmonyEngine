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
	const HyGuiItemType     m_eTYPE;
	const QString           m_sPATH;

	bool                    m_bIsProjectItem;
	
	QTreeWidgetItem *       m_pTreeItemPtr;
	
public:
	ExplorerTreeItem(HyGuiItemType eType, const QString sPath);
	virtual ~ExplorerTreeItem();

	HyGuiItemType GetType() const                   { return m_eTYPE; }
	QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }

	bool IsProjectItem() const;
	
	QString GetName(bool bWithPrefix) const;
	QString GetPrefix() const;
	QString GetPath() const                         { return m_sPATH; }
	QIcon GetIcon(SubIcon eSubIcon) const           { return HyGlobal::ItemIcon(m_eTYPE, eSubIcon); }
	void SetTreeItemSubIcon(SubIcon eSubIcon);
};
Q_DECLARE_METATYPE(ExplorerTreeItem *)

QDataStream &operator<<(QDataStream &out, ExplorerTreeItem *const &rhs);
QDataStream &operator>>(QDataStream &in, ExplorerTreeItem *rhs);

#endif // EXPLORERTREEITEM_H
