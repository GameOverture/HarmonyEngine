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
	QSortFilterProxyModel(pParent),
	m_uiFilterFlags(ITEMFILTER_All)
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

/*virtual*/ bool ExplorerProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const /*override*/
{
	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
	ExplorerItemData *pItemData = sourceModel()->data(index, Qt::UserRole).value<ExplorerItemData *>();

	if(pItemData)
	{
		if(pItemData->GetType() == ITEM_Project)
			return true;

		QRegExp searchFilter = filterRegExp();
		if(pItemData->GetType() == ITEM_Prefix)
		{
			QList<TreeModelItemData *> itemsInFilterList = static_cast<ExplorerModel *>(sourceModel())->GetItemsRecursively(index);
			if(itemsInFilterList.size() == 1)
				return (m_uiFilterFlags == ITEMFILTER_All);

			for(auto pItem : itemsInFilterList)
			{
				if(pItem->GetType() != ITEM_Prefix)
				{
					if(IsTypeFiltered(pItem->GetType()) &&
						(searchFilter.isValid() == false || pItem->GetText().contains(searchFilter)))
					{
						return true;
					}
				}
			}
			return false;
		}
		else if(IsTypeFiltered(pItemData->GetType()) == false ||
			(searchFilter.isValid() && pItemData->GetText().contains(searchFilter) == false))
		{
			return false;
		}
		else
			return true;
	}

	return false;
}

void ExplorerProxyModel::FilterByType(uint32 uiFilterFlags)
{
	m_uiFilterFlags = uiFilterFlags;
	invalidateFilter();
}

bool ExplorerProxyModel::IsTypeFiltered(ItemType eType) const
{
	switch(eType)
	{
	case ITEM_Project:
	case ITEM_Prefix:
		return true;
	
	case ITEM_Audio:
		return (m_uiFilterFlags & ITEMFILTER_Audio) != 0;

	case ITEM_Text:
		return (m_uiFilterFlags & ITEMFILTER_Text) != 0;

	case ITEM_Spine:
		return (m_uiFilterFlags & ITEMFILTER_Spine) != 0;

	case ITEM_Sprite:
		return (m_uiFilterFlags & ITEMFILTER_Sprite) != 0;

	case ITEM_Entity:
		return (m_uiFilterFlags & ITEMFILTER_Entity) != 0;

	default:
		HyGuiLog("ExplorerProxyModel::IsTypeFiltered() encountered an unknown ItemType", LOGTYPE_Error);
		break;
	}

	return false;
}

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

	QPixmap pixmap = indexes.first().data(Qt::DecorationRole).value<QPixmap>();
	QDrag *pDrag = new QDrag(this);
	pDrag->setPixmap(pixmap);
	pDrag->setMimeData(pMimeData);
	pDrag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

	Qt::DropAction eDropAction = pDrag->exec(supportedActions);
	//if(eDropAction != Qt::MoveAction)
	//{
	//	supportedActions &= ~Qt::MoveAction;
	//	eDropAction = pDrag->exec(supportedActions);
	//}




	//QList<QPersistentModelIndex> persistentIndexes;
	//for(int i = 0; i < indexes.count(); i++)
	//	persistentIndexes.append(QPersistentModelIndex(indexes.at(i)));

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
	ui(new Ui::ExplorerWidget),
	m_FilterActionGroup(this)
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

	ui->btnFilterAll->setDefaultAction(ui->actionFilterAll);
	ui->btnFilterSprite->setDefaultAction(ui->actionFilterSprite);
	ui->btnFilterText->setDefaultAction(ui->actionFilterText);
	ui->btnFilterSpine->setDefaultAction(ui->actionFilterSpine);
	ui->btnFilterAudio->setDefaultAction(ui->actionFilterAudio);
	ui->btnFilterEntity->setDefaultAction(ui->actionFilterEntity);

	m_FilterActionGroup.addAction(ui->actionFilterAll);
	m_FilterActionGroup.addAction(ui->actionFilterSprite);
	m_FilterActionGroup.addAction(ui->actionFilterText);
	m_FilterActionGroup.addAction(ui->actionFilterSpine);
	m_FilterActionGroup.addAction(ui->actionFilterAudio);
	m_FilterActionGroup.addAction(ui->actionFilterEntity);

	// TODO: Load from settings
	ui->actionFilterAll->setChecked(true);
	ui->lblActiveFilter->setVisible(false);
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

