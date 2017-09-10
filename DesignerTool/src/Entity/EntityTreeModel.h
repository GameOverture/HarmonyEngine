#ifndef ENTITYTREEMODEL_H
#define ENTITYTREEMODEL_H

#include "ProjectItem.h"
#include "GlobalWidgetMappers.h"

#include <QAbstractItemModel>

class EntityTreeModel;

class EntityTreeItemData
{
public:
    DoubleSpinBoxMapper     m_PosX;
    DoubleSpinBoxMapper     m_PosY;
    DoubleSpinBoxMapper     m_ScaleX;
    DoubleSpinBoxMapper     m_ScaleY;
    DoubleSpinBoxMapper     m_Rotation;

    CheckBoxMapper          m_Enabled;
    CheckBoxMapper          m_UpdateWhilePaused;
    SpinBoxMapper           m_Tag;
    SpinBoxMapper           m_DisplayOrder;

public:
    EntityTreeItemData();
    ~EntityTreeItemData();

    QJsonObject GetJson();
};

class EntityTreeItem
{
    EntityTreeModel *                       m_pTreeModel;
    ProjectItem *                           m_pItem;

    EntityTreeItem *                        m_pParentItem;
    QList<EntityTreeItem *>                 m_ChildList;

    QList<EntityTreeItemData>               m_StateDataList;

public:
    explicit EntityTreeItem(EntityTreeModel *pTreeModel, ProjectItem *pItem);
    ~EntityTreeItem();

    ProjectItem *GetItem();
    EntityTreeItem *GetParent();
    EntityTreeItemData &GetData(int iIndex);

    EntityTreeItem *GetChild(int iRow);
    void AppendChild(EntityTreeItem *pChild);
    void InsertChild(int iIndex, EntityTreeItem *pChild);
    void RemoveChild(int iIndex);

    int GetNumChildren() const;
    int GetRow() const;
    int GetCol() const;

    QString GetToolTip() const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    EntityTreeItem *                    m_pRootItem;
    EntityTreeItem *                    m_pEntityItem;

public:
    explicit EntityTreeModel(ProjectItem &entityItemRef, QObject *parent = nullptr);
    virtual ~EntityTreeModel();

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
