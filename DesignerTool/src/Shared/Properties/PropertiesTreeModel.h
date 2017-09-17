#ifndef PROPERTIESTREEMODEL_H
#define PROPERTIESTREEMODEL_H

#include "PropertiesTreeView.h"
#include "PropertiesTreeItem.h"
#include "ProjectItem.h"

#include <QAbstractItemModel>
#include <QUndoCommand>

#define HYRANGE_Int 16777215 // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
#define HYRANGE_double 16777215.0

class PropertiesTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    ProjectItem &                               m_ItemRef;
    const int                                   m_iSTATE_INDEX;
    const QVariant                              m_iSUBSTATE;

    PropertiesTreeItem *                        m_pRootItem;
    QList<PropertiesTreeItem *>                 m_CategoryList;

public:
    explicit PropertiesTreeModel(ProjectItem &itemRef, int iStateIndex, QVariant &subState, QObject *parent = 0);
    virtual ~PropertiesTreeModel();

    ProjectItem &GetItem();

    bool AppendCategory(QString sName);

    bool AppendProperty_Bool(QString sCategoryName, QString sName, bool bDefaultValue);
    bool AppendProperty_Int(QString sCategoryName, QString sName, int iDefaultValue, int iMinRange = -HYRANGE_Int, int iMaxRange = HYRANGE_Int, QString sPrefix = "", QString sPostfix = "");
    bool AppendProperty_Double(QString sCategoryName, QString sName, double dDefaultValue, double dMinRange = -HYRANGE_double, double dMaxRange = HYRANGE_double, QString sPrefix = "", QString sPostfix = "");
    bool AppendProperty_IntVec2(QString sCategoryName, QString sName, glm::ivec2 vDefaultValue);
    bool AppendProperty_Vec2(QString sCategoryName, QString sName, glm::vec2 vDefaultValue);

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
    bool setData(const QModelIndex &index, const QVariant &value, int iRole = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    //bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    PropertiesTreeItem *ValidateCategory(QString sCategoryName, QString sUniquePropertyName);
    void InsertItem(int iRow, PropertiesTreeItem *pItem, PropertiesTreeItem *pParentItem);
    void InsertItems(int iRow, QList<PropertiesTreeItem *> itemList, PropertiesTreeItem *pParentItem);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PropertiesUndoCmd : public QUndoCommand
{
    PropertiesTreeModel &   m_ModelRef;
    const int               m_iSTATE_INDEX;
    const QVariant          m_iSUBSTATE;
    PropertiesTreeItem &    m_TreeItemRef;
    QModelIndex             m_Index;

    QVariant                m_NewData;
    QVariant                m_OldData;

    int                     m_iRole;

public:
    PropertiesUndoCmd(PropertiesTreeModel &modelRef,
                      int iStateIndex,
                      const QVariant &subState,
                      PropertiesTreeItem &treeItemRef,
                      const QModelIndex &index,
                      const QVariant &newData,
                      int iRole,
                      QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
                                                    m_ModelRef(modelRef),
                                                    m_iSTATE_INDEX(iStateIndex),
                                                    m_iSUBSTATE(subState),
                                                    m_TreeItemRef(treeItemRef),
                                                    m_Index(index),
                                                    m_NewData(newData),
                                                    m_OldData(m_TreeItemRef.GetData()),
                                                    m_iRole(iRole)
    {
        setText(m_TreeItemRef.GetName());
    }

    virtual ~PropertiesUndoCmd()
    { }

    virtual void redo() override
    {
        m_TreeItemRef.SetData(m_NewData);

        m_ModelRef.dataChanged(m_Index, m_Index, QVector<int>() << m_iRole);
        m_ModelRef.GetItem().FocusWidgetState(m_iSTATE_INDEX, m_iSUBSTATE);
    }

    virtual void undo() override
    {
        m_TreeItemRef.SetData(m_OldData);

        m_ModelRef.dataChanged(m_Index, m_Index, QVector<int>() << m_iRole);
        m_ModelRef.GetItem().FocusWidgetState(m_iSTATE_INDEX, m_iSUBSTATE);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PROPERTIESTREEMODEL_H
