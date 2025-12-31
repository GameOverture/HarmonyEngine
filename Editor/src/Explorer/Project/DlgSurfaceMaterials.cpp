/**************************************************************************
 *	DlgSurfaceMaterials.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgSurfaceMaterials.h"
#include "ui_DlgSurfaceMaterials.h"
#include "SurfaceMaterialsModel.h"

#include <QPushButton>

DlgSurfaceMaterials::DlgSurfaceMaterials(SurfaceMaterialsModel &surfaceModelRef, QWidget *pParent) :
	QDialog(pParent),
	m_SurfaceMaterialsModelRef(surfaceModelRef),
	ui(new Ui::DlgSurfaceMaterials)
{
	ui->setupUi(this);
	setWindowIcon(QIcon(QString::fromUtf8(":/icons16x16/collision.png")));

	ui->listView->setModel(&m_SurfaceMaterialsModelRef);

	ErrorCheck();
}

/*virtual*/ DlgSurfaceMaterials::~DlgSurfaceMaterials()
{
	delete ui;
}

QUuid DlgSurfaceMaterials::GetSelectedMaterialUuid() const
{
	if(ui->listView->currentIndex().isValid())
		return m_SurfaceMaterialsModelRef.GetUuid(ui->listView->currentIndex().row());

	return QUuid();
}

/*virtual*/ void DlgSurfaceMaterials::done(int r) /*override*/
{

	//if(r == QDialog::Accepted)
	//{
	//	if(bAssetsChanged && QMessageBox::Ok == QMessageBox::warning(nullptr, QString("Save asset properties?"), QString("Save asset properties? Changed assets will need to be repacked."), QMessageBox::Ok, QMessageBox::Cancel))
	//	{
	//		QDialog::done(r);
	//	}
	//	else
	//		QDialog::done(QDialog::Rejected);
	//}
	
	QDialog::done(r);
}

void DlgSurfaceMaterials::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->listView->currentIndex().isValid() == false)
		{
			ui->lblError->setText("No surface material selected");
			bIsError = true;
			break;
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
