/**************************************************************************
 *	DlgNewItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgNewItem.h"
#include "ui_DlgNewItem.h"
#include "ExplorerModel.h"
#include "MainWindow.h"

#include <QDirIterator>
#include <QStringBuilder>
#include <QPushButton>
#include <QFileDialog>

DlgNewItem::DlgNewItem(Project *pItemProject, ItemType eItem, QString sDefaultPrefix, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::DlgNewItem),
	m_pItemProject(pItemProject),
	m_eItemType(eItem)
{
	if(m_pItemProject == nullptr)
		HyGuiLog("DlgNewItem::DlgNewItem was given a Project * that was nullptr", LOGTYPE_Error);

	ui->setupUi(this);

	setWindowTitle("Add a new " % HyGlobal::ItemName(eItem, false) % " item");
	setWindowIcon(HyGlobal::ItemIcon(eItem, SUBICON_New));
	
	ui->txtName->setValidator(HyGlobal::FileNameValidator());
	ui->txtPrefix->setValidator(HyGlobal::FilePathValidator());
	on_chkNewPrefix_stateChanged(ui->chkNewPrefix->isChecked() ? Qt::Checked : Qt::Unchecked);

	m_PrefixStringList = MainWindow::GetExplorerModel().GetPrefixList(pItemProject);

	std::sort(m_PrefixStringList.begin(), m_PrefixStringList.end());
	m_PrefixStringList.prepend(QString("<no prefix>"));

	ui->cmbPrefixList->clear();
	ui->cmbPrefixList->addItems(m_PrefixStringList);

	int iDefaultIndex = ui->cmbPrefixList->findText(sDefaultPrefix);
	if(iDefaultIndex >= 0)
		ui->cmbPrefixList->setCurrentIndex(iDefaultIndex);
	
	ui->lblName->setText(HyGlobal::ItemName(eItem, false) % " Name:");
	ui->txtName->setText("New" % HyGlobal::ItemName(eItem, false));
	
	ui->txtName->selectAll();

	bool bImportDlgType = (eItem == ITEM_Prefab || eItem == ITEM_Spine);
	if(bImportDlgType == false)
		ui->grpImport->hide();

	int iWidth = width();
	adjustSize();
	resize(iWidth, height());

	ErrorCheck();
}

DlgNewItem::~DlgNewItem()
{
	delete ui;
}

QString DlgNewItem::GetName()
{
	return ui->txtName->text();
}

QString DlgNewItem::GetPrefix()
{
	if(ui->chkNewPrefix->isChecked())
		return QString(HyIO::CleanPath(ui->txtPrefix->text().toStdString().c_str(), nullptr, false).c_str());
	else
		return ui->cmbPrefixList->currentIndex() == 0 ? QString() : ui->cmbPrefixList->currentText();
}

QString DlgNewItem::GetImportFile()
{
	if(ui->grpImport->isHidden() == false)
		return ui->txtImport->text();
	else
		return QString();
}

void DlgNewItem::on_chkNewPrefix_stateChanged(int arg1)
{
	if(arg1 == Qt::Checked)
	{
		ui->txtPrefix->setVisible(true);
		ui->txtPrefix->setFocus();
		ui->txtPrefix->selectAll();
		ui->lblPrefixInstructions->setText(QString("e.g. \"<b>Level2</b>\" or \"<b>Level3/Intro</b>\""));

		ui->cmbPrefixList->setVisible(false);
	}
	else
	{
		ui->txtPrefix->setVisible(false);
		ui->lblPrefixInstructions->setText("");

		ui->cmbPrefixList->setVisible(true);
	}

	ErrorCheck();
}

void DlgNewItem::on_txtName_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void DlgNewItem::on_txtPrefix_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void DlgNewItem::on_cmbPrefixList_currentIndexChanged(const QString &arg1)
{
	ErrorCheck();
}

void DlgNewItem::ErrorCheck()
{
	QString sPrefix = GetPrefix();
	if(sPrefix.endsWith('/', Qt::CaseInsensitive) == false)
		sPrefix += "/";
	
	bool bIsError = false;
	do
	{
		//QStringList sSplitPrefixList = sPrefix.split('/');
		//for(int i = 0; i < sSplitPrefixList.size() && bIsError == false; ++i)
		//{
		//	QStringList sTypeNameList = HyGlobal::GetTypeNameList();
		//	for(int j = 0; j < sTypeNameList.size() && bIsError == false; ++j)
		//	{
		//		if(sSplitPrefixList[i].compare(sTypeNameList[j], Qt::CaseInsensitive) == 0 ||
		//		   ui->txtName->text().compare(sTypeNameList[j], Qt::CaseInsensitive) == 0)
		//		{
		//			ui->lblError->setText("Error: The prefix and/or name is using a reserved name.");
		//			bIsError = true;
		//			break;
		//		}
		//	}
		//}
		
		if(ui->txtName->text().isEmpty())
		{
			ui->lblError->setText("Error: An item's name cannot be blank");
			bIsError = true;
			break;
		}
		
		bool bFoundDup = false;
		if(ui->chkNewPrefix->isChecked())
		{
			for(uint32 i = 0; i < static_cast<uint32>(m_PrefixStringList.size()); ++i)
			{
				if(0 == sPrefix.compare(m_PrefixStringList[i], Qt::CaseInsensitive))
				{
					bFoundDup = true;
					break;
				}
			}
			if(bFoundDup)
			{
				ui->lblError->setText("Error: This prefix already exists.");
				bIsError = true;
				break;
			}
		}

		QString sNewItemPath = sPrefix % '/' % ui->txtName->text();
		if(m_pItemProject->DoesItemExist(m_eItemType, sNewItemPath))
		{
			ui->lblError->setText("Error: An item with this name at this prefix already exists.");
			bIsError = true;
			break;
		}

		if(ui->grpImport->isHidden() == false)
		{
			QFile file(ui->txtImport->text());
			if(file.exists() == false)
			{
				ui->lblError->setText("Error: Import file does not exist");
				bIsError = true;
				break;
			}
		}

	}while(false);

	if(bIsError)
		ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	else
	{
		ui->lblError->setStyleSheet("QLabel { color : black; }");
		ui->lblError->setText("");
	}
	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}

void DlgNewItem::on_buttonBox_accepted()
{
}

void DlgNewItem::on_btnImportBrowse_clicked()
{
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	QString sCaption;
	QString sFilters;
	QString sSelectedFilter;
	if(m_eItemType == ITEM_Prefab)
	{
		sCaption = "Import new 3d asset";
		sFilters = tr("All files (*.*);;FBX (*.fbx)");
		sSelectedFilter = tr("FBX (*.fbx)");
	}
	else if(m_eItemType == ITEM_Spine)
	{
		sCaption = "Import new spine file";
		sFilters = tr("Spine Files (*.skel *.json)");
		sSelectedFilter = tr("Spine Files (*.skel *.json)");
	}
	else
	{
		HyGuiLog("DlgNewItem has unknown import type", LOGTYPE_Error);
	}
	QString sImportFile = QFileDialog::getOpenFileName(this,
													   sCaption,
													   QString(),
													   sFilters,
													   &sSelectedFilter);

	if(sImportFile.isEmpty() == false)
		ui->txtImport->setText(sImportFile);
}

void DlgNewItem::on_txtImport_textChanged(const QString &arg1)
{
	ErrorCheck();
}