//ExplorerItemData *ExplorerWidget::GetFirstSelectedItem()
//{
//	QModelIndex curIndex = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapToSource(ui->treeView->selectionModel()->currentIndex());
//	if(curIndex.isValid() == false)
//		return nullptr;
//
//	return GetExplorerModel()->data(curIndex, Qt::UserRole).value<ExplorerItemData *>();
//}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE: ManagerWidget::GetSelected is a synonymous function - all fixes/enhancements should be copied over until refactored into a base class
ExplorerItemData *ExplorerWidget::GetSelected()
{
	ExplorerItemData *pFirstItemSelected = nullptr;
	QModelIndex curIndex = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapToSource(ui->treeView->selectionModel()->currentIndex());
	if(curIndex.isValid())
		pFirstItemSelected = GetExplorerModel()->data(curIndex, Qt::UserRole).value<ExplorerItemData *>();
	return pFirstItemSelected;
}
void ExplorerWidget::GetSelected(QList<ProjectItemData *> &selectedItemsOut, QList<ExplorerItemData *> &selectedPrefixesOut, bool bSortAlphabetically)
{
	selectedItemsOut.clear();
	selectedPrefixesOut.clear();
	QItemSelection selectedItems = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapSelectionToSource(ui->treeView->selectionModel()->selection());
	QModelIndexList selectedIndices = selectedItems.indexes();

	QList<TreeModelItemData *> itemList;
	for(int i = 0; i < selectedIndices.size(); ++i)
	{
		if(selectedIndices[i].column() != 0)
			continue;

		itemList += GetExplorerModel()->GetItemsRecursively(selectedIndices[i]);
	}

	// Separate out items and prefixes to their own respective lists, while ignoring any duplicate items while preserving the order in 'itemList'
	QSet<TreeModelItemData *> seenItemSet;
	for(int i = 0; i < itemList.size(); ++i)
	{
		if(seenItemSet.contains(itemList[i]))
			continue;
		seenItemSet.insert(itemList[i]);

		if(itemList[i]->GetType() == ITEM_Prefix)
			selectedPrefixesOut.append(static_cast<ExplorerItemData *>(itemList[i]));
		else
			selectedItemsOut.append(static_cast<ProjectItemData *>(itemList[i]));
	}

	// The items within 'selectedItemsOut' and 'selectedPrefixesOut' are not sorted. Sort them either by row depth or alphabetically
	if(bSortAlphabetically)
	{
		std::sort(selectedItemsOut.begin(), selectedItemsOut.end(), [](ProjectItemData *pA, ProjectItemData *pB) {
			return QString::localeAwareCompare(pA->GetName(true), pB->GetName(true)) < 0;
			});
		std::sort(selectedPrefixesOut.begin(), selectedPrefixesOut.end(), [](ExplorerItemData *pA, ExplorerItemData *pB) {
			return QString::localeAwareCompare(pA->GetName(true), pB->GetName(true)) < 0;
			});
	}
	else // Sort by row depth
	{
		// Sort 'selectedItemsOut' by row depth so that we can process the deepest items first
		std::sort(selectedItemsOut.begin(), selectedItemsOut.end(), [&](TreeModelItemData *pA, TreeModelItemData *pB) {
			return GetExplorerModel()->CalculateDepth(pA) > GetExplorerModel()->CalculateDepth(pB);
			});
		std::sort(selectedPrefixesOut.begin(), selectedPrefixesOut.end(), [&](TreeModelItemData *pA, TreeModelItemData *pB) {
			return GetExplorerModel()->CalculateDepth(pA) > GetExplorerModel()->CalculateDepth(pB);
			});
	}
}

void ExplorerWidget::OnFilterUpdate()
{
	uint32 uiFilter = 0;
	if(ui->btnFilterAll->isChecked())
	{
		ui->lblActiveFilter->setVisible(false);
		uiFilter |= ITEMFILTER_All;
	}
	else if(ui->btnFilterAudio->isChecked())
	{
		ui->lblActiveFilter->setVisible(true);
		ui->lblActiveFilter->setText("Show Only " + HyGlobal::ItemName(ITEM_Audio, true));
		uiFilter |= ITEMFILTER_Audio;
	}
	else if(ui->btnFilterEntity->isChecked())
	{
		ui->lblActiveFilter->setVisible(true);
		ui->lblActiveFilter->setText("Show Only " + HyGlobal::ItemName(ITEM_Entity, true));
		uiFilter |= ITEMFILTER_Entity;
	}
	else if(ui->btnFilterSpine->isChecked())
	{
		ui->lblActiveFilter->setVisible(true);
		ui->lblActiveFilter->setText("Show Only " + HyGlobal::ItemName(ITEM_Spine, true));
		uiFilter |= ITEMFILTER_Spine;
	}
	else if(ui->btnFilterSprite->isChecked())
	{
		ui->lblActiveFilter->setVisible(true);
		ui->lblActiveFilter->setText("Show Only " + HyGlobal::ItemName(ITEM_Sprite, true));
		uiFilter |= ITEMFILTER_Sprite;
	}
	else if(ui->btnFilterText->isChecked())
	{
		ui->lblActiveFilter->setVisible(true);
		ui->lblActiveFilter->setText("Show Only " + HyGlobal::ItemName(ITEM_Text, true));
		uiFilter |= ITEMFILTER_Text;
	}

	static_cast<ExplorerProxyModel *>(ui->treeView->model())->FilterByType(uiFilter);
}

