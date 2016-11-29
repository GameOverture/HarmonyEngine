/**************************************************************************
 *	WidgetAudioState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAudioState.h"
#include "ui_WidgetAudioState.h"

#include "HyGlobal.h"

#include "WidgetAudio.h"
#include "WidgetAudioManager.h"

#include "ItemAudioCmds.h"

WidgetAudioState::WidgetAudioState(WidgetAudio *pOwner, QList<QAction *> stateActionList, QWidget *parent) :    QWidget(parent),
                                                                                                                ui(new Ui::WidgetAudioState),
                                                                                                                m_pOwner(pOwner),
                                                                                                                m_sName("Unnamed")
{
    ui->setupUi(this);
    
    ui->btnAddWaves->setDefaultAction(FindAction(stateActionList, "actionImportWaves"));
    ui->btnRemoveWave->setDefaultAction(FindAction(stateActionList, "actionRemoveWave"));
    ui->btnOrderWaveUp->setDefaultAction(FindAction(stateActionList, "actionOrderWaveUpwards"));
    ui->btnOrderWaveDown->setDefaultAction(FindAction(stateActionList, "actionOrderWaveDownwards"));
    
    ui->cmbCategory->setModel(m_pOwner->GetItemAudio()->GetAudioManager().GetCategoryModel());
    
    m_iPrevCategoryIndex = ui->cmbCategory->currentIndex();
    m_iPrevPlayTypeIndex = ui->cmbPlayType->currentIndex();
}

WidgetAudioState::~WidgetAudioState()
{
    delete ui;
}

QString WidgetAudioState::GetName()
{
    return m_sName;
}

void WidgetAudioState::SetName(QString sName)
{
    m_sName = sName;
}

void WidgetAudioState::UpdateActions()
{
    ui->sbInstMax->setEnabled(ui->chkLimitInst->isChecked());
    ui->radInstFail->setEnabled(ui->chkLimitInst->isChecked());
    ui->radInstQueue->setEnabled(ui->chkLimitInst->isChecked());
}

void WidgetAudioState::on_cmbCategory_currentIndexChanged(int index)
{
    ItemAudioCmd_CategoryChanged *pCmd = new ItemAudioCmd_CategoryChanged(*this, ui->cmbCategory, m_iPrevCategoryIndex, index);
    m_pOwner->GetItemAudio()->GetUndoStack()->push(pCmd);
    
    m_iPrevCategoryIndex = index;
}

void WidgetAudioState::on_chkLimitInst_clicked()
{
    ItemAudioCmd_CheckBox *pCmd = new ItemAudioCmd_CheckBox(*this, ui->chkLimitInst);
    m_pOwner->GetItemAudio()->GetUndoStack()->push(pCmd);
}

void WidgetAudioState::on_chkLooping_clicked()
{
    ItemAudioCmd_CheckBox *pCmd = new ItemAudioCmd_CheckBox(*this, ui->chkLooping);
    m_pOwner->GetItemAudio()->GetUndoStack()->push(pCmd);
}

void WidgetAudioState::on_cmbPlayType_currentIndexChanged(int index)
{
    ItemAudioCmd_PlayTypeChanged *pCmd = new ItemAudioCmd_PlayTypeChanged(*this, ui->cmbPlayType, m_iPrevPlayTypeIndex, index);
    m_pOwner->GetItemAudio()->GetUndoStack()->push(pCmd);
    
    m_iPrevPlayTypeIndex = index;
}
