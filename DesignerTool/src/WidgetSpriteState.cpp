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

#include <QLineEdit>
#include <QDoubleSpinBox>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteStateTableView::WidgetSpriteStateTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetSpriteStateTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64 + 32 + 64 + 50;
    setColumnWidth(SpriteFramesModel::COLUMN_Frame, iWidth);
    setColumnWidth(SpriteFramesModel::COLUMN_Offset, 64);
    setColumnWidth(SpriteFramesModel::COLUMN_Rotation, 32);
    setColumnWidth(SpriteFramesModel::COLUMN_Scale, 64);
    setColumnWidth(SpriteFramesModel::COLUMN_Duration, 50);
    
    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteStateDelegate::WidgetSpriteStateDelegate(QObject *pParent /*= 0*/) : QStyledItemDelegate(pParent)
{
}

/*virtual*/ QWidget *WidgetSpriteStateDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;
    
    switch(index.column())
    {
    case SpriteFramesModel::COLUMN_Offset:
    case SpriteFramesModel::COLUMN_Scale:
        pReturnWidget = new QLineEdit(pParent);
        static_cast<QLineEdit *>(pReturnWidget)->setValidator(HyGlobal::Vector2dValidator());
        break;
        
    case SpriteFramesModel::COLUMN_Rotation:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(0.0, 360.0);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix("°");
        break;
        
    case SpriteFramesModel::COLUMN_Duration:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix("ms");
        break;
    }
    
    return pReturnWidget;
}

/*virtual*/ void WidgetSpriteStateDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    QString sCurValue = index.model()->data(index, Qt::EditRole).toString();
    
    switch(index.column())
    {
    case SpriteFramesModel::COLUMN_Offset:
    case SpriteFramesModel::COLUMN_Scale:
        static_cast<QLineEdit *>(pEditor)->setText(sCurValue);
        break;
    
    case SpriteFramesModel::COLUMN_Rotation:
    case SpriteFramesModel::COLUMN_Duration:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(sCurValue.toDouble());
        break;
    }
}

