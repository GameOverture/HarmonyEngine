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
#include "ItemSpriteCmds.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    m_pSpriteFramesModel = new WidgetSpriteModel(this);

    QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
    connect(pSelModel, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(on_framesView_itemSelectionChanged(QModelIndex,QModelIndex)));
    ui->framesView->setModel(m_pSpriteFramesModel);
    ui->framesView->resize(ui->framesView->size());
    ui->framesView->setItemDelegate(new WidgetSpriteDelegate(m_pOwner->Owner(), ui->framesView, this));
    
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
    m_pSpriteFramesModel->Add(pFrame);
    ui->framesView->selectRow(0);
}

void WidgetSpriteState::RemoveFrame(HyGuiFrame *pFrame)
{
    m_pSpriteFramesModel->Remove(pFrame);
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
        frameObj.insert("rotation", QJsonValue(pSpriteFrame->m_fRotation));
        frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_vOffset.x()));
        frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_vOffset.y()));
        frameObj.insert("scaleX", QJsonValue(pSpriteFrame->m_vScale.x()));
        frameObj.insert("scaleY", QJsonValue(pSpriteFrame->m_vScale.y()));
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
    ItemSprite *pItemSprite = m_pOwner->Owner();
    
    QUndoCommand *pCmd = new ItemSpriteCmd_DurationFrame(ui->framesView, -1, 1000.0f / 10.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz20_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->Owner();
    
    QUndoCommand *pCmd = new ItemSpriteCmd_DurationFrame(ui->framesView, -1, 1000.0f / 20.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz30_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->Owner();
    
    QUndoCommand *pCmd = new ItemSpriteCmd_DurationFrame(ui->framesView, -1, 1000.0f / 30.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz40_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->Owner();
    
    QUndoCommand *pCmd = new ItemSpriteCmd_DurationFrame(ui->framesView, -1, 1000.0f / 40.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz50_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->Owner();
    
    QUndoCommand *pCmd = new ItemSpriteCmd_DurationFrame(ui->framesView, -1, 1000.0f / 50.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSpriteState::on_btnHz60_clicked()
{
    ItemSprite *pItemSprite = m_pOwner->Owner();
    
    QUndoCommand *pCmd = new ItemSpriteCmd_DurationFrame(ui->framesView, -1, 1000.0f / 60.0f);
    pItemSprite->GetUndoStack()->push(pCmd);
}
