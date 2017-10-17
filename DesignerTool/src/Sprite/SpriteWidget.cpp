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

#include "GlobalUndoCmds.h"
#include "SpriteUndoCmds.h"
#include "DlgInputName.h"
#include "AtlasWidget.h"
#include "SpriteDraw.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>

SpriteWidget::SpriteWidget(ProjectItem &itemRef, QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::SpriteWidget),
                                                                    m_ItemRef(itemRef),
                                                                    m_bPlayActive(false),
                                                                    m_fElapsedTime(0.0),
                                                                    m_bIsBounced(false)
{
    ui->setupUi(this);
    
    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);
    ui->btnAddFrames->setDefaultAction(ui->actionImportFrames);
    ui->btnRemoveFrame->setDefaultAction(ui->actionRemoveFrames);
    ui->btnOrderFrameUp->setDefaultAction(ui->actionOrderFrameUpwards);
    ui->btnOrderFrameDown->setDefaultAction(ui->actionOrderFrameDownwards);
    ui->btnPlay->setDefaultAction(ui->actionPlay);
    ui->btnFirstFrame->setDefaultAction(ui->actionFirstFrame);
    ui->btnLastFrame->setDefaultAction(ui->actionLastFrame);
    
    ui->cmbStates->clear();
    ui->cmbStates->setModel(m_ItemRef.GetModel());

    //ui->framesView->setModel(static_cast<SpriteModel *>(m_ItemRef.GetModel())->GetStateData(0)->pFramesModel);
    ui->framesView->setItemDelegate(new WidgetSpriteDelegate(&m_ItemRef, ui->framesView, this));
    QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
    connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));

    FocusState(0, -1);
}

SpriteWidget::~SpriteWidget()
{
    delete ui;
}

ProjectItem &SpriteWidget::GetItem()
{
    return m_ItemRef;
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

bool SpriteWidget::IsPlayingAnim()
{
    return m_bPlayActive;
}

void SpriteWidget::SetSelectedFrame(int iFrameIndex)
{
    ui->framesView->selectRow(iFrameIndex);
}

void SpriteWidget::GetSpriteInfo(int &iStateIndexOut, int &iFrameIndexOut)
{
    iStateIndexOut = ui->cmbStates->currentIndex();
    iFrameIndexOut = ui->framesView->currentIndex().row();
}

void SpriteWidget::FocusState(int iStateIndex, QVariant subState)
{
    if(iStateIndex >= 0)
    {
        ui->cmbStates->blockSignals(true);
        ui->cmbStates->setCurrentIndex(iStateIndex);
        ui->cmbStates->blockSignals(false);

        // Set the model of 'iStateIndex'
        SpriteStateData *pCurStateData = static_cast<SpriteStateData *>(static_cast<SpriteModel *>(m_ItemRef.GetModel())->GetStateData(iStateIndex));
        ui->framesView->setModel(pCurStateData->GetFramesModel());

        QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
        connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));

        pCurStateData->GetLoopMapper()->AddCheckBoxMapping(ui->chkLoop);
        pCurStateData->GetReverseMapper()->AddCheckBoxMapping(ui->chkReverse);
        pCurStateData->GetBounceMapper()->AddCheckBoxMapping(ui->chkBounce);

        // iSubStateIndex represents which row to select
        if(subState.toInt() >= 0)
        {
            if(subState.toInt() >= ui->framesView->model()->rowCount() &&
               ui->framesView->model()->rowCount() > 0)
            {
                ui->framesView->selectRow(0);
            }
            else
                ui->framesView->selectRow(subState.toInt());
        }
    }
    
    UpdateActions();
}

void SpriteWidget::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
    
    int iCurNumFrames = GetCurStateData()->GetFramesModel()->rowCount();
    bool bFrameIsSelected = iCurNumFrames > 0 && ui->framesView->currentIndex().row() >= 0;
    
    ui->actionAlignCenterHorizontal->setEnabled(bFrameIsSelected);
    ui->actionAlignCenterVertical->setEnabled(bFrameIsSelected);
    ui->actionAlignUp->setEnabled(bFrameIsSelected);
    ui->actionAlignDown->setEnabled(bFrameIsSelected);
    ui->actionAlignLeft->setEnabled(bFrameIsSelected);
    ui->actionAlignRight->setEnabled(bFrameIsSelected);

    ui->actionOrderFrameUpwards->setEnabled(ui->framesView->currentIndex().row() != 0 && iCurNumFrames > 1);
    ui->actionOrderFrameDownwards->setEnabled(ui->framesView->currentIndex().row() != iCurNumFrames - 1 && iCurNumFrames > 1);
    ui->actionRemoveFrames->setEnabled(bFrameIsSelected);
}

