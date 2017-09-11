#include "PropertiesModel.h"

PropertiesModel::PropertiesModel(QObject *parent) : QAbstractItemModel(parent)
{
}

/*virtual*/ PropertiesModel::~PropertiesModel()
{
}

bool PropertiesModel::AppendCategory(QString sName)
{
    for(int i = 0; i < m_CategoryList.size(); ++i)
    {
        if(0 == m_CategoryList[i]->GetName().compare(sName, Qt::CaseInsensitive))
            return false;
    }

    m_CategoryList.push_back(new PropertiesTreeItem(PROPERTIESTYPE_Category, sName, this));
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

    pCategory->AppendChild(new PropertiesTreeItem(eType, sName, this));
}

QVariant PropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

bool PropertiesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        Q_EMIT headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex PropertiesModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
    return QModelIndex();
}

QModelIndex PropertiesModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
    return QModelIndex();
}

int PropertiesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int PropertiesModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant PropertiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
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
