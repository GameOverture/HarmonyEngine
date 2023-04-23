/**************************************************************************
 *	AudioAssetsWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioAssetsWidget.h"
#include "ui_AudioAssetsWidget.h"

#include "DlgInputName.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineEdit>
#include <QProgressDialog>

AudioAssetsWidget::AudioAssetsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AudioAssetsWidget)
{
	ui->setupUi(this);
	
	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("WidgetAudioManager::WidgetAudioManager() invalid constructor used", LOGTYPE_Error);
}

AudioAssetsWidget::AudioAssetsWidget(Project *pProjOwner, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AudioAssetsWidget),
	m_pProjOwner(pProjOwner)
{
	ui->setupUi(this);

	ui->cmbAudioMiddleware->insertItem(0, "FMOD");

	//connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(ReadData()));
	//connect(&m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)));
}

AudioAssetsWidget::~AudioAssetsWidget()
{
	delete ui;
}

Project *AudioAssetsWidget::GetItemProject()
{
	return m_pProjOwner;
}

void AudioAssetsWidget::on_btnScanAudio_pressed()
{
	SetupForFMOD();
//	m_pProjOwner->
}

void AudioAssetsWidget::on_AudioMiddleware_currentIndexChanged(int index)
{
}

void AudioAssetsWidget::SetupForFMOD()
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

	QDir dataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::AssetName(ASSETMAN_Audio));
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
				//  studio.project.build();
				QString sSource = "studio.project.save(); studio.project.exportGUIDs();";
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
			m_sBankNameList.clear();
			QTextStream in(&guidFile);
			while(!in.atEnd())
			{
				QString sLine = in.readLine();

				if(sLine.contains("bank:/", Qt::CaseInsensitive))
					m_sBankNameList.append(sLine.right(sLine.length() - sLine.indexOf("bank:/")));
			}
			guidFile.close();

			QString sSource;
			sSource += "var eventMap = {};";
			for(int i = 0; i < m_sBankNameList.size(); ++i)
			{
				sSource += "{";
				sSource += "    var bankObj = studio.project.lookup(\"" % m_sBankNameList[i] % "\");";
				sSource += "    if(bankObj != undefined) {";
				sSource += "        var eventsArray = bankObj.events;";
				sSource += "        for(i = 0; i < eventsArray.length; i++) {";
				sSource += "            var eventObj = {};";
				sSource += "            eventObj[\"bank\"] = \"" % m_sBankNameList[i] % "\";";
				sSource += "            eventObj[\"guid\"] = eventsArray[i].id;";
				sSource += "            eventMap[eventsArray[i].getPath()] = eventObj;";
				sSource += "        }";
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

				// Prep response to pristine json syntax
				socketResponse.replace("out():", "");
				socketResponse.replace("event:/", "");
				socketResponse.replace("bank:/", "");
				socketResponse.remove(socketResponse.lastIndexOf('}') + 1, socketResponse.length() - socketResponse.lastIndexOf('}'));
				socketResponse = socketResponse.trimmed();

				// Set the "Audio" portion of the data.json file
				QJsonParseError error;
				QJsonDocument audioDoc = QJsonDocument::fromJson(socketResponse, &error);
				if(error.error != QJsonParseError::NoError)
				{
					HyGuiLog("FMOD parsing response error:" % error.errorString(), LOGTYPE_Error);
					iProgressStep = NUM_FMODSTEPS;
					break;
				}

				// TODO: Revive FMOD here?
				//m_pProjOwner->SetAudioModel(audioDoc.object());

				// Write out separate audio.json soundbank file
				QFile soundBanksFile(dataDir.absoluteFilePath(HYASSETS_AudioFile));
				if(soundBanksFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
				{
					HyGuiLog(QString("Couldn't open ") % HYASSETS_AudioFile % " for writing: " % soundBanksFile.errorString(), LOGTYPE_Error);
					iProgressStep = NUM_FMODSTEPS;
					break;
				}

				// Find full paths to sound banks
				QStringList sAbsBankFilePathList;
				HyGlobal::RecursiveFindFileOfExt(".bank", sAbsBankFilePathList, dataDir);

				int iMasterStringsBankIndex = -1;
				for(int i = 0; i < sAbsBankFilePathList.size(); ++i)
				{
					if(sAbsBankFilePathList[i].contains("strings.bank", Qt::CaseInsensitive))
					{
						iMasterStringsBankIndex = i;
						break;
					}
				}
				if(iMasterStringsBankIndex == -1)
					HyGuiLog("Did not find master string bank", LOGTYPE_Error);

				int iMasterBankIndex = -1;
				QString sMasterBankPath = sAbsBankFilePathList[iMasterStringsBankIndex];
				sMasterBankPath.replace(".strings.bank", ".bank", Qt::CaseInsensitive);
				for(int i = 0; i < sAbsBankFilePathList.size(); ++i)
				{
					if(i == iMasterStringsBankIndex)
						continue;
					if(sAbsBankFilePathList[i].compare(sMasterBankPath, Qt::CaseInsensitive) == 0)
					{
						iMasterBankIndex = i;
						break;
					}
				}
				if(iMasterBankIndex == -1)
					HyGuiLog("Did not find master bank", LOGTYPE_Error);

				QJsonObject soundBanksObj;
				for(int i = 0; i < sAbsBankFilePathList.size(); ++i)
				{
					QString sBankName = "";
					QString sPath = "";
					for(int j = 0; j < m_sBankNameList.size(); ++j)
					{
						QString sTestBankName = m_sBankNameList[j];
						sTestBankName.replace("bank:/", "");
						QFileInfo sBankFileInfo(sTestBankName);

						// Match with full "path" first, then try just the base name since folder structure may vary on FMOD output.
						if(sAbsBankFilePathList[i].contains(sTestBankName, Qt::CaseInsensitive))
						{
							sBankName = sTestBankName;
							sPath = sAbsBankFilePathList[i];
							break;
						}
						else if(sBankName == "" && sAbsBankFilePathList[i].contains(sBankFileInfo.baseName(), Qt::CaseInsensitive))
						{
							sBankName = sTestBankName;
							sPath = sAbsBankFilePathList[i];
						}
					}

					sPath = dataDir.relativeFilePath(sPath);

					QJsonObject sndBankObj;
					sndBankObj.insert("master", i == iMasterStringsBankIndex || i == iMasterBankIndex);
					sndBankObj.insert("filePath", sPath);

					if(soundBanksObj.contains(sBankName) == false)
						soundBanksObj.insert(sBankName, sndBankObj);
					else
						soundBanksObj.insert(sBankName % ".strings", sndBankObj);
				}

				QJsonDocument soundBanksDoc;
				soundBanksDoc.setObject(soundBanksObj);
				qint64 iBytesWritten = soundBanksFile.write(soundBanksDoc.toJson());
				if(0 == iBytesWritten || -1 == iBytesWritten)
					HyGuiLog(QString("Could not write to ") % HYASSETS_AudioFile % " file: " % soundBanksFile.errorString(), LOGTYPE_Error);
				soundBanksFile.close();

				iProgressStep = NUM_FMODSTEPS;
			}
			break; }
		}
	}

	progressDlg.setValue(NUM_FMODSTEPS);
	socket.abort();
}
