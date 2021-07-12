/**************************************************************************
 *	SourceSettingsDlg.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SourceSettingsDlg.h"
#include "ui_SourceSettingsDlg.h"
#include "WgtSrcDependency.h"

#include <QPushButton>

SourceSettingsDlg::SourceSettingsDlg(const Project &projectRef, QJsonObject settingsObj, QWidget *pParent /*= nullptr*/) :
	QDialog(pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::SourceSettingsDlg),
	m_ProjectRef(projectRef)
{
	ui->setupUi(this);

	ui->txtOutputName->setText(settingsObj["OutputName"].toString());

	ui->chkUseGlfw->setChecked(settingsObj["UseGlfw"].toBool());
	ui->chkUseSdlNet->setChecked(settingsObj["UseSdlNet"].toBool());
	
	QJsonArray dependsArray = settingsObj["SrcDepends"].toArray();
	for(int32 i = 0; i < dependsArray.size(); ++i)
	{
		QJsonObject depObj = dependsArray[i].toObject();

		QDir metaDir(m_ProjectRef.GetSourceAbsPath());
		if(metaDir.cd(depObj["RelPath"].toString()) == false)
			HyGuiLog("SourceSettingsDlg could not derive absolute dependency path", LOGTYPE_Error);

		WgtSrcDependency *pNewWgtSrcDep = new WgtSrcDependency(this);
		pNewWgtSrcDep->Set(depObj["ProjectName"].toString(), metaDir.absolutePath());

		m_SrcDependencyList.append(pNewWgtSrcDep);
		ui->lytDependencies->addWidget(pNewWgtSrcDep);
		connect(pNewWgtSrcDep, &WgtSrcDependency::OnDirty, this, &SourceSettingsDlg::ErrorCheck);
	}

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	Refresh();
}

SourceSettingsDlg::~SourceSettingsDlg()
{
	for(int i = 0; i < m_SrcDependencyList.count(); ++i)
		delete m_SrcDependencyList[i];

	delete ui;
}

QString SourceSettingsDlg::GetProjectDir() const
{
	return m_ProjectRef.GetDirPath();
}

void SourceSettingsDlg::RemoveSrcDep(WgtSrcDependency *pRemoved)
{
	for(int i = 0; i < m_SrcDependencyList.count(); ++i)
	{
		if(m_SrcDependencyList[i] == pRemoved)
		{
			delete m_SrcDependencyList.takeAt(i);
			break;
		}
	}
	Refresh();
}

void SourceSettingsDlg::Refresh()
{
	if(ui->lytDependencies->count() != m_SrcDependencyList.count())
	{
		if(ui->lytDependencies->count() < m_SrcDependencyList.count())
		{
			for(int i = ui->lytDependencies->count(); i < m_SrcDependencyList.count(); ++i)
				ui->lytDependencies->addWidget(m_SrcDependencyList[i]);
		}
		else
		{
			for(int i = m_SrcDependencyList.count(); i < ui->lytDependencies->count() - 1; ++i) // Keep at least '1'
				delete ui->lytDependencies->takeAt(i);
		}
	}

	ErrorCheck();
}

void SourceSettingsDlg::UpdateMetaObj(QJsonObject &metaObjRef) const
{
	metaObjRef.insert("OutputName", ui->txtOutputName->text());

	metaObjRef.insert("UseGlfw", ui->chkUseGlfw->isChecked());
	metaObjRef.insert("UseSdlNet", ui->chkUseSdlNet->isChecked());

	QDir metaDir(m_ProjectRef.GetSourceAbsPath());
	QJsonArray srcDependsArray;
	for(auto srcDep : m_SrcDependencyList)
	{
		QJsonObject srcDepObj;
		srcDepObj.insert("RelPath", metaDir.relativeFilePath(srcDep->GetAbsPath()));
		srcDepObj.insert("ProjectName", srcDep->GetProjectName());

		srcDependsArray.append(srcDepObj);
	}
	metaObjRef.insert("SrcDepends", srcDependsArray);
}

void SourceSettingsDlg::on_txtOutputName_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void SourceSettingsDlg::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->txtOutputName->text().isEmpty())
		{
			ui->lblError->setText("'Output Name' cannot be blank");
			bIsError = true;
			break;
		}

		for(auto srcDep : m_SrcDependencyList)
		{
			QString sError = srcDep->GetError();
			if(sError.isEmpty() == false)
			{
				ui->lblError->setText(sError);
				bIsError = true;
				break;
			}
		}
		if(bIsError)
			break;
	}while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}

void SourceSettingsDlg::on_btnAddDependency_clicked()
{
	m_SrcDependencyList.append(new WgtSrcDependency(this));
	ui->lytDependencies->addWidget(m_SrcDependencyList[m_SrcDependencyList.count() - 1]);
	connect(m_SrcDependencyList[m_SrcDependencyList.count() - 1], &WgtSrcDependency::OnDirty, this, &SourceSettingsDlg::ErrorCheck);

	Refresh();
}
