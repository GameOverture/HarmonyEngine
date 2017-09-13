#ifndef PROPERTIESMODEL_H
#define PROPERTIESMODEL_H

#include "PropertiesTreeItems.h"

#include <QAbstractItemModel>

class PropertiesModel : public QAbstractItemModel
{
    Q_OBJECT

    PropertiesTreeItem *                        m_pRootItem;

    QList<PropertiesTreeItem *>                 m_CategoryList;

public:
    explicit PropertiesModel(QObject *parent = 0);
    virtual ~PropertiesModel();

    bool AppendCategory(QString sName);
    bool AppendProperty(QString sCategoryName, QString sName, PropertiesType eType);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    //bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    QModelIndex index(int iRow, int iColumn, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parentIndex = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
};

#endif // PROPERTIESMODEL_H
