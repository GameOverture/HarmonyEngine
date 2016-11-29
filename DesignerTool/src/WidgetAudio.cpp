/**************************************************************************
 *	WidgetAudio.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAudio.h"
#include "ui_WidgetAudio.h"

#include "DlgInputName.h"
#include "WidgetAudioState.h"

#include "ItemAudioCmds.h"

#include <QJsonDocument>
#include <QJsonObject>

WidgetAudio::WidgetAudio(ItemAudio *pOwner, QWidget *parent) :  QWidget(parent),
                                                                ui(new Ui::WidgetAudio),
                                                                m_pItemAudio(pOwner),
                                                                m_pCurAudioState(NULL)
{
    ui->setupUi(this);
    
    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    m_StateActionsList.push_back(ui->actionImportWaves);
    m_StateActionsList.push_back(ui->actionRemoveWave);
    m_StateActionsList.push_back(ui->actionOrderWaveUpwards);
    m_StateActionsList.push_back(ui->actionOrderWaveDownwards);

    ui->cmbStates->clear();
    
    
    
    // If a .hyaud file exists, parse and initalize with it, otherwise make default empty audio
    QFile audioFile(m_pItemAudio->GetAbsPath());
    if(audioFile.exists())
    {
        if(!audioFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetAudio::WidgetAudio() could not open ") % m_pItemAudio->GetAbsPath(), LOGTYPE_Error);

        QJsonDocument fontJsonDoc = QJsonDocument::fromJson(audioFile.readAll());
        audioFile.close();

        QJsonObject fontObj = fontJsonDoc.object();
    }
    else
    {
        on_actionAddState_triggered();
    }
    
    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    m_pItemAudio->GetUndoStack()->clear();

    UpdateActions();
}

WidgetAudio::~WidgetAudio()
{
    delete ui;
}

ItemAudio *WidgetAudio::GetItemAudio()
{
    return m_pItemAudio;
}

void WidgetAudio::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
}


void WidgetAudio::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new ItemAudioCmd_AddState(this, m_StateActionsList, ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetAudio::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new ItemAudioCmd_RemoveState(ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetAudio::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Audio State", ui->cmbStates->currentData().value<WidgetAudioState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new ItemAudioCmd_RenameState(ui->cmbStates, pDlg->GetName());
        m_pItemAudio->GetUndoStack()->push(pCmd);
    }
}

void WidgetAudio::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new ItemAudioCmd_MoveStateBack(ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetAudio::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new ItemAudioCmd_MoveStateForward(ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetAudio::on_cmbStates_currentIndexChanged(int index)
{
    WidgetAudioState *pAudioState = ui->cmbStates->itemData(index).value<WidgetAudioState *>();
    if(m_pCurAudioState == pAudioState)
        return;

    if(m_pCurAudioState)
        m_pCurAudioState->hide();

    if(pAudioState)
    {
        ui->grpStateLayout->addWidget(pAudioState);
    
        m_pCurAudioState = pAudioState;
        m_pCurAudioState->show();
    }

    UpdateActions();
}
