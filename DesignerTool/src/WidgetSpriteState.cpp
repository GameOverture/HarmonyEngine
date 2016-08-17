/**************************************************************************
 *	WidgetSpriteState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSpriteState.h"
#include "ui_WidgetSpriteState.h"

#include "HyGlobal.h"
#include "ItemSprite.h"
#include "WidgetSprite.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WidgetSpriteStateTableView::WidgetSpriteStateTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetSpriteStateTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64 + 32 + 64 + 48;
    setColumnWidth(SpriteFramesModel::COLUMN_Frame, iWidth);
    setColumnWidth(SpriteFramesModel::COLUMN_Offset, 64);
    setColumnWidth(SpriteFramesModel::COLUMN_Rotation, 32);
    setColumnWidth(SpriteFramesModel::COLUMN_Scale, 64);
    setColumnWidth(SpriteFramesModel::COLUMN_Duration, 48);
    
    QTableView::resizeEvent(pResizeEvent);
}

SpriteFramesModel::SpriteFramesModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void SpriteFramesModel::Add(HyGuiFrame *pFrame)
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

void SpriteFramesModel::Remove(HyGuiFrame *pFrame)
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

void SpriteFramesModel::MoveRowUp(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
        return;
    
    m_FramesList.swap(iIndex, iIndex - 1);
    endMoveRows();
}

void SpriteFramesModel::MoveRowDown(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;
    
    m_FramesList.swap(iIndex, iIndex + 1);
    endMoveRows();
}

SpriteFrame *SpriteFramesModel::GetFrameAt(int iIndex)
{
    if(iIndex < 0)
        return NULL;

    return m_FramesList[iIndex];
}

/*virtual*/ int SpriteFramesModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_FramesList.count();
}

/*virtual*/ int SpriteFramesModel::columnCount(const QModelIndex & /*parent*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant SpriteFramesModel::data(const QModelIndex &index, int role) const
{
    SpriteFrame *pFrame = m_FramesList[index.row()];
    
    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case COLUMN_Frame:
            return pFrame->m_pFrame->GetName();
        case COLUMN_Offset:
            return PointToQString(pFrame->m_ptOffset);
        case COLUMN_Rotation:
            return QString::number(pFrame->m_fRotation);
        case COLUMN_Scale:
            return PointToQString(pFrame->m_ptScale);
        case COLUMN_Duration:
            return QString::number(pFrame->m_fDuration);
        }
    }
    
    return QVariant();
}

/*virtual*/ QVariant SpriteFramesModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(iIndex)
            {
            case COLUMN_Frame:
                return QString("Frame");
            case COLUMN_Offset:
                return QString("Offset");
            case COLUMN_Rotation:
                return QString("Rot");
            case COLUMN_Scale:
                return QString("Scale");
            case COLUMN_Duration:
                return QString("Dur");
            }
        }
        else
            return QString::number(iIndex);
    }
    
    return QVariant();
}

/*virtual*/ bool SpriteFramesModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    SpriteFrame *pFrame = m_FramesList[index.row()];
    
    if(role == Qt::EditRole)
    {
//        switch(index.column())
//        {
//        case COLUMN_Offset:
//            return PointToQString(pFrame->m_ptOffset);
//        case COLUMN_Rotation:
//            return QString::number(pFrame->m_fRotation);
//        case COLUMN_Scale:
//            return PointToQString(pFrame->m_ptScale);
//        case COLUMN_Duration:
//            return QString::number(pFrame->m_fDuration);
//        }

        //emit editCompleted( result );
    }

    return true;
}

