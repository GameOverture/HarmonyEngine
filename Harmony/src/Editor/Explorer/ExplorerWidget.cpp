/**************************************************************************
 *	ExplorerWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ExplorerWidget.h"
#include "ui_ExplorerWidget.h"

#include "Global.h"
#include "MainWindow.h"
#include "Harmony.h"
#include "FontItem.h"
#include "Project.h"
#include "ProjectItemMimeData.h"
#include "ExplorerItem.h"
#include "AtlasWidget.h"
#include "IModel.h"
#include "DlgInputName.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QClipboard>

///*virtual*/ void DataExplorerLoadThread::run() /*override*/
//{
//    /* ... here is the expensive or blocking operation ... */
//    Project *pNewItemProject = new Project(nullptr, m_sPath);
//    Q_EMIT LoadFinished(pNewItemProject);
//}

ExplorerWidget::ExplorerWidget(QWidget *parent) :	QWidget(parent),
													ui(new Ui::ExplorerWidget),
													m_pNewItemMenuRef(nullptr)
{
	ui->setupUi(this);
	ui->treeWidget->SetOwner(this);

	setAcceptDrops(true);

	ui->actionCopyItem->setEnabled(false);
	ui->actionPasteItem->setEnabled(false);

	connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

ExplorerWidget::~ExplorerWidget()
{
	delete ui;
}

void ExplorerWidget::SetItemMenuPtr(QMenu *pMenu)
{
	m_pNewItemMenuRef = pMenu;
}

Project *ExplorerWidget::AddProject(const QString sNewProjectFilePath)
{
	Project *pNewProject = new Project(this, sNewProjectFilePath);
	if(pNewProject->HasError())
	{
		HyGuiLog("Project: " % pNewProject->GetAbsPath() % " had an error and will not be opened", LOGTYPE_Error);
		delete pNewProject;
		return nullptr;
	}

	ui->treeWidget->sortItems(0, Qt::AscendingOrder);
	HyGuiLog("Opening project: " % pNewProject->GetAbsPath(), LOGTYPE_Info);

	return pNewProject;

	// BELOW BREAKS QTABBAR and UNDOSTACK SIGNAL/SLOT CONNECTIONS (I guess because QObject must be created on main thread?.. fucking waste of time)
	//
	//MainWindow::StartLoading(MDI_Explorer);
	//ExplorerLoadThread *pNewLoadThread = new ExplorerLoadThread(sNewProjectFilePath, this);
	//connect(pNewLoadThread, &ExplorerLoadThread::LoadFinished, this, &DataExplorerWidget::OnProjectLoaded);
	//connect(pNewLoadThread, &ExplorerLoadThread::finished, pNewLoadThread, &QObject::deleteLater);
	//pNewLoadThread->start();
}

ProjectItem *ExplorerWidget::AddNewItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue initValue)
{
	if(pProj == nullptr)
	{
		HyGuiLog("Could not find associated project for item: " % sPrefix % "/" % sName, LOGTYPE_Error);
		return nullptr;
	}
	
	if(eNewItemType == ITEM_Project)
	{
		HyGuiLog("Do not use WidgetExplorer::AddItem for projects... use AddProjectItem instead", LOGTYPE_Error);
		return nullptr;
	}

	QTreeWidgetItem *pParentTreeItem = pProj->GetTreeItem();

	if(sPrefix.isEmpty() == false)
	{
		QStringList sPathSplitList = sPrefix.split(QChar('/'));
		// Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
		for(int i = 0; i < sPathSplitList.size(); ++i)
		{
			bool bFound = false;
			for(int j = 0; j < pParentTreeItem->childCount(); ++j)
			{
				if(QString::compare(sPathSplitList[i], pParentTreeItem->child(j)->text(0), Qt::CaseInsensitive) == 0)
				{
					pParentTreeItem = pParentTreeItem->child(j);
					bFound = true;
					break;
				}
			}

			if(bFound == false)
			{
				// Still more directories to dig thru, so this means we're at a prefix. Add the prefix TreeItem here and continue traversing down the tree
				ExplorerItem *pPrefixItem = new ExplorerItem(ITEM_Prefix, sPathSplitList[i], pParentTreeItem);
				pParentTreeItem = pPrefixItem->GetTreeItem();
			}
		}
	}

	ProjectItem *pItem = new ProjectItem(*pProj, eNewItemType, pParentTreeItem, sName, initValue, true);

	pItem->SetTreeItemSubIcon(SUBICON_New);

	if(bOpenAfterAdd)
	{
		QTreeWidgetItem *pExpandItem = pItem->GetTreeItem();
		while(pExpandItem->parent() != nullptr)
		{
			ui->treeWidget->expandItem(pExpandItem->parent());
			pExpandItem = pExpandItem->parent();
		}
	
		MainWindow::OpenItem(pItem);
	}

	ui->treeWidget->sortItems(0, Qt::AscendingOrder);

	return pItem;
}

