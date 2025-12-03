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
#include "WgtWindowInfo.h"

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

	ui->vsbConsoleSize->Init(SPINBOXTYPE_Int2d, 1, 16384);
	ui->vsbConsoleLocation->Init(SPINBOXTYPE_Int2d, -16384, 16384);

	QJsonObject projSettingsObj = m_ProjectRef.GetSettingsObj();
	HyInit defaultVals;

	if(projSettingsObj.contains("Title"))
		ui->txtTitleName->setText(projSettingsObj["Title"].toString());
	else
		ui->txtTitleName->setText(defaultVals.sGameName.c_str());

	if(projSettingsObj.contains("WindowInfo"))
	{
		QJsonArray windowInfoArray = projSettingsObj["WindowInfo"].toArray();
		for(int i = 0; i < windowInfoArray.size(); ++i)
		{
			QJsonObject windowInfoObj = windowInfoArray.at(i).toObject();
			WgtWindowInfo *pWindowInfo = new WgtWindowInfo(this, windowInfoObj);
			m_WindowInfoList.push_back(pWindowInfo);
			ui->lytRenderWindows->addWidget(pWindowInfo);
		}
	}

	ui->wgtDataDir->Setup("Assets", "data", m_ProjectRef.GetDirPath(), m_ProjectRef.GetAssetsRelPath(), false);
	ui->wgtMetaDir->Setup("Meta-Data", "meta", m_ProjectRef.GetDirPath(), m_ProjectRef.GetMetaRelPath(), false);
	ui->wgtSourceDir->Setup("Source Code", "Source", m_ProjectRef.GetDirPath(), m_ProjectRef.GetSourceRelPath(), false);
	ui->wgtBuildDir->Setup("Build", "build", m_ProjectRef.GetDirPath(), m_ProjectRef.GetBuildRelPath(), false);

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
		ui->chkVSync->setChecked(projSettingsObj["VSync"].toInt() != 0);
	else
		ui->chkVSync->setChecked(defaultVals.iVSync != 0);

	if(projSettingsObj.contains("ShowCursor"))
		ui->chkShowCursor->setChecked(projSettingsObj["ShowCursor"].toBool());
	else
		ui->chkShowCursor->setChecked(defaultVals.bShowCursor);

	if(projSettingsObj.contains("NumInputMaps"))
		ui->sbInputMaps->setValue(projSettingsObj["NumInputMaps"].toInt());
	else
		ui->sbInputMaps->setValue(defaultVals.uiNumInputMaps);

	ui->vsbGravity2d->Init(SPINBOXTYPE_Double2d, QVariant(-100.0), QVariant(100.0));
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
	ui->vsbGravity2d->SetValue(vGravity2d);

	if(projSettingsObj.contains("PixelsPerMeter"))
		ui->sbPixelsPerMeter->setValue(projSettingsObj["PixelsPerMeter"].toDouble());
	else
		ui->sbPixelsPerMeter->setValue(defaultVals.fPixelsPerMeter);

	if(projSettingsObj.contains("UseConsole"))
		ui->grpConsoleWindow->setChecked(projSettingsObj["UseConsole"].toBool());
	else
		ui->grpConsoleWindow->setChecked(defaultVals.bUseConsole);

	if(projSettingsObj.contains("ConsoleInfo"))
	{
		QJsonObject consoleObj = projSettingsObj["ConsoleInfo"].toObject();
		ui->txtConsoleTitle->setText(consoleObj["Name"].toString());
		ui->vsbConsoleSize->SetValue(QPoint(consoleObj["ResolutionX"].toInt(), consoleObj["ResolutionY"].toInt()));
		ui->vsbConsoleLocation->SetValue(QPoint(consoleObj["LocationX"].toInt(), consoleObj["LocationY"].toInt()));
	}

	setWindowTitle(m_ProjectRef.GetTitle() % " Settings");
	setWindowIcon(HyGlobal::ItemIcon(ITEM_Project, SUBICON_Settings));
}

