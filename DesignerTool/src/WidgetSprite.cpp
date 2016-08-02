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

    // If a .hyspr file exists, parse and initalize with it, otherwise make default empty sprite
    QFile spriteFile(m_pItemSprite->GetAbsPath());
    if(spriteFile.exists())
    {
        if(!spriteFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetSprite::WidgetSprite() could not open ") % m_pItemSprite->GetAbsPath(), LOGTYPE_Error);

        QJsonDocument spriteJsonDoc = QJsonDocument::fromJson(spriteFile.readAll());
        spriteFile.close();

        QJsonArray spriteStateArray = spriteJsonDoc.array();
        for(int i = 0; i < spriteStateArray.size(); ++i)
        {
            on_actionAddState_triggered();
            
            QJsonArray spriteFrameArray = spriteStateArray[i].toArray();
            for(int j = 0; j < spriteFrameArray.size(); ++j)
            {
                QJsonObject spriteFrameObj = spriteFrameArray[j].toObject();
                
                QList<quint32> requestList;
                requestList.append(JSONOBJ_TOINT(spriteFrameObj, "hash"));
                m_pItemSprite->GetAtlasManager().RequestFrames(m_pItemSprite, requestList);
                
                // TODO: set things like offset, rotation, duration, etc for each frame
            }
        }
    }
    else
    {
        on_actionAddState_triggered();
        Save();
    }

    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    // I don't believe any on_actionAddState_triggered() calls will leak their dynamically allocated 'm_pSpriteState', since they should become children of 'ui->grpStateLayout'
    m_pItemSprite->GetUndoStack()->clear();

    UpdateActions();
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}

void WidgetSprite::Save()
{
    QJsonArray spriteStateArray;
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        QJsonArray spriteFrameArray;
        ui->cmbStates->itemData(i).value<WidgetSpriteState *>()->GetStateFrameInfo(spriteFrameArray);
        
        spriteStateArray.append(spriteFrameArray);
    }

    QJsonDocument settingsDoc(spriteStateArray);

    QFile spriteFile(m_pItemSprite->GetAbsPath());
    if(spriteFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qint64 iBytesWritten = spriteFile.write(settingsDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
            HyGuiLog("Could not write to atlas settings file: " % spriteFile.errorString(), LOGTYPE_Error);
    }
    else
        HyGuiLog("Couldn't open item file " % m_pItemSprite->GetAbsPath() % ": " % spriteFile.errorString(), LOGTYPE_Error);

    spriteFile.close();
}

WidgetSpriteState *WidgetSprite::GetCurSpriteState()
{
    return ui->cmbStates->currentData().value<WidgetSpriteState *>();
}

void WidgetSprite::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_AddState(m_pItemSprite, m_StateActionsList, ui->cmbStates);
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
    HyGuiFrame *pSelectredFrame = pSpriteState->SelectedFrame();

    QUndoCommand *pCmd = new ItemSpriteCmd_DeleteFrame(m_pItemSprite, pSelectredFrame);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionOrderFrameUpwards_triggered()
{
    
}

void WidgetSprite::on_actionOrderFrameDownwards_triggered()
{
    
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

void WidgetSprite::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
}
