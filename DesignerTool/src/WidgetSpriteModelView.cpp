/**************************************************************************
 *	WidgetSpriteModelView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSpriteModelView.h"

#include "HyGlobal.h"
#include "ItemSprite.h"
#include "ItemSpriteCmds.h"

#include <QLineEdit>
#include <QDoubleSpinBox>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteTableView::WidgetSpriteTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetSpriteTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64 + 64 + 64;
    setColumnWidth(WidgetSpriteModel::COLUMN_Frame, iWidth);
    setColumnWidth(WidgetSpriteModel::COLUMN_OffsetX, 64);
    setColumnWidth(WidgetSpriteModel::COLUMN_OffsetY, 64);
    setColumnWidth(WidgetSpriteModel::COLUMN_Duration, 64);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteDelegate::WidgetSpriteDelegate(ItemSprite *pItemSprite, WidgetSpriteTableView *pTableView, QObject *pParent /*= 0*/) :  QStyledItemDelegate(pParent),
                                                                                                                                    m_pItemSprite(pItemSprite),
                                                                                                                                    m_pTableView(pTableView)
{
}

/*virtual*/ QWidget *WidgetSpriteDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    switch(index.column())
    {
    case WidgetSpriteModel::COLUMN_OffsetX:
        pReturnWidget = new QSpinBox(pParent);
        static_cast<QSpinBox *>(pReturnWidget)->setPrefix("X:");
        static_cast<QSpinBox *>(pReturnWidget)->setRange(-4096, 4096);
        break;
        
    case WidgetSpriteModel::COLUMN_OffsetY:
        pReturnWidget = new QSpinBox(pParent);
        static_cast<QSpinBox *>(pReturnWidget)->setPrefix("Y:");
        static_cast<QSpinBox *>(pReturnWidget)->setRange(-4096, 4096);
        break;

    case WidgetSpriteModel::COLUMN_Duration:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setSingleStep(0.001);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix("sec");
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void WidgetSpriteDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    QString sCurValue = index.model()->data(index, Qt::EditRole).toString();

    switch(index.column())
    {
    case WidgetSpriteModel::COLUMN_OffsetX:
        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
        break;
        
    case WidgetSpriteModel::COLUMN_OffsetY:
        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
        break;

    case WidgetSpriteModel::COLUMN_Duration:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(sCurValue.toDouble());
        break;
    }
}

