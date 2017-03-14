/**************************************************************************
 *	Item.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEM_H
#define ITEM_H

#include <QTreeWidget>

#include "HyGuiGlobal.h"
#include "Harmony/HyEngine.h"

class HyGuiDependencies;
class WidgetRenderer;
class HyGuiFrame;

class Item : public QObject
{
    Q_OBJECT

protected:
    const eItemType     m_eTYPE;
    const QString       m_sPATH;
    
    QTreeWidgetItem *   m_pTreeItemPtr;
    
public:
    Item(eItemType eType, const QString sPath);
    virtual ~Item();

    eItemType GetType() const                       { return m_eTYPE; }
    QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }
    
    QString GetName(bool bWithPrefix) const;
    QString GetPath() const                         { return m_sPATH; }
    QIcon GetIcon() const                           { return HyGlobal::ItemIcon(m_eTYPE); }
};
Q_DECLARE_METATYPE(Item *)

#endif // ITEM_H
