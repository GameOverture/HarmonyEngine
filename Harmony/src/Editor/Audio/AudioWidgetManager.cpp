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
	ui(new Ui::AudioWidgetManager)
{
	ui->setupUi(this);
	
	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("WidgetAudioManager::WidgetAudioManager() invalid constructor used", LOGTYPE_Error);
}

AudioWidgetManager::AudioWidgetManager(Project *pProjOwner, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AudioWidgetManager),
	m_pProjOwner(pProjOwner)
{
	ui->setupUi(this);

	ui->cmbAudioMiddleware;

	//connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(ReadData()));
	//connect(&m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)));
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
	HyGuiLog("Connecting to FMOD Studio", LOGTYPE_Title);

	enum FMODStep
	{
		FMODSTEP_Start = 0,
		FMODSTEP_DeletingGuidFile,
		FMODSTEP_Connecting,
		FMODSTEP_ExportingGuids,
		FMODSTEP_FlushingResponse,
		FMODSTEP_SendingQuery,
		FMODSTEP_ReadingResponse,

		NUM_FMODSTEPS
	};

	QDir dataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_Audio, true));
	QFile guidFile(dataDir.filePath("GUIDs.txt"));

	QTcpSocket socket(this);
	QByteArray socketResponse;

	QProgressDialog progressDlg("Connecting to FMOD Studio...", "", FMODSTEP_Start, NUM_FMODSTEPS, this);
	progressDlg.setWindowTitle("Connecting");
	progressDlg.setCancelButton(nullptr);
	progressDlg.setWindowModality(Qt::WindowModal);
	progressDlg.setMinimumDuration(0);

	int iProgressStep = FMODSTEP_Start;
	while(iProgressStep != NUM_FMODSTEPS)
	{
		progressDlg.setValue(iProgressStep);
		if(progressDlg.wasCanceled())
			break;

		switch(iProgressStep)
		{
		case FMODSTEP_Start:
			iProgressStep = FMODSTEP_DeletingGuidFile;
			break;

		case FMODSTEP_DeletingGuidFile:
			if(guidFile.exists())
				guidFile.remove();
			else
			{
				socket.connectToHost("127.0.0.1", 3663);
				iProgressStep = FMODSTEP_Connecting;
			}
			break;

		case FMODSTEP_Connecting:
			if(socket.waitForConnected(3000))
			{
				QString sSource = "studio.project.save(); studio.project.build(); studio.project.exportGUIDs();";
				socket.write(sSource.toUtf8());

				iProgressStep = FMODSTEP_ExportingGuids;
			}
			else
			{
				HyGuiLog("Could not establish connection to FMOD Studio", LOGTYPE_Warning);
				iProgressStep = NUM_FMODSTEPS;
			}
			break;
				
		case FMODSTEP_ExportingGuids:
			if(guidFile.exists())
			{
				if(guidFile.open(QIODevice::ReadOnly))
					iProgressStep = FMODSTEP_FlushingResponse;
				else
				{
					HyGuiLog(guidFile.errorString(), LOGTYPE_Error);
					iProgressStep = NUM_FMODSTEPS;
				}
			}
			break;

		case FMODSTEP_FlushingResponse: {
			char buffer[50];
			qint64 iNumBytesRead = socket.read(buffer, 50);
			socketResponse.append(buffer, iNumBytesRead);

			if(iNumBytesRead == 0 && !socket.waitForReadyRead(1000))
			{
				socketResponse.clear();
				iProgressStep = FMODSTEP_SendingQuery;
			}
			break; }
				
		case FMODSTEP_SendingQuery: {
			QStringList sBankNames;
			QTextStream in(&guidFile);
			while(!in.atEnd())
			{
				QString sLine = in.readLine();

				if(sLine.contains("bank:/", Qt::CaseInsensitive))
					sBankNames.append(sLine.right(sLine.length() - sLine.indexOf("bank:/")));
			}
			guidFile.close();

			QString sSource;
			sSource += "var eventMap = {};";
			for(int i = 0; i < sBankNames.size(); ++i)
			{
				sSource += "{";
				sSource += "    var bankObj = studio.project.lookup(\"" % sBankNames[i] % "\");";
				sSource += "    if(bankObj != undefined) {";
				sSource += "        var eventsArray = bankObj.events;";
				sSource += "        for(i = 0; i < eventsArray.length; i++)";
				sSource += "            eventMap[eventsArray[i].getPath()] = \"" % sBankNames[i] % "\";";
				sSource += "    }";
				sSource += "}";
			}
			sSource += "JSON.stringify(eventMap);";

			socket.write(sSource.toUtf8());
			iProgressStep = FMODSTEP_ReadingResponse;
			break; }

		case FMODSTEP_ReadingResponse: {
			char buffer[50];
			qint64 iNumBytesRead = socket.read(buffer, 50);
			socketResponse.append(buffer, iNumBytesRead);

			if(iNumBytesRead == 0 && !socket.waitForReadyRead(1000))
			{
				HyGuiLog(socketResponse, LOGTYPE_Normal);
				iProgressStep = NUM_FMODSTEPS;
			}
			break; }
		}
	}

	progressDlg.setValue(NUM_FMODSTEPS);
	socket.abort();
}

void AudioWidgetManager::on_AudioMiddleware_currentIndexChanged(int index)
{
}

//void AudioWidgetManager::ReadData()
//{
//	QDataStream in(&m_Socket);
//	//in.setVersion(QDataStream::Qt_4_0);
//
//	QString sMessage;
//	in >> sMessage;
//
//	HyGuiLog("Read: " % sMessage, LOGTYPE_Info);
//}

//void AudioWidgetManager::OnError(QAbstractSocket::SocketError socketError)
//{
//	switch(socketError)
//	{
//	case QAbstractSocket::RemoteHostClosedError:
//		HyGuiLog("The game debugger connection has been lost", LOGTYPE_Info);
//		break;
//
//	case QAbstractSocket::HostNotFoundError:
//		HyGuiLog("FMOD Studio at [127.0.0.1:3663] was not found", LOGTYPE_Error);
//		break;
//
//	case QAbstractSocket::ConnectionRefusedError:
//		HyGuiLog("FMOD Studio at [127.0.0.1:3663] has refused the connection", LOGTYPE_Error);
//		break;
//
//	default:
//		HyGuiLog("Audio FMOD Socket Error: " % m_Socket.errorString(), LOGTYPE_Error);
//		break;
//	}
//}