void SpriteWidget::ApplyTransform(QPoint &vTransformAmtRef)
{
    QUndoCommand *pCmd = new SpriteUndoCmd_OffsetFrame(ui->framesView, ui->actionApplyToAll->isChecked() ? -1 : ui->framesView->currentIndex().row(), vTransformAmtRef, true);
    m_ItemRef.GetUndoStack()->push(pCmd);
}

SpriteStateData *SpriteWidget::GetCurStateData()
{
    return static_cast<SpriteStateData *>(static_cast<SpriteModel *>(m_ItemRef.GetModel())->GetStateData(ui->cmbStates->currentIndex()));
}

void SpriteWidget::on_framesView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
{
    static_cast<SpriteDraw *>(m_ItemRef.GetDraw())->SetFrame(ui->cmbStates->currentIndex(), ui->framesView->currentIndex().row());
    UpdateActions();
}

void SpriteWidget::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_AddState<SpriteStateData>("Add Sprite State", m_ItemRef, nullptr);
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_RemoveState<SpriteStateData>("Remove Sprite State", m_ItemRef, ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Sprite State", GetCurStateData()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new UndoCmd_RenameState("Rename Sprite State", m_ItemRef, pDlg->GetName(), ui->cmbStates->currentIndex());
        m_ItemRef.GetUndoStack()->push(pCmd);
    }
    
    delete pDlg;
}

void SpriteWidget::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateBack("Shift Sprite State Index <-", m_ItemRef, ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateForward("Shift Sprite State Index ->", m_ItemRef, ui->cmbStates->currentIndex());
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_cmbStates_currentIndexChanged(int index)
{
    FocusState(index, -1);
}

void SpriteWidget::on_actionAlignLeft_triggered()
{
    SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(0.0f);

        m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(ui->framesView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(0.0f);
    m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(ui->framesView, ui->framesView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignRight_triggered()
{
    SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -1);

        m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(ui->framesView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().width() * -1);
    m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(ui->framesView, ui->framesView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignUp_triggered()
{
    SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -1);

        m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(ui->framesView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().height() * -1);
    m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(ui->framesView, ui->framesView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignDown_triggered()
{
    SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(0.0f);

        m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(ui->framesView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(0.0f);
    m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(ui->framesView, ui->framesView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignCenterVertical_triggered()
{
    SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -0.5f);

        m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(ui->framesView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().height() * -0.5f);
    m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetYFrame(ui->framesView, ui->framesView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionAlignCenterHorizontal_triggered()
{
    SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -0.5f);

        m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(ui->framesView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().width() * -0.5f);
    m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_OffsetXFrame(ui->framesView, ui->framesView->currentIndex().row(), newOffsetList));
}

void SpriteWidget::on_actionImportFrames_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_AddFrames("Add Frames", m_ItemRef, ui->cmbStates->currentIndex());
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionRemoveFrames_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_DeleteFrame("Remove Frame",
                                                              m_ItemRef,
                                                              ui->cmbStates->currentIndex(),
                                                              static_cast<SpriteFramesModel *>(ui->framesView->model())->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionOrderFrameUpwards_triggered()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_OrderFrame(ui->framesView, ui->framesView->currentIndex().row(), ui->framesView->currentIndex().row() - 1);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionOrderFrameDownwards_triggered()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_OrderFrame(ui->framesView, ui->framesView->currentIndex().row(), ui->framesView->currentIndex().row() + 1);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionPlay_triggered()
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

void SpriteWidget::on_actionFirstFrame_triggered()
{
    ui->framesView->selectRow(0);
}

void SpriteWidget::on_actionLastFrame_triggered()
{
    ui->framesView->selectRow(static_cast<SpriteFramesModel *>(ui->framesView->model())->rowCount() - 1);
}

void SpriteWidget::on_chkReverse_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox(ui->chkReverse->text(), GetItem(), GetCurStateData()->GetReverseMapper(), ui->cmbStates->currentIndex());
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_chkLoop_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox(ui->chkLoop->text(), GetItem(), GetCurStateData()->GetLoopMapper(), ui->cmbStates->currentIndex());
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_chkBounce_clicked()
{
    QUndoCommand *pCmd = new UndoCmd_CheckBox(ui->chkBounce->text(), GetItem(), GetCurStateData()->GetBounceMapper(), ui->cmbStates->currentIndex());
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz10_clicked()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 10.0f);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz20_clicked()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 20.0f);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz30_clicked()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 30.0f);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz40_clicked()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 40.0f);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz50_clicked()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 50.0f);
    GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz60_clicked()
{
    QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 60.0f);
    GetItem().GetUndoStack()->push(pCmd);
}
