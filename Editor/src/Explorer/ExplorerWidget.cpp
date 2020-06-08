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
#include "ExplorerItemData.h"
#include "ExplorerModel.h"
#include "IModel.h"
#include "DlgInputName.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QClipboard>
#include <QDrag>

ExplorerProxyModel::ExplorerProxyModel(QObject *pParent /*= nullptr*/) :
	QSortFilterProxyModel(pParent)
{ }

/*virtual*/ bool ExplorerProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const /*override*/
{
	ExplorerItemData *pLeftItem = sourceModel()->data(left, Qt::UserRole).value<ExplorerItemData *>();
	ExplorerItemData *pRightItem = sourceModel()->data(right, Qt::UserRole).value<ExplorerItemData *>();

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

ExplorerTreeView::ExplorerTreeView(QWidget *pParent /*= nullptr*/) :
	QTreeView(pParent)
{ }

/*virtual*/ void ExplorerTreeView::startDrag(Qt::DropActions supportedActions) /*override*/
{
	QModelIndexList indexes = selectedIndexes();
	if(indexes.empty())
		return;

	QMimeData *pMimeData = model()->mimeData(indexes);
	if(pMimeData == nullptr)
		return;

	QList<QPersistentModelIndex> persistentIndexes;
	for (int i = 0; i<indexes.count(); i++)
		persistentIndexes.append(QPersistentModelIndex(indexes.at(i)));

	QPixmap pixmap = indexes.first().data(Qt::DecorationRole).value<QPixmap>();
	QDrag *pDrag = new QDrag(this);
	pDrag->setPixmap(pixmap);
	pDrag->setMimeData(pMimeData);
	pDrag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

	Qt::DropAction eDropAction = pDrag->exec(supportedActions);
	if(eDropAction != Qt::MoveAction)
	{
		supportedActions &= ~Qt::MoveAction;
		eDropAction = pDrag->exec(supportedActions);
	}

	//Qt::DropAction defaultDropAction = Qt::IgnoreAction;
	//if(supportedActions & Qt::MoveAction || dragDropMode() == QAbstractItemView::InternalMove)
	//	defaultDropAction = Qt::MoveAction; // was Qt::CopyAction THIS WAS THE CULPRIT!
	//else if(supportedActions & Qt::CopyAction)
	//	defaultDropAction = Qt::CopyAction;

	//if(pDrag->exec(supportedActions, defaultDropAction) == Qt::MoveAction)
	//{
	//	//when we get here any copying done in dropMimeData has messed up our selected indexes
	//	//that's why we use persistent indexes
	//	for (int i = 0; i<indexes.count(); i++){
	//		QPersistentModelIndex idx = persistentIndexes.at(i);
	//		qDebug() << "\tDragged item to delete" << i << " is: " << idx.data(NODE_TITLE).toString();
	//		qDebug() << "Row is: " << idx.row();
	//		if (idx.isValid()){ //the item is not top level
	//			model()->removeRow(idx.row(), idx.parent());
	//		}
	//		else{
	//			model()->removeRow(idx.row(), QModelIndex());
	//		}
	//	}
	//}
}


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

	// Don't display anything that starts with '+'
	pProxyModel->setFilterRegExp(QRegExp("^[^+]*$", Qt::CaseInsensitive));
	pProxyModel->setFilterKeyColumn(0);

	ui->treeView->setModel(pProxyModel);
}

ExplorerModel *ExplorerWidget::GetExplorerModel()
{
	if(ui->treeView->model() == nullptr)
		return nullptr;

	return static_cast<ExplorerModel *>(static_cast<ExplorerProxyModel *>(ui->treeView->model())->sourceModel());
}

ExplorerItemData *ExplorerWidget::GetFirstSelectedItem()
{
	QModelIndex curIndex = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapToSource(ui->treeView->selectionModel()->currentIndex());
	if(curIndex.isValid() == false)
		return nullptr;

	return GetExplorerModel()->data(curIndex, Qt::UserRole).value<ExplorerItemData *>();
}

void ExplorerWidget::GetSelectedItems(QList<ExplorerItemData *> &selectedItemsOut, QList<ExplorerItemData *> &selectedPrefixesOut)
{
	selectedItemsOut.clear();
	selectedPrefixesOut.clear();

	QItemSelection selectedItems = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapSelectionToSource(ui->treeView->selectionModel()->selection());
	QModelIndexList selectedIndices = selectedItems.indexes();
	for(int i = 0; i < selectedIndices.size(); ++i)
	{
		QList<TreeModelItemData *> itemsList = GetExplorerModel()->GetItemsRecursively(selectedIndices[i]);
		for(auto item : itemsList)
			selectedItemsOut += static_cast<ExplorerItemData *>(item);
	}
	
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
	ExplorerItemData *pContextExplorerItem = GetFirstSelectedItem();//ui->treeView->model()->data(index, Qt::UserRole).value<ExplorerItemData *>();
	//QModelIndex index = ui->treeView->indexAt(pos);
	
	QMenu contextMenu;
	if(pContextExplorerItem == nullptr)
	{
		contextMenu.addAction(FINDACTION("actionNewProject"));
		contextMenu.addAction(FINDACTION("actionOpenProject"));
	}
	else
	{
		QList<ExplorerItemData *> selectedItems, selectedPrefixes;
		GetSelectedItems(selectedItems, selectedPrefixes);

		switch(pContextExplorerItem->GetType())
		{
		case ITEM_Project:
			if(Harmony::GetProject() != pContextExplorerItem)
				contextMenu.addAction(FINDACTION("actionActivateProject"));
			else
				contextMenu.addMenu(MainWindow::GetNewItemMenu());
			contextMenu.addSeparator();
			contextMenu.addAction(FINDACTION("actionCloseProject"));
			contextMenu.addAction(FINDACTION("actionProjectSettings"));
			break;
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Text:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_Shader:
		case ITEM_Entity:
		case ITEM_Prefab:
		case ITEM_Prefix:
			if(Harmony::GetProject() != &pContextExplorerItem->GetProject())
			{
				contextMenu.addAction(FINDACTION("actionActivateProject"));
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
			HyGuiLog("ExplorerWidget::OnContextMenu - Unknown TreeModelItemData type", LOGTYPE_Error);
			} break;
		}
	}
	
	contextMenu.exec(ui->treeView->mapToGlobal(pos));
}

