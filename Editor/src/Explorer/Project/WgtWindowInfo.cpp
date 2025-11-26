/**************************************************************************
 *	WgtWindowInfo.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtWindowInfo.h"
#include "DlgProjectSettings.h"
#include "ui_WgtWindowInfo.h"
#include "DlgNewProject.h"

#include <QFileDialog>

WgtWindowInfo::WgtWindowInfo(DlgProjectSettings *pParentDlg, QJsonObject windowInfoObj, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::WgtWindowInfo),
	m_pParentDlg(pParentDlg)
{
	ui->setupUi(this);
	//ui->txtTitle->setValidator(HyGlobal::FileNameValidator());

	if(windowInfoObj.isEmpty())
	{
		HyWindowInfo winInfo;
		windowInfoObj.insert("Name", QString(winInfo.sName.c_str()));
		windowInfoObj.insert("Type", static_cast<int>(winInfo.eMode));
		windowInfoObj.insert("ResolutionX", winInfo.vSize.x);
		windowInfoObj.insert("ResolutionY", winInfo.vSize.y);
		windowInfoObj.insert("LocationX", winInfo.ptLocation.x);
		windowInfoObj.insert("LocationY", winInfo.ptLocation.y);
	}

	ui->vsbResolution->Init(SPINBOXTYPE_Int2d, 1, 16384);
	ui->vsbLocation->Init(SPINBOXTYPE_Int2d, -16384, 16384);

	ui->txtTitle->setText(windowInfoObj["Name"].toString());
	ui->cmbType->setCurrentIndex(windowInfoObj["Type"].toInt());
	ui->vsbResolution->SetValue(QPoint(windowInfoObj["ResolutionX"].toInt(),
									   windowInfoObj["ResolutionY"].toInt()));
	ui->vsbLocation->SetValue(QPoint(windowInfoObj["LocationX"].toInt(),
									  windowInfoObj["LocationY"].toInt()));
}

/*virtual*/ WgtWindowInfo::~WgtWindowInfo()
{
	delete ui;
}

QJsonObject WgtWindowInfo::SerializeWidgets()
{
	QJsonObject windowInfoObj;
	windowInfoObj.insert("Name", ui->txtTitle->text());
	windowInfoObj.insert("Type", ui->cmbType->currentIndex());
	windowInfoObj.insert("ResolutionX", ui->vsbResolution->GetValue().toPoint().x());
	windowInfoObj.insert("ResolutionY", ui->vsbResolution->GetValue().toPoint().y());
	windowInfoObj.insert("LocationX", ui->vsbLocation->GetValue().toPoint().x());
	windowInfoObj.insert("LocationY", ui->vsbLocation->GetValue().toPoint().y());
	return windowInfoObj;
}

void WgtWindowInfo::on_btnRemoveWindow_clicked()
{
	m_pParentDlg->RemoveWindowInfo(this);
}