/*virtual*/ Qt::ItemFlags SpriteFramesModel::flags(const QModelIndex &index) const
{
    if(index.column() == COLUMN_Frame)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteState::WidgetSpriteState(WidgetSprite *pOwner, QList<QAction *> stateActionList, QWidget *parent) : QWidget(parent),
                                                                                                                m_pOwner(pOwner),
                                                                                                                ui(new Ui::WidgetSpriteState),
                                                                                                                m_sName("Unnamed")
{
    ui->setupUi(this);

    ui->btnAddFrames->setDefaultAction(FindAction(stateActionList, "actionImportFrames"));
    ui->btnRemoveFrame->setDefaultAction(FindAction(stateActionList, "actionRemoveFrames"));
    ui->btnOrderFrameUp->setDefaultAction(FindAction(stateActionList, "actionOrderFrameUpwards"));
    ui->btnOrderFrameDown->setDefaultAction(FindAction(stateActionList, "actionOrderFrameDownwards"));
    
    //    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Frame, 100);
    //    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Offset, 64);
    //    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Rotation, 32);
    //    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Scale, 64);
    //    ui->frames->setColumnWidth(SpriteFrame::COLUMN_Duration, 32);
    //    ui->frames->setMinimumWidth(100+64+32+64+32);

    m_pSpriteFramesModel = new SpriteFramesModel(this);
    ui->framesView->setModel(m_pSpriteFramesModel);
//    ui->framesView->setColumnWidth(SpriteFramesModel::COLUMN_Frame, 100);
//    ui->framesView->setColumnWidth(SpriteFramesModel::COLUMN_Offset, 64);
//    ui->framesView->setColumnWidth(SpriteFramesModel::COLUMN_Rotation, 32);
//    ui->framesView->setColumnWidth(SpriteFramesModel::COLUMN_Scale, 64);
//    ui->framesView->setColumnWidth(SpriteFramesModel::COLUMN_Duration, 48);

    QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
    connect(pSelModel, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(on_framesView_itemSelectionChanged(QModelIndex,QModelIndex)));
    connect(ui->framesView, SIGNAL(resizeEvent(QResizeEvent *)), this, SLOT(on_framesView_resizeEvent(QResizeEvent *)));
}

WidgetSpriteState::~WidgetSpriteState()
{
    ui->framesView->blockSignals(true);
    delete ui;
}

QString WidgetSpriteState::GetName()
{
    return m_sName;
}

void WidgetSpriteState::SetName(QString sNewName)
{
    m_sName = sNewName;
}

void WidgetSpriteState::InsertFrame(HyGuiFrame *pFrame)
{
    m_pSpriteFramesModel->Add(pFrame);
    ui->framesView->selectRow(0);
}

void WidgetSpriteState::RemoveFrame(HyGuiFrame *pFrame)
{
    m_pSpriteFramesModel->Remove(pFrame);
}

SpriteFrame *WidgetSpriteState::GetSelectedFrame()
{
    if(m_pSpriteFramesModel->rowCount() == 0)
        return NULL;
    
    SpriteFrame *pSpriteFrame = m_pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row());
    return pSpriteFrame;
}

int WidgetSpriteState::GetSelectedIndex()
{
    return ui->framesView->currentIndex().row();
}

void WidgetSpriteState::SelectIndex(int iIndex)
{
    ui->framesView->selectRow(iIndex);
}

int WidgetSpriteState::GetNumFrames()
{
    return m_pSpriteFramesModel->rowCount();
}

void WidgetSpriteState::OrderFrame(int iIndex, int iOffset)
{
    while(iOffset > 0)
    {
        m_pSpriteFramesModel->MoveRowDown(iIndex);
        iOffset--;
    }
    
    while(iOffset < 0)
    {
        m_pSpriteFramesModel->MoveRowUp(iIndex);
        iOffset++;
    }
}

void WidgetSpriteState::AppendFramesToListRef(QList<HyGuiFrame *> &drawInstListRef)
{
    for(int i = 0; i < GetNumFrames(); ++i)
        drawInstListRef.append(m_pSpriteFramesModel->GetFrameAt(i)->m_pFrame);
}

void WidgetSpriteState::GetStateFrameInfo(QJsonObject &stateObjOut)
{
    QJsonArray frameArray;
    for(int i = 0; i < GetNumFrames(); ++i)
    {
        SpriteFrame *pSpriteFrame = m_pSpriteFramesModel->GetFrameAt(i);

        QJsonObject frameObj;
        frameObj.insert("duration", QJsonValue(pSpriteFrame->m_fDuration));
        frameObj.insert("rotation", QJsonValue(pSpriteFrame->m_fRotation));
        frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_ptOffset.x()));
        frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_ptOffset.y()));
        frameObj.insert("scaleX", QJsonValue(pSpriteFrame->m_ptScale.x()));
        frameObj.insert("scaleY", QJsonValue(pSpriteFrame->m_ptScale.y()));
        frameObj.insert("hash", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetHash())));
        frameObj.insert("atlasGroupId", QJsonValue(pSpriteFrame->m_pFrame->GetAtlasGroupdId()));

        frameArray.append(frameObj);
    }

    stateObjOut.insert("name", QJsonValue(m_sName));
    stateObjOut.insert("frames", QJsonValue(frameArray));
}

void WidgetSpriteState::on_framesView_itemSelectionChanged(QModelIndex current, QModelIndex previous)
{
    m_pOwner->UpdateActions();
}

