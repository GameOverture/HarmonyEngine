#include "EntityTreeModel.h"

EntityTreeItem::EntityTreeItem(ProjectItem &itemRef, EntityTreeItem *pParentTreeItem) : m_ItemRef(itemRef),
                                                                                        m_pParentItem(pParentTreeItem)
{
}

EntityTreeItem::~EntityTreeItem()
{
}

ProjectItem &EntityTreeItem::GetItem()
{
    return m_ItemRef;
}

EntityTreeItem *EntityTreeItem::GetParent()
{
    return m_pParentItem;
}

EntityTreeItem *EntityTreeItem::GetChild(int iRow)
{
    return m_ChildList[iRow];
}

void EntityTreeItem::AppendChild(EntityTreeItem *pChild)
{
    InsertChild(m_ChildList.size(), pChild);
}

void EntityTreeItem::InsertChild(int iIndex, EntityTreeItem *pChild)
{
    if(pChild->m_pParentItem == this)
    {
        m_ChildList.move(m_ChildList.indexOf(pChild), iIndex);
        return;
    }
    else if(pChild->m_pParentItem)
        pChild->m_pParentItem->RemoveChild(pChild->GetRow());

    pChild->m_pParentItem = this;
    m_ChildList.insert(iIndex, pChild);
}

void EntityTreeItem::RemoveChild(int iIndex)
{
    m_ChildList[iIndex]->m_pParentItem = nullptr;
    m_ChildList.removeAt(iIndex);
}

int EntityTreeItem::GetNumChildren() const
{
    return m_ChildList.size();
}

int EntityTreeItem::GetRow() const
{
    if(m_pParentItem)
        return m_pParentItem->m_ChildList.indexOf(const_cast<EntityTreeItem *>(this));

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityTreeModel::EntityTreeModel(ProjectItem &entityItemRef, QObject *parent) : QAbstractItemModel(parent)
{
    m_pEntityRootItem = new EntityTreeItem(entityItemRef, nullptr);
}

/*virtual*/ EntityTreeModel::~EntityTreeModel()
{
    delete m_pEntityRootItem;
}

QModelIndex EntityTreeModel::index(int iRow, int iColumn, const QModelIndex &parent) const
{
    if(hasIndex(iRow, iColumn, parent) == false)
        return QModelIndex();

    EntityTreeItem *pParentItem;
    if(parent.isValid() == false)
        pParentItem = m_pEntityRootItem;
    else
        pParentItem = static_cast<EntityTreeItem *>(parent.internalPointer());

    EntityTreeItem *pChildItem = pParentItem->GetChild(iRow);
    if(pChildItem)
        return createIndex(iRow, iColumn, pChildItem);
    else
        return QModelIndex();
}

QModelIndex EntityTreeModel::parent(const QModelIndex &index) const
{
    if(index.isValid() == false)
        return QModelIndex();

    EntityTreeItem *pChildItem = static_cast<EntityTreeItem *>(index.internalPointer());
    EntityTreeItem *pParentItem = pChildItem->GetParent();

    if(pParentItem == m_pEntityRootItem)
        return QModelIndex();

    return createIndex(pParentItem->GetRow(), 0, pParentItem);
}

int EntityTreeModel::rowCount(const QModelIndex &parentIndex) const
{
    EntityTreeItem *pParentItem;
    if(parentIndex.isValid() == false)
        pParentItem = m_pEntityRootItem;
    else
        pParentItem = static_cast<EntityTreeItem *>(parentIndex.internalPointer());

    return pParentItem->GetNumChildren();
}

int EntityTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant EntityTreeModel::data(const QModelIndex &index, int iRole /*= Qt::DisplayRole*/) const
{
    if(index.isValid() == false)
        return QVariant();

    ProjectItem &projItemRef = static_cast<EntityTreeItem *>(index.internalPointer())->GetItem();

    switch(iRole)
    {
    case Qt::DisplayRole:
        return projItemRef.GetName(false);
    }

    return QVariant();
}

void EntityTreeModel::InsertItems(int iRow, QList<EntityTreeItem *> itemList, const QModelIndex &parentIndex)
{
    EntityTreeItem *pParent;
    if(parentIndex.isValid() == false)
        pParent = m_pEntityRootItem;
    else
        pParent = static_cast<EntityTreeItem *>(parentIndex.internalPointer());

    beginInsertRows(parentIndex, iRow, iRow + itemList.size() - 1);

    for(int i = 0; i < itemList.size(); ++i)
        pParent->InsertChild(iRow + i, itemList[i]);

    endInsertRows();
}

bool EntityTreeModel::removeRows(int iRow, int iCount, const QModelIndex &parentIndex)
{
    EntityTreeItem *pParent;
    if(parentIndex.isValid() == false)
        pParent = m_pEntityRootItem;
    else
        pParent = static_cast<EntityTreeItem *>(parentIndex.internalPointer());

    if(pParent->GetNumChildren() < iRow + iCount)
        return false;

    beginRemoveRows(parentIndex, iRow, iRow + iCount - 1);

    for(int i = 0; i < iCount; ++i)
        pParent->RemoveChild(iRow);

    endRemoveRows();

    return true;
}
