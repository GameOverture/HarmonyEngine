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

	ui->txtTitleName->setText(projSettingsObj["Title"].toString());
	ui->txtCodeName->setText(projSettingsObj["CodeName"].toString());

	ui->txtAssetsLocation->setText(projSettingsObj["DataPath"].toString());
	ui->txtMetaDataLocation->setText(projSettingsObj["MetaDataPath"].toString());

	ui->sbInputMaps->setValue(projSettingsObj["NumInputMappings"].toInt());
	ui->sbUpdateFpsCap->setValue(projSettingsObj["UpdateFpsCap"].toInt());
	ui->sbPixelsPerMeter->setValue(projSettingsObj["PixelsPerMeter"].toInt());
	ui->chkShowCursor->setChecked(projSettingsObj["ShowCursor"].toBool());

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
