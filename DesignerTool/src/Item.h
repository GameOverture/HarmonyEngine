#ifndef ITEM_H
#define ITEM_H

#include <QTreeWidget>

#include "HyGlobal.h"
#include "Harmony/HyEngine.h"

class WidgetRenderer;

class Item
{
    friend class WidgetExplorer;
    
protected:
    eItemType           m_eType;
    QString             m_sPath;
    
    QTreeWidgetItem *   m_pTreeItemPtr;
    
    Item(eItemType eType, const QString sPath);
    Item(const Item &other);
    ~Item();
    
public:


    eItemType GetType() const                       { return m_eType; }
    QTreeWidgetItem *GetTreeItem() const            { return m_pTreeItemPtr; }
    QString GetName() const;
    QString GetPath() const                         { return m_sPath; }
    QIcon GetIcon() const                           { return HyGlobal::ItemIcon(m_eType); }
    
    void SetTreeItem(QTreeWidgetItem *pTreeItem)    { m_pTreeItemPtr = pTreeItem; }
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(WidgetRenderer &renderer);
    
    virtual void Save();
    
signals:
    
public slots:

private:
    void Initialize(eItemType eType, const QString sPath);
    
};
Q_DECLARE_METATYPE(Item *)

#endif // ITEM_H
