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
#include "ItemFontCmds.h"
#include "WidgetFont.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

int WidgetFontModel::sm_iUniqueIdCounter = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontTableView::WidgetFontTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetFontTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64;
    setColumnWidth(WidgetFontModel::COLUMN_Type, iWidth / 2);
    setColumnWidth(WidgetFontModel::COLUMN_Thickness, 64);
    setColumnWidth(WidgetFontModel::COLUMN_DefaultColor, iWidth / 2);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontDelegate::WidgetFontDelegate(ItemFont *pItemFont, QObject *pParent /*= 0*/) : QStyledItemDelegate(pParent),
                                                                                        m_pItemFont(pItemFont)
{
}

/*virtual*/ QWidget* WidgetFontDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    const WidgetFontModel *pFontModel = static_cast<const WidgetFontModel *>(index.model());

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        pReturnWidget = new QComboBox(pParent);
        static_cast<QComboBox *>(pReturnWidget)->addItem("Normal");
        static_cast<QComboBox *>(pReturnWidget)->addItem("Outline Edge");
        static_cast<QComboBox *>(pReturnWidget)->addItem("Outline Added");
        static_cast<QComboBox *>(pReturnWidget)->addItem("Outline Removed");
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
    const WidgetFontModel *pFontModel = static_cast<const WidgetFontModel *>(index.model());

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        static_cast<QComboBox *>(pEditor)->setCurrentIndex(pFontModel->GetStageRenderMode(index.row()));
        break;

    case WidgetFontModel::COLUMN_Thickness:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(pFontModel->GetStageOutlineThickness(index.row()));
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        break;
    }
}

/*virtual*/ void WidgetFontDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    WidgetFontModel *pFontModel = static_cast<WidgetFontModel *>(pModel);

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageRenderMode(*static_cast<WidgetFont *>(m_pItemFont->GetWidget()),
                                                                          pFontModel,
                                                                          index.row(),
                                                                          pFontModel->GetStageRenderMode(index.row()),
                                                                          static_cast<rendermode_t>(static_cast<QComboBox *>(pEditor)->currentIndex())));
        break;

    case WidgetFontModel::COLUMN_Thickness:
        m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageOutlineThickness(*static_cast<WidgetFont *>(m_pItemFont->GetWidget()),
                                                                                pFontModel,
                                                                                index.row(),
                                                                                pFontModel->GetStageOutlineThickness(index.row()),
                                                                                static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        //m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageColor(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void WidgetFontDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontModel::WidgetFontModel(QObject *parent) : QAbstractTableModel(parent)
{
}

/*virtual*/ WidgetFontModel::~WidgetFontModel()
{
    for(int i = 0; i < m_MasterStageList.count(); ++i)
        delete m_MasterStageList[i];
    
    QMap<int, FontStage *>::iterator iter;
    for(iter = m_RemovedStageMap.begin(); iter != m_RemovedStageMap.end(); ++iter)
        delete iter.value();
}

int WidgetFontModel::RequestStage(QString sFullFontPath, rendermode_t eRenderMode, float fSize, float fOutlineThickness)
{
    // Look for an existing stage that matches the request first
    for(int i = 0; i < sm_MasterStageList.count(); ++i)
    {
        FontStage *pStage = sm_MasterStageList[i];
        
        QFileInfo stageFontPath(pStage->pTextureFont->filename);
        QFileInfo requestFontPath(sFullFontPath);
        
        if(QString::compare(stageFontPath.fileName(), requestFontPath.fileName(), Qt::CaseInsensitive) == 0 &&
           pStage->eMode == eRenderMode &&
           pStage->fSize == fSize &&
           pStage->fOutlineThickness == fOutlineThickness)
        {
            return pStage->iUNIQUE_ID;
        }
    }
    
    sm_iUniqueIdCounter++;
    int iRowIndex = m_StageList.count();

    beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
    FontStage *pNewFontStage = new FontStage(sm_iUniqueIdCounter, eRenderMode, fSize, fOutlineThickness);
    m_StageList.append(pNewFontStage);
    endInsertRows();

    return pNewFontStage->iUNIQUE_ID;
}

void WidgetFontModel::RequestStage(int iId)
{
    for(int i = 0; i < m_RemovedStageList.count(); ++i)
    {
        if(m_RemovedStageList[i].first->iUNIQUE_ID == iId)
        {
            beginInsertRows(QModelIndex(), m_RemovedStageList[i].second, m_RemovedStageList[i].second);
            m_MasterStageList.insert(m_RemovedStageList[i].second, m_RemovedStageList[i].first);
            m_RemovedStageList.removeAt(i);
            endInsertRows();

            break;
        }
    }
}

void WidgetFontModel::RemoveStage(int iId)
{
    for(int i = 0; i < m_MasterStageList.count(); ++i)
    {
        if(m_MasterStageList[i]->iUNIQUE_ID == iId)
        {
            m_RemovedStageList.append(QPair<FontStage *, int>(m_MasterStageList[i], i));
            m_MasterStageList.removeAt(i);

            return;
        }
    }
}

int WidgetFontModel::GetStageId(int iRowIndex) const
{
    return m_MasterStageList[iRowIndex]->iUNIQUE_ID;
}

rendermode_t WidgetFontModel::GetStageRenderMode(int iRowIndex) const
{
    return m_MasterStageList[iRowIndex]->eMode;
}

void WidgetFontModel::SetStageRenderMode(int iRowIndex, rendermode_t eRenderMode)
{
    m_MasterStageList[iRowIndex]->eMode = eRenderMode;
}

float WidgetFontModel::GetStageOutlineThickness(int iRowIndex) const
{
    return m_MasterStageList[iRowIndex]->fOutlineThickness;
}

void WidgetFontModel::SetStageOutlineThickness(int iRowIndex, float fThickness)
{
    m_MasterStageList[iRowIndex]->fOutlineThickness = fThickness;
}

void WidgetFontModel::SetTextureFont(int iRowIndex, texture_font_t *pTextureFont)
{
    if(m_MasterStageList[iRowIndex]->pTextureFont)
        texture_font_delete(m_MasterStageList[iRowIndex]->pTextureFont);

    m_MasterStageList[iRowIndex]->pTextureFont = pTextureFont;
    m_MasterStageList[iRowIndex]->pTextureFont->rendermode = m_MasterStageList[iRowIndex]->eMode;
    m_MasterStageList[iRowIndex]->pTextureFont->outline_thickness = m_MasterStageList[iRowIndex]->fOutlineThickness;
}

/*virtual*/ int WidgetFontModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_MasterStageList.count();
}

/*virtual*/ int WidgetFontModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant WidgetFontModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    FontStage *pStage = m_MasterStageList[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Type)
    {
        return Qt::AlignCenter;
    }
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Type:
            return GetRenderModeString(pStage->eMode);
        case COLUMN_Thickness:
            return QString::number(GetStageOutlineThickness(index.row()), 'g', 2);
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
