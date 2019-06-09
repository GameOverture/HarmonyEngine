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
#include <QProgressDialog>

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
	enum FMODStep
	{
		FMODSTEP_Start = 0,
		FMODSTEP_DeleteGuidFile,
		FMODSTEP_Connecting,
		FMODSTEP_ExportGuids,
		FMODSTEP_ReadBankNames,
		FMODSTEP_RequestMetaData,

		NUM_FMODSTEPS
	};

	QFile guidFile(m_DataDir.filePath("GUIDs.txt"));
	QFile metaFile(m_MetaDir.filePath("HyOutput.txt"));
	m_Socket.abort();

	QProgressDialog audioScanDlg("Scanning for audio information", "Abort", 0, NUM_FMODSTEPS, this);
	audioScanDlg.setWindowModality(Qt::WindowModal);
	audioScanDlg.setMinimumDuration(0);

	int iAudioScanStep = FMODSTEP_Start;
	while(iAudioScanStep != NUM_FMODSTEPS)
	{
		switch(iAudioScanStep)
		{
		case FMODSTEP_Start:
			iAudioScanStep = FMODSTEP_DeleteGuidFile;
			break;

		case FMODSTEP_DeleteGuidFile:
			if(guidFile.exists())
				guidFile.remove();
			else
			{
				m_Socket.connectToHost("127.0.0.1", 3663);
				iAudioScanStep = FMODSTEP_Connecting;
			}
			break;

		case FMODSTEP_Connecting:
			if(m_Socket.state() == QAbstractSocket::ConnectedState)
			{
				// studio.project.save(); studio.project.build(); 
				QString sSource = "studio.project.exportGUIDs();";
				m_Socket.write(sSource.toUtf8());
				iAudioScanStep = FMODSTEP_ExportGuids;
			}
			break;
				
		case FMODSTEP_ExportGuids:
			if(guidFile.exists())
			{
				if(guidFile.open(QIODevice::ReadOnly))
					iAudioScanStep = FMODSTEP_ReadBankNames;
				else
				{
					HyGuiLog(guidFile.errorString(), LOGTYPE_Error);
					iAudioScanStep = NUM_FMODSTEPS;
				}
			}
			break;
				
		case FMODSTEP_ReadBankNames: {
			QStringList sBankNames;
			QTextStream in(&guidFile);
			while(!in.atEnd())
			{
				QString sLine = in.readLine();

				if(sLine.contains("bank:/", Qt::CaseInsensitive))
					sBankNames.append(sLine.right(sLine.length() - sLine.indexOf("bank:/")));
			}
			guidFile.close();

			QString sSource = "var sStr;";
			for(int i = 0; i < sBankNames.size(); ++i)
			{
				sSource += "sStr += \":" % sBankNames[i] % ":\";";
				sSource += "var eventsArray = studio.project.lookup(\"" % sBankNames[i] % ").events;";
				sSource += "for (i = 0; i < eventsArray.length; i++) {";
				sSource += "	sStr += eventsArray[i].getPath();";
				sSource += "	sStr += \"|\";";
				sSource += "}";
			}

			sSource += "var file = system.getFile(\"" % metaFile.fileName() % "\");";
			sSource += "if(file.open(system.openMode.WriteOnly)) {";
			sSource += "	file.writeText(sStr);";
			sSource += "}";
			sSource += "file.close();";

			m_Socket.write(sSource.toUtf8());
			iAudioScanStep = FMODSTEP_RequestMetaData;
			break; }

		case FMODSTEP_RequestMetaData:
			if(metaFile.exists())
				iAudioScanStep = NUM_FMODSTEPS;
			break;
		}

		audioScanDlg.setValue(iAudioScanStep);
		if(audioScanDlg.wasCanceled())
			break;
	}

	audioScanDlg.setValue(NUM_FMODSTEPS);
	m_Socket.abort();
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
