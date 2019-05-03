/**************************************************************************
 *	ExplorerWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ExplorerWidget.h"
#include "ui_ExplorerWidget.h"
#include "MainWindow.h"
#include "Harmony.h"
#include "Project.h"
#include "ExplorerItemMimeData.h"
#include "ExplorerItem.h"
#include "ExplorerModel.h"
#include "AtlasWidget.h"
#include "IModel.h"
#include "DlgInputName.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QClipboard>
#include <QSortFilterProxyModel>

///*virtual*/ void DataExplorerLoadThread::run() /*override*/
//{
//    /* ... here is the expensive or blocking operation ... */
//    Project *pNewItemProject = new Project(nullptr, m_sPath);
//    Q_EMIT LoadFinished(pNewItemProject);
//}

ExplorerWidget::ExplorerWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ExplorerWidget),
	m_pNewItemMenuRef(nullptr)
{
	ui->setupUi(this);
	setAcceptDrops(true);

	ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);//ExtendedSelection);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setAcceptDrops(true);
	ui->treeView->setDropIndicatorShown(true);
	ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	ui->actionCopyItem->setEnabled(false);
	ui->actionPasteItem->setEnabled(false);

	connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

ExplorerWidget::~ExplorerWidget()
{
	delete ui;
}

void ExplorerWidget::SetModel(ExplorerModel &modelRef)
{
	ui->treeView->setModel(&modelRef);
}

void ExplorerWidget::SetItemMenuPtr(QMenu *pMenu)
{
	m_pNewItemMenuRef = pMenu;
}