void ExplorerWidget::RemoveItem(ExplorerItem *pItem)
{
	if(pItem == nullptr)
		return;

	ui->treeWidget->clearSelection();
	ui->treeWidget->blockSignals(true);
	RecursiveRemoveItem(pItem);
	ui->treeWidget->blockSignals(false);
}

void ExplorerWidget::SelectItem(ExplorerItem *pItem)
{
	if(pItem == nullptr)
		return;

	for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItemIterator it(ui->treeWidget->topLevelItem(i));
		while (*it)
		{
			(*it)->setSelected(false);
			++it;
		}
	}
	
	pItem->GetTreeItem()->setSelected(true);
}

QStringList ExplorerWidget::GetOpenProjectPaths()
{
	QStringList sListOpenProjs;
	sListOpenProjs.clear();
	
	for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
	{
		ExplorerItem *pItem = ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<ExplorerItem *>();
		Project *pItemProject = static_cast<Project *>(pItem);
		sListOpenProjs.append(pItemProject->GetAbsPath());
	}
	
	return sListOpenProjs;
}

Project *ExplorerWidget::GetCurProjSelected()
{
	return HyGlobal::GetProjectFromItem(GetSelectedTreeItem());
}

ExplorerItem *ExplorerWidget::GetCurItemSelected()
{
	QTreeWidgetItem *pCurItem = GetSelectedTreeItem();
	if(pCurItem == nullptr)
		return nullptr;
	
	QVariant v = pCurItem->data(0, Qt::UserRole);
	return v.value<ExplorerItem *>();
}

ExplorerTreeWidget *ExplorerWidget::GetTreeWidget()
{
	return ui->treeWidget;
}

