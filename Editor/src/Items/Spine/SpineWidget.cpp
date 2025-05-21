/**************************************************************************
*	SpineWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "SpineWidget.h"
#include "Project.h"
#include "SpineUndoCmds.h"
#include "SpineDraw.h"
#include "GlobalUndoCmds.h"

SpineWidget::SpineWidget(ProjectItemData &itemRef, QWidget *parent) :
	IWidget(itemRef, parent),
	ui(new Ui::SpineWidget)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->lytMainVerticalLayout);
	GetAboveStatesLayout()->addWidget(ui->grpAnimationMixing);
	GetBelowStatesLayout()->addItem(ui->lytMainVerticalLayout);

	SpineModel *pSpineModel = static_cast<SpineModel *>(m_ItemRef.GetModel());
	pSpineModel->GetDefaultMixMapper()->AddSpinBoxMapping(ui->sbDefaultMix);
	ui->sbDefaultMix->setSingleStep(0.01);

	ui->mixTableView->setModel(&pSpineModel->GetCrossFadeModel());
	ui->mixTableView->setColumnWidth(1, 50);
	ui->mixTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->mixTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	ui->mixTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

	ui->btnAddMix->setDefaultAction(ui->actionAddMix);
	ui->btnRemoveMix->setDefaultAction(ui->actionRemoveMix);
	ui->btnOrderMixUp->setDefaultAction(ui->actionOrderMixUp);
	ui->btnOrderMixDown->setDefaultAction(ui->actionOrderMixDown);

	//QJsonArray dependsArray = settingsObj["SrcDepends"].toArray();
	//for(int32 i = 0; i < dependsArray.size(); ++i)
	//{
	//	QJsonObject depObj = dependsArray[i].toObject();

	//	QDir metaDir(m_ProjectRef.GetSourceAbsPath());
	//	if(metaDir.cd(depObj["RelPath"].toString()) == false)
	//		HyGuiLog("SourceSettingsDlg could not derive absolute dependency path", LOGTYPE_Error);

	//	WgtSrcDependency *pNewWgtSrcDep = new WgtSrcDependency(this);
	//	pNewWgtSrcDep->Set(depObj["ProjectName"].toString(), metaDir.absolutePath(), depObj["Options"].toString());

	//	m_SrcDependencyList.append(pNewWgtSrcDep);
	//	ui->lytDependencies->addWidget(pNewWgtSrcDep);
	//	connect(pNewWgtSrcDep, &WgtSrcDependency::OnDirty, this, &SourceSettingsDlg::ErrorCheck);
	//}
}

SpineWidget::~SpineWidget()
{
	delete ui;
}

/*virtual*/ void SpineWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	//pMenu->addAction(ui.actionAddFill);
	//pMenu->addAction(ui.actionAddEdge);
	//pMenu->addAction(ui.actionAddEdgeFill);
	//pMenu->addAction(ui.actionAddInner);
	//pMenu->addAction(ui.actionAddSDF);
	//pMenu->addSeparator();
	//pMenu->addAction(ui.actionOrderLayerUp);
	//pMenu->addAction(ui.actionOrderLayerDown);
	//pMenu->addAction(ui.actionRemoveLayer);
}

/*virtual*/ void SpineWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void SpineWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	SpineDraw *pDraw = static_cast<SpineDraw *>(m_ItemRef.GetDraw());
}

void SpineWidget::on_sbDefaultMix_valueChanged(double dValue)
{
	QUndoCommand *pCmd = new UndoCmd_DoubleSpinBox("Default Mix changed",
												   GetItem(),
												   static_cast<SpineModel *>(m_ItemRef.GetModel())->GetDefaultMixMapper(),
												   -1,
												   dValue);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpineWidget::on_btnAddMix_clicked()
{
	//  ui->mixTableWidget

	//SpineUndoCmd_AddNewMix *pCmd = new SpineUndoCmd_AddNewMix(GetItem(), sAnimOne, sAnimTwo, ui->sbDefaultMix->value());
	//GetItem().GetUndoStack->push(pCmd);
}

void SpineWidget::on_actionAddMix_triggered()
{
	SpineModel *pSpineModel = static_cast<SpineModel *>(m_ItemRef.GetModel());

	QString sAnimOne, sAnimTwo;
	float fMix;
	if(pSpineModel->GetNextCrossFadeAnims(sAnimOne, sAnimTwo, fMix))
	{
		QUndoCommand *pCmd = new SpineUndoCmd_AddNewCrossFade(GetItem(), sAnimOne, sAnimTwo, fMix);
		GetItem().GetUndoStack()->push(pCmd);
	}
}

void SpineWidget::on_actionRemoveMix_triggered()
{

}

void SpineWidget::on_actionOrderMixUp_triggered()
{
}

void SpineWidget::on_actionOrderMixDown_triggered()
{
}
