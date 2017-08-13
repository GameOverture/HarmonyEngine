/**************************************************************************
 *	WidgetFontModelView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontModelView.h"
#include "FontUndoCmds.h"
#include "FontWidget.h"
#include "DlgColorPicker.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontTableView::FontTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void FontTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 144;
    setColumnWidth(FontStateLayersModel::COLUMN_Type, iWidth);
    setColumnWidth(FontStateLayersModel::COLUMN_Thickness, 64);
    setColumnWidth(FontStateLayersModel::COLUMN_DefaultColor, 80);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FontDelegate::FontDelegate(ProjectItem *pItem, QComboBox *pCmbStates, QObject *pParent /*= 0*/) :   QStyledItemDelegate(pParent),
                                                                                                    m_pItem(pItem),
                                                                                                    m_pCmbStates(pCmbStates)
{
}

/*virtual*/ QWidget* FontDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    const FontStateLayersModel *pFontModel = static_cast<const FontStateLayersModel *>(index.model());

    switch(index.column())
    {
    case FontStateLayersModel::COLUMN_Type:
        pReturnWidget = new QComboBox(pParent);
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_NORMAL));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_EDGE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_POSITIVE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_NEGATIVE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_SIGNED_DISTANCE_FIELD));
        break;

    case FontStateLayersModel::COLUMN_Thickness:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(0.0, 4096.0);
        break;

    case FontStateLayersModel::COLUMN_DefaultColor:
        DlgColorPicker *pDlg = new DlgColorPicker("Choose Font Layer Color", pFontModel->GetLayerTopColor(index.row()), pFontModel->GetLayerBotColor(index.row()), pParent);
        if(pDlg->exec() == QDialog::Accepted)
        {
            QColor topColor, botColor;
            if(pDlg->IsSolidColor())
            {
                topColor = pDlg->GetSolidColor();
                botColor = pDlg->GetSolidColor();
            }
            else
            {
                topColor = pDlg->GetVgTopColor();
                botColor = pDlg->GetVgBotColor();
            }
            m_pItem->GetUndoStack()->push(new FontUndoCmd_LayerColors(*m_pItem,
                                                                      m_pCmbStates->currentIndex(),
                                                                      pFontModel->GetLayerId(index.row()),
                                                                      pFontModel->GetLayerTopColor(index.row()),
                                                                      pFontModel->GetLayerBotColor(index.row()),
                                                                      topColor,
                                                                      botColor));
        }
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void FontDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    const FontStateLayersModel *pFontModel = static_cast<const FontStateLayersModel *>(index.model());

    switch(index.column())
    {
    case FontStateLayersModel::COLUMN_Type:
        static_cast<QComboBox *>(pEditor)->setCurrentIndex(pFontModel->GetLayerRenderMode(index.row()));
        break;

    case FontStateLayersModel::COLUMN_Thickness:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(pFontModel->GetLayerOutlineThickness(index.row()));
        break;

    case FontStateLayersModel::COLUMN_DefaultColor:
        break;
    }
}

/*virtual*/ void FontDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    FontStateLayersModel *pFontModel = static_cast<FontStateLayersModel *>(pModel);

    switch(index.column())
    {
    case FontStateLayersModel::COLUMN_Type:
        m_pItem->GetUndoStack()->push(new FontUndoCmd_LayerRenderMode(*m_pItem,
                                                                      m_pCmbStates->currentIndex(),
                                                                      pFontModel->GetLayerId(index.row()),
                                                                      pFontModel->GetLayerRenderMode(index.row()),
                                                                      static_cast<rendermode_t>(static_cast<QComboBox *>(pEditor)->currentIndex())));
        break;

    case FontStateLayersModel::COLUMN_Thickness:
        m_pItem->GetUndoStack()->push(new FontUndoCmd_LayerOutlineThickness(*m_pItem,
                                                                            m_pCmbStates->currentIndex(),
                                                                            pFontModel->GetLayerId(index.row()),
                                                                            pFontModel->GetLayerOutlineThickness(index.row()),
                                                                            static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;

    case FontStateLayersModel::COLUMN_DefaultColor:
        //m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageColor(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void FontDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int FontStateLayersModel::sm_iUniqueIdCounter = 0;

FontStateLayersModel::FontStateLayersModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_sRenderModeStringList.append("Normal");
    m_sRenderModeStringList.append("Outline Edge");
    m_sRenderModeStringList.append("Outline Added");
    m_sRenderModeStringList.append("Outline Removed");
    m_sRenderModeStringList.append("Distance Field");
}

/*virtual*/ FontStateLayersModel::~FontStateLayersModel()
{
}


QString FontStateLayersModel::GetRenderModeString(rendermode_t eMode) const
{
    return m_sRenderModeStringList[eMode];
}

int FontStateLayersModel::AddNewLayer(rendermode_t eRenderMode, int iSize, float fOutlineThickness)
{
    sm_iUniqueIdCounter++;

    int iRowIndex = m_LayerList.count();
    FontLayer *pLayer = new FontLayer(sm_iUniqueIdCounter, eRenderMode, iSize, fOutlineThickness);

    beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
    m_LayerList.append(pLayer);
    endInsertRows();

    return pLayer->iUNIQUE_ID;
}

void FontStateLayersModel::RemoveLayer(int iId)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(m_LayerList[i]->iUNIQUE_ID == iId)
        {
            beginRemoveRows(QModelIndex(), i, i);
            m_RemovedLayerList.append(QPair<int, FontLayer *>(i, m_LayerList[i]));
            m_LayerList.removeAt(i);
            endRemoveRows();
        }
    }
}