void ExplorerWidget::PasteItemSrc(QByteArray sSrc, Project *pProject, QString sPrefixOverride)
{
	QDir metaDir(pProject->GetMetaDataAbsPath());
	QDir metaTempDir = HyGlobal::PrepTempDir(pProject);

	QJsonDocument pasteDoc = QJsonDocument::fromJson(sSrc);
	QJsonObject pasteObj = pasteDoc.object();

	if(pasteObj["project"].toString().toLower() == pProject->GetAbsPath().toLower())
		return;

	// Determine the pasted item type
	HyGuiItemType ePasteItemType = ITEM_Unknown;
	QString sItemType = pasteObj["itemType"].toString();
	QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
	for(int i = 0; i < typeList.size(); ++i)
	{
        if(sItemType == HyGlobal::ItemName(typeList[i], false))
		{
			ePasteItemType = typeList[i];
			break;
		}
	}

	// Import any missing fonts (.ttf)
	if(ePasteItemType == ITEM_Font)
	{
		QString sFontMetaDir = metaDir.absoluteFilePath(HyGlobal::ItemName(ITEM_Font, true));
		QDir fontMetaDir(sFontMetaDir);
		fontMetaDir.mkdir(".");

		QJsonArray fontArray = pasteObj["fonts"].toArray();
		for(int i = 0; i < fontArray.size(); ++i)
		{
			QFileInfo pasteFontFileInfo(fontArray[i].toString());

			QString sAbsFilePath = pasteFontFileInfo.absoluteFilePath();
			if(QFile::copy(sAbsFilePath, sFontMetaDir % "/" % pasteFontFileInfo.fileName()))
				HyGuiLog("Paste Imported font: " % pasteFontFileInfo.fileName(), LOGTYPE_Normal);
		}
	}

	// Copy images to meta-temp dir first
	QJsonArray imageArray = pasteObj["images"].toArray();
	for(int i = 0; i < imageArray.size(); ++i)
	{
		QJsonObject imageObj = imageArray[i].toObject();

		if(pProject->GetAtlasModel().DoesImageExist(JSONOBJ_TOINT(imageObj, "checksum")) == false)
		{
			QFileInfo pasteImageFileInfo(imageObj["uri"].toString());
			QFile::copy(pasteImageFileInfo.absoluteFilePath(), metaTempDir.absolutePath() % "/" % imageObj["name"].toString() % "." % pasteImageFileInfo.suffix());
		}
	}
	// Get string list of the copied images paths
	QStringList importImageList;
	QFileInfoList importFileInfoList = metaTempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	for(int i = 0; i < importFileInfoList.size(); ++i)
		importImageList.append(importFileInfoList[i].absoluteFilePath());

	// Import images into the selected atlas group, or default one
	quint32 uiAtlasGrpId = pProject->GetAtlasModel().GetAtlasGrpIdFromAtlasGrpIndex(0);
	if(pProject->GetAtlasWidget())
		uiAtlasGrpId = pProject->GetAtlasWidget()->GetSelectedAtlasGrpId();

	// TODO: Create filters that match the source of the pasted images
	QList<AtlasTreeItem *> correspondingParentList;
	for(int i = 0; i < importImageList.size(); ++i)
		correspondingParentList.push_back(nullptr);


	// Repack this atlas group with imported images
	HyGuiItemType eType;
	switch(ePasteItemType)
	{
	case ITEM_Prefab: eType = ITEM_Prefab; break;
	case ITEM_Font: eType = ITEM_Font; break;
	default:
		eType = ITEM_AtlasImage;
		break;
	}

	QSet<AtlasFrame *> importedFramesSet = pProject->GetAtlasModel().ImportImages(importImageList, uiAtlasGrpId, eType, correspondingParentList);
	if(importedFramesSet.empty() == false)
		pProject->GetAtlasModel().Repack(pProject->GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(uiAtlasGrpId), QSet<int>(), importedFramesSet);

	// Replace any image "id" with the newly imported frames' ids
	if(pasteObj["src"].isArray())
	{
		QJsonArray srcArray = pasteObj["src"].toArray();
		if(srcArray.empty() == false && srcArray[0].isObject() == false)
			HyGuiLog("DataExplorerWidget::PasteItemSrc - src array isn't of QJsonObjects", LOGTYPE_Error);

		// Copy everything into newSrcArray, while replacing "id" with proper value
		QJsonArray newSrcArray;
		for(int i = 0; i < srcArray.size(); ++i)
		{
			QJsonObject srcArrayObj = srcArray[i].toObject();

			srcArrayObj = ReplaceIdWithProperValue(srcArrayObj, importedFramesSet);
			newSrcArray.append(srcArrayObj);
		}

		pasteObj["src"] = newSrcArray;
	}
	else if(pasteObj["src"].isObject())
	{
		QJsonObject srcObj = pasteObj["src"].toObject();
		srcObj = ReplaceIdWithProperValue(srcObj, importedFramesSet);

		pasteObj["src"] = srcObj;
	}
	else
		HyGuiLog("DataExplorerWidget::PasteItemSrc - src isn't an object or array", LOGTYPE_Error);

	// Create a new project item representing the pasted item and save it
	QFileInfo itemNameFileInfo(pasteObj["itemName"].toString());
	QString sPrefix = sPrefixOverride.isEmpty() ? itemNameFileInfo.path() : sPrefixOverride;
	QString sName = itemNameFileInfo.baseName();
	ProjectItem *pNewItem = AddNewItem(pProject, ePasteItemType, sPrefix, sName, false, pasteObj["src"]);
	if(pNewItem)
		pNewItem->Save();
}

void ExplorerWidget::RecursiveRemoveItem(ExplorerItem *pItem)
{
	if(pItem == nullptr)
		return;

	for(int i = 0; i < pItem->GetTreeItem()->childCount(); ++i)
	{
		QVariant v = pItem->GetTreeItem()->child(i)->data(0, Qt::UserRole);
		RecursiveRemoveItem(v.value<ExplorerItem *>());
	}

	// Children are taken care of at this point, now remove self
	delete pItem;
}

QJsonObject ExplorerWidget::ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames)
{
	QStringList srcObjKeyList = srcObj.keys();
	for(int j = 0; j < srcObjKeyList.size(); ++j)
	{
		if(srcObjKeyList[j] == "checksum")
		{
			for(auto iter = importedFrames.begin(); iter != importedFrames.end(); ++iter)
			{
				if((*iter)->GetImageChecksum() == JSONOBJ_TOINT(srcObj, "checksum"))
				{
					srcObj.insert("id", QJsonValue(static_cast<qint64>((*iter)->GetId())));
					break;
				}
			}
			break;
		}
	}

	return srcObj;
}

QTreeWidgetItem *ExplorerWidget::GetSelectedTreeItem()
{
	QTreeWidgetItem *pCurSelected = nullptr;
	if(ui->treeWidget->selectedItems().empty() == false)
		pCurSelected = ui->treeWidget->selectedItems()[0];  // Only single selection is allowed in explorer because two projects may be opened

	return pCurSelected;
}

