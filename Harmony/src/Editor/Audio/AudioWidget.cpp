/**************************************************************************
 *	AudioWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioWidget.h"
#include "ui_AudioWidget.h"

#include "DlgInputName.h"
#include "AudioWidgetState.h"
#include "GlobalUndoCmds.h"

#include <QJsonDocument>
#include <QJsonObject>

AudioWidget::AudioWidget(ProjectItem &itemRef, QWidget *pParent /*= nullptr*/) :
	IWidget(itemRef, pParent),
	ui(new Ui::AudioWidget),
	m_pCurAudioState(nullptr)
{
	ui->setupUi(this);

	m_StateActionsList.push_back(ui->actionImportWaves);
	m_StateActionsList.push_back(ui->actionRemoveWave);
	m_StateActionsList.push_back(ui->actionOrderWaveUpwards);
	m_StateActionsList.push_back(ui->actionOrderWaveDownwards);
	
	
	
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
//	{
//		on_actionAddState_triggered();
//	}
	
	// Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
	//m_pItemAudio->GetUndoStack()->clear();

	UpdateActions();
}

AudioWidget::~AudioWidget()
{
	delete ui;
}

/*virtual*/ void AudioWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
}

/*virtual*/ void AudioWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void AudioWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
}
