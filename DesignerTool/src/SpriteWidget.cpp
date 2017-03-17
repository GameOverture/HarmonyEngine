/**************************************************************************
 *	WidgetSprite.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "SpriteWidget.h"
#include "ui_SpriteWidget.h"

#include "SpriteItem.h"
#include "UndoCmds.h"
#include "SpriteUndoCmds.h"
#include "DlgInputName.h"
#include "AtlasesWidget.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>

SpriteWidget::SpriteWidget(SpriteItem *pItemSprite, QWidget *parent) :   QWidget(parent),
                                                                         m_pItemSprite(pItemSprite),
                                                                         ui(new Ui::SpriteWidget),
                                                                         m_pCurSpriteState(NULL)
{
    ui->setupUi(this);

    ui->txtPrefixAndName->setText(m_pItemSprite->GetName(true));
    
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

SpriteWidget::~SpriteWidget()
{
    delete ui;
}

// This function exists because below cannot be placed in constructor due to RequestFrames() trying to access ItemSprite::m_pWidget (aka this) before variable is assigned
void SpriteWidget::Load()
{
    // If item's init value is defined, parse and initalize with it, otherwise make default empty sprite
    if(m_pItemSprite->GetInitValue().type() != QJsonValue::Null)
    {
        QJsonArray stateArray = m_pItemSprite->GetInitValue().toArray();
        for(int i = 0; i < stateArray.size(); ++i)
        {
            QJsonObject stateObj = stateArray[i].toObject();

            m_pItemSprite->GetUndoStack()->push(new UndoCmd_AddState<SpriteWidget, SpriteWidgetState>("Add Sprite State", this, m_StateActionsList, ui->cmbStates));
            m_pItemSprite->GetUndoStack()->push(new UndoCmd_RenameState<SpriteWidgetState>("Rename Sprite State", ui->cmbStates, stateObj["name"].toString()));

            SpriteWidgetState *pSpriteState = GetCurSpriteState();

            pSpriteState->GetChkBox_Reverse()->setChecked(stateObj["reverse"].toBool());
            pSpriteState->GetChkBox_Looping()->setChecked(stateObj["loop"].toBool());
            pSpriteState->GetChkBox_Bounce()->setChecked(stateObj["bounce"].toBool());

            QJsonArray spriteFrameArray = stateObj["frames"].toArray();
            for(int j = 0; j < spriteFrameArray.size(); ++j)
            {
                QJsonObject spriteFrameObj = spriteFrameArray[j].toObject();

                QList<quint32> requestList;
                requestList.append(JSONOBJ_TOINT(spriteFrameObj, "checksum"));
                QList<AtlasFrame *> pRequestedList = m_pItemSprite->GetItemProject()->GetAtlasesData().RequestFrames(m_pItemSprite, requestList);

                QPoint vOffset(spriteFrameObj["offsetX"].toInt() - pRequestedList[0]->GetCrop().left(),
                               spriteFrameObj["offsetY"].toInt() - (pRequestedList[0]->GetSize().height() - pRequestedList[0]->GetCrop().bottom()));
                m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetFrame(pSpriteState->GetFrameView(), j, vOffset));
                m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_DurationFrame(pSpriteState->GetFrameView(), j, spriteFrameObj["duration"].toDouble()));
            }
        }
    }
    else
    {
        on_actionAddState_triggered();

        //m_pItemSprite->Save();
    }

    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    // I don't believe any 'ItemSpriteCmd_AddState' will leak their dynamically allocated 'm_pSpriteState', since they should become children of 'ui->grpStateLayout'
    m_pItemSprite->GetUndoStack()->clear();

    UpdateActions();
}

SpriteItem *SpriteWidget::GetData()
{
    return m_pItemSprite;
}

void SpriteWidget::OnGiveMenuActions(QMenu *pMenu)
{
    pMenu->addAction(ui->actionAddState);
    pMenu->addAction(ui->actionRemoveState);
    pMenu->addAction(ui->actionRenameState);
    pMenu->addAction(ui->actionOrderStateBackwards);
    pMenu->addAction(ui->actionOrderStateForwards);
    pMenu->addSeparator();
    pMenu->addAction(ui->actionImportFrames);
    pMenu->addAction(ui->actionRemoveFrames);
    pMenu->addAction(ui->actionOrderFrameUpwards);
    pMenu->addAction(ui->actionOrderFrameDownwards);
    pMenu->addSeparator();
    pMenu->addAction(ui->actionAlignUp);
    pMenu->addAction(ui->actionAlignUp);
    pMenu->addAction(ui->actionAlignLeft);
    pMenu->addAction(ui->actionAlignDown);
    pMenu->addAction(ui->actionAlignRight);
    pMenu->addAction(ui->actionAlignCenterHorizontal);
    pMenu->addAction(ui->actionAlignCenterVertical);
    pMenu->addAction(ui->actionApplyToAll);
}

void SpriteWidget::GetSaveInfo(QJsonArray &spriteStateArrayRef)
{
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        QJsonObject spriteState;
        ui->cmbStates->itemData(i).value<SpriteWidgetState *>()->GetStateFrameInfo(spriteState);
        
        spriteStateArrayRef.append(spriteState);
    }
}

SpriteWidgetState *SpriteWidget::GetCurSpriteState()
{
    return ui->cmbStates->currentData().value<SpriteWidgetState *>();
}

void SpriteWidget::Relink(AtlasFrame *pFrame)
{
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        SpriteWidgetState *pSpriteState = ui->cmbStates->itemData(i).value<SpriteWidgetState *>();
        pSpriteState->RefreshFrame(pFrame);
    }
}

QList<AtlasFrame *> SpriteWidget::GetAllDrawInsts()
{
    QList<AtlasFrame *> returnList;
    
    for(int i = 0; i < ui->cmbStates->count(); ++i)
        ui->cmbStates->itemData(i).value<SpriteWidgetState *>()->AppendFramesToListRef(returnList);
    
    return returnList;
}

void SpriteWidget::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
    
    SpriteWidgetState *pCurState = GetCurSpriteState();
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

void SpriteWidget::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_AddState<SpriteWidget, SpriteWidgetState>("Add Sprite State", this, m_StateActionsList, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void SpriteWidget::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_RemoveState<SpriteWidget, SpriteWidgetState>("Remove Sprite State", this, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void SpriteWidget::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Sprite State", ui->cmbStates->currentData().value<SpriteWidgetState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new UndoCmd_RenameState<SpriteWidgetState>("Rename Sprite State", ui->cmbStates, pDlg->GetName());
        m_pItemSprite->GetUndoStack()->push(pCmd);
    }
}

void SpriteWidget::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateBack<SpriteWidget, SpriteWidgetState>("Shift Sprite State Index <-", this, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateForward<SpriteWidget, SpriteWidgetState>("Shift Sprite State Index ->", this, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionImportFrames_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_AddFrames<SpriteWidget>("Add Frames", this);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionRemoveFrames_triggered()
{
    SpriteWidgetState *pSpriteState = ui->cmbStates->itemData(ui->cmbStates->currentIndex()).value<SpriteWidgetState *>();
    SpriteFrame *pSpriteFrame = pSpriteState->GetSelectedFrame();

    QUndoCommand *pCmd = new UndoCmd_DeleteFrame<SpriteWidget>("Remove Frame", this, pSpriteFrame->m_pFrame);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_cmbStates_currentIndexChanged(int index)
{
    SpriteWidgetState *pSpriteState = ui->cmbStates->itemData(index).value<SpriteWidgetState *>();
    if(m_pCurSpriteState == pSpriteState)
        return;

    if(m_pCurSpriteState)
        m_pCurSpriteState->hide();

    ui->grpStateLayout->addWidget(pSpriteState);

    m_pCurSpriteState = pSpriteState;
    m_pCurSpriteState->show();

    UpdateActions();
}

void SpriteWidget::on_actionAlignLeft_triggered()
{
    SpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    SpriteTableModel *pSpriteFramesModel = static_cast<SpriteTableModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(0.0f);

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(0.0f);
    m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignRight_triggered()
{
    SpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    SpriteTableModel *pSpriteFramesModel = static_cast<SpriteTableModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -1);

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().width() * -1);
    m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignUp_triggered()
{
    SpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    SpriteTableModel *pSpriteFramesModel = static_cast<SpriteTableModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -1);

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().height() * -1);
    m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignDown_triggered()
{
    SpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    SpriteTableModel *pSpriteFramesModel = static_cast<SpriteTableModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(0.0f);

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(0.0f);
    m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignCenterVertical_triggered()
{
    SpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    SpriteTableModel *pSpriteFramesModel = static_cast<SpriteTableModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -0.5f);

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().height() * -0.5f);
    m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignCenterHorizontal_triggered()
{
    SpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    SpriteTableModel *pSpriteFramesModel = static_cast<SpriteTableModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -0.5f);

        m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().width() * -0.5f);
    m_pItemSprite->GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionOrderFrameUpwards_triggered()
{
    int iSelectedIndex = GetCurSpriteState()->GetSelectedIndex();

    QUndoCommand *pCmd = new SpriteUndoCmd_OrderFrame(GetCurSpriteState()->GetFrameView(), iSelectedIndex, iSelectedIndex - 1);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void SpriteWidget::on_actionOrderFrameDownwards_triggered()
{
    int iSelectedIndex = GetCurSpriteState()->GetSelectedIndex();

    QUndoCommand *pCmd = new SpriteUndoCmd_OrderFrame(GetCurSpriteState()->GetFrameView(), iSelectedIndex, iSelectedIndex + 1);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}
