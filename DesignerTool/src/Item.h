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

class HyGuiDependencies;
class WidgetRenderer;

class Item : public IHyGuiDrawItem
{
    friend class WidgetExplorer;
    
protected:
    const eItemType     m_eTYPE;
    const QString       m_sPATH;
    
    QTreeWidgetItem *   m_pTreeItemPtr;
    QWidget *           m_pWidget;
    QMenu *             m_pEditMenu;
    
    Item(eItemType eType, const QString sPath);
    ~Item();
    
public:
    eItemType GetType() const                       { return m_eTYPE; }
    QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }
    QWidget *GetWidget() const                      { return m_pWidget; }
    QMenu *GetEditMenu() const                      { return m_pEditMenu; }
    
    QString GetName(bool bWithPrefix) const;
    QString GetRelPath() const;
    QString GetAbsPath() const                      { return m_sPATH; }
    QIcon GetIcon() const                           { return HyGlobal::ItemIcon(m_eTYPE); }
    
    void SetTreeItem(QTreeWidgetItem *pTreeItem)    { m_pTreeItemPtr = pTreeItem; }
    
    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    virtual void Save();
    
signals:
    
public slots:
    
};
Q_DECLARE_METATYPE(Item *)

#endif // ITEM_H
