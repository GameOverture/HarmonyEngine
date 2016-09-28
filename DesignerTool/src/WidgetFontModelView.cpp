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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontTableView::WidgetFontTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetFontTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64 + 64 + 64;
    setColumnWidth(WidgetFontModel::COLUMN_Type, iWidth);
    setColumnWidth(WidgetFontModel::COLUMN_Size, 64);
    setColumnWidth(WidgetFontModel::COLUMN_Thickness, 64);
    setColumnWidth(WidgetFontModel::COLUMN_DefaultColor, 64);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    for(int i = 0; i < m_StageList.count(); ++i)
        delete m_StageList[i];

    for(int i = 0; i < m_RemovedStageList.count(); ++i)
        delete m_RemovedStageList[i];
}

void WidgetFontModel::AddStage(FontStage::eType eRenderType, float fSize, float fOutlineThickness, QColor topColor, QColor botColor)
{
    int iRowIndex = m_StageList.count();

    beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
    m_StageList.append(new FontStage(eRenderType, fSize, fOutlineThickness, topColor, botColor));
    endInsertRows();
}

void WidgetFontModel::AddStage(FontStage *pExistingStage, int iRowIndex)
{
    beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
    m_StageList.insert(iRowIndex, pExistingStage);
    endInsertRows();
}

void WidgetFontModel::RemoveStage(FontStage *pStage)
{
    for(int i = 0; i < m_StageList.count(); ++i)
    {
        if(m_StageList[i] == pStage)
        {
            delete m_StageList[i];
            m_StageList.removeAt(i);

            break;
        }
    }
}

/*virtual*/ int WidgetFontModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_StageList.count();
}

/*virtual*/ int WidgetFontModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant WidgetFontModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    FontStage *pStage = m_StageList[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Type)
    {
        return Qt::AlignCenter;
    }
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Type:
            return  pStage->m_eType;
        case COLUMN_Thickness:
            return pStage->m_fOutlineThickness;
        case COLUMN_DefaultColor:
            return QVariant();//pStage->m_fOutlineThickness;
        }
    }

    return QVariant();
}

/*virtual*/ QVariant WidgetFontModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(iIndex)
            {
            case COLUMN_Type:
                return QString("Type");
            case COLUMN_Size:
                return QString("Size");
            case COLUMN_Thickness:
                return QString("Thickness");
            case COLUMN_DefaultColor:
                return QString("Default Color");
            }
        }
        else
            return QString::number(iIndex);
    }

    return QVariant();
}

/*virtual*/ bool WidgetFontModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
    HyGuiLog("WidgetFontModel::setData was invoked", LOGTYPE_Error);

//    SpriteFrame *pFrame = m_FramesList[index.row()];

//    if(role == Qt::EditRole)
//    {
//        switch(index.column())
//        {
//        case COLUMN_OffsetX:
//            pFrame->m_vOffset.setX(value.toInt());
//            break;
//        case COLUMN_OffsetY:
//            pFrame->m_vOffset.setY(value.toInt());
//            break;
//        case COLUMN_Duration:
//            pFrame->m_fDuration = value.toFloat();
//            break;
//        }
//    }

//    QVector<int> vRolesChanged;
//    vRolesChanged.append(role);
//    dataChanged(index, index, vRolesChanged);

    return true;
}

/*virtual*/ Qt::ItemFlags WidgetFontModel::flags(const QModelIndex & index) const
{
    // TODO: Make a read only version of all entries
//    if(index.column() == COLUMN_Type)
//        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
//    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
