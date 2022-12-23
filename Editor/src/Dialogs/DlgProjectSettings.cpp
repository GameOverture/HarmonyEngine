/**************************************************************************
*	DlgProjectSettings.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "DlgProjectSettings.h"
#include "ui_DlgProjectSettings.h"
#include "MainWindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QPushButton>

DlgProjectSettings::DlgProjectSettings(Project &projectRef, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgProjectSettings),
	m_ProjectRef(projectRef)
{
	ui->setupUi(this);

	QJsonObject projSettingsObj = m_ProjectRef.GetSettingsObj();
	HarmonyInit defaultVals;

	if(projSettingsObj.contains("Title"))
		ui->txtTitleName->setText(projSettingsObj["Title"].toString());
	else
		ui->txtTitleName->setText(defaultVals.sGameName.c_str());

	ui->wgtDataDir->Setup("Assets", "data", m_ProjectRef.GetAbsPath(), m_ProjectRef.GetAssetsRelPath());
	ui->wgtMetaDir->Setup("Meta-Data", "meta", m_ProjectRef.GetAbsPath(), m_ProjectRef.GetMetaRelPath());
	ui->wgtSourceDir->Setup("Source Code", "Source", m_ProjectRef.GetAbsPath(), m_ProjectRef.GetSourceRelPath());
	ui->wgtBuildDir->Setup("Build", "build", m_ProjectRef.GetAbsPath(), m_ProjectRef.GetBuildRelPath());

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	connect(ui->wgtDataDir, &WgtMakeRelDir::OnDirty, this, &DlgProjectSettings::ErrorCheck);
	connect(ui->wgtMetaDir, &WgtMakeRelDir::OnDirty, this, &DlgProjectSettings::ErrorCheck);
	connect(ui->wgtSourceDir, &WgtMakeRelDir::OnDirty, this, &DlgProjectSettings::ErrorCheck);
	connect(ui->wgtBuildDir, &WgtMakeRelDir::OnDirty, this, &DlgProjectSettings::ErrorCheck);
	ErrorCheck();

	if(projSettingsObj.contains("UpdatesPerSec"))
		ui->sbUpdatePerSec->setValue(projSettingsObj["UpdatesPerSec"].toInt());
	else
		ui->sbUpdatePerSec->setValue(defaultVals.uiUpdatesPerSec);

	if(projSettingsObj.contains("VSync"))
		ui->chkVSync->setChecked(projSettingsObj["VSync"].toBool());
	else
		ui->chkVSync->setChecked(defaultVals.iVSync);

	if(projSettingsObj.contains("ShowCursor"))
		ui->chkShowCursor->setChecked(projSettingsObj["ShowCursor"].toBool());
	else
		ui->chkShowCursor->setChecked(defaultVals.bShowCursor);

	if(projSettingsObj.contains("NumInputMaps"))
		ui->sbInputMaps->setValue(projSettingsObj["NumInputMaps"].toInt());
	else
		ui->sbInputMaps->setValue(defaultVals.uiNumInputMaps);

	ui->wgtGravity2d->Init(SPINBOXTYPE_Double2d, QVariant(-100.0), QVariant(100.0));
	QVariant vGravity2d;
	if(projSettingsObj.contains("Gravity2d"))
	{
		vGravity2d.setValue(QPointF(projSettingsObj["Gravity2d"].toArray().at(0).toDouble(),
									projSettingsObj["Gravity2d"].toArray().at(1).toDouble()));
	}
	else
	{
		vGravity2d.setValue(QPointF(defaultVals.vGravity2d.x,
									defaultVals.vGravity2d.y));
	}
	ui->wgtGravity2d->SetValue(vGravity2d);

	if(projSettingsObj.contains("PixelsPerMeter"))
		ui->sbPixelsPerMeter->setValue(projSettingsObj["PixelsPerMeter"].toDouble());
	else
		ui->sbPixelsPerMeter->setValue(defaultVals.fPixelsPerMeter);

	if(projSettingsObj.contains("UseConsole"))
		ui->grpConsoleWindow->setChecked(projSettingsObj["UseConsole"].toBool());
	else
		ui->grpConsoleWindow->setChecked(defaultVals.bUseConsole);

	setWindowTitle(m_ProjectRef.GetTitle() % " Settings");
	setWindowIcon(HyGlobal::ItemIcon(ITEM_Project, SUBICON_Settings));
}

DlgProjectSettings::~DlgProjectSettings()
{
	delete ui;
}

bool DlgProjectSettings::HasSettingsChanged()
{
	return false;
}

QJsonObject DlgProjectSettings::GetNewSettingsObj()
{
	return m_ProjectRef.GetSettingsObj();
}

void DlgProjectSettings::on_buttonBox_accepted()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//// Create workspace file tree
	//QDir buildDir(m_ProjectRef.GetAbsPath());
	//buildDir.mkpath(".");

	//// DATA
	//buildDir.mkdir(ui->wgtDataDir->GetRelPath());
	//buildDir.cd(ui->wgtDataDir->GetRelPath());
	//buildDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));
	//buildDir.mkdir(HyGlobal::ItemName(ITEM_Audio, true));

	//// META-DATA
	//buildDir.setPath(m_ProjectRef.GetAbsPath());
	//buildDir.mkdir(ui->wgtMetaDir->GetRelPath());
	//buildDir.cd(ui->wgtMetaDir->GetRelPath());
	//buildDir.mkdir(HyGlobal::ItemName(ITEM_Source, true));
	//buildDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));
	//buildDir.mkdir(HyGlobal::ItemName(ITEM_Audio, true));

	//// SOURCE
	//buildDir.setPath(m_ProjectRef.GetAbsPath());
	//buildDir.mkdir(ui->wgtSourceDir->GetRelPath());

	////// BUILD
	////buildDir.setPath(GetProjDirPath());
	////buildDir.mkdir(ui->wgtBuildDir->GetRelPath());

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//// Generate HyProj file. Insert the minimum required fields for it.
	//// The project's DlgProjectSettings will fill in the rest of the defaults
	//HarmonyInit hyInit; // Use default values of struct;
	//QJsonObject jsonObj;
	//jsonObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	//jsonObj.insert("Title", ui->txtTitleName->text());
	//jsonObj.insert("DataPath", QString(ui->wgtDataDir->GetRelPath() + "/"));
	//jsonObj.insert("MetaPath", QString(ui->wgtMetaDir->GetRelPath() + "/"));
	//jsonObj.insert("SourcePath", QString(ui->wgtSourceDir->GetRelPath() + "/"));
	//jsonObj.insert("BuildPath", QString(ui->wgtBuildDir->GetRelPath() + "/"));

	//jsonObj.insert("UpdatesPerSec", static_cast<qint64>(hyInit.uiUpdatesPerSec));
	//jsonObj.insert("VSync", hyInit.iVSync);
	//jsonObj.insert("NumInputMaps", static_cast<qint64>(hyInit.uiNumInputMaps));
	//jsonObj.insert("ShowCursor", hyInit.bShowCursor);

	//QJsonArray windowInfoArray;
	//QJsonObject windowInfoObj;
	//windowInfoObj.insert("Name", ui->txtTitleName->text());
	//windowInfoObj.insert("Type", hyInit.windowInfo[0].eMode);
	//windowInfoObj.insert("ResolutionX", hyInit.windowInfo[0].vSize.x);
	//windowInfoObj.insert("ResolutionY", hyInit.windowInfo[0].vSize.y);
	//windowInfoObj.insert("LocationX", hyInit.windowInfo[0].ptLocation.x);
	//windowInfoObj.insert("LocationY", hyInit.windowInfo[0].ptLocation.y);
	//windowInfoArray.append(windowInfoObj);
	//jsonObj.insert("WindowInfo", windowInfoArray);

	//jsonObj.insert("UseConsole", true);
	//QJsonObject consoleInfoObj;
	//consoleInfoObj.insert("LocationX", hyInit.consoleInfo.ptLocation.x);
	//consoleInfoObj.insert("LocationY", hyInit.consoleInfo.ptLocation.y);
	//consoleInfoObj.insert("Name", hyInit.consoleInfo.sName.c_str());
	//consoleInfoObj.insert("ResolutionX", hyInit.consoleInfo.vSize.x);
	//consoleInfoObj.insert("ResolutionY", hyInit.consoleInfo.vSize.y);
	//consoleInfoObj.insert("Type", hyInit.consoleInfo.eMode);
	//jsonObj.insert("ConsoleInfo", consoleInfoObj);

	//QFile newProjectFile(GetProjFilePath());
	//if(newProjectFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
	//{
	//	HyGuiLog("Couldn't open new project file for writing", LOGTYPE_Error);
	//}
	//else
	//{
	//	QJsonDocument newProjectDoc(jsonObj);
	//	qint64 iBytesWritten = newProjectFile.write(newProjectDoc.toJson());
	//	if(0 == iBytesWritten || -1 == iBytesWritten)
	//	{
	//		HyGuiLog("Could not write new project file: " % newProjectFile.errorString(), LOGTYPE_Error);
	//	}

	//	newProjectFile.close();
	//}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//// Copy standard git files to new project location
	//if(ui->chkUseGit->isChecked())
	//{
	//	QDir projGenDir(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir);
	//	QFileInfoList fileInfoList = projGenDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//	for(int i = 0; i < fileInfoList.size(); ++i)
	//		QFile::copy(fileInfoList[i].absoluteFilePath(), QDir(GetProjDirPath()).absoluteFilePath(fileInfoList[i].fileName()));
	//}
}

void DlgProjectSettings::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->txtTitleName->text().isEmpty())
		{
			ui->lblError->setText("Error: Game title cannot be blank.");
			bIsError = true;
			break;
		}
		
		QString sError;
		sError = ui->wgtDataDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}

		sError = ui->wgtMetaDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}

		sError = ui->wgtSourceDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}

		sError = ui->wgtBuildDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}
	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
