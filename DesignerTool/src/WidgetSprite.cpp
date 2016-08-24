/**************************************************************************
 *	WidgetSprite.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSprite.h"
#include "ui_WidgetSprite.h"

#include "ItemSprite.h"
#include "ItemSpriteCmds.h"
#include "DlgInputName.h"
#include "WidgetAtlasManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>

WidgetSprite::WidgetSprite(ItemSprite *pItemSprite, QWidget *parent) :   QWidget(parent),
                                                                         m_pItemSprite(pItemSprite),
                                                                         ui(new Ui::WidgetSprite),
                                                                         m_pCurSpriteState(NULL)
{
    ui->setupUi(this);

    ui->txtPrefixAndName->setText(m_pItemSprite->GetName(true));
    
    QMenu *pEditMenu = m_pItemSprite->GetEditMenu();
    pEditMenu->addAction(ui->actionAddState);
    pEditMenu->addAction(ui->actionRemoveState);
    pEditMenu->addAction(ui->actionRenameState);
    pEditMenu->addAction(ui->actionOrderStateBackwards);
    pEditMenu->addAction(ui->actionOrderStateForwards);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionImportFrames);
    pEditMenu->addAction(ui->actionRemoveFrames);
    pEditMenu->addAction(ui->actionOrderFrameUpwards);
    pEditMenu->addAction(ui->actionOrderFrameDownwards);

    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    m_StateActionsList.push_back(ui->actionImportFrames);
    m_StateActionsList.push_back(ui->actionRemoveFrames);
    m_StateActionsList.push_back(ui->actionOrderFrameUpwards);
    m_StateActionsList.push_back(ui->actionOrderFrameDownwards);
    
    ui->cmbStates->clear();
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}

ItemSprite *WidgetSprite::Owner()
{
    return m_pItemSprite;
}

void WidgetSprite::LoadAndInit()
{
    // If a .hyspr file exists, parse and initalize with it, otherwise make default empty sprite
    QFile spriteFile(m_pItemSprite->GetAbsPath());
    if(spriteFile.exists())
    {
        if(!spriteFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetSprite::WidgetSprite() could not open ") % m_pItemSprite->GetAbsPath(), LOGTYPE_Error);

        QJsonDocument spriteJsonDoc = QJsonDocument::fromJson(spriteFile.readAll());
        spriteFile.close();

        QJsonArray stateArray = spriteJsonDoc.array();
        for(int i = 0; i < stateArray.size(); ++i)
        {
            QJsonObject stateObj = stateArray[i].toObject();

            m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_AddState(this, m_StateActionsList, ui->cmbStates));
            m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_RenameState(ui->cmbStates, stateObj["name"].toString()));
            
            QJsonArray spriteFrameArray = stateObj["frames"].toArray();
            for(int j = 0; j < spriteFrameArray.size(); ++j)
            {
                QJsonObject spriteFrameObj = spriteFrameArray[j].toObject();
                
                QList<quint32> requestList;
                requestList.append(JSONOBJ_TOINT(spriteFrameObj, "hash"));
                m_pItemSprite->GetAtlasManager().RequestFrames(m_pItemSprite, requestList);

                WidgetSpriteState *pSpriteState = GetCurSpriteState();

                QPointF vOffset(spriteFrameObj["offsetX"].toDouble(), spriteFrameObj["offsetY"].toDouble());
                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_TranslateFrame(pSpriteState->GetFrameView(), j, vOffset));

                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_RotateFrame(pSpriteState->GetFrameView(), j, spriteFrameObj["rotation"].toDouble()));

                QPointF vScale(spriteFrameObj["scaleX"].toDouble(), spriteFrameObj["scaleY"].toDouble());
                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_TranslateFrame(pSpriteState->GetFrameView(), j, vScale));

                m_pItemSprite->GetUndoStack()->push(new ItemSpriteCmd_DurationFrame(pSpriteState->GetFrameView(), j, spriteFrameObj["duration"].toDouble()));
            }
        }
    }
    else
    {
        on_actionAddState_triggered();
        m_pItemSprite->Save();
    }

    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    // I don't believe any 'ItemSpriteCmd_AddState' will leak their dynamically allocated 'm_pSpriteState', since they should become children of 'ui->grpStateLayout'
    m_pItemSprite->GetUndoStack()->clear();

    UpdateActions();
}

void WidgetSprite::AppendActionsForToolBar(QList<QAction *> &actionList)
{
    actionList.append(ui->actionAlignUp);
    actionList.append(ui->actionAlignLeft);
    actionList.append(ui->actionAlignDown);
    actionList.append(ui->actionAlignRight);
    actionList.append(ui->actionAlignCenterHorizontal);
    actionList.append(ui->actionAlignCenterVertical);
    actionList.append(ui->actionApplyToAll);
}

void WidgetSprite::GetSpriteStateInfo(QJsonArray &spriteStateArrayRef)
{
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        QJsonObject spriteState;
        ui->cmbStates->itemData(i).value<WidgetSpriteState *>()->GetStateFrameInfo(spriteState);
        
        spriteStateArrayRef.append(spriteState);
    }
}

WidgetSpriteState *WidgetSprite::GetCurSpriteState()
{
    return ui->cmbStates->currentData().value<WidgetSpriteState *>();
}

QList<HyGuiFrame *> WidgetSprite::GetAllDrawInsts()
{
    QList<HyGuiFrame *> returnList;
    
    for(int i = 0; i < ui->cmbStates->count(); ++i)
        ui->cmbStates->itemData(i).value<WidgetSpriteState *>()->AppendFramesToListRef(returnList);
    
    return returnList;
}

void WidgetSprite::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
    
    WidgetSpriteState *pCurState = GetCurSpriteState();
    bool bFrameIsSelected = pCurState && pCurState->GetNumFrames() > 0 && pCurState->GetSelectedIndex() >= 0;
    
    ui->actionOrderFrameUpwards->setEnabled(pCurState && pCurState->GetSelectedIndex() != 0 && pCurState->GetNumFrames() > 1);
    ui->actionOrderFrameDownwards->setEnabled(pCurState && pCurState->GetSelectedIndex() != pCurState->GetNumFrames() - 1 && pCurState->GetNumFrames() > 1);
    ui->actionRemoveFrames->setEnabled(bFrameIsSelected);
    ui->actionAlignCenterHorizontal->setEnabled(bFrameIsSelected);
    ui->actionAlignCenterVertical->setEnabled(bFrameIsSelected);
    ui->actionAlignUp->setEnabled(bFrameIsSelected);
    ui->actionAlignDown->setEnabled(bFrameIsSelected);
    ui->actionAlignLeft->setEnabled(bFrameIsSelected);
    ui->actionAlignRight->setEnabled(bFrameIsSelected);
}

void WidgetSprite::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_AddState(this, m_StateActionsList, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_RemoveState(ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Sprite State", ui->cmbStates->currentData().value<WidgetSpriteState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new ItemSpriteCmd_RenameState(ui->cmbStates, pDlg->GetName());
        m_pItemSprite->GetUndoStack()->push(pCmd);
    }
}

void WidgetSprite::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_MoveStateBack(ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_MoveStateForward(ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionImportFrames_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_AddFrames(m_pItemSprite);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRemoveFrames_triggered()
{
    WidgetSpriteState *pSpriteState = ui->cmbStates->itemData(ui->cmbStates->currentIndex()).value<WidgetSpriteState *>();
    SpriteFrame *pSpriteFrame = pSpriteState->GetSelectedFrame();

    QUndoCommand *pCmd = new ItemSpriteCmd_DeleteFrame(m_pItemSprite, pSpriteFrame->m_pFrame);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_cmbStates_currentIndexChanged(int index)
{
    WidgetSpriteState *pSpriteState = ui->cmbStates->itemData(index).value<WidgetSpriteState *>();
    if(m_pCurSpriteState == pSpriteState)
        return;

    if(m_pCurSpriteState)
        m_pCurSpriteState->hide();

    ui->grpStateLayout->addWidget(pSpriteState);

#if _DEBUG
    int iDebugTest = ui->grpStateLayout->count();
#endif

    m_pCurSpriteState = pSpriteState;
    m_pCurSpriteState->show();

    UpdateActions();
}

void WidgetSprite::on_actionAlignLeft_triggered()
{
    
}

void WidgetSprite::on_actionAlignRight_triggered()
{
    
}

void WidgetSprite::on_actionAlignUp_triggered()
{
    
}

void WidgetSprite::on_actionAlignDown_triggered()
{
    
}

void WidgetSprite::on_actionAlignCenterVertical_triggered()
{
    
}

void WidgetSprite::on_actionAlignCenterHorizontal_triggered()
{
    
}

void WidgetSprite::on_actionOrderFrameUpwards_triggered()
{
    int iSelectedIndex = GetCurSpriteState()->GetSelectedIndex();

    QUndoCommand *pCmd = new ItemSpriteCmd_OrderFrame(GetCurSpriteState()->GetFrameView(), iSelectedIndex, iSelectedIndex - 1);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionOrderFrameDownwards_triggered()
{
    int iSelectedIndex = GetCurSpriteState()->GetSelectedIndex();

    QUndoCommand *pCmd = new ItemSpriteCmd_OrderFrame(GetCurSpriteState()->GetFrameView(), iSelectedIndex, iSelectedIndex + 1);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}
