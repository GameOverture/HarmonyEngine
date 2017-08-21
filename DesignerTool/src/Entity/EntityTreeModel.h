#ifndef ENTITYTREEMODEL_H
#define ENTITYTREEMODEL_H

#include "ProjectItem.h"

#include <QAbstractItemModel>

class EntityTreeItem
{
    ProjectItem &                   m_ItemRef;

    EntityTreeItem *                m_pParentItem;
    QList<EntityTreeItem *>         m_ChildList;

public:
    explicit EntityTreeItem(ProjectItem &itemRef, EntityTreeItem *pParentTreeItem);
    ~EntityTreeItem();

    ProjectItem &GetItem();
    EntityTreeItem *GetParent();

    EntityTreeItem *GetChild(int iRow);
    void AppendChild(EntityTreeItem *pChild);
    void InsertChild(int iIndex, EntityTreeItem *pChild);
    void RemoveChild(int iIndex);

    int GetNumChildren() const;
    int GetRow() const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    EntityTreeItem *                    m_pEntityRootItem;

public:
    explicit EntityTreeModel(ProjectItem &entityItemRef, QObject *parent = nullptr);
    virtual ~EntityTreeModel();

    // Basic functionality:
    QModelIndex index(int iRow, int iColumn, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parentIndex = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;

    void InsertItems(int iRow, QList<EntityTreeItem *> itemList, const QModelIndex &parentIndex);
    bool removeRows(int iRow, int iCount, const QModelIndex &parentIndex = QModelIndex()) override;

private:
};

#endif // ENTITYTREEMODEL_H