void ExplorerWidget::OnContextMenu(const QPoint &pos)
{
	QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
	QTreeWidgetItem *pTreeNode = ui->treeWidget->itemAt(pos);
	
	QMenu contextMenu;

	if(pTreeNode == nullptr)
	{
		contextMenu.addAction(FINDACTION("actionNewProject"));
		contextMenu.addAction(FINDACTION("actionOpenProject"));
	}
	else
	{
		ExplorerItem *pSelectedExplorerItem = pTreeNode->data(0, Qt::UserRole).value<ExplorerItem *>();
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
	
	contextMenu.exec(globalPos);
}

void ExplorerWidget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	// setCurrentItem() required if this function is manually invoked. E.g. AddItem()
	ui->treeWidget->setCurrentItem(item);
	
	ExplorerItem *pTreeVariantItem = item->data(0, Qt::UserRole).value<ExplorerItem *>();
	
	switch(pTreeVariantItem->GetType())
	{
	case ITEM_Project:
	case ITEM_Prefix:
		item->setExpanded(!item->isExpanded());
		break;
	
	case ITEM_Audio:
	case ITEM_Particles:
	case ITEM_Font:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Shader:
	case ITEM_Entity:
		MainWindow::OpenItem(static_cast<ProjectItem *>(pTreeVariantItem));
		break;
		
	default:
		HyGuiLog("ExplorerWidget::on_treeWidget_itemDoubleClicked was invoked on an unknown item type:" % QString::number(pTreeVariantItem->GetType()), LOGTYPE_Error);
	}
}

void ExplorerWidget::on_treeWidget_itemSelectionChanged()
{
	QTreeWidgetItem *pCurSelected = GetSelectedTreeItem();
	
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
		ExplorerItem *pTreeVariantItem = pCurSelected->data(0, Qt::UserRole).value<ExplorerItem *>();
		switch(pTreeVariantItem->GetType())
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
	ExplorerItem *pItem = GetCurItemSelected();
	
	DlgInputName *pDlg = new DlgInputName(HyGlobal::ItemName(pItem->GetType(), false), pItem->GetName(false));
	if(pDlg->exec() == QDialog::Accepted)
		pItem->Rename(pDlg->GetName());

	delete pDlg;
}

void ExplorerWidget::on_actionDeleteItem_triggered()
{
	ExplorerItem *pItem = GetCurItemSelected();
	
	switch(pItem->GetType())
	{
	case ITEM_Prefix:
		if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the prefix:\n" % pItem->GetName(true) % "\n\nAnd all of its contents? This action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
		{
			GetCurProjSelected()->DeletePrefixAndContents(pItem->GetName(true));
			pItem->GetTreeItem()->parent()->removeChild(pItem->GetTreeItem());
		}
		break;
		
	case ITEM_Audio:
	case ITEM_Particles:
	case ITEM_Font:
	case ITEM_Spine:
	case ITEM_Sprite:
	case ITEM_Shader:
	case ITEM_Entity:
		if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the " % HyGlobal::ItemName(pItem->GetType(), false) % ":\n" % pItem->GetName(true) % "?\n\nThis action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
		{
			static_cast<ProjectItem *>(pItem)->DeleteFromProject();
			
			if(pItem->GetTreeItem()->parent() != nullptr)
				pItem->GetTreeItem()->parent()->removeChild(pItem->GetTreeItem());
		}
		break;
		
	default:
		HyGuiLog("ExplorerWidget::on_actionDeleteItem_triggered was invoked on an non-item/prefix:" % QString::number(pItem->GetType()), LOGTYPE_Error);
	}
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
	ExplorerItem *pCurItemSelected = GetCurItemSelected();
	if(pCurItemSelected == nullptr || pCurItemSelected->IsProjectItem() == false)
	{
		HyGuiLog("ExplorerWidget::on_actionCutItem_triggered - Unsupported item:" % (pCurItemSelected ? QString::number(pCurItemSelected->GetType()) : " nullptr"), LOGTYPE_Error);
		return;
	}

	ProjectItem *pProjItem = static_cast<ProjectItem *>(pCurItemSelected);
	ProjectItemMimeData *pNewMimeData = new ProjectItemMimeData(pProjItem);
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
	ExplorerItem *pCurSelected = GetCurItemSelected();
	if(pCurSelected->GetType() != ITEM_Project)
	{
		if(pCurSelected->GetType() == ITEM_Prefix)
			sPrefixOverride = pCurSelected->GetName(true);
		else
			sPrefixOverride = pCurSelected->GetPrefix();
	}

	if(pData->hasFormat(HYGUI_MIMETYPE))
		PasteItemSrc(pData->data(HYGUI_MIMETYPE), pCurProj, sPrefixOverride);
}

void ExplorerWidget::on_actionOpen_triggered()
{
	ExplorerItem *pCurItemSelected = GetCurItemSelected();
	if(pCurItemSelected->IsProjectItem())
		MainWindow::OpenItem(static_cast<ProjectItem *>(pCurItemSelected));
}
