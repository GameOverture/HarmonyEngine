/**************************************************************************
 *	AtlasWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasWidget.h"
#include "ui_AtlasWidget.h"
#include "Project.h"
#include "SpriteWidget.h"
#include "ExplorerItem.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QPainter>
#include <QMessageBox>

#include "MainWindow.h"
#include "DlgInputName.h"
#include "DlgAtlasGroupSettings.h"

const float fTRANS_DUR = 0.5f;
const int iPADDING = 2;

AtlasTreeWidget::AtlasTreeWidget(QWidget *parent /*= Q_NULLPTR*/) :
	QTreeWidget(parent)
{
}
void AtlasTreeWidget::SetAtlasOwner(AtlasWidget *pOwner)
{
	m_pOwner = pOwner;
}
/*virtual*/ void AtlasTreeWidget::dropEvent(QDropEvent *e)
{
	QTreeWidget::dropEvent(e);

	sortItems(0, Qt::AscendingOrder);
	m_pOwner->GetData().WriteMetaSettings();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AtlasTreeItem::operator<(const QTreeWidgetItem &rhs) const
{
	bool bLeftIsFilter = this->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;
	bool bRightIsFilter = rhs.data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;

	if(bLeftIsFilter && bRightIsFilter == false)
		return true;
	if(bLeftIsFilter == false && bRightIsFilter)
		return false;

	return this->text(0) < rhs.text(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AtlasWidget::AtlasWidget(QWidget *pParent) :
	QWidget(pParent),
	m_pModel(nullptr),
	m_Draw(nullptr),
	ui(new Ui::AtlasWidget)
{
	ui->setupUi(this);
	
	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("AtlasWidget::AtlasWidget() invalid constructor used", LOGTYPE_Error);
}

AtlasWidget::AtlasWidget(AtlasModel *pModel, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AtlasWidget),
	m_pModel(pModel),
	m_Draw(pModel),
	m_pMouseHoverItem(nullptr)
{
	ui->setupUi(this);

	ui->actionDeleteImages->setEnabled(false);
	ui->actionReplaceImages->setEnabled(false);

	ui->btnSettings->setDefaultAction(ui->actionGroupSettings);
	ui->btnAddImages->setDefaultAction(ui->actionImportImages);
	ui->btnAddDir->setDefaultAction(ui->actionImportDirectory);

	ui->btnDeleteImages->setDefaultAction(ui->actionDeleteImages);
	ui->btnReplaceImages->setDefaultAction(ui->actionReplaceImages);
	ui->btnAddFilter->setDefaultAction(ui->actionAddFilter);
	
	ui->btnAddGroup->setDefaultAction(ui->actionAddGroup);
	ui->btnRemoveGroup->setDefaultAction(ui->actionRemoveGroup);

	ui->atlasList->SetAtlasOwner(this);
	ui->atlasList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->atlasList->setDragEnabled(true);
	ui->atlasList->setDropIndicatorShown(true);
	ui->atlasList->setDragDropMode(QAbstractItemView::InternalMove);
	//ui->atlasList->viewport()->setAcceptDrops(true);
	
	ui->cmbAtlasGroups->clear();
	ui->cmbAtlasGroups->setModel(m_pModel);

	QList<AtlasTreeItem *> atlasTreeItemList = m_pModel->GetTopLevelTreeItemList();
	for(int i = 0; i < atlasTreeItemList.size(); ++i)
		ui->atlasList->addTopLevelItem(atlasTreeItemList[i]);

	ui->atlasList->sortByColumn(0, Qt::AscendingOrder);
	ui->atlasList->setSortingEnabled(true);
	ui->atlasList->sortItems(0, Qt::AscendingOrder);
	ui->atlasList->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->atlasList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));

	//QJsonArray expandedArray = settingsObj["expanded"].toArray();
	//if(expandedArray.isEmpty() == false)
	//{
	//	for(int i = 0; i < atlasFiltersTreeItemList.size(); ++i)
	//		atlasFiltersTreeItemList[i]->setExpanded(expandedArray[i].toBool());
	//}
	
	RefreshInfo();
}

AtlasWidget::~AtlasWidget()
{
	delete ui;
}

quint32 AtlasWidget::GetSelectedAtlasGrpId()
{
	return m_pModel->GetAtlasGrpIdFromAtlasGrpIndex(ui->cmbAtlasGroups->currentIndex());
}

AtlasModel &AtlasWidget::GetData()
{
	return *m_pModel;
}

AtlasTreeWidget *AtlasWidget::GetFramesTreeWidget()
{
	return ui->atlasList;
}

void AtlasWidget::DrawUpdate()
{
	m_Draw.SetHover(ui->atlasList->itemAt(ui->atlasList->mapFromGlobal(QCursor::pos())));
	m_Draw.DrawUpdate();
}

void AtlasWidget::StashTreeWidgets()
{
	ui->atlasList->selectionModel()->clearSelection();
	
	QList<AtlasTreeItem *> stashedTreeItemList;
	while(ui->atlasList->topLevelItemCount())
		stashedTreeItemList.append(static_cast<AtlasTreeItem *>(ui->atlasList->takeTopLevelItem(0)));

	m_pModel->StashTreeWidgets(stashedTreeItemList);
}

void AtlasWidget::RefreshInfo()
{
	uint uiAtlasGrpIndex = ui->cmbAtlasGroups->currentIndex();
	
	ui->lblNumTextures->setText(QString::number(m_pModel->GetNumTextures(uiAtlasGrpIndex)));
	ui->lblTexDimensions->setText("(" % QString::number(m_pModel->GetAtlasDimensions(uiAtlasGrpIndex).width()) % "x" % QString::number(m_pModel->GetAtlasDimensions(uiAtlasGrpIndex).height()) % ")");
	ui->lblTexType->setText(HyGlobal::AtlasTextureTypeString(m_pModel->GetAtlasTextureType(uiAtlasGrpIndex)));
	
	ui->actionRemoveGroup->setEnabled(uiAtlasGrpIndex != 0);
}

/*virtual*/ void AtlasWidget::enterEvent(QEvent *pEvent) /*override*/
{
	m_Draw.Show();
	QWidget::enterEvent(pEvent);
}

/*virtual*/ void AtlasWidget::leaveEvent(QEvent *pEvent) /*override*/
{
	m_Draw.Hide();
	QWidget::leaveEvent(pEvent);
}

/*virtual*/ void AtlasWidget::resizeEvent(QResizeEvent *event) /*override*/
{
	QWidget::resizeEvent(event);

	if(ui->atlasList == nullptr)
		return;

	int iTotalWidth = ui->atlasList->size().width();
	ui->atlasList->setColumnWidth(0, iTotalWidth - 60);
}

bool AtlasWidget::DoAtlasGroupSettingsDlg()
{
	bool bAccepted = false;
	bool bAtlasGrpHasImages = m_pModel->GetFrames(ui->cmbAtlasGroups->currentIndex()).size() > 0;

	DlgAtlasGroupSettings *pDlg = new DlgAtlasGroupSettings(bAtlasGrpHasImages, m_pModel->GetPackerSettings(ui->cmbAtlasGroups->currentIndex()));
	if(QDialog::Accepted == pDlg->exec())
	{
		QJsonObject newPackerSettings = m_pModel->GetPackerSettings(ui->cmbAtlasGroups->currentIndex());
		pDlg->ApplyCurrentSettingsToObj(newPackerSettings);

		m_pModel->SetPackerSettings(ui->cmbAtlasGroups->currentIndex(), newPackerSettings);

		if(pDlg->IsSettingsDirty() && bAtlasGrpHasImages)
			m_pModel->RepackAll(ui->cmbAtlasGroups->currentIndex());
		else if(pDlg->IsNameChanged() || pDlg->IsSettingsDirty())
			m_pModel->WriteMetaSettings();

		bAccepted = true;
	}

	delete pDlg;
	RefreshInfo();

	return bAccepted;
}

void AtlasWidget::on_actionDeleteImages_triggered()
{
	QList<QTreeWidgetItem *> selectedTreeItemList = ui->atlasList->selectedItems();

	QList<QTreeWidgetItem *> selectedFrameList;
	QList<QTreeWidgetItem *> selectedFilterList;

	GetSelectedItemsRecursively(selectedTreeItemList, selectedFrameList, selectedFilterList);
	
	// First loop through and check to see if any links are present, and abort if dependecies are found
	for(int i = 0; i < selectedFrameList.count(); ++i)
	{
		AtlasFrame *pFrame = selectedFrameList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
		QSet<ProjectItem *> sLinks = pFrame->GetLinks();
		if(sLinks.empty() == false)
		{
			QString sMessage = "'" % pFrame->GetName() % "' image cannot be deleted because it is in use by the following items: \n\n";
			for(QSet<ProjectItem *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
				sMessage.append(HyGlobal::ItemName((*LinksIter)->GetType(), true) % "/" % (*LinksIter)->GetName(true) % "\n");

			HyGuiLog(sMessage, LOGTYPE_Warning);
			return;
		}
	}

	if(selectedFrameList.size() > 0)
	{
		if(QMessageBox::No == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete " % QString::number(selectedFrameList.size()) % " frames?", QMessageBox::Yes, QMessageBox::No))
			return;
	}

	// No dependencies found, resume with deleting
	QMap<uint, QSet<int> > affectedTextureIndexMap;
	for(int i = 0; i < selectedFrameList.count(); ++i)
	{
		AtlasFrame *pFrame = selectedFrameList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
		affectedTextureIndexMap[m_pModel->GetAtlasGrpIndexFromAtlasGrpId(pFrame->GetAtlasGrpId())].insert(pFrame->GetTextureIndex());

		m_pModel->RemoveFrame(pFrame);
		delete selectedFrameList[i];
	}

	if(affectedTextureIndexMap.empty() == false)
	{
		for(auto iter = affectedTextureIndexMap.begin(); iter != affectedTextureIndexMap.end(); ++iter)
			m_pModel->Repack(iter.key(), iter.value(), QSet<AtlasFrame *>());
	}

	// Delete the selected filters. When deleting a parent of another filter, it will delete its children too.
	// Make sure to just delete children filters first or else it will crash, so process 'selectedFilterList' backwards
	for(int i = selectedFilterList.size() - 1; i >= 0; --i)
		delete selectedFilterList[i];
	
	m_pModel->WriteMetaSettings();
	RefreshInfo();
}

void AtlasWidget::on_actionReplaceImages_triggered()
{
	// Query all selected frames in Atlas Manager
	QList<QTreeWidgetItem *> selectedTreeItemList = ui->atlasList->selectedItems();
	QList<QTreeWidgetItem *> selectedTreeItemFrameList;
	QList<QTreeWidgetItem *> selectedFilterList;
	GetSelectedItemsRecursively(selectedTreeItemList, selectedTreeItemFrameList, selectedFilterList);
	if(selectedFilterList.empty() == false)
	{
		HyGuiLog("Please unselect any atlas filters before replacing images", LOGTYPE_Warning);
		return;
	}
	
	ProjectTabBar *pTabBar = m_pModel->GetProjOwner()->GetTabBar();
	
	// Keep track of any linked/refrenced items as they will need to be resaved after image replacement
	QList<ProjectItem *> affectedItemList;
	for(int i = 0; i < selectedTreeItemFrameList.count(); ++i)
	{
		AtlasFrame *pFrame = selectedTreeItemFrameList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
		QSet<ProjectItem *> sLinks = pFrame->GetLinks();
		
		if(sLinks.empty() == false)
		{
			for(QSet<ProjectItem *>::iterator linksIter = sLinks.begin(); linksIter != sLinks.end(); ++linksIter)
			{
				ProjectItem *pLinkedItem = *linksIter;
				affectedItemList.append(pLinkedItem);
				
				// Abort if any of these linked items are currently opened & unsaved
				for(int i = 0; i < pTabBar->count(); ++i)
				{
					QVariant v = pTabBar->tabData(i);
					ProjectItem *pOpenItem = v.value<ProjectItem *>();
					
					if(pLinkedItem == pOpenItem && pTabBar->tabText(i).contains('*', Qt::CaseInsensitive))
					{
						QString sMessage = "'" % pFrame->GetName() % "' image cannot be replaced because an item that refrences it is currently opened and unsaved:\n" % pOpenItem->GetName(true);
						HyGuiLog(sMessage, LOGTYPE_Warning);
						return;
					}
				}
			}
		}
	}

	// Store a list of the frames, since 'selectedAtlasTreeItemList' will become invalid within Refresh()
	QList<AtlasFrame *> selectedFrameList;
	for(int i = 0; i < selectedTreeItemFrameList.count(); ++i)
		selectedFrameList.append(selectedTreeItemFrameList[i]->data(0, Qt::UserRole).value<AtlasFrame *>());

	QFileDialog dlg(this);
	dlg.setDirectory("");

	if(selectedFrameList.count() == 1)
	{
		dlg.setFileMode(QFileDialog::ExistingFile);
		dlg.setWindowTitle("Select an image as the replacement");
	}
	else
	{
		dlg.setFileMode(QFileDialog::ExistingFiles);
		dlg.setWindowTitle("Select " % QString::number(selectedFrameList.count()) % " images as replacements");
	}
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);
	QStringList sFilterList;
	sFilterList << "*.png" << "*.*";
	dlg.setNameFilters(sFilterList);

	QStringList sImportImgList;
	do
	{
		if(dlg.exec() == QDialog::Rejected)
			return;

		sImportImgList = dlg.selectedFiles();

		if(sImportImgList.count() != selectedFrameList.count())
			HyGuiLog("You must select " % QString::number(selectedFrameList.count()) % " images", LOGTYPE_Warning);
	}
	while(sImportImgList.count() != selectedFrameList.count());

	QMap<uint, QSet<int> > affectedTextureIndexMap;
	for(int i = 0; i < selectedFrameList.count(); ++i)
	{
		HyGuiLog("Replacing: " % selectedFrameList[i]->GetName() % " -> " % sImportImgList[i], LOGTYPE_Info);

		QFileInfo fileInfo(sImportImgList[i]);
		QImage newImage(fileInfo.absoluteFilePath());

		affectedTextureIndexMap[m_pModel->GetAtlasGrpIndexFromAtlasGrpId(selectedFrameList[i]->GetAtlasGrpId())].insert(selectedFrameList[i]->GetTextureIndex());
		
		m_pModel->ReplaceFrame(selectedFrameList[i], fileInfo.baseName(), newImage, false);
	}

	if(affectedTextureIndexMap.empty() == false)
	{
		for(auto iter = affectedTextureIndexMap.begin(); iter != affectedTextureIndexMap.end(); ++iter)
			m_pModel->Repack(iter.key(), iter.value(), QSet<AtlasFrame *>());
	}
	
	RefreshInfo();
	
	// Resave all affected items that had a replaced atlas frame
	for(int i = 0; i < affectedItemList.size(); ++i)
		affectedItemList[i]->Save(i == (affectedItemList.size() - 1));
}

void AtlasWidget::on_atlasList_itemSelectionChanged()
{
	QList<QTreeWidgetItem *> selectedItemList = ui->atlasList->selectedItems();

	m_Draw.SetSelected(selectedItemList);

	int iNumSelected = selectedItemList.count();
	ui->actionRename->setEnabled(iNumSelected == 1);
	ui->actionDeleteImages->setEnabled(iNumSelected != 0);
	ui->actionReplaceImages->setEnabled(iNumSelected != 0);
	
	// Determine the best suited icon based on selection
	HyGuiItemType eIconType = ITEM_Unknown;
	for(int i = 0; i < selectedItemList.size(); ++i)
	{
		if(selectedItemList[i]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
		{
			ui->actionReplaceImages->setEnabled(false);

			if(eIconType != ITEM_AtlasImage)
				eIconType = ITEM_Filter;
		}
		else
			eIconType = ITEM_AtlasImage;
	}

	if(eIconType != ITEM_Unknown)
		ui->actionDeleteImages->setIcon(HyGlobal::ItemIcon(eIconType, SUBICON_Delete));
}

void AtlasWidget::OnContextMenu(const QPoint &pos)
{
	QPoint globalPos = ui->atlasList->mapToGlobal(pos);
	QTreeWidgetItem *pTreeNode = ui->atlasList->itemAt(pos);

	QMenu contextMenu;
	QMenu atlasGrpMenu;
	
	if(pTreeNode == nullptr)
	{
		contextMenu.addAction(ui->actionImportImages);
		contextMenu.addAction(ui->actionImportDirectory);
		contextMenu.addAction(ui->actionAddFilter);
	}
	else
	{
		if(m_pModel->GetNumAtlasGroups() > 1)
		{
			atlasGrpMenu.setIcon(QIcon(":/icons16x16/atlas-assemble.png"));
			atlasGrpMenu.setTitle("Atlas Groups...");
			atlasGrpMenu.setToolTip("By organizing images into groups, you can logically partition assets to avoid unnecessary loads.");
			
			for(int i = 0; i < m_pModel->GetNumAtlasGroups(); ++i)
			{
				QAction *pActionAtlasGrpMove = new QAction();
				pActionAtlasGrpMove->setText(m_pModel->GetAtlasGroupName(i));
				pActionAtlasGrpMove->setIcon(QIcon(":/icons16x16/atlas-assemble.png"));
				pActionAtlasGrpMove->setData(m_pModel->GetAtlasGrpIdFromAtlasGrpIndex(i));
				
				atlasGrpMenu.addAction(pActionAtlasGrpMove);
			}
			
			connect(&atlasGrpMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_actionAtlasGrpTransfer_triggered(QAction*)));
			
			contextMenu.addMenu(&atlasGrpMenu);
			contextMenu.addSeparator();
		}
		
		contextMenu.addAction(ui->actionImportImages);
		contextMenu.addAction(ui->actionImportDirectory);
		contextMenu.addAction(ui->actionAddFilter);
		contextMenu.addSeparator();
		contextMenu.addAction(ui->actionDeleteImages);
		contextMenu.addAction(ui->actionReplaceImages);
		contextMenu.addAction(ui->actionRename);
	}

	contextMenu.exec(globalPos);
	
	QList<QAction *> actionAtlasGrpMoveList = atlasGrpMenu.actions();
	for(int i = 0; i < actionAtlasGrpMoveList.size(); ++i)
		delete actionAtlasGrpMoveList[i];
}

void AtlasWidget::GetSelectedItemsRecursively(QList<QTreeWidgetItem *> selectedTreeItems, QList<QTreeWidgetItem *> &frameListRef, QList<QTreeWidgetItem *> &filterListRef)
{
	for(int i = 0; i < selectedTreeItems.count(); ++i)
	{
		// First determine if any selected QTreeWidgetItem is a filter, and dig into its children if so
		if(selectedTreeItems[i]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
		{
			filterListRef.append(selectedTreeItems[i]);

			QList<QTreeWidgetItem *> filterContentsList;
			for(int j = 0; j < selectedTreeItems[i]->childCount(); ++j)
				filterContentsList.append(selectedTreeItems[i]->child(j));

			GetSelectedItemsRecursively(filterContentsList, frameListRef, filterListRef);
		}
		else
			frameListRef.append(selectedTreeItems[i]);
	}
}

void AtlasWidget::on_actionRename_triggered()
{
	QTreeWidgetItem *pSelectedItem = ui->atlasList->selectedItems()[0];

	DlgInputName *pDlg = new DlgInputName("Rename " % pSelectedItem->text(0), pSelectedItem->text(0));
	if(pDlg->exec() == QDialog::Accepted)
	{
		if(pSelectedItem->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
			pSelectedItem->setText(0, pDlg->GetName());
		else
		{
			AtlasFrame *pFrame = pSelectedItem->data(0, Qt::UserRole).value<AtlasFrame *>();
			pFrame->SetName(pDlg->GetName());

			m_pModel->WriteMetaSettings();
		}
	}

	delete pDlg;
}

void AtlasWidget::on_cmbAtlasGroups_currentIndexChanged(int index)
{
	RefreshInfo();
}

void AtlasWidget::on_actionAddGroup_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Creating New Atlas Group", "NewAtlasGroup");
	
	if(pDlg->exec() == QDialog::Accepted)
	{
		ui->cmbAtlasGroups->setCurrentIndex(m_pModel->CreateNewAtlasGrp(pDlg->GetName()));
		if(DoAtlasGroupSettingsDlg() == false)
			on_actionRemoveGroup_triggered();
	}
	
	delete pDlg;
}

void AtlasWidget::on_actionGroupSettings_triggered()
{
	DoAtlasGroupSettingsDlg();
}

void AtlasWidget::on_actionRemoveGroup_triggered()
{
	uint uiCurIndex = ui->cmbAtlasGroups->currentIndex();
	ui->cmbAtlasGroups->setCurrentIndex(0);
	
	m_pModel->RemoveAtlasGrp(m_pModel->GetAtlasGrpIdFromAtlasGrpIndex(uiCurIndex));
}

void AtlasWidget::on_actionAtlasGrpTransfer_triggered(QAction *pAction)
{
	quint32 uiNewAtlasGrpId = static_cast<quint32>(pAction->data().toInt());    // Which atlas group ID we're transfering to
	
	
	QList<QTreeWidgetItem *> selectedTreeItemList = ui->atlasList->selectedItems();
	
	QList<QTreeWidgetItem *> selectedTreeItemFrameList;
	QList<QTreeWidgetItem *> selectedFilterList;

	GetSelectedItemsRecursively(selectedTreeItemList, selectedTreeItemFrameList, selectedFilterList);

	QMap<uint, QSet<int> > affectedTextureIndexMap; // old
	QSet<AtlasFrame *> framesGoingToNewAtlasGrpSet; // new
	
	for(int i = 0; i < selectedTreeItemFrameList.count(); ++i)
	{
		AtlasFrame *pFrame = selectedTreeItemFrameList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
		if(pFrame->GetAtlasGrpId() == uiNewAtlasGrpId)
			continue;
		
		affectedTextureIndexMap[m_pModel->GetAtlasGrpIndexFromAtlasGrpId(pFrame->GetAtlasGrpId())].insert(pFrame->GetTextureIndex());
		framesGoingToNewAtlasGrpSet.insert(pFrame);
		
		m_pModel->TransferFrame(pFrame, uiNewAtlasGrpId);
	}
	
	// Repack all old affected atlas groups
	if(affectedTextureIndexMap.empty() == false)
	{
		for(auto iter = affectedTextureIndexMap.begin(); iter != affectedTextureIndexMap.end(); ++iter)
			m_pModel->Repack(iter.key(), iter.value(), QSet<AtlasFrame *>());
	}
	
	// Repack new affected atlas group
	m_pModel->Repack(m_pModel->GetAtlasGrpIndexFromAtlasGrpId(uiNewAtlasGrpId), QSet<int>(), framesGoingToNewAtlasGrpSet);
	
	RefreshInfo();
}

void AtlasWidget::on_actionImportImages_triggered()
{
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	QString sSelectedFilter(tr("PNG (*.png)"));
	QStringList sImportImgList = QFileDialog::getOpenFileNames(this,
															   "Import image(s) into atlases",
															   QString(),
															   tr("All files (*.*);;PNG (*.png)"),
															   &sSelectedFilter);

	AtlasTreeItem *pParent = nullptr;
	QList<QTreeWidgetItem *> selectedList = ui->atlasList->selectedItems();
	if(selectedList.empty() == false)
	{
		if(selectedList[0]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
			pParent = static_cast<AtlasTreeItem *>(selectedList[0]);
		else if(selectedList[0]->parent() != nullptr) // Parent tree item is always a filter
			pParent = static_cast<AtlasTreeItem *>(selectedList[0]->parent());
	}

	QList<AtlasTreeItem *> correspondingParentList;
	for(int i = 0; i < sImportImgList.size(); ++i)
		correspondingParentList.append(pParent);

	if(sImportImgList.empty() == false)
	{
		m_pModel->Repack(ui->cmbAtlasGroups->currentIndex(),
						 QSet<int>(),
						 m_pModel->ImportImages(sImportImgList, m_pModel->GetAtlasGrpIdFromAtlasGrpIndex(ui->cmbAtlasGroups->currentIndex()), ITEM_AtlasImage, correspondingParentList));
	}

	RefreshInfo();
}

void AtlasWidget::on_actionImportDirectory_triggered()
{
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::Directory);
	dlg.setOption(QFileDialog::ShowDirsOnly, true);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	if(dlg.exec() == QDialog::Rejected)
		return;

	// The 'pImportParent' will be the root point for all new AtlasTreeItem insertions (both filters and images)
	AtlasTreeItem *pImportParent = nullptr;
	QList<QTreeWidgetItem *> selectedList = ui->atlasList->selectedItems();
	if(selectedList.empty() == false)
	{
		if(selectedList[0]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
			pImportParent = static_cast<AtlasTreeItem *>(selectedList[0]);
		else if(selectedList[0]->parent() != nullptr) // Parent tree item is always a filter
			pImportParent = static_cast<AtlasTreeItem *>(selectedList[0]->parent());
	}

	// Store all the specified imported image paths and their corresponding parent tree items they should be inserted into
	QStringList sImportImgList;
	QList<AtlasTreeItem *> correspondingParentList;

	// Loop through all the specified import directories
	QStringList sDirs = dlg.selectedFiles();
	for(int iDirIndex = 0; iDirIndex < sDirs.size(); ++iDirIndex)
	{
		// Dig recursively through this directory and grab all the image files (while creating filters that resemble the folder structure they're stored in)
		QDir dirEntry(sDirs[iDirIndex]);
		AtlasTreeItem *pCurFilter = m_pModel->CreateFilter(dirEntry.dirName(), pImportParent);

		QStack<QPair<QFileInfoList, AtlasTreeItem *>> dirStack;
		dirStack.push(QPair<QFileInfoList, AtlasTreeItem *>(dirEntry.entryInfoList(), pCurFilter));

		while(dirStack.isEmpty() == false)
		{
			QPair<QFileInfoList, AtlasTreeItem *> curDir = dirStack.pop();
			QFileInfoList list = curDir.first;

			for(int i = 0; i < list.count(); i++)
			{
				QFileInfo info = list[i];
				if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
				{
					QDir subDir(info.filePath());
					dirStack.push(QPair<QFileInfoList, AtlasTreeItem *>(subDir.entryInfoList(), m_pModel->CreateFilter(subDir.dirName(), curDir.second)));
				}
				else if(info.suffix().toLower() == "png")
				{
					sImportImgList.push_back(info.filePath());
					correspondingParentList.push_back(curDir.second);
				}
			}
		}
	}

	if(sImportImgList.empty() == false)
	{
		m_pModel->Repack(ui->cmbAtlasGroups->currentIndex(),
						 QSet<int>(),
						 m_pModel->ImportImages(sImportImgList, m_pModel->GetAtlasGrpIdFromAtlasGrpIndex(ui->cmbAtlasGroups->currentIndex()), ITEM_AtlasImage, correspondingParentList));
	}

	RefreshInfo();
}

void AtlasWidget::on_actionAddFilter_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Enter Atlas Group Filter Name", "New Filter");
	if(pDlg->exec() == QDialog::Accepted)
	{
		AtlasTreeItem *pNewFilter = nullptr;

		QList<QTreeWidgetItem *> selectedItemList = ui->atlasList->selectedItems();
		if(selectedItemList.empty())
			pNewFilter = m_pModel->CreateFilter(pDlg->GetName(), nullptr);
		else
		{
			if(selectedItemList[0]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
				pNewFilter = m_pModel->CreateFilter(pDlg->GetName(), static_cast<AtlasTreeItem *>(selectedItemList[0]));
			else if(selectedItemList[0]->parent())  // Parent must either be a filter or nullptr
				pNewFilter = m_pModel->CreateFilter(pDlg->GetName(), static_cast<AtlasTreeItem *>(selectedItemList[0]->parent()));
			else
				pNewFilter = m_pModel->CreateFilter(pDlg->GetName(), nullptr);
		}

		ui->atlasList->sortItems(0, Qt::AscendingOrder);
		ui->atlasList->clearSelection();
		ui->atlasList->expandItem(pNewFilter);
		pNewFilter->setSelected(true);
	}

	delete pDlg;
}