void ExplorerWidget::SelectItem(ExplorerItem *pItem)
{
	if(pItem == nullptr)
		return;

	QItemSelectionModel *pSelectionModel = ui->treeView->selectionModel();
	pSelectionModel->select(static_cast<ExplorerModel *>(ui->treeView->model())->FindIndex<ExplorerItem *>(pItem, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

Project *ExplorerWidget::GetCurProjSelected()
{
	ExplorerItem *pCurSelected = GetFirstSelectedItem();
	if(pCurSelected == nullptr)
		return nullptr;

	return &pCurSelected->GetProject();
}

ExplorerItem *ExplorerWidget::GetFirstSelectedItem()
{
	QList<ExplorerItem *> itemList = GetSelectedItems();
	if(itemList.empty())
		return nullptr;

	return itemList[0];
}

QList<ExplorerItem *> ExplorerWidget::GetSelectedItems()
{
	QItemSelectionModel *pSelectionModel = ui->treeView->selectionModel();
	QModelIndexList selectedIndices = pSelectionModel->selectedIndexes();

	QList<ExplorerItem *> retList;
	for(int i = 0; i < selectedIndices.size(); ++i)
	{
		ExplorerItem *pItem = ui->treeView->model()->data(selectedIndices[i], Qt::UserRole).value<ExplorerItem *>();
		retList.push_back(pItem);
	}

	return retList;
}

void ExplorerWidget::OnContextMenu(const QPoint &pos)
{
	QModelIndex index = ui->treeView->indexAt(pos);
	
	QMenu contextMenu;
	if(index.isValid() == false)
	{
		contextMenu.addAction(FINDACTION("actionNewProject"));
		contextMenu.addAction(FINDACTION("actionOpenProject"));
	}
	else
	{
		ExplorerItem *pSelectedExplorerItem = static_cast<ExplorerItem *>(index.internalPointer());
		HyGuiItemType eSelectedItemType = pSelectedExplorerItem->GetType();
		switch(eSelectedItemType)
		{
		case ITEM_Project: {
			contextMenu.addMenu(m_pNewItemMenuRef);
			contextMenu.addSeparator();
			contextMenu.addAction(FINDACTION("actionCloseProject"));
			contextMenu.addAction(FINDACTION("actionProjectSettings"));
			break; }
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Font:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_Shader:
		case ITEM_Entity:
		case ITEM_Prefab:
			ui->actionOpen->setText("Open " % pSelectedExplorerItem->GetName(false));
			ui->actionOpen->setIcon(HyGlobal::ItemIcon(eSelectedItemType, SUBICON_None));
			contextMenu.addAction(ui->actionOpen);
			contextMenu.addSeparator();
			contextMenu.addAction(ui->actionCopyItem);
			contextMenu.addAction(ui->actionPasteItem);
			contextMenu.addSeparator();
			// Fall through
		case ITEM_Prefix:
			if(eSelectedItemType == ITEM_Prefix)
			{
				contextMenu.addMenu(m_pNewItemMenuRef);
				contextMenu.addSeparator();
			}
			contextMenu.addAction(ui->actionRename);
			ui->actionDeleteItem->setIcon(HyGlobal::ItemIcon(eSelectedItemType, SUBICON_Delete));
			ui->actionDeleteItem->setText("Delete " % pSelectedExplorerItem->GetName(false));
			contextMenu.addAction(ui->actionDeleteItem);
			break;

		default: {
			HyGuiLog("ExplorerWidget::OnContextMenu - Unknown ExplorerItem type", LOGTYPE_Error);
			} break;
		}
	}
	
	contextMenu.exec(ui->treeView->mapToGlobal(pos));
}

void ExplorerWidget::on_treeView_doubleClicked(QModelIndex index)
{
	ExplorerItem *pItem = ui->treeView->model()->data(index, Qt::UserRole).value<ExplorerItem *>();
	switch(pItem->GetType())
	{
	case ITEM_Project:
	case ITEM_Prefix:
		ui->treeView->isExpanded(index) ? ui->treeView->collapse(index) : ui->treeView->expand(index);
		break;
	
	case ITEM_Audio:
	case ITEM_Particles:
	case ITEM_Font:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Shader:
	case ITEM_Entity:
	case ITEM_Prefab:
		MainWindow::OpenItem(static_cast<ProjectItem *>(pItem));
		break;
		
	default:
		HyGuiLog("ExplorerWidget::on_treeView_itemDoubleClicked was invoked on an unknown item type:" % QString::number(pItem->GetType()), LOGTYPE_Error);
	}
}

void ExplorerWidget::on_treeView_clicked(QModelIndex index)
{
	ExplorerItem *pCurSelected = ui->treeView->model()->data(index, Qt::UserRole).value<ExplorerItem *>();
	bool bValidItem = (pCurSelected != nullptr);
	FINDACTION("actionProjectSettings")->setEnabled(bValidItem);
	FINDACTION("actionCloseProject")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewFont")->setEnabled(bValidItem);
	FINDACTION("actionNewSprite")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity")->setEnabled(bValidItem);
	FINDACTION("actionNewPrefab")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity3d")->setEnabled(bValidItem);
	FINDACTION("actionLaunchIDE")->setEnabled(bValidItem);

	if(pCurSelected)
	{
		switch(pCurSelected->GetType())
		{
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Font:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_Shader:
		case ITEM_Entity:
			ui->actionCopyItem->setEnabled(true);
			break;
		default:
			ui->actionCopyItem->setEnabled(false);
			break;
		}
	}

	QClipboard *pClipboard = QApplication::clipboard();
	const QMimeData *pMimeData = pClipboard->mimeData();
	ui->actionPasteItem->setEnabled(pMimeData && pMimeData->hasFormat(HYGUI_MIMETYPE));
	
	if(bValidItem)
		Harmony::SetProject(GetCurProjSelected());
}

void ExplorerWidget::on_actionRename_triggered()
{
	ExplorerItem *pItem = GetFirstSelectedItem();
	
	DlgInputName *pDlg = new DlgInputName(HyGlobal::ItemName(pItem->GetType(), false), pItem->GetName(false));
	if(pDlg->exec() == QDialog::Accepted)
		pItem->Rename(pDlg->GetName());

	delete pDlg;
}

void ExplorerWidget::on_actionDeleteItem_triggered()
{
	ExplorerItem *pItem = GetFirstSelectedItem();
	
	switch(pItem->GetType())
	{
	case ITEM_Prefix:
		if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the prefix:\n" % pItem->GetName(true) % "\n\nAnd all of its contents? This action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
		{
			GetCurProjSelected()->DeletePrefixAndContents(pItem->GetName(true));

			if(static_cast<ExplorerModel *>(ui->treeView->model())->RemoveItem(pItem) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % pItem->GetName(true), LOGTYPE_Error);
		}
		break;
		
	case ITEM_Audio:
	case ITEM_Particles:
	case ITEM_Font:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Shader:
	case ITEM_Entity:
	case ITEM_Prefab:
		if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the " % HyGlobal::ItemName(pItem->GetType(), false) % ":\n" % pItem->GetName(true) % "?\n\nThis action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
		{
			static_cast<ProjectItem *>(pItem)->DeleteFromProject();

			if(static_cast<ExplorerModel *>(ui->treeView->model())->RemoveItem(pItem) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % pItem->GetName(true), LOGTYPE_Error);
		}
		break;
		
	default:
		HyGuiLog("ExplorerWidget::on_actionDeleteItem_triggered was invoked on an non-item/prefix:" % QString::number(pItem->GetType()), LOGTYPE_Error);
	}
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
	ExplorerItem *pCurItemSelected = GetFirstSelectedItem();
	if(pCurItemSelected == nullptr || pCurItemSelected->IsProjectItem() == false)
	{
		HyGuiLog("ExplorerWidget::on_actionCutItem_triggered - Unsupported item:" % (pCurItemSelected ? QString::number(pCurItemSelected->GetType()) : " nullptr"), LOGTYPE_Error);
		return;
	}

	ProjectItem *pProjItem = static_cast<ProjectItem *>(pCurItemSelected);
	ExplorerItemMimeData *pNewMimeData = new ExplorerItemMimeData(pProjItem);
	QClipboard *pClipboard = QApplication::clipboard();
	pClipboard->setMimeData(pNewMimeData);
	//pClipboard->setText(pNewMimeData->data(HYGUI_MIMETYPE));

	HyGuiLog("Copied " % HyGlobal::ItemName(pCurItemSelected->GetType(), false) % " item (" % pProjItem->GetName(true) % ") to the clipboard.", LOGTYPE_Normal);
	ui->actionPasteItem->setEnabled(true);
}

void ExplorerWidget::on_actionPasteItem_triggered()
{
	Project *pCurProj = GetCurProjSelected();

	QClipboard *pClipboard = QApplication::clipboard();
	const QMimeData *pData = pClipboard->mimeData();

	QString sPrefixOverride; // Leave uninitialized for no override
	ExplorerItem *pCurSelected = GetFirstSelectedItem();
	if(pCurSelected->GetType() != ITEM_Project)
	{
		if(pCurSelected->GetType() == ITEM_Prefix)
			sPrefixOverride = pCurSelected->GetName(true);
		else
			sPrefixOverride = pCurSelected->GetPrefix();
	}

	if(pData->hasFormat(HYGUI_MIMETYPE))
		static_cast<ExplorerModel *>(ui->treeView->model())->PasteItemSrc(pData->data(HYGUI_MIMETYPE), pCurProj, sPrefixOverride);
}

void ExplorerWidget::on_actionOpen_triggered()
{
	ExplorerItem *pCurItemSelected = GetFirstSelectedItem();
	if(pCurItemSelected->IsProjectItem())
		MainWindow::OpenItem(static_cast<ProjectItem *>(pCurItemSelected));
}
