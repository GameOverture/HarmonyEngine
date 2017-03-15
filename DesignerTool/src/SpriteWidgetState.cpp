/**************************************************************************
 *	WidgetSpriteState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "SpriteWidgetState.h"
#include "ui_SpriteWidgetState.h"

#include "SpriteItem.h"
#include "SpriteWidget.h"
#include "UndoCmds.h"
#include "SpriteUndoCmds.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SpriteWidgetState::SpriteWidgetState(SpriteWidget *pOwner, QList<QAction *> stateActionList, QWidget *parent) : QWidget(parent),
                                                                                                                m_pOwner(pOwner),
                                                                                                                ui(new Ui::SpriteWidgetState),
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

    m_pSpriteFramesModel = new SpriteTableModel(this);

    ui->framesView->setModel(m_pSpriteFramesModel);
    ui->framesView->resize(ui->framesView->size());
    ui->framesView->setItemDelegate(new WidgetSpriteDelegate(m_pOwner->GetData(), ui->framesView, this));
    QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
    connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));
    
    ui->btnPlay->setDefaultAction(ui->actionPlay);
    ui->btnFirstFrame->setDefaultAction(ui->actionFirstFrame);
    ui->btnLastFrame->setDefaultAction(ui->actionLastFrame);
}

SpriteWidgetState::~SpriteWidgetState()
{
    ui->framesView->blockSignals(true);
    delete ui;
}

QString SpriteWidgetState::GetName()
{
    return m_sName;
}

void SpriteWidgetState::SetName(QString sNewName)
{
    m_sName = sNewName;
}

void SpriteWidgetState::InsertFrame(AtlasFrame *pFrame)
{
    int iRowIndex = m_pSpriteFramesModel->Add(pFrame);
    ui->framesView->selectRow(iRowIndex);
}

void SpriteWidgetState::RefreshFrame(AtlasFrame *pFrame)
{
    m_pSpriteFramesModel->RefreshFrame(pFrame);
}

void SpriteWidgetState::RemoveFrame(AtlasFrame *pFrame)
{
    m_pSpriteFramesModel->Remove(pFrame);
}

QCheckBox *SpriteWidgetState::GetChkBox_Reverse()
{
    return ui->chkReverse;
}

QCheckBox *SpriteWidgetState::GetChkBox_Looping()
{
    return ui->chkLoop;
}

QCheckBox *SpriteWidgetState::GetChkBox_Bounce()
{
    return ui->chkBounce;
}

SpriteTableView *SpriteWidgetState::GetFrameView()
{
    return static_cast<SpriteTableView *>(ui->framesView);
}

SpriteFrame *SpriteWidgetState::GetSelectedFrame()
{
    if(m_pSpriteFramesModel->rowCount() == 0)
        return NULL;
    
    SpriteFrame *pSpriteFrame = m_pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row());
    return pSpriteFrame;
}

int SpriteWidgetState::GetSelectedIndex()
{
    return ui->framesView->currentIndex().row();
}

int SpriteWidgetState::GetNumFrames()
{
    return m_pSpriteFramesModel->rowCount();
}

void SpriteWidgetState::AppendFramesToListRef(QList<AtlasFrame *> &drawInstListRef)
{
    for(int i = 0; i < GetNumFrames(); ++i)
        drawInstListRef.append(m_pSpriteFramesModel->GetFrameAt(i)->m_pFrame);
}

void SpriteWidgetState::GetStateFrameInfo(QJsonObject &stateObjOut)
{
    QJsonArray frameArray;
    float fTotalDuration = 0.0f;
    for(int i = 0; i < GetNumFrames(); ++i)
    {
        SpriteFrame *pSpriteFrame = m_pSpriteFramesModel->GetFrameAt(i);

        QJsonObject frameObj;
        frameObj.insert("duration", QJsonValue(pSpriteFrame->m_fDuration));
        fTotalDuration += pSpriteFrame->m_fDuration;
        frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_vOffset.x() + pSpriteFrame->m_pFrame->GetCrop().left()));
        frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_vOffset.y() + (pSpriteFrame->m_pFrame->GetSize().height() - pSpriteFrame->m_pFrame->GetCrop().bottom())));
        frameObj.insert("checksum", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetChecksum())));
        frameObj.insert("atlasIndex", QJsonValue(pSpriteFrame->m_pFrame->GetTextureIndex()));

        frameArray.append(frameObj);
    }

    stateObjOut.insert("name", QJsonValue(m_sName));
    stateObjOut.insert("loop", QJsonValue(ui->chkLoop->isChecked()));
    stateObjOut.insert("reverse", QJsonValue(ui->chkReverse->isChecked()));
    stateObjOut.insert("bounce", QJsonValue(ui->chkBounce->isChecked()));
    stateObjOut.insert("duration", QJsonValue(fTotalDuration));
    stateObjOut.insert("frames", QJsonValue(frameArray));
}

void SpriteWidgetState::UpdateTimeStep()
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

void SpriteWidgetState::UpdateActions()
{
}

void SpriteWidgetState::on_framesView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
{
    m_pOwner->UpdateActions();
}

void SpriteWidgetState::on_actionPlay_triggered()
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

void SpriteWidgetState::on_btnHz10_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();
    
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 10.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_btnHz20_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();
    
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 20.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_btnHz30_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();
    
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 30.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_btnHz40_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();
    
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 40.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_btnHz50_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();
    
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 50.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_btnHz60_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();
    
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 60.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_actionFirstFrame_triggered()
{
    ui->framesView->selectRow(0);
}

void SpriteWidgetState::on_actionLastFrame_triggered()
{
    ui->framesView->selectRow(GetNumFrames() - 1);
}

void SpriteWidgetState::on_chkReverse_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();

    QUndoCommand *pCmd = new UndoCmd_CheckBox<SpriteWidgetState>(this, ui->chkReverse);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_chkLoop_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();

    QUndoCommand *pCmd = new UndoCmd_CheckBox<SpriteWidgetState>(this, ui->chkLoop);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidgetState::on_chkBounce_clicked()
{
    SpriteItem *pItemSprite = m_pOwner->GetData();

    QUndoCommand *pCmd = new UndoCmd_CheckBox<SpriteWidgetState>(this, ui->chkBounce);
    pItemSprite->GetUndoStack()->push(pCmd);
}
