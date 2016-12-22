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

#include "ItemSprite.h"
#include "WidgetSprite.h"
#include "WidgetUndoCmds.h"
#include "WidgetSpriteUndoCmds.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WidgetSpriteState::WidgetSpriteState(WidgetSprite *pOwner, QList<QAction *> stateActionList, QWidget *parent) : QWidget(parent),
                                                                                                                m_pOwner(pOwner),
                                                                                                                ui(new Ui::WidgetSpriteState),
                                                                                                                m_sName("Unnamed"),
                                                                                                                m_bPlayActive(false),
                                                                                                                m_fElapsedTime(0.0),
                                                                                                                m_bIsBounced(false)
{
    ui->setupUi(this);

    ui->btnAddFrames->setDefaultAction(FindAction(stateActionList, "actionImportFrames"));
    ui->btnRemoveFrame->setDefaultAction(FindAction(stateActionList, "actionRemoveFrames"));
    ui->btnOrderFrameUp->setDefaultAction(FindAction(stateActionList, "actionOrderFrameUpwards"));
    ui->btnOrderFrameDown->setDefaultAction(FindAction(stateActionList, "actionOrderFrameDownwards"));

    m_pSpriteFramesModel = new WidgetSpriteModel(this);

    ui->framesView->setModel(m_pSpriteFramesModel);
    ui->framesView->resize(ui->framesView->size());
    ui->framesView->setItemDelegate(new WidgetSpriteDelegate(m_pOwner->GetItemOwner(), ui->framesView, this));
    QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
    connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));
    
    ui->btnPlay->setDefaultAction(ui->actionPlay);
    ui->btnFirstFrame->setDefaultAction(ui->actionFirstFrame);
    ui->btnLastFrame->setDefaultAction(ui->actionLastFrame);
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
    int iRowIndex = m_pSpriteFramesModel->Add(pFrame);
    ui->framesView->selectRow(iRowIndex);
}

void WidgetSpriteState::RefreshFrame(HyGuiFrame *pFrame)
{
    m_pSpriteFramesModel->RefreshFrame(pFrame);
}

void WidgetSpriteState::RemoveFrame(HyGuiFrame *pFrame)
{
    m_pSpriteFramesModel->Remove(pFrame);
}

QCheckBox *WidgetSpriteState::GetChkBox_Reverse()
{
    return ui->chkReverse;
}

QCheckBox *WidgetSpriteState::GetChkBox_Looping()
{
    return ui->chkLoop;
}

QCheckBox *WidgetSpriteState::GetChkBox_Bounce()
{
    return ui->chkBounce;
}

WidgetSpriteTableView *WidgetSpriteState::GetFrameView()
{
    return static_cast<WidgetSpriteTableView *>(ui->framesView);
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

int WidgetSpriteState::GetNumFrames()
{
    return m_pSpriteFramesModel->rowCount();
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
        frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_vOffset.x() + pSpriteFrame->m_pFrame->GetCrop().left()));
        frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_vOffset.y() + (pSpriteFrame->m_pFrame->GetSize().height() - pSpriteFrame->m_pFrame->GetCrop().bottom())));
        frameObj.insert("checksum", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetChecksum())));
        frameObj.insert("atlasGroupId", QJsonValue(pSpriteFrame->m_pFrame->GetAtlasGroupdId()));

        frameArray.append(frameObj);
    }

    stateObjOut.insert("name", QJsonValue(m_sName));
    stateObjOut.insert("loop", QJsonValue(ui->chkLoop->isChecked()));
    stateObjOut.insert("reverse", QJsonValue(ui->chkReverse->isChecked()));
    stateObjOut.insert("bounce", QJsonValue(ui->chkBounce->isChecked()));
    stateObjOut.insert("frames", QJsonValue(frameArray));
}

void WidgetSpriteState::UpdateTimeStep()
{
    SpriteFrame *pFrame = GetSelectedFrame();
    
    if(m_bPlayActive == false && pFrame != NULL)
        return;

    m_fElapsedTime += IHyTime::GetUpdateStepSeconds();
    while(m_fElapsedTime >= pFrame->m_fDuration)
    {
        bool bBounce = ui->chkBounce->isChecked();
        bool bReverse = ui->chkReverse->isChecked();
        bool bLoop = ui->chkLoop->isChecked();
        int iNumFrames = GetNumFrames();
        
        int iNextRow = ui->framesView->currentIndex().row();
        
        if(bReverse == false)
        {
            m_bIsBounced ? iNextRow-- : iNextRow++;
            
            if(iNextRow < 0)
            {
                m_bIsBounced = false;
                
                if(bLoop)
                    iNextRow = 1;
                else
                    on_actionPlay_triggered();  // Stop playback
            }
            else if(iNextRow >= iNumFrames)
            {
                if(bBounce)
                {
                    iNextRow = iNumFrames - 2;
                    m_bIsBounced = true;
                }
                else if(bLoop)
                    iNextRow = 0;
                else
                    on_actionPlay_triggered();  // Stop playback
            }
        }
        else
        {
            m_bIsBounced ? iNextRow++ : iNextRow--;
            
            if(iNextRow < 0)
            {
                if(bBounce)
                {
                    iNextRow = 1;
                    m_bIsBounced = true;
                }
                else if(bLoop)
                    iNextRow = iNumFrames - 1;
                else
                    on_actionPlay_triggered();  // Stop playback
            }
            else if(iNextRow >= iNumFrames)
            {
                m_bIsBounced = false;
                
                if(bLoop)
                    iNextRow = iNumFrames - 2;
                else
                    on_actionPlay_triggered();  // Stop playback
            }
        }
        
        if(m_bPlayActive)
        {
            ui->framesView->selectRow(iNextRow);
            m_fElapsedTime -= pFrame->m_fDuration;
        }
        else
            break;
    }
}

void WidgetSpriteState::UpdateActions()
{
}

void WidgetSpriteState::on_framesView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
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
        m_fElapsedTime = 0.0f;
    }
    else
        ui->btnPlay->setIcon(QIcon(":/icons16x16/media-play.png"));
}

void WidgetSpriteState::on_btnHz10_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();
    
    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 10.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz20_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();
    
    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 20.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz30_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();
    
    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 30.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz40_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();
    
    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 40.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz50_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();
    
    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 50.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz60_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();
    
    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 60.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_actionFirstFrame_triggered()
{
    ui->framesView->selectRow(0);
}

void WidgetSpriteState::on_actionLastFrame_triggered()
{
    ui->framesView->selectRow(GetNumFrames() - 1);
}

void WidgetSpriteState::on_chkReverse_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();

    QUndoCommand *pCmd = new WidgetUndoCmd_CheckBox<WidgetSpriteState>(this, ui->chkReverse);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_chkLoop_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();

    QUndoCommand *pCmd = new WidgetUndoCmd_CheckBox<WidgetSpriteState>(this, ui->chkLoop);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_chkBounce_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->GetItemOwner();

    QUndoCommand *pCmd = new WidgetUndoCmd_CheckBox<WidgetSpriteState>(this, ui->chkBounce);
    pItemSprite->GetUndoStack()->push(pCmd);
}
