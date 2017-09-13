#include "PropertiesModel.h"

PropertiesModel::PropertiesModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_pRootItem = new PropertiesTreeItem(PROPERTIESTYPE_Root, "Root", this);
}

/*virtual*/ PropertiesModel::~PropertiesModel()
{
    delete m_pRootItem;
}

bool PropertiesModel::AppendCategory(QString sName)
{
    for(int i = 0; i < m_CategoryList.size(); ++i)
    {
        if(0 == m_CategoryList[i]->GetName().compare(sName, Qt::CaseInsensitive))
            return false;
    }

    PropertiesTreeItem *pNewTreeItem = new PropertiesTreeItem(PROPERTIESTYPE_Category, sName, this);
    m_CategoryList.push_back(pNewTreeItem);


    QModelIndex parentIndex = createIndex(m_pRootItem->GetRow(), 0, m_pRootItem);
    int iRow = m_pRootItem->GetNumChildren();//HyClamp(iRow, 0, );

    beginInsertRows(parentIndex, iRow, iRow);
    m_pRootItem->InsertChild(iRow, pNewTreeItem);
    endInsertRows();


    return true;
}

bool PropertiesModel::AppendProperty(QString sCategoryName, QString sName, PropertiesType eType)
{
    if(eType == PROPERTIESTYPE_Category || eType == PROPERTIESTYPE_Root)
        return false;

    PropertiesTreeItem *pCategory = nullptr;
    for(int i = 0; i < m_CategoryList.size(); ++i)
    {
        if(0 == m_CategoryList[i]->GetName().compare(sCategoryName, Qt::CaseInsensitive))
        {
            pCategory = m_CategoryList[i];
            break;
        }
    }

    if(pCategory == nullptr)
        return false;

    PropertiesTreeItem *pNewTreeItem = new PropertiesTreeItem(eType, sName, this);

    QModelIndex parentIndex = createIndex(pCategory->GetRow(), 0, pCategory);
    int iRow = pCategory->GetNumChildren();//HyClamp(iRow, 0, );

    beginInsertRows(parentIndex, iRow, iRow);
    pCategory->InsertChild(iRow, pNewTreeItem);
    endInsertRows();
}

QVariant PropertiesModel::headerData(int iSection, Qt::Orientation orientation, int role) const
{
    if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if(role == Qt::DisplayRole)
    {
        if(iSection == 0)
            return "Property";
        else
            return "Value";
    }

    return QVariant();
}

//bool PropertiesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
//{
//    if (value != headerData(section, orientation, role)) {
//        // FIXME: Implement me!
//        Q_EMIT headerDataChanged(orientation, section, section);
//        return true;
//    }
//    return false;
//}

QModelIndex PropertiesModel::index(int iRow, int iColumn, const QModelIndex &parent) const
{
    if(hasIndex(iRow, iColumn, parent) == false)
        return QModelIndex();

    PropertiesTreeItem *pParentItem;

    if(parent.isValid() == false)
        pParentItem = m_pRootItem;
    else
        pParentItem = static_cast<PropertiesTreeItem *>(parent.internalPointer());

    PropertiesTreeItem *pChildItem = static_cast<PropertiesTreeItem *>(pParentItem->GetChild(iRow));
    if(pChildItem)
        return createIndex(iRow, iColumn, pChildItem);
    else
        return QModelIndex();
}

QModelIndex PropertiesModel::parent(const QModelIndex &index) const
{
    if(index.isValid() == false)
        return QModelIndex();

    PropertiesTreeItem *pChildItem = static_cast<PropertiesTreeItem *>(index.internalPointer());
    PropertiesTreeItem *pParentItem = static_cast<PropertiesTreeItem *>(pChildItem->GetParent());

    if(pParentItem == m_pRootItem)
        return QModelIndex();

    return createIndex(pParentItem->GetRow(), 0, pParentItem);
}

int PropertiesModel::rowCount(const QModelIndex &parentIndex) const
{
    // Only data in column '0' has rows
    if(!parentIndex.isValid() || parentIndex.column() > 0)
        return 0;

    PropertiesTreeItem *pParentItem;
    if(parentIndex.isValid() == false)
        pParentItem = m_pRootItem;
    else
        pParentItem = static_cast<PropertiesTreeItem *>(parentIndex.internalPointer());

    return pParentItem->GetNumChildren();
}

int PropertiesModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant PropertiesModel::data(const QModelIndex &index, int iRole) const
{
    if(index.isValid() == false)
        return QVariant();

    PropertiesTreeItem *pTreeItem = static_cast<PropertiesTreeItem *>(index.internalPointer());

    switch(iRole)
    {
    case Qt::DisplayRole:
        if(index.column() == 0)
            return pTreeItem->GetName();
        else if(index.column() == 1)
            return pTreeItem->GetValue();

    //case Qt::DecorationRole:
    //    return pTreeItem->GetItem()->GetIcon(SUBICON_None);
    case Qt::ToolTipRole:
        return pTreeItem->GetToolTip();
    }

    return QVariant();
}

bool PropertiesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags PropertiesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool PropertiesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();

    return true;
}

bool PropertiesModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();

    return true;
}

bool PropertiesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();

    return true;
}

bool PropertiesModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();

    return true;
}
