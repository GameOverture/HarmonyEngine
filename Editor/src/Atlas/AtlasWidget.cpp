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
#include "ExplorerItemData.h"

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

//AtlasTreeWidget::AtlasTreeWidget(QWidget *parent /*= Q_NULLPTR*/) :
//	QTreeWidget(parent)
//{
//}
//void AtlasTreeWidget::SetAtlasOwner(AtlasWidget *pOwner)
//{
//	m_pOwner = pOwner;
//}
///*virtual*/ void AtlasTreeWidget::dropEvent(QDropEvent *e)
//{
//	QTreeWidget::dropEvent(e);
//
//	sortItems(0, Qt::AscendingOrder);
//	m_pOwner->GetData().WriteMetaSettings();
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//bool AtlasTreeItem::operator<(const QTreeWidgetItem &rhs) const
//{
//	bool bLeftIsFilter = this->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;
//	bool bRightIsFilter = rhs.data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;
//
//	if(bLeftIsFilter && bRightIsFilter == false)
//		return true;
//	if(bLeftIsFilter == false && bRightIsFilter)
//		return false;
//
//	return this->text(0) < rhs.text(0);
//}
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









void AtlasWidget::on_actionDeleteImages_triggered()
{

}

void AtlasWidget::on_actionReplaceImages_triggered()
{

}

void AtlasWidget::on_atlasList_itemSelectionChanged()
{

}

void AtlasWidget::on_actionRename_triggered()
{

}

void AtlasWidget::on_cmbAtlasGroups_currentIndexChanged(int index)
{
	
}

void AtlasWidget::on_actionAddGroup_triggered()
{

}

void AtlasWidget::on_actionGroupSettings_triggered()
{
	
}

void AtlasWidget::on_actionRemoveGroup_triggered()
{

}

void AtlasWidget::on_actionAtlasGrpTransfer_triggered(QAction *pAction)
{

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

	QSet<AtlasFrame *> importedImagesSet = m_pModel->ImportImages(sImportImgList, m_pModel->GetAtlasGrpIdFromAtlasGrpIndex(ui->cmbAtlasGroups->currentIndex()), ITEM_AtlasImage, correspondingParentList);

	if(sImportImgList.empty() == false && importedImagesSet.empty() == false)
	{
		m_pModel->Repack(ui->cmbAtlasGroups->currentIndex(),
						 QSet<int>(),
						 importedImagesSet);
	}
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

	QSet<AtlasFrame *> importedImagesSet = m_pModel->ImportImages(sImportImgList, m_pModel->GetAtlasGrpIdFromAtlasGrpIndex(ui->cmbAtlasGroups->currentIndex()), ITEM_AtlasImage, correspondingParentList);

	if(sImportImgList.empty() == false && importedImagesSet.empty() == false)
	{
		m_pModel->Repack(ui->cmbAtlasGroups->currentIndex(),
						 QSet<int>(),
						 importedImagesSet);
	}
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

