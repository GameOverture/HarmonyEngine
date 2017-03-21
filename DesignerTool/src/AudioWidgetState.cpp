/**************************************************************************
 *	WidgetAudioState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AudioWidgetState.h"
#include "ui_AudioWidgetState.h"

#include "HyGuiGlobal.h"

#include "AudioWidget.h"
#include "AudioWidgetManager.h"

#include "UndoCmds.h"

AudioWidgetState::AudioWidgetState(AudioWidget *pOwner, QList<QAction *> stateActionList, QWidget *parent) :    QWidget(parent),
                                                                                                                ui(new Ui::AudioWidgetState),
                                                                                                                m_pOwner(pOwner),
                                                                                                                m_sName("Unnamed")
{
    ui->setupUi(this);
    
    ui->btnAddWaves->setDefaultAction(FindAction(stateActionList, "actionImportWaves"));
    ui->btnRemoveWave->setDefaultAction(FindAction(stateActionList, "actionRemoveWave"));
    ui->btnOrderWaveUp->setDefaultAction(FindAction(stateActionList, "actionOrderWaveUpwards"));
    ui->btnOrderWaveDown->setDefaultAction(FindAction(stateActionList, "actionOrderWaveDownwards"));
    
    ui->cmbCategory->setModel(m_pOwner->GetItem().GetProject().GetAudioWidget()->GetCategoryModel());
    
    UpdateActions();
}

AudioWidgetState::~AudioWidgetState()
{
    delete ui;
}

QString AudioWidgetState::GetName()
{
    return m_sName;
}

void AudioWidgetState::SetName(QString sName)
{
    m_sName = sName;
}

void AudioWidgetState::UpdateActions()
{
    m_iPrevCategoryIndex = ui->cmbCategory->currentIndex();
    m_iPrevPlayTypeIndex = ui->cmbPlayType->currentIndex();
    m_iPrevNumInst = ui->sbInstMax->value();
    
    ui->sbInstMax->setEnabled(ui->chkLimitInst->isChecked());
    ui->radInstFail->setEnabled(ui->chkLimitInst->isChecked());
    ui->radInstQueue->setEnabled(ui->chkLimitInst->isChecked());
    
    QComboBox *pCmbStates = m_pOwner->GetCmbStates();
    for(int i = 0; i < pCmbStates->count(); ++i)
    {
        if(pCmbStates->itemData(i).value<AudioWidgetState *>() == this)
        {
            pCmbStates->setCurrentIndex(i);
            break;
        }
    }
}

void AudioWidgetState::on_cmbCategory_currentIndexChanged(int index)
{
//    QUndoCommand *pCmd = new UndoCmd_ComboBox<AudioWidgetState>("Audio Category", this, ui->cmbCategory, m_iPrevCategoryIndex, index);
//    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidgetState::on_chkLimitInst_clicked()
{
//    QUndoCommand *pCmd = new UndoCmd_CheckBox<AudioWidgetState>(this, ui->chkLimitInst);
//    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidgetState::on_chkLooping_clicked()
{
//    QUndoCommand *pCmd = new UndoCmd_CheckBox<AudioWidgetState>(this, ui->chkLooping);
//    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidgetState::on_cmbPlayType_currentIndexChanged(int index)
{
//    QUndoCommand *pCmd = new UndoCmd_ComboBox<AudioWidgetState>("Play Type", this, ui->cmbPlayType, m_iPrevPlayTypeIndex, index);
//    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidgetState::on_sbInstMax_editingFinished()
{
    if(m_iPrevNumInst == ui->sbInstMax->value())
        return;
    
    QUndoCommand *pCmd = new UndoCmd_SpinBox<AudioWidgetState>("Number of Instances", this, ui->sbInstMax, m_iPrevNumInst, ui->sbInstMax->value());
    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidgetState::on_radInstFail_toggled(bool checked)
{
    if(checked == false)
        return;
    
    QUndoCommand *pCmd = new UndoCmd_RadioToggle<AudioWidgetState>("Instance Limit Behavior", this, ui->radInstFail, ui->radInstQueue);
    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidgetState::on_radInstQueue_toggled(bool checked)
{
    if(checked == false)
        return;
    
    QUndoCommand *pCmd = new UndoCmd_RadioToggle<AudioWidgetState>("Instance Limit Behavior", this, ui->radInstQueue, ui->radInstFail);
    m_pOwner->GetItem().GetUndoStack()->push(pCmd);
}
