/**************************************************************************
 *	WidgetAudio.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AudioWidget.h"
#include "ui_AudioWidget.h"

#include "DlgInputName.h"
#include "AudioWidgetState.h"

#include "UndoCmds.h"

#include <QJsonDocument>
#include <QJsonObject>

AudioWidget::AudioWidget(AudioItem *pOwner, QWidget *parent) :  QWidget(parent),
                                                                ui(new Ui::AudioWidget),
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
    
    
    
//    // If a .hyaud file exists, parse and initalize with it, otherwise make default empty audio
//    QFile audioFile(m_pItemAudio->GetAbsPath());
//    if(audioFile.exists())
//    {
//        if(!audioFile.open(QIODevice::ReadOnly))
//            HyGuiLog(QString("WidgetAudio::WidgetAudio() could not open ") % m_pItemAudio->GetAbsPath(), LOGTYPE_Error);

//        QJsonDocument fontJsonDoc = QJsonDocument::fromJson(audioFile.readAll());
//        audioFile.close();

//        QJsonObject fontObj = fontJsonDoc.object();
//    }
//    else
    {
        on_actionAddState_triggered();
    }
    
    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    m_pItemAudio->GetUndoStack()->clear();

    UpdateActions();
}

AudioWidget::~AudioWidget()
{
    delete ui;
}

AudioItem *AudioWidget::GetData()
{
    return m_pItemAudio;
}

QComboBox *AudioWidget::GetCmbStates()
{
    return ui->cmbStates;
}

void AudioWidget::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
}

void AudioWidget::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_AddState<AudioWidget, AudioWidgetState>("Add Audio State", this, m_StateActionsList, ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);
}

void AudioWidget::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_RemoveState<AudioWidget, AudioWidgetState>("Remove Audio State", this, ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);
}

void AudioWidget::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Audio State", ui->cmbStates->currentData().value<AudioWidgetState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new UndoCmd_RenameState<AudioWidgetState>("Rename Audio State", ui->cmbStates, pDlg->GetName());
        m_pItemAudio->GetUndoStack()->push(pCmd);
    }
    delete pDlg;
}

void AudioWidget::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateBack<AudioWidget, AudioWidgetState>("Shift Audio State Index <-", this, ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);
}

void AudioWidget::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new UndoCmd_MoveStateForward<AudioWidget, AudioWidgetState>("Shift Audio State Index ->", this, ui->cmbStates);
    m_pItemAudio->GetUndoStack()->push(pCmd);
}

void AudioWidget::on_cmbStates_currentIndexChanged(int index)
{
    AudioWidgetState *pAudioState = ui->cmbStates->itemData(index).value<AudioWidgetState *>();
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
