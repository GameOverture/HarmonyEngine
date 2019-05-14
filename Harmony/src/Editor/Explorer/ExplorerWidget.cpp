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
#include "ProjectItemMimeData.h"
#include "ExplorerItem.h"
#include "ExplorerModel.h"
#include "AtlasWidget.h"
#include "IModel.h"
#include "DlgInputName.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QClipboard>

ExplorerProxyModel::ExplorerProxyModel(QObject *pParent /*= nullptr*/) :
	QSortFilterProxyModel(pParent)
{ }

/*virtual*/ bool ExplorerProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const /*override*/
{
	ExplorerItem *pLeftItem = sourceModel()->data(left, Qt::UserRole).value<ExplorerItem *>();
	ExplorerItem *pRightItem = sourceModel()->data(right, Qt::UserRole).value<ExplorerItem *>();

	if((pLeftItem == nullptr && pRightItem == nullptr) || (pLeftItem == nullptr && pRightItem != nullptr))
		return false;
	if(pLeftItem != nullptr && pRightItem == nullptr)
		return true;

	if(pLeftItem->GetType() == ITEM_Prefix && pRightItem->GetType() != ITEM_Prefix)
		return true;
	if(pLeftItem->GetType() != ITEM_Prefix && pRightItem->GetType() == ITEM_Prefix)
		return false;
	 
	if(pLeftItem->GetType() != pRightItem->GetType())
		return pLeftItem->GetType() < pRightItem->GetType();

	return QString::localeAwareCompare(pLeftItem->GetName(false), pRightItem->GetName(false)) < 0;
}

///*virtual*/ void DataExplorerLoadThread::run() /*override*/
//{
//    /* ... here is the expensive or blocking operation ... */
//    Project *pNewItemProject = new Project(nullptr, m_sPath);
//    Q_EMIT LoadFinished(pNewItemProject);
//}

ExplorerWidget::ExplorerWidget(QWidget *pParent) :
	QWidget(pParent),
	ui(new Ui::ExplorerWidget)
{
	ui->setupUi(this);
	setAcceptDrops(true);

	ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->treeView->setDragDropMode(QAbstractItemView::DragDrop);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setAcceptDrops(true);
	ui->treeView->setDropIndicatorShown(true);
	ui->treeView->setSortingEnabled(true);
	ui->treeView->sortByColumn(0, Qt::AscendingOrder);
	ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));

	ui->actionCopyItem->setEnabled(false);
	ui->actionPasteItem->setEnabled(false);
}

ExplorerWidget::~ExplorerWidget()
{
	delete ui;
}

void ExplorerWidget::SetModel(ExplorerModel &modelRef)
{
	ExplorerProxyModel *pProxyModel = new ExplorerProxyModel(this);
	pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	pProxyModel->setSourceModel(&modelRef);

	pProxyModel->setFilterRegExp(QRegExp("^[^+]*$", Qt::CaseInsensitive));
	pProxyModel->setFilterKeyColumn(0);

	ui->treeView->setModel(pProxyModel);
}

ExplorerModel *ExplorerWidget::GetSourceModel()
{
	if(ui->treeView->model() == nullptr)
		return nullptr;

	return static_cast<ExplorerModel *>(static_cast<ExplorerProxyModel *>(ui->treeView->model())->sourceModel());
}

ExplorerItem *ExplorerWidget::GetFirstSelectedItem()
{
	QModelIndex curIndex = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapToSource(ui->treeView->selectionModel()->currentIndex());
	if(curIndex.isValid() == false)
		return nullptr;

	return GetSourceModel()->data(curIndex, Qt::UserRole).value<ExplorerItem *>();
}

void ExplorerWidget::GetSelectedItems(QList<ExplorerItem *> &selectedItemsOut, QList<ExplorerItem *> &selectedPrefixesOut)
{
	selectedItemsOut.clear();
	selectedPrefixesOut.clear();

	QItemSelection selectedItems = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapSelectionToSource(ui->treeView->selectionModel()->selection());
	QModelIndexList selectedIndices = selectedItems.indexes();
	for(int i = 0; i < selectedIndices.size(); ++i)
		selectedItemsOut += GetSourceModel()->GetItemsRecursively(selectedIndices[i]);
	
	// Poor man's unique only algorithm
	selectedItemsOut = selectedItemsOut.toSet().toList();

	for(int i = 0; i < selectedItemsOut.size();)
	{
		if(selectedItemsOut[i]->GetType() == ITEM_Prefix)
			selectedPrefixesOut.push_back(selectedItemsOut.takeAt(i));
		else
			++i;
	}
}