void FontStateLayersModel::ReAddLayer(int iId)
{
    for(int i = 0; i < m_RemovedLayerList.count(); ++i)
    {
        if(m_RemovedLayerList[i].second->iUNIQUE_ID == iId)
        {
            beginInsertRows(QModelIndex(), m_RemovedLayerList[i].first, m_RemovedLayerList[i].first);
            m_LayerList.insert(m_RemovedLayerList[i].first, m_RemovedLayerList[i].second);
            m_RemovedLayerList.removeAt(i);
            endInsertRows();
        }
    }
}

int FontStateLayersModel::GetLayerId(int iRowIndex) const
{
    return m_LayerList[iRowIndex]->iUNIQUE_ID;
}

FontTypeface *FontStateLayersModel::GetStageRef(int iRowIndex)
{
    return m_LayerList[iRowIndex]->pReference;
}

rendermode_t FontStateLayersModel::GetLayerRenderMode(int iRowIndex) const
{
    return m_LayerList[iRowIndex]->eMode;
}

void FontStateLayersModel::SetLayerRenderMode(int iId, rendermode_t eMode)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(m_LayerList[i]->iUNIQUE_ID == iId)
        {
            m_LayerList[i]->eMode = eMode;
            dataChanged(createIndex(i, COLUMN_Type), createIndex(i, COLUMN_Type));
        }
    }
}

float FontStateLayersModel::GetLayerOutlineThickness(int iRowIndex) const
{
    return m_LayerList[iRowIndex]->fOutlineThickness;
}

void FontStateLayersModel::SetLayerOutlineThickness(int iId, float fThickness)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(m_LayerList[i]->iUNIQUE_ID == iId)
        {
            m_LayerList[i]->fOutlineThickness = fThickness;
            dataChanged(createIndex(i, COLUMN_Thickness), createIndex(i, COLUMN_Thickness));
        }
    }
}

QColor FontStateLayersModel::GetLayerTopColor(int iRowIndex) const
{
    return QColor(m_LayerList[iRowIndex]->vTopColor.x * 255.0f, m_LayerList[iRowIndex]->vTopColor.y * 255.0f, m_LayerList[iRowIndex]->vTopColor.z * 255.0f);
}

QColor FontStateLayersModel::GetLayerBotColor(int iRowIndex) const
{
    return QColor(m_LayerList[iRowIndex]->vBotColor.x * 255.0f, m_LayerList[iRowIndex]->vBotColor.y * 255.0f, m_LayerList[iRowIndex]->vBotColor.z * 255.0f);
}

void FontStateLayersModel::SetLayerColors(int iId, QColor topColor, QColor botColor)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(m_LayerList[i]->iUNIQUE_ID == iId)
        {
            m_LayerList[i]->vTopColor = glm::vec4(topColor.redF(), topColor.greenF(), topColor.blueF(), 1.0f);
            m_LayerList[i]->vBotColor = glm::vec4(botColor.redF(), botColor.greenF(), botColor.blueF(), 1.0f);
            dataChanged(createIndex(i, COLUMN_DefaultColor), createIndex(i, COLUMN_DefaultColor));
        }
    }
}

