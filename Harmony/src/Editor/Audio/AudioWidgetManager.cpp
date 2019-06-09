/**************************************************************************
 *	AudioWidgetManager.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioWidgetManager.h"
#include "ui_AudioWidgetManager.h"

#include "DlgInputName.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineEdit>

AudioWidgetManager::AudioWidgetManager(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AudioWidgetManager),
	m_Socket(this)
{
	ui->setupUi(this);
	
	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("WidgetAudioManager::WidgetAudioManager() invalid constructor used", LOGTYPE_Error);
}

AudioWidgetManager::AudioWidgetManager(Project *pProjOwner, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AudioWidgetManager),
	m_pProjOwner(pProjOwner),
	m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_Audio, true)),
	m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_Audio, true))
{
	ui->setupUi(this);

	ui->cmbAudioMiddleware;

	connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(ReadData()));
	connect(&m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)));
}

AudioWidgetManager::~AudioWidgetManager()
{
	delete ui;
}

Project *AudioWidgetManager::GetItemProject()
{
	return m_pProjOwner;
}

void AudioWidgetManager::on_btnScanAudio_pressed()
{
	HyGuiLog("Attempting to connect FMOD Studio at [127.0.0.1:3663]", LOGTYPE_Normal);
	m_Socket.abort();
	m_Socket.connectToHost("127.0.0.1", 3663);
	if(m_Socket.waitForConnected())
	{
		HyGuiLog("FMOD Studio connection made", LOGTYPE_Normal);
		QString sSource = "studio.project.save(); studio.project.build(); studio.project.exportGUIDs();";
		m_Socket.write(sSource.toUtf8());
	}
}

void AudioWidgetManager::on_AudioMiddleware_currentIndexChanged(int index)
{
}

void AudioWidgetManager::ReadData()
{
	QDataStream in(&m_Socket);
	in.setVersion(QDataStream::Qt_4_0);

	QString sMessage;
	in >> sMessage;

	HyGuiLog("Read: " % sMessage, LOGTYPE_Info);
}

void AudioWidgetManager::OnError(QAbstractSocket::SocketError socketError)
{
	switch(socketError)
	{
	case QAbstractSocket::RemoteHostClosedError:
		HyGuiLog("The game debugger connection has been lost", LOGTYPE_Info);
		break;

	case QAbstractSocket::HostNotFoundError:
		HyGuiLog("FMOD Studio at [127.0.0.1:3663] was not found", LOGTYPE_Error);
		break;

	case QAbstractSocket::ConnectionRefusedError:
		HyGuiLog("FMOD Studio at [127.0.0.1:3663] has refused the connection", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("Audio FMOD Socket Error: " % m_Socket.errorString(), LOGTYPE_Error);
		break;
	}
}
