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
	
	QJsonArray dependsArray = settingsObj["SrcDepends"].toArray();
	for(int32 i = 0; i < dependsArray.size(); ++i)
	{
		QJsonObject depObj = dependsArray[i].toObject();

		QDir metaDir(m_ProjectRef.GetMetaDataAbsPath() % HyGlobal::AssetName(ASSET_Source));
		if(metaDir.cd(depObj["RelPath"].toString()) == false)
			HyGuiLog("SourceSettingsDlg could not derive absolute dependency path", LOGTYPE_Error);

		m_SrcDependencyList.append(new WgtSrcDependency(this));
		m_SrcDependencyList[m_SrcDependencyList.count() - 1]->Set(depObj["ProjectName"].toString(), metaDir.absolutePath());

		ui->lytDependencies->addWidget(m_SrcDependencyList[m_SrcDependencyList.count() - 1]);
		connect(m_SrcDependencyList[m_SrcDependencyList.count() - 1], &WgtSrcDependency::OnDirty, this, &SourceSettingsDlg::ErrorCheck);
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

void SourceSettingsDlg::AddSrcDep()
{
	m_SrcDependencyList.append(new WgtSrcDependency(this));
	ui->lytDependencies->addWidget(m_SrcDependencyList[m_SrcDependencyList.count() - 1]);
	connect(m_SrcDependencyList[m_SrcDependencyList.count() - 1], &WgtSrcDependency::OnDirty, this, &SourceSettingsDlg::ErrorCheck);

	Refresh();
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
	if(m_SrcDependencyList.empty())
	{
		m_SrcDependencyList.append(new WgtSrcDependency(this));
		connect(m_SrcDependencyList[m_SrcDependencyList.count() - 1], &WgtSrcDependency::OnDirty, this, &SourceSettingsDlg::ErrorCheck);
		return;
	}

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

		// Remove and re-add the layout that holds SrcDependency widgets. Otherwise it jumbles them together.
		//ui->grpAdvanced->layout()->removeItem(ui->lytDependencies);
		//ui->grpAdvanced->layout()->addItem(ui->lytDependencies);
	}

	ErrorCheck();
}

QJsonObject SourceSettingsDlg::GetMetaObj() const
{
	QJsonObject metaObj;
	metaObj.insert("OutputName", ui->txtOutputName->text());

	QDir metaDir(m_ProjectRef.GetMetaDataAbsPath() % HyGlobal::AssetName(ASSET_Source));
	QJsonArray srcDependsArray;
	for(auto srcDep : m_SrcDependencyList)
	{
		QJsonObject srcDepObj;
		srcDepObj.insert("RelPath", metaDir.relativeFilePath(srcDep->GetAbsPath()));
		srcDepObj.insert("ProjectName", srcDep->GetProjectName());

		srcDependsArray.append(srcDepObj);
	}
	metaObj.insert("SrcDepends", srcDependsArray);

	return metaObj;
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
		QString sError;
		for(auto srcDep : m_SrcDependencyList)
		{
			sError = srcDep->GetError();
			if(sError.isEmpty() == false)
			{
				bIsError = true;
				break;
			}
		}
	}while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