float FontStateLayersModel::GetLineHeight()
{
    float fHeight = 0.0f;
    
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(fHeight < m_LayerList[i]->pReference->pTextureFont->height)
            fHeight = m_LayerList[i]->pReference->pTextureFont->height;
    }
    
    return fHeight;
}

float FontStateLayersModel::GetLineAscender()
{
    float fAscender = 0.0f;
    
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(fAscender < abs(m_LayerList[i]->pReference->pTextureFont->ascender))
            fAscender = abs(m_LayerList[i]->pReference->pTextureFont->ascender);
    }
    
    return fAscender;
}

float FontStateLayersModel::GetLineDescender()
{
    float fDescender = 0.0f;
    
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(fDescender < abs(m_LayerList[i]->pReference->pTextureFont->descender))
            fDescender = abs(m_LayerList[i]->pReference->pTextureFont->descender);
    }
    
    return fDescender;
}

float FontStateLayersModel::GetLeftSideNudgeAmt(QString sAvailableTypefaceGlyphs)
{
    float fLeftSideNudgeAmt = 0.0f;

    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        for(int j = 0; j < sAvailableTypefaceGlyphs.count(); ++j)
        {
            // NOTE: Assumes LITTLE ENDIAN
            QString sSingleChar = sAvailableTypefaceGlyphs[j];
            texture_glyph_t *pGlyph = texture_font_get_glyph(m_LayerList[i]->pReference->pTextureFont, sSingleChar.toUtf8().data());

            // Only keep track of negative offset_x's
            if(pGlyph->offset_x < 0 && fLeftSideNudgeAmt < abs(pGlyph->offset_x))
                fLeftSideNudgeAmt = abs(pGlyph->offset_x);
        }
    }

    return fLeftSideNudgeAmt;
}

void FontStateLayersModel::MoveRowUp(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
        return;

    m_LayerList.swap(iIndex, iIndex - 1);
    endMoveRows();
}

void FontStateLayersModel::MoveRowDown(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;

    m_LayerList.swap(iIndex, iIndex + 1);
    endMoveRows();
}

void FontStateLayersModel::SetFontSize(int iSize)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
        m_LayerList[i]->iSize = iSize;
}

void FontStateLayersModel::SetFontStageReference(int iRowIndex, FontTypeface *pStageRef)
{
    m_LayerList[iRowIndex]->pReference = pStageRef;
}

/*virtual*/ int FontStateLayersModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_LayerList.count();
}

/*virtual*/ int FontStateLayersModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant FontStateLayersModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    FontLayer *pLayer = m_LayerList[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Type)
    {
        return Qt::AlignCenter;
    }

    if(role == Qt::BackgroundRole && index.column() == COLUMN_DefaultColor)
    {
        QLinearGradient gradient(0, 0, 0, 25.0f);
        gradient.setColorAt(0.0, QColor(pLayer->vTopColor.x * 255.0f, pLayer->vTopColor.y * 255.0f, pLayer->vTopColor.z * 255.0f));
        gradient.setColorAt(1.0, QColor(pLayer->vBotColor.x * 255.0f, pLayer->vBotColor.y * 255.0f, pLayer->vBotColor.z * 255.0f));

        QBrush bgColorBrush(gradient);
        return QVariant(bgColorBrush);
    }

    if(role == Qt::ForegroundRole && index.column() == COLUMN_DefaultColor)
    {
        // Counting the perceptive luminance - human eye favors green color
        double a = 1 - ( 0.299 * pLayer->vTopColor.x + 0.587 * pLayer->vTopColor.y + 0.114 * pLayer->vTopColor.z)/255;

        if (a < 0.5)
        {
            QBrush bgColorBrush(Qt::black);
            return QVariant(bgColorBrush);
        }
        else
        {
            QBrush bgColorBrush(Qt::white);
            return QVariant(bgColorBrush);
        }
    }
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Type:
            return GetRenderModeString(pLayer->eMode);
        case COLUMN_Thickness:
            return QString::number(GetLayerOutlineThickness(index.row()), 'g', 2);
        case COLUMN_DefaultColor:
            return "";
        }
    }

    return QVariant();
}

/*virtual*/ QVariant FontStateLayersModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

/*virtual*/ bool FontStateLayersModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
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

/*virtual*/ Qt::ItemFlags FontStateLayersModel::flags(const QModelIndex & index) const
{
    // TODO: Make a read only version of all entries
//    if(index.column() == COLUMN_Type)
//        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
//    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
