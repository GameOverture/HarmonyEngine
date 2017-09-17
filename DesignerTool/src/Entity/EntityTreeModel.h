#ifndef ENTITYTREEMODEL_H
#define ENTITYTREEMODEL_H

#include "ProjectItem.h"
#include "GlobalWidgetMappers.h"
#include "PropertiesTreeModel.h"
#include "IModelTreeItem.h"

#include <QAbstractItemModel>

class EntityModel;
class EntityTreeModel;

class EntityTreeItem : public IModelTreeItem
{
    EntityTreeModel *           m_pTreeModel;
    ProjectItem *               m_pItem;

public:
    explicit EntityTreeItem(EntityTreeModel *pTreeModel, ProjectItem *pItem);
    virtual ~EntityTreeItem();

    ProjectItem *GetItem();

    virtual QString GetToolTip() const override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    EntityModel *               m_pEntityModel;
    EntityTreeItem *            m_pRootItem;
    EntityTreeItem *            m_pEntityItem;

public:
    explicit EntityTreeModel(EntityModel *pEntityModel, ProjectItem &entityItemRef, QObject *parent = nullptr);
    virtual ~EntityTreeModel();

    int GetNumStates();

    // Basic functionality:
    QModelIndex index(int iRow, int iColumn, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parentIndex = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;

    void InsertItem(int iRow, EntityTreeItem * pItem, EntityTreeItem *pParentItem);
    void InsertItems(int iRow, QList<EntityTreeItem *> itemList, EntityTreeItem *pParentItem);

    void RemoveItems(int iRow, int iCount, EntityTreeItem *pParentItem);
    bool removeRows(int iRow, int iCount, const QModelIndex &parentIndex = QModelIndex()) override;

private:
};

#endif // ENTITYTREEMODEL_H
