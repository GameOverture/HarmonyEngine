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

DlgProjectSettings::DlgProjectSettings(Project &projectRef, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgProjectSettings),
	m_ProjectRef(projectRef)
{
	ui->setupUi(this);

	QJsonObject projSettingsObj = m_ProjectRef.GetSettingsObj();

	ui->txtTitleName->setText(projSettingsObj["GameName"].toString());
	ui->txtClassName->setText(projSettingsObj["ClassName"].toString());

	ui->txtAssetsLocation->setText(projSettingsObj["DataPath"].toString());
	ui->txtMetaDataLocation->setText(projSettingsObj["MetaDataPath"].toString());
	ui->txtSourceLocation->setText(projSettingsObj["SourcePath"].toString());

	ui->sbInputMaps->setValue(projSettingsObj["NumInputMappings"].toInt());
	ui->sbUpdateFpsCap->setValue(projSettingsObj["UpdateFpsCap"].toInt());
	ui->sbPixelsPerMeter->setValue(projSettingsObj["PixelsPerMeter"].toInt());
	ui->chkShowCursor->setChecked(projSettingsObj["ShowCursor"].toBool());

	setWindowTitle(m_ProjectRef.GetGameName() % " Game Settings");
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
