/**************************************************************************
 *	DlgTabCycle.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgTabCycle.h"
#include "ui_DlgTabCycle.h"
#include "MainWindow.h"

#include <QKeyEvent>
#include <QListWidgetItem>

DlgTabCycle::DlgTabCycle(ProjectTabBar *pProjTabBar, bool bSelectNext, QWidget *pParent /*= nullptr*/) :
	QDialog(pParent, Qt::FramelessWindowHint),
	ui(new Ui::DlgTabCycle)
{
	if(pProjTabBar == nullptr)
	{
		HyGuiLog("DlgTabCycle recieved a nullptr ProjectTabBar pointer", LOGTYPE_Error);
		return;
	}
	ui->setupUi(this);

	m_pProjectOwner = pProjTabBar->GetProjectOwner();

	QList<ProjectItemData *> cycleList = pProjTabBar->GetCycleOrder();
	for(int i = 0; i < cycleList.size(); ++i)
	{
		QListWidgetItem *pCycleItem = new QListWidgetItem(cycleList[i]->GetIcon(SUBICON_None), cycleList[i]->GetName(true), ui->itemList);
		pCycleItem->setData(Qt::UserRole, cycleList[i]->GetUuid());
	}
	
	if(ui->itemList->count() > 0)
	{
		if(ui->itemList->count() == 1)
			ui->itemList->setCurrentRow(0);
		else if(bSelectNext)
			ui->itemList->setCurrentRow(1);
		else
			ui->itemList->setCurrentRow(ui->itemList->count() - 1);
	}
}

DlgTabCycle::~DlgTabCycle()
{
	delete ui;
}

/*virtual*/ void DlgTabCycle::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->key() == Qt::Key_Backtab) // Shift+Tab
	{
		if(ui->itemList->currentRow() == 0)
			ui->itemList->setCurrentRow(ui->itemList->count() - 1);
		else
			ui->itemList->setCurrentRow(ui->itemList->currentRow() - 1);
	}
	if(pEvent->key() == Qt::Key_Tab)
	{
		if(ui->itemList->currentRow() == ui->itemList->count() - 1)
			ui->itemList->setCurrentRow(0);
		else
			ui->itemList->setCurrentRow(ui->itemList->currentRow() + 1);
	}
	else
		QDialog::keyPressEvent(pEvent);
}

/*virtual*/ void DlgTabCycle::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->modifiers().testFlag(Qt::ControlModifier) == false)
		close();
}

/*virtual*/ void DlgTabCycle::closeEvent(QCloseEvent *pEvent) /*override*/
{
	QListWidgetItem *pCurItem = ui->itemList->currentItem();
	if(pCurItem == nullptr)
		return;

	TreeModelItemData *pSelectedItem = m_pProjectOwner->FindItemData(pCurItem->data(Qt::UserRole).toUuid());
	MainWindow::OpenItem(static_cast<ProjectItemData *>(pSelectedItem));
}

void DlgTabCycle::on_itemList_itemClicked(QListWidgetItem *pListWidgetItem)
{
	close();
}
