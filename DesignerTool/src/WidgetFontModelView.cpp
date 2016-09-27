/**************************************************************************
 *	WidgetFontModelView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFontModelView.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

/*virtual*/ QWidget* WidgetFontDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        pReturnWidget = new QComboBox(pParent);
        static_cast<QComboBox *>(pReturnWidget)->addItem("Normal");
        static_cast<QComboBox *>(pReturnWidget)->addItem("Outline Edge");
        static_cast<QComboBox *>(pReturnWidget)->addItem("Outline Positive");
        static_cast<QComboBox *>(pReturnWidget)->addItem("Outline Negative");
        static_cast<QComboBox *>(pReturnWidget)->addItem("Distance Field");
        break;
        
    case WidgetFontModel::COLUMN_Thickness:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(0.0, 4096.0);
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        pReturnWidget = new QPushButton(pParent);
        //static_cast<QPushButton *>(pReturnWidget)->
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void WidgetFontDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
}

/*virtual*/ void WidgetFontDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
}

/*virtual*/ void WidgetFontDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontModel::WidgetFontModel(QObject *parent) : QAbstractTableModel(parent)
{
    
}

/*virtual*/ int WidgetFontModel::rowCount(const QModelIndex &parent = QModelIndex()) const
{
    return m_StageList.count();
}

/*virtual*/ int WidgetFontModel::columnCount(const QModelIndex &parent = QModelIndex()) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant WidgetFontModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const
{
    FontStage *pStage = m_StageList[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Frame)
    {
        return Qt::AlignCenter;
    }
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Type:
            return pFrame->m_pFrame->GetName();
        case COLUMN_Thickness:
            return QString::number(pFrame->m_vOffset.x());
        case COLUMN_DefaultColor:
            return QString::number(pFrame->m_vOffset.y());
        }
    }

    return QVariant();
}

/*virtual*/ QVariant WidgetFontModel::headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const
{
}

/*virtual*/ bool WidgetFontModel::setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole)
{
}

/*virtual*/ Qt::ItemFlags WidgetFontModel::flags(const QModelIndex & index) const
{
}
