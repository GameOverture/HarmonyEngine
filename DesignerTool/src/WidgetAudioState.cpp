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