void ExplorerWidget::OnContextMenu(const QPoint &pos)
{
	ExplorerItemData *pContextExplorerItem = GetSelected();
	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	GetSelected(selectedItems, selectedPrefixes, true);

	bool bPreserveCloseProjectEnabled = FINDACTION("actionCloseProject")->isEnabled();
	
	QMenu contextMenu;
	if(pContextExplorerItem == nullptr)
	{
		contextMenu.addAction(FINDACTION("actionNewProject"));
		contextMenu.addAction(FINDACTION("actionOpenProject"));
	}
	else
	{
		switch(pContextExplorerItem->GetType())
		{
		case ITEM_Project:
			if(Harmony::GetProject() != pContextExplorerItem)
				contextMenu.addAction(FINDACTION("actionActivateProject"));
			else
				contextMenu.addMenu(MainWindow::GetNewItemMenu());
			contextMenu.addSeparator();
			FINDACTION("actionCloseProject")->setEnabled(true);
			contextMenu.addAction(FINDACTION("actionCloseProject"));
			contextMenu.addAction(FINDACTION("actionProjectSettings"));
			contextMenu.addSeparator();
			contextMenu.addAction(FINDACTION("actionOpenFolderExplorer"));
			contextMenu.addSeparator();
			contextMenu.addAction(ui->actionPasteItem);
			break;
		case ITEM_Audio:
		case ITEM_Particles:
		case ITEM_Text:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_Source:
		case ITEM_Header:
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

	FINDACTION("actionCloseProject")->setEnabled(bPreserveCloseProjectEnabled);
}

void ExplorerWidget::on_actionFilterAll_triggered()
{
	OnFilterUpdate();
}

void ExplorerWidget::on_actionFilterSprite_triggered()
{
	OnFilterUpdate();
}

void ExplorerWidget::on_actionFilterText_triggered()
{
	OnFilterUpdate();
}

void ExplorerWidget::on_actionFilterSpine_triggered()
{
	OnFilterUpdate();
}

void ExplorerWidget::on_actionFilterAudio_triggered()
{
	OnFilterUpdate();
}

void ExplorerWidget::on_actionFilterEntity_triggered()
{
	OnFilterUpdate();
}

void ExplorerWidget::on_txtSearch_textChanged(const QString &text)
{
	static_cast<ExplorerProxyModel *>(ui->treeView->model())->setFilterWildcard(text);
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
	case ITEM_Source:
	case ITEM_Header:
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
	FINDACTION("actionNewPrefix")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewText")->setEnabled(bValidItem);
	FINDACTION("actionNewSprite")->setEnabled(bValidItem);
	FINDACTION("actionNewSpine")->setEnabled(bValidItem);
	FINDACTION("actionNewParticle")->setEnabled(bValidItem);
	FINDACTION("actionNewAudio")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity")->setEnabled(bValidItem);
	FINDACTION("actionNewPrefab")->setEnabled(bValidItem);
	FINDACTION("actionNewEntity3d")->setEnabled(bValidItem);
	FINDACTION("actionBuildSettings")->setEnabled(bValidItem);
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
		case ITEM_Source:
		case ITEM_Header:
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
	ui->actionPasteItem->setEnabled(pMimeData && pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)));
	
	if(Harmony::GetProject() == nullptr && bValidItem)
		Harmony::SetProject(&pCurSelected->GetProject());

	IWidget *pItemProperties = MainWindow::GetItemProperties();
	if(pItemProperties)
		pItemProperties->UpdateActions();
}

