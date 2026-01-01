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
#include "DlgInputName.h"

#include <QPushButton>

DlgSurfaceMaterials::DlgSurfaceMaterials(SurfaceMaterialsModel &surfaceModelRef, QWidget *pParent) :
	QDialog(pParent),
	m_SurfaceMaterialsModelRef(surfaceModelRef),
	ui(new Ui::DlgSurfaceMaterials)
{
	ui->setupUi(this);
	setWindowIcon(QIcon(QString::fromUtf8(":/icons16x16/collision.png")));

	ui->btnNewMat->setDefaultAction(ui->actionNewMat);
	ui->btnRemoveMat->setDefaultAction(ui->actionRemoveMat);
	ui->btnRenameMat->setDefaultAction(ui->actionRenameMat);
	ui->btnSortMatUp->setDefaultAction(ui->actionSortMatUp);
	ui->btnSortMatDown->setDefaultAction(ui->actionSortMatDown);

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

void DlgSurfaceMaterials::on_actionNewMat_triggered()
{
	m_SurfaceMaterialsModelRef.AppendNewSurface();
}

void DlgSurfaceMaterials::on_actionRemoveMat_triggered()
{
	if(ui->listView->currentIndex().isValid())
	{
		QJsonObject surfaceMatObj = m_SurfaceMaterialsModelRef.data(ui->listView->currentIndex(), Qt::UserRole).toJsonObject();
		if(surfaceMatObj.contains("TileSetDependants") == false)
		{
			HyGuiLog("DlgSurfaceMaterials::on_actionRemoveMat_triggered(): Surface material object is missing 'TileSetDependants' field!", LOGTYPE_Error);
			return;
		}

		QJsonArray dependeeTileSetArray = surfaceMatObj["TileSetDependants"].toArray();
		if(dependeeTileSetArray.size() > 0)
		{
			QStringList sDependeeList = m_SurfaceMaterialsModelRef.GetDependeeStringList(ui->listView->currentIndex().row());
			HyGuiLog("Cannot remove surface material that is in use. It is referenced by the following: " + sDependeeList.join(", "), LOGTYPE_Warning);
			return;
		}

		m_SurfaceMaterialsModelRef.removeRow(ui->listView->currentIndex().row());
	}
}

void DlgSurfaceMaterials::on_actionRenameMat_triggered()
{
	if(ui->listView->currentIndex().isValid())
	{
		QString sCurName = m_SurfaceMaterialsModelRef.GetName(ui->listView->currentIndex().row());
		DlgInputName *pDlg = new DlgInputName("Rename Surface Material", sCurName, HyGlobal::FreeFormValidator(), nullptr, nullptr);

		if(pDlg->exec() == QDialog::Accepted)
			m_SurfaceMaterialsModelRef.SetName(ui->listView->currentIndex().row(), pDlg->GetName());
		delete pDlg;

		ui->listView->edit(ui->listView->currentIndex());
	}
}

void DlgSurfaceMaterials::on_actionSortMatUp_triggered()
{
	if(ui->listView->currentIndex().isValid())
		m_SurfaceMaterialsModelRef.MoveSurfaceBack(ui->listView->currentIndex().row());
}

void DlgSurfaceMaterials::on_actionSortMatDown_triggered()
{
	if(ui->listView->currentIndex().isValid())
		m_SurfaceMaterialsModelRef.MoveSurfaceForward(ui->listView->currentIndex().row());
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