/*virtual*/ void WidgetSpriteStateDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    switch(index.column())
    {
    case SpriteFramesModel::COLUMN_Offset:
    case SpriteFramesModel::COLUMN_Scale:
        pModel->setData(index, static_cast<QLineEdit *>(pEditor)->text());
        break;
    
    case SpriteFramesModel::COLUMN_Rotation:
    case SpriteFramesModel::COLUMN_Duration:
        pModel->setData(index, QString::number(static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void WidgetSpriteStateDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void SpriteFramesModel::SetUpdateFreq(uint uiHertz)
{
    float fDur = 1000.0f / static_cast<float>(uiHertz);
    
    for(int i = 0; i < m_FramesList.count(); ++i)
        m_FramesList[i]->m_fDuration = fDur;
    
    QModelIndex startIndex = createIndex(0, COLUMN_Duration);
    QModelIndex endIndex = createIndex(m_FramesList.count() - 1, COLUMN_Duration);
    dataChanged(startIndex, endIndex);
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
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Frame:
            return pFrame->m_pFrame->GetName();
        case COLUMN_Offset:
            return PointToQString(pFrame->m_ptOffset);
        case COLUMN_Rotation:
            return QString::number(pFrame->m_fRotation) % ((role == Qt::DisplayRole) ? "°" : "");
        case COLUMN_Scale:
            return PointToQString(pFrame->m_ptScale);
        case COLUMN_Duration:
            return QString::number(pFrame->m_fDuration, 'g', 4) % ((role == Qt::DisplayRole) ? "ms" : "");
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
        switch(index.column())
        {
        case COLUMN_Offset:
            pFrame->m_ptOffset = StringToPoint(value.toString());
            break;
        case COLUMN_Rotation:
            pFrame->m_fRotation = value.toString().toFloat();
            break;
        case COLUMN_Scale:
            pFrame->m_ptScale = StringToPoint(value.toString());
            break;
        case COLUMN_Duration:
            pFrame->m_fDuration = value.toString().toFloat();
            break;
        }
    }
    
    QVector<int> vRolesChanged;
    vRolesChanged.append(role);
    dataChanged(index, index, vRolesChanged);

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
                                                                                                                m_sName("Unnamed"),
                                                                                                                m_bPlayActive(false),
                                                                                                                m_dElapsedTime(0.0),
                                                                                                                m_bIsBounced(false)
{
    ui->setupUi(this);

    ui->btnAddFrames->setDefaultAction(FindAction(stateActionList, "actionImportFrames"));
    ui->btnRemoveFrame->setDefaultAction(FindAction(stateActionList, "actionRemoveFrames"));
    ui->btnOrderFrameUp->setDefaultAction(FindAction(stateActionList, "actionOrderFrameUpwards"));
    ui->btnOrderFrameDown->setDefaultAction(FindAction(stateActionList, "actionOrderFrameDownwards"));

    m_pSpriteFramesModel = new SpriteFramesModel(this);
    ui->framesView->setModel(m_pSpriteFramesModel);
    ui->framesView->setItemDelegate(new WidgetSpriteStateDelegate(this));
    
    ui->btnPlay->setDefaultAction(ui->actionPlay);
    ui->btnFirstFrame->setDefaultAction(ui->actionFirstFrame);
    ui->btnLastFrame->setDefaultAction(ui->actionLastFrame);

    QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
    connect(pSelModel, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(on_framesView_itemSelectionChanged(QModelIndex,QModelIndex)));
    ui->framesView->resize(ui->framesView->size());
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

void WidgetSpriteState::UpdateTimeStep(double dDelta)
{
    if(m_bPlayActive == false)
        return;
    
    SpriteFrame *pFrame = GetSelectedFrame();
    if(pFrame)
    {
        m_dElapsedTime += dDelta;
        while(m_dElapsedTime >= pFrame->m_fDuration)
        {
            int iCurRow = ui->framesView->currentIndex().row();
            int iNextRow;
            
            if(ui->chkReverse->isChecked())
            {
                if(iCurRow == 0)
                {
                }
                
                if(ui->chkBounce->isChecked() && m_bIsBounced)
                    iNextRow = iCurRow + 1;
                else
                    iNextRow = iCurRow - 1;
            }
            
            ui->framesView->selectRow(iNextRow);
            SpriteFrame *pFrame = GetSelectedFrame();
            
            m_dElapsedTime -= pFrame->m_fDuration;
        }
    }
}

void WidgetSpriteState::on_framesView_itemSelectionChanged(QModelIndex current, QModelIndex previous)
{
    m_pOwner->UpdateActions();
}

void WidgetSpriteState::on_actionPlay_triggered()
{
    m_bPlayActive = !m_bPlayActive;
    
    if(m_bPlayActive)
    {
        ui->btnPlay->setIcon(QIcon(":/icons16x16/media-pause.png"));
        m_bIsBounced = false;
    }
    else
        ui->btnPlay->setIcon(QIcon(":/icons16x16/media-play.png"));
}

void WidgetSpriteState::on_btnHz10_clicked()
{
    m_pSpriteFramesModel->SetUpdateFreq(10);
}

void WidgetSpriteState::on_btnHz20_clicked()
{
    m_pSpriteFramesModel->SetUpdateFreq(20);
}

void WidgetSpriteState::on_btnHz30_clicked()
{
    m_pSpriteFramesModel->SetUpdateFreq(30);
}

void WidgetSpriteState::on_btnHz40_clicked()
{
    m_pSpriteFramesModel->SetUpdateFreq(40);
}

void WidgetSpriteState::on_btnHz50_clicked()
{
    m_pSpriteFramesModel->SetUpdateFreq(50);
}

void WidgetSpriteState::on_btnHz60_clicked()
{
    m_pSpriteFramesModel->SetUpdateFreq(60);
}