void ExplorerWidget::on_treeView_doubleClicked(QModelIndex index)
{
	ExplorerItemData *pItem = ui->treeView->model()->data(index, Qt::UserRole).value<ExplorerItemData *>();
	switch(pItem->GetType())
	{
	case ITEM_Project:
	case ITEM_Prefix:
		ui->treeView->isExpanded(index) ? ui->treeView->collapse(index) : ui->treeView->expand(index);
		break;
	
	case ITEM_Audio:
	case ITEM_Particles:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Shader:
	case ITEM_Entity:
	case ITEM_Prefab:
		MainWindow::OpenItem(static_cast<ProjectItemData *>(pItem));
		break;
		
	default:
		HyGuiLog("ExplorerWidget::on_treeView_itemDoubleClicked was invoked on an unknown item type:" % QString::number(pItem->GetType()), LOGTYPE_Error);
	}
}

void ExplorerWidget::on_treeView_clicked(QModelIndex index)
{
	ExplorerItemData *pCurSelected = ui->treeView->model()->data(index, Qt::UserRole).value<ExplorerItemData *>();
	bool bValidItem = (pCurSelected != nullptr);
	FINDACTION("actionProjectSettings")->setEnabled(bValidItem);
	FINDACTION("actionCloseProject")->setEnabled(bValidItem);
	FINDACTION("actionNewPrefix")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewText")->setEnabled(bValidItem);
	FINDACTION("actionNewSprite")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity")->setEnabled(bValidItem);
	FINDACTION("actionNewPrefab")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity3d")->setEnabled(bValidItem);
	FINDACTION("actionNewBuild")->setEnabled(bValidItem);

	if(pCurSelected)
	{
		switch(pCurSelected->GetType())
		{
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Text:
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

	IWidget *pItemProperties = MainWindow::GetItemProperties();
	if(pItemProperties)
		pItemProperties->UpdateActions();
}

void ExplorerWidget::on_actionRename_triggered()
{
	ExplorerItemData *pItem = GetFirstSelectedItem();
	if(pItem == nullptr)
	{
		HyGuiLog("on_actionRename_triggered() was invoked on a nullptr ExplorerItemData *", LOGTYPE_Error);
		return;
	}
	
	DlgInputName *pDlg = new DlgInputName(HyGlobal::ItemName(pItem->GetType(), false), pItem->GetName(false));
	if(pDlg->exec() == QDialog::Accepted)
		pItem->Rename(pDlg->GetName());

	delete pDlg;
}

void ExplorerWidget::on_actionDeleteItem_triggered()
{
	QList<ExplorerItemData *> selectedItems, selectedPrefixes;
	GetSelectedItems(selectedItems, selectedPrefixes);
	if(selectedItems.size() + selectedPrefixes.size() == 0)
	{
		HyGuiLog("on_actionDeleteItem_triggered() was invoked on nothing selected", LOGTYPE_Error);
		return;
	}

	// Check for dependencies
	for(int32 i = 0; i < selectedItems.size(); ++i)
	{
		if(selectedItems[i]->IsProjectItem())
		{
			ProjectItemData *pProjItem = static_cast<ProjectItemData *>(selectedItems[i]);

			auto itemOwners = pProjItem->GetProject().GetItemOwners(pProjItem);
			if(itemOwners.empty() == false)
			{
				QString sMessage = "'" % selectedItems[i]->GetName(true) % "' cannot be deleted because it is in use by the following items: \n\n";
				for(auto itemOwner : itemOwners)
					sMessage.append(HyGlobal::ItemName(itemOwner->GetType(), true) % "/" % itemOwner->GetName(true) % "\n");

				HyGuiLog(sMessage, LOGTYPE_Warning);
				return;
			}
		}
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
			if(GetExplorerModel()->RemoveItem(selectedItems[i]) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % selectedItems[i]->GetName(true), LOGTYPE_Error);
		}

		for(int i = 0; i < selectedPrefixes.size(); ++i)
		{
			if(GetExplorerModel()->RemoveItem(selectedItems[i]) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % selectedItems[i]->GetName(true), LOGTYPE_Error);
		}
	}
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
	QList<ExplorerItemData *> selectedItems, selectedPrefixes;
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
	{
		QModelIndex curIndex = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapToSource(ui->treeView->selectionModel()->currentIndex());
		if(curIndex.isValid())
			GetExplorerModel()->PasteItemSrc(pMimeData->data(HYGUI_MIMETYPE), curIndex);
	}
}

void ExplorerWidget::on_actionOpen_triggered()
{
	QList<ExplorerItemData *> selectedItems, selectedPrefixes;
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
				MainWindow::OpenItem(static_cast<ProjectItemData *>(selectedItems[i]));
			else
				HyGuiLog("Cannot open item '" % selectedItems[i]->GetName(true) % "' because it doesn't belong to the currently active project.", LOGTYPE_Normal);
		}
	}
}