/*virtual*/ void WidgetSpriteDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    switch(index.column())
    {
    case WidgetSpriteModel::COLUMN_OffsetX:
    {
        WidgetSpriteModel *pSpriteModel = static_cast<WidgetSpriteModel *>(pModel);
        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
        vOffset.setX(static_cast<QSpinBox *>(pEditor)->value());

        m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_OffsetFrame(m_pTableView, index.row(), vOffset));
        break;
    }

    case WidgetSpriteModel::COLUMN_OffsetY:
    {
        WidgetSpriteModel *pSpriteModel = static_cast<WidgetSpriteModel *>(pModel);
        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
        vOffset.setY(static_cast<QSpinBox *>(pEditor)->value());

        m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_OffsetFrame(m_pTableView, index.row(), vOffset));
        break;
    }
    

    case WidgetSpriteModel::COLUMN_Duration:
        m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_DurationFrame(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void WidgetSpriteDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteModel::WidgetSpriteModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void WidgetSpriteModel::Add(HyGuiFrame *pFrame)
{
    SpriteFrame *pFrameToInsert = NULL;

    // See if this frame has been recently removed, and re-add if possible. Otherwise, create a new Frame
    QMap<quint32, SpriteFrame *>::iterator iter = m_RemovedFrameMap.find(pFrame->GetHash());
    if(iter == m_RemovedFrameMap.end())
        pFrameToInsert = new SpriteFrame(pFrame, m_FramesList.count());
    else
    {
        pFrameToInsert = iter.value();
        m_RemovedFrameMap.remove(pFrame->GetHash());
    }

    beginInsertRows(QModelIndex(), pFrameToInsert->m_iRowIndex, pFrameToInsert->m_iRowIndex);
    m_FramesList.insert(pFrameToInsert->m_iRowIndex, pFrameToInsert);
    endInsertRows();
}

void WidgetSpriteModel::Remove(HyGuiFrame *pFrame)
{
    for(int i = 0; i < m_FramesList.count(); ++i)
    {
        if(m_FramesList[i]->m_pFrame == pFrame)
        {
            m_RemovedFrameMap[pFrame->GetHash()] = m_FramesList[i];

            beginRemoveRows(QModelIndex(), i, i);
            m_FramesList.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

void WidgetSpriteModel::MoveRowUp(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
        return;

    m_FramesList.swap(iIndex, iIndex - 1);
    endMoveRows();
}

void WidgetSpriteModel::MoveRowDown(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;

    m_FramesList.swap(iIndex, iIndex + 1);
    endMoveRows();
}

// iIndex of -1 will apply to all
void WidgetSpriteModel::OffsetFrame(int iIndex, QPoint vOffset)
{
    if(iIndex == -1)
    {
        for(int i = 0; i < m_FramesList.count(); ++i)
            m_FramesList[i]->m_vOffset = vOffset;

        dataChanged(createIndex(0, COLUMN_OffsetX), createIndex(m_FramesList.count() - 1, COLUMN_OffsetY));
    }
    else
    {
        m_FramesList[iIndex]->m_vOffset = vOffset;
        dataChanged(createIndex(iIndex, COLUMN_OffsetX), createIndex(iIndex, COLUMN_OffsetY));
    }
}

// iIndex of -1 will apply to all
void WidgetSpriteModel::DurationFrame(int iIndex, float fDuration)
{
    if(iIndex == -1)
    {
        for(int i = 0; i < m_FramesList.count(); ++i)
            m_FramesList[i]->m_fDuration = fDuration;

        dataChanged(createIndex(0, COLUMN_Duration), createIndex(m_FramesList.count() - 1, COLUMN_Duration));
    }
    else
    {
        m_FramesList[iIndex]->m_fDuration = fDuration;
        dataChanged(createIndex(iIndex, COLUMN_Duration), createIndex(iIndex, COLUMN_Duration));
    }
}

SpriteFrame *WidgetSpriteModel::GetFrameAt(int iIndex)
{
    if(iIndex < 0)
        return NULL;

    return m_FramesList[iIndex];
}

/*virtual*/ int WidgetSpriteModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_FramesList.count();
}

/*virtual*/ int WidgetSpriteModel::columnCount(const QModelIndex & /*parent*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant WidgetSpriteModel::data(const QModelIndex &index, int role) const
{
    SpriteFrame *pFrame = m_FramesList[index.row()];

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Frame:
            return pFrame->m_pFrame->GetName();
        case COLUMN_OffsetX:
            return QString::number(pFrame->m_vOffset.x());
        case COLUMN_OffsetY:
            return QString::number(pFrame->m_vOffset.y());
        case COLUMN_Duration:
            return QString::number(pFrame->m_fDuration, 'g', 5) % ((role == Qt::DisplayRole) ? "sec" : "");
        }
    }

    return QVariant();
}

/*virtual*/ QVariant WidgetSpriteModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(iIndex)
            {
            case COLUMN_Frame:
                return QString("Frame");
            case COLUMN_OffsetX:
                return QString("X Offset");
            case COLUMN_OffsetY:
                return QString("Y Offset");
            case COLUMN_Duration:
                return QString("Duration");
            }
        }
        else
            return QString::number(iIndex);
    }

    return QVariant();
}

/*virtual*/ bool WidgetSpriteModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    HyGuiLog("SpriteFramesModel::setData was invoked", LOGTYPE_Error);

    SpriteFrame *pFrame = m_FramesList[index.row()];

    if(role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_OffsetX:
            pFrame->m_vOffset.setX(value.toInt());
            break;
        case COLUMN_OffsetY:
            pFrame->m_vOffset.setY(value.toInt());
            break;
        case COLUMN_Duration:
            pFrame->m_fDuration = value.toFloat();
            break;
        }
    }

    QVector<int> vRolesChanged;
    vRolesChanged.append(role);
    dataChanged(index, index, vRolesChanged);

    return true;
}

/*virtual*/ Qt::ItemFlags WidgetSpriteModel::flags(const QModelIndex &index) const
{
    if(index.column() == COLUMN_Frame)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
