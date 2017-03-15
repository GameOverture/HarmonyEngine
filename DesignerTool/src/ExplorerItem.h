/**************************************************************************
 *	Item.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERITEM_H
#define EXPLORERITEM_H

#include <QObject>
#include <QTreeWidget>

#include "HyGuiGlobal.h"
#include "Harmony/HyEngine.h"

class HyGuiDependencies;
class WidgetRenderer;
class AtlasFrame;

class ExplorerItem : public QObject
{
    Q_OBJECT

protected:
    const eItemType     m_eTYPE;
    const QString       m_sPATH;
    
    QTreeWidgetItem *   m_pTreeItemPtr;
    
public:
    ExplorerItem(eItemType eType, const QString sPath);
    virtual ~ExplorerItem();

    eItemType GetType() const                       { return m_eTYPE; }
    QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }
    
    QString GetName(bool bWithPrefix) const;
    QString GetPath() const                         { return m_sPATH; }
    QIcon GetIcon() const                           { return HyGlobal::ItemIcon(m_eTYPE); }
};
Q_DECLARE_METATYPE(ExplorerItem *)

#endif // EXPLORERITEM_H
