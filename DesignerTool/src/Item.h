#ifndef ITEM_H
#define ITEM_H

#include <QTreeWidget>

#include "HyGlobal.h"

class Item
{
    friend class WidgetExplorer;
    
protected:
    eItemType           m_eType;
    QString             m_sPath;
    
    QTreeWidgetItem *   m_pTreeItemPtr;
    
    Item();
    Item(const Item &other);
    ~Item();
    
public:
    void Set(eItemType eType, const QString sPath);
    
    eItemType GetType() const                       { return m_eType; }
    QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }
    QString GetName() const;
    QString GetPath() const                         { return m_sPath; }
    QIcon GetIcon() const                           { return HyGlobal::ItemIcon(m_eType); }
    
    void SetTreeItem(QTreeWidgetItem *pTreeItem)    { m_pTreeItemPtr = pTreeItem; }
    
    virtual void Draw();
    
signals:
    
public slots:
    
};
Q_DECLARE_METATYPE(Item *)

#endif // ITEM_H