void ExplorerWidget::OnContextMenu(const QPoint &pos)
{
	ExplorerItem *pContextExplorerItem = GetFirstSelectedItem();//ui->treeView->model()->data(index, Qt::UserRole).value<ExplorerItem *>();
	//QModelIndex index = ui->treeView->indexAt(pos);
	
	QMenu contextMenu;
	if(pContextExplorerItem == nullptr)
	{
		contextMenu.addAction(FINDACTION("actionNewProject"));
		contextMenu.addAction(FINDACTION("actionOpenProject"));
	}
	else
	{
		QList<ExplorerItem *> selectedItems, selectedPrefixes;
		GetSelectedItems(selectedItems, selectedPrefixes);

		switch(pContextExplorerItem->GetType())
		{
		case ITEM_Project:
			if(Harmony::GetProject() != pContextExplorerItem)
				contextMenu.addAction(FINDACTION("actionLoadProject"));
			else
				contextMenu.addMenu(MainWindow::GetNewItemMenu());
			contextMenu.addSeparator();
			contextMenu.addAction(FINDACTION("actionCloseProject"));
			contextMenu.addAction(FINDACTION("actionProjectSettings"));
			break;
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Font:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_Shader:
		case ITEM_Entity:
		case ITEM_Prefab:
		case ITEM_Prefix:
			if(Harmony::GetProject() != &pContextExplorerItem->GetProject())
			{
				contextMenu.addAction(FINDACTION("actionLoadProject"));
				contextMenu.addSeparator();
			}
			else
			{
				if(selectedItems.count() > 0)
				{
					if(selectedItems.count() > 1)
					{
						ui->actionOpen->setText("Open Selected Items");
						ui->actionOpen->setIcon(HyGlobal::ItemIcon(ITEM_Prefix, SUBICON_None));
					}
					else
					{
						ui->actionOpen->setText("Open " % pContextExplorerItem->GetName(false));
						ui->actionOpen->setIcon(HyGlobal::ItemIcon(pContextExplorerItem->GetType(), SUBICON_None));
					}
					contextMenu.addAction(ui->actionOpen);
					contextMenu.addSeparator();
				}
				
				contextMenu.addMenu(MainWindow::GetNewItemMenu());
				contextMenu.addSeparator();
			}
			
			if(selectedItems.count() + selectedPrefixes.count() == 1)
				contextMenu.addAction(ui->actionRename);
			contextMenu.addAction(ui->actionCopyItem);
			contextMenu.addAction(ui->actionPasteItem);
			contextMenu.addSeparator();
			if(selectedItems.count() + selectedPrefixes.count() == 1)
			{
				ui->actionDeleteItem->setText("Delete " % pContextExplorerItem->GetName(false));
				ui->actionDeleteItem->setIcon(HyGlobal::ItemIcon(pContextExplorerItem->GetType(), SUBICON_Delete));
			}
			else
			{
				ui->actionDeleteItem->setText("Delete Selected Items");
				ui->actionDeleteItem->setIcon(HyGlobal::ItemIcon(ITEM_Prefix, SUBICON_Delete));
			}
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
	FINDACTION("actionNewPrefix")->setEnabled(bValidItem);
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
	
	if(Harmony::GetProject() == nullptr && bValidItem)
		Harmony::SetProject(&pCurSelected->GetProject());
}

void ExplorerWidget::on_actionRename_triggered()
{
	ExplorerItem *pItem = GetFirstSelectedItem();
	if(pItem == nullptr)
	{
		HyGuiLog("on_actionRename_triggered() was invoked on a nullptr ExplorerItem *", LOGTYPE_Error);
		return;
	}
	
	DlgInputName *pDlg = new DlgInputName(HyGlobal::ItemName(pItem->GetType(), false), pItem->GetName(false));
	if(pDlg->exec() == QDialog::Accepted)
		pItem->Rename(pDlg->GetName());

	delete pDlg;
}

void ExplorerWidget::on_actionDeleteItem_triggered()
{
	QList<ExplorerItem *> selectedItems, selectedPrefixes;
	GetSelectedItems(selectedItems, selectedPrefixes);
	if(selectedItems.size() + selectedPrefixes.size() == 0)
	{
		HyGuiLog("on_actionDeleteItem_triggered() was invoked on nothing selected", LOGTYPE_Error);
		return;
	}

	QString sDeleteMsg = "Do you want to delete the ";
	if(selectedItems.size() == 1)
		sDeleteMsg += HyGlobal::ItemName(selectedItems[0]->GetType(), false) % ":\n" % selectedItems[0]->GetName(true) % "?";
	else if(selectedItems.size() == 0)
	{
		sDeleteMsg += QString("Prefix") % (selectedPrefixes.size() > 1 ? "es" : "") % ":\n";
		for(int i = 0; i < selectedPrefixes.size(); ++i)
			sDeleteMsg += selectedPrefixes[i]->GetName(true) % "\n";
	}
	else
	{
		sDeleteMsg += "following items:\n";
		for(int i = 0; i < selectedItems.size(); ++i)
			sDeleteMsg += selectedItems[i]->GetName(true) % "\n";
	}
	
	sDeleteMsg +="\n\nThis action cannot be undone.";
	if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", sDeleteMsg, QMessageBox::Yes, QMessageBox::No))
	{
		for(int i = 0; i < selectedItems.size(); ++i)
		{
			if(GetSourceModel()->RemoveItem(selectedItems[i]) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % selectedItems[i]->GetName(true), LOGTYPE_Error);
		}

		for(int i = 0; i < selectedPrefixes.size(); ++i)
		{
			if(GetSourceModel()->RemoveItem(selectedItems[i]) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % selectedItems[i]->GetName(true), LOGTYPE_Error);
		}
	}
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
	QList<ExplorerItem *> selectedItems, selectedPrefixes;
	GetSelectedItems(selectedItems, selectedPrefixes);

	if(selectedItems.empty())
	{
		HyGuiLog("ExplorerWidget::on_actionCopyItem_triggered - empty or unsupported item", LOGTYPE_Error);
		return;
	}

	ProjectItemMimeData *pNewMimeData = new ProjectItemMimeData(selectedItems);
	QClipboard *pClipboard = QApplication::clipboard();
	pClipboard->setMimeData(pNewMimeData);

	for(int i = 0; i < selectedItems.size(); ++i)
	{
		if(selectedItems[i]->IsProjectItem())
			HyGuiLog("Copied " % HyGlobal::ItemName(selectedItems[i]->GetType(), false) % " item (" % selectedItems[i]->GetName(true) % ") to the clipboard.", LOGTYPE_Normal);
	}

	ui->actionPasteItem->setEnabled(true);
}

void ExplorerWidget::on_actionPasteItem_triggered()
{
	QClipboard *pClipboard = QApplication::clipboard();
	const QMimeData *pMimeData = pClipboard->mimeData();

	if(pMimeData->hasFormat(HYGUI_MIMETYPE))
		GetSourceModel()->PasteItemSrc(pMimeData->data(HYGUI_MIMETYPE), pCurProj, sPrefixOverride);
}

void ExplorerWidget::on_actionOpen_triggered()
{
	QList<ExplorerItem *> selectedItems, selectedPrefixes;
	GetSelectedItems(selectedItems, selectedPrefixes);
	if(selectedItems.size() == 0)
	{
		HyGuiLog("on_actionOpen_triggered() was invoked on no item selected", LOGTYPE_Error);
		return;
	}

	for(int i = 0; i < selectedItems.size(); ++i)
	{
		if(selectedItems[i]->IsProjectItem())
		{
			if(Harmony::GetProject() == &selectedItems[i]->GetProject())
				MainWindow::OpenItem(static_cast<ProjectItem *>(selectedItems[i]));
			else
				HyGuiLog("Cannot open item '" % selectedItems[i]->GetName(true) % "' because it doesn't belong to the currently active project.", LOGTYPE_Normal);
		}
	}
}