DlgProjectSettings::~DlgProjectSettings()
{
	delete ui;
}

QJsonObject DlgProjectSettings::SerializeWidgets()
{
	QJsonObject settingsObj;

	settingsObj.insert("Title", ui->txtTitleName->text());
	QJsonArray windowInfoArray;
	for(int i = 0; i < m_WindowInfoList.size(); ++i)
	{
		windowInfoArray.append(m_WindowInfoList[i]->SerializeWidgets());
	}
	settingsObj.insert("WindowInfo", windowInfoArray);
	settingsObj.insert("DataPath", QString(ui->wgtDataDir->GetRelPath() + "/"));
	settingsObj.insert("MetaPath", QString(ui->wgtMetaDir->GetRelPath() + "/"));
	settingsObj.insert("SourcePath", QString(ui->wgtSourceDir->GetRelPath() + "/"));
	settingsObj.insert("BuildPath", QString(ui->wgtBuildDir->GetRelPath() + "/"));
	settingsObj.insert("UpdatesPerSec", static_cast<qint64>(ui->sbUpdatePerSec->value()));
	settingsObj.insert("VSync", ui->chkVSync->isChecked() ? 1 : 0);
	settingsObj.insert("NumInputMaps", static_cast<qint64>(ui->sbInputMaps->value()));
	settingsObj.insert("ShowCursor", ui->chkShowCursor->isChecked());
	QVariant vGravity2d = ui->vsbGravity2d->GetValue();
	QJsonArray gravity2dArray;
	gravity2dArray.append(vGravity2d.toPointF().x());
	gravity2dArray.append(vGravity2d.toPointF().y());
	settingsObj.insert("Gravity2d", gravity2dArray);
	settingsObj.insert("PixelsPerMeter", ui->sbPixelsPerMeter->value());
	settingsObj.insert("UseConsole", ui->grpConsoleWindow->isChecked());
	QJsonObject consoleObj;
	consoleObj.insert("Name", ui->txtConsoleTitle->text());
	QVariant vConsoleSize = ui->vsbConsoleSize->GetValue();
	consoleObj.insert("Type", 1);
	consoleObj.insert("ResolutionX", vConsoleSize.toPoint().x());
	consoleObj.insert("ResolutionY", vConsoleSize.toPoint().y());
	QVariant vConsoleLocation = ui->vsbConsoleLocation->GetValue();
	consoleObj.insert("LocationX", vConsoleLocation.toPoint().x());
	consoleObj.insert("LocationY", vConsoleLocation.toPoint().y());
	settingsObj.insert("ConsoleInfo", consoleObj);

	return settingsObj;
}

//bool DlgProjectSettings::HasSettingsChanged()
//{
//	QJsonObject serializedObj = SerializeWidgets();
//	QJsonObject curWidgetsObj = m_ProjectRef.GetSettingsObj();
//
//	QJsonDocument serializedDoc(serializedObj);
//	QJsonDocument curWidgetsDoc(curWidgetsObj);
//	return strcmp(serializedDoc.toJson().data(), curWidgetsDoc.toJson().data()) != 0;
//}

void DlgProjectSettings::RemoveWindowInfo(WgtWindowInfo *pWindowInfo)
{
	m_WindowInfoList.removeOne(pWindowInfo);
	ui->lytRenderWindows->removeWidget(pWindowInfo);
	delete pWindowInfo;

	ErrorCheck();
}

void DlgProjectSettings::on_btnAddWindow_pressed()
{
	WgtWindowInfo *pWindowInfo = new WgtWindowInfo(this, QJsonObject());
	m_WindowInfoList.push_back(pWindowInfo);
	ui->lytRenderWindows->addWidget(pWindowInfo);

	ErrorCheck();
}

void DlgProjectSettings::on_txtTitle_textChanged(const QString &arg1)
{
	ErrorCheck();
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

		if(m_WindowInfoList.isEmpty())
		{
			ui->lblError->setText("Error: At least one render window must be defined.");
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
