/**************************************************************************
 *	DataExplorerItem.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DATAEXPLORERITEM_H
#define DATAEXPLORERITEM_H

#include <QObject>
#include <QTreeWidget>

#include "HyGuiGlobal.h"
#include "Harmony/HyEngine.h"

class HyGuiDependencies;
class WidgetRenderer;
class AtlasFrame;

class DataExplorerItem : public QObject
{
    Q_OBJECT

protected:
    const HyGuiItemType     m_eTYPE;
    const QString           m_sPATH;

    bool                    m_bIsProjectItem;
    
    QTreeWidgetItem *       m_pTreeItemPtr;
    
public:
    DataExplorerItem(HyGuiItemType eType, const QString sPath);
    virtual ~DataExplorerItem();

    HyGuiItemType GetType() const                   { return m_eTYPE; }
    QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }

    bool IsProjectItem() const;
    
    QString GetName(bool bWithPrefix) const;
    QString GetPrefix() const;
    QString GetPath() const                         { return m_sPATH; }
    QIcon GetIcon(SubIcon eSubIcon) const           { return HyGlobal::ItemIcon(m_eTYPE, eSubIcon); }
    void SetTreeItemSubIcon(SubIcon eSubIcon);
};
Q_DECLARE_METATYPE(DataExplorerItem *)

QDataStream &operator<<(QDataStream &out, DataExplorerItem *const &rhs);
QDataStream &operator>>(QDataStream &in, DataExplorerItem *rhs);

#endif // DATAEXPLORERITEM_H