void ExplorerWidget::on_actionRename_triggered()
{
	ExplorerItemData *pFirstSelected = GetSelected();
	if(pFirstSelected == nullptr)
	{
		HyGuiLog("on_actionRename_triggered() was invoked on a nullptr ExplorerItemData *", LOGTYPE_Error);
		return;
	}
	
	DlgInputName *pDlg = new DlgInputName(HyGlobal::ItemName(pFirstSelected->GetType(), false), pFirstSelected->GetName(false), HyGlobal::CodeNameValidator(), nullptr, nullptr);
	if(pDlg->exec() == QDialog::Accepted)
		pFirstSelected->Rename(pDlg->GetName());

	delete pDlg;
}

void ExplorerWidget::on_actionDeleteItem_triggered()
{
	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	GetSelected(selectedItems, selectedPrefixes, false); // False so it's sorted by depth when deleting
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

			QList<TreeModelItemData *> dependantList = pProjItem->GetDependants();
			if(dependantList.empty() == false)
			{
				QString sMessage = "'" % selectedItems[i]->GetName(true) % "' cannot be deleted because it is in use by the following items: \n\n";
				for(TreeModelItemData *pDependant : dependantList)
				{
					if(pDependant->IsProjectItem())
						sMessage.append(HyGlobal::ItemName(pDependant->GetType(), false) % " - " % static_cast<ProjectItemData *>(pDependant)->GetName(true) % "\n");
					else
						sMessage.append(HyGlobal::ItemName(pDependant->GetType(), false) % " - " % pDependant->GetText() % "\n");
				}

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
		sDeleteMsg += QString("Prefix") % (selectedPrefixes.size() > 1 ? "es" : "") % ":\n\n";
		for(int i = 0; i < selectedPrefixes.size(); ++i)
			sDeleteMsg += selectedPrefixes[i]->GetName(true) % "\n";
	}
	else
	{
		sDeleteMsg += "following items:\n\n";
		for(int i = 0; i < selectedItems.size(); ++i)
			sDeleteMsg += selectedItems[i]->GetName(true) % "\n";
	}
	
	//sDeleteMsg +="\n\nThis action cannot be undone.";
	if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", sDeleteMsg, QMessageBox::Yes, QMessageBox::No))
	{
		for(int i = 0; i < selectedItems.size(); ++i)
		{
			if(GetExplorerModel()->RemoveItem(selectedItems[i]) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % selectedItems[i]->GetName(true), LOGTYPE_Error);
		}

		for(int i = 0; i < selectedPrefixes.size(); ++i)
		{
			if(GetExplorerModel()->RemoveItem(selectedPrefixes[i]) == false)
				HyGuiLog("ExplorerModel::RemoveItem returned false on: " % selectedPrefixes[i]->GetName(true), LOGTYPE_Error);
		}
	}
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	GetSelected(selectedItems, selectedPrefixes, true);

	if(selectedItems.empty())
	{
		HyGuiLog("ExplorerWidget::on_actionCopyItem_triggered - empty or unsupported item", LOGTYPE_Error);
		return;
	}

	QList<ExplorerItemData *> treeItemList;
	for(auto item : selectedItems)
		treeItemList << item;
	ProjectItemMimeData *pNewMimeData = new ProjectItemMimeData(treeItemList);
	QClipboard *pClipboard = QApplication::clipboard();
	pClipboard->setMimeData(pNewMimeData);

	HyGuiLog("Copied...", LOGTYPE_Normal);
	bool bNothing = true;
	for(int i = 0; i < selectedItems.size(); ++i)
	{
		if(selectedItems[i]->IsProjectItem())
		{
			HyGuiLog(HyGlobal::ItemName(selectedItems[i]->GetType(), false) % " item (" % selectedItems[i]->GetName(true) % ")", LOGTYPE_Normal);
			bNothing = false;
		}
	}
	if(bNothing)
		HyGuiLog("NOTHING!", LOGTYPE_Normal);
	HyGuiLog("...to the clipboard.", LOGTYPE_Normal);

	ui->actionPasteItem->setEnabled(true);
}

void ExplorerWidget::on_actionPasteItem_triggered()
{
	QClipboard *pClipboard = QApplication::clipboard();
	const QMimeData *pMimeData = pClipboard->mimeData();

	if(pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)))
	{
		QModelIndex curIndex = static_cast<ExplorerProxyModel *>(ui->treeView->model())->mapToSource(ui->treeView->selectionModel()->currentIndex());
		if(curIndex.isValid())
		{
			if(GetExplorerModel()->PasteItemSrc(static_cast<const ProjectItemMimeData *>(pMimeData), curIndex) == false)
				HyGuiLog("ExplorerModel::PasteItemSrc returned false", LOGTYPE_Error);
		}
	}
}

void ExplorerWidget::on_actionOpen_triggered()
{
	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	GetSelected(selectedItems, selectedPrefixes, false);
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
