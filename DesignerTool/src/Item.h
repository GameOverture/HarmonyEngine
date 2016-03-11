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

#include "HyGlobal.h"
#include "IHyGuiDrawItem.h"
#include "Harmony/HyEngine.h"

class WidgetRenderer;

class Item : public IHyGuiDrawItem
{
    friend class WidgetExplorer;
    
protected:
    eItemType           m_eType;
    QString             m_sPath;
    
    QTreeWidgetItem *   m_pTreeItemPtr;
    QWidget *           m_pWidget;
    
    Item(eItemType eType, const QString sPath);
    Item(const Item &other);
    ~Item();
    
public:
    eItemType GetType() const                       { return m_eType; }
    QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }
    QWidget *GetWidget() const                      { return m_pWidget; }
    
    QString GetName() const;
    QString GetPath() const                         { return m_sPath; }
    QIcon GetIcon() const                           { return HyGlobal::ItemIcon(m_eType); }
    
    void SetTreeItem(QTreeWidgetItem *pTreeItem)    { m_pTreeItemPtr = pTreeItem; }
    
    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    virtual void Save();
    
signals:
    
public slots:

private:
    void Initialize(eItemType eType, const QString sPath);
    
};
Q_DECLARE_METATYPE(Item *)

#endif // ITEM_H
