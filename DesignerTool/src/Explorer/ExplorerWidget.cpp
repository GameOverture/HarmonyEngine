/**************************************************************************
 *	ExplorerWidget.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ExplorerWidget.h"
#include "ui_ExplorerWidget.h"

#include "Global.h"
#include "MainWindow.h"
#include "Harmony.h"
#include "FontItem.h"
#include "Project.h"
#include "ProjectItemMimeData.h"
#include "ExplorerTreeItem.h"
#include "AtlasWidget.h"
#include "IModel.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QClipboard>

QByteArray ExplorerWidget::sm_sInternalClipboard = "";

///*virtual*/ void DataExplorerLoadThread::run() /*override*/
//{
//    /* ... here is the expensive or blocking operation ... */
//    Project *pNewItemProject = new Project(nullptr, m_sPath);
//    Q_EMIT LoadFinished(pNewItemProject);
//}

ExplorerWidget::ExplorerWidget(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::ExplorerWidget)
{
    ui->setupUi(this);
    ui->treeWidget->SetOwner(this);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->treeWidget->setDragEnabled(true);
    ui->treeWidget->setDropIndicatorShown(true);

    setAcceptDrops(true);

    ui->actionCutItem->setEnabled(false);
    ui->actionCopyItem->setEnabled(false);
    ui->actionPasteItem->setEnabled(false);

    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

ExplorerWidget::~ExplorerWidget()
{
    delete ui;
}

Project *ExplorerWidget::AddItemProject(const QString sNewProjectFilePath)
{
    Project *pNewProject = new Project(this, sNewProjectFilePath);
    if(pNewProject->HasError())
    {
        HyGuiLog("Project: " % pNewProject->GetAbsPath() % " had an error and will not be opened", LOGTYPE_Error);
        delete pNewProject;
        return nullptr;
    }

    HyGuiLog("Opening project: " % pNewProject->GetAbsPath(), LOGTYPE_Info);

    QTreeWidgetItem *pProjTreeItem = pNewProject->GetTreeItem();
    ui->treeWidget->insertTopLevelItem(0, pProjTreeItem);
    ui->treeWidget->expandItem(pProjTreeItem);
    
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

    ProjectItem *pItem = new ProjectItem(*pProj, eNewItemType, sPrefix, sName, initValue, true);
    
    if(sName[0] != HYDEFAULT_PrefixChar)
    {
        // Get the relative path from [ProjectDir->ItemPath] e.g. "Sprites/SpritePrefix/MySprite"
        QString sRelativePath = pItem->GetPath();
        QStringList sPathSplitList = sRelativePath.split(QChar('/'));

        // Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
        QTreeWidgetItem *pParentTreeItem = pProj->GetTreeItem();
        bool bSucceeded = false;
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
                if(i == 0)
                {
                    HyGuiLog("Cannot find valid sub directory: " % sPathSplitList[i], LOGTYPE_Error);
                    return nullptr;
                }

                if(i != sPathSplitList.size()-1)
                {
                    // Still more directories to dig thru, so this means we're at a prefix. Add the prefix TreeItem here and continue traversing down the tree
                    //
                    QString sPath = pParentTreeItem->data(0, Qt::UserRole).value<ExplorerTreeItem *>()->GetName(true) % "/" % sPathSplitList[i];

                    ExplorerTreeItem *pPrefixItem = new ExplorerTreeItem(ITEM_Prefix, sPath);
                    QTreeWidgetItem *pPrefixTreeItem = pPrefixItem->GetTreeItem();

                    pParentTreeItem->addChild(pPrefixTreeItem);
                    pParentTreeItem = pPrefixTreeItem;
                }
                else
                {
                    // At the final traversal, which is the item itself.
                    pParentTreeItem->addChild(pItem->GetTreeItem());

                    bSucceeded = true;
                    break;
                }
            }
        }

        if(bSucceeded == false)
        {
            HyGuiLog("Did not add item: " % pItem->GetName(true) % " successfully", LOGTYPE_Error);
            return nullptr;
        }

        pItem->SetTreeItemSubIcon(SUBICON_New);
    }

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

    return pItem;
}

void ExplorerWidget::RemoveItem(ExplorerTreeItem *pItem)
{
    if(pItem == nullptr)
        return;

    ui->treeWidget->clearSelection();
    ui->treeWidget->blockSignals(true);
    RecursiveRemoveItem(pItem);
    ui->treeWidget->blockSignals(false);
}

void ExplorerWidget::SelectItem(ExplorerTreeItem *pItem)
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
        ExplorerTreeItem *pItem = ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<ExplorerTreeItem *>();
        Project *pItemProject = static_cast<Project *>(pItem);
        sListOpenProjs.append(pItemProject->GetAbsPath());
    }
    
    return sListOpenProjs;
}

Project *ExplorerWidget::GetCurProjSelected()
{
    QTreeWidgetItem *pCurProjItem = GetSelectedTreeItem();
    if(pCurProjItem == nullptr)
        return nullptr;
    
    while(pCurProjItem->parent())
        pCurProjItem = pCurProjItem->parent();
    
    QVariant v = pCurProjItem->data(0, Qt::UserRole);
    ExplorerTreeItem *pItem = v.value<ExplorerTreeItem *>();

    if(pItem->GetType() != ITEM_Project)
        HyGuiLog("WidgetExplorer::GetCurProjSelected() returned a non project item", LOGTYPE_Error);

    return reinterpret_cast<Project *>(pItem);
}

ExplorerTreeItem *ExplorerWidget::GetCurItemSelected()
{
    QTreeWidgetItem *pCurItem = GetSelectedTreeItem();
    if(pCurItem == nullptr)
        return nullptr;
    
    QVariant v = pCurItem->data(0, Qt::UserRole);
    return v.value<ExplorerTreeItem *>();
}

ExplorerTreeItem *ExplorerWidget::GetCurSubDirSelected()
{
    QTreeWidgetItem *pCurTreeItem = GetSelectedTreeItem();
    if(pCurTreeItem == nullptr)
        return nullptr;
    
    ExplorerTreeItem *pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<ExplorerTreeItem *>();
    while(pCurItem->GetType() != DIR_Audio &&
          pCurItem->GetType() != DIR_Particles &&
          pCurItem->GetType() != DIR_Fonts &&
          pCurItem->GetType() != DIR_Spine &&
          pCurItem->GetType() != DIR_Sprites &&
          pCurItem->GetType() != DIR_Shaders &&
          pCurItem->GetType() != DIR_Entities)
    {
        pCurTreeItem = pCurItem->GetTreeItem()->parent();
        if(pCurTreeItem == nullptr)
            return nullptr;
        
        pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<ExplorerTreeItem *>();
    }
    
    return pCurItem;
}

void ExplorerWidget::PasteItemSrc(QByteArray sSrc, Project *pProject)
{
    QDir metaDir(pProject->GetMetaDataAbsPath());
    QDir metaTempDir = HyGlobal::PrepTempDir(pProject);

    QJsonDocument pasteDoc = QJsonDocument::fromJson(sSrc);
    QJsonObject pasteObj = pasteDoc.object();

    if(pasteObj["project"].toString().toLower() == pProject->GetAbsPath().toLower())
        return;

    // Determine the pasted item type
    HyGuiItemType ePasteItemType = TYPE_Unknown;
    QString sItemType = pasteObj["itemType"].toString();
    QList<HyGuiItemType> subDirList = HyGlobal::SubDirList();
    for(int i = 0; i < subDirList.size(); ++i)
    {
        HyGuiItemType eItemType = HyGlobal::GetItemFromDir(subDirList[i]);

        if(sItemType == HyGlobal::ItemName(eItemType))
        {
            ePasteItemType = eItemType;
            break;
        }
    }

    // Import any missing fonts (.ttf)
    if(ePasteItemType == ITEM_Font)
    {
        QString sFontMetaDir = metaDir.absoluteFilePath(HyGlobal::ItemName(DIR_Fonts));
        QJsonArray fontArray = pasteObj["fonts"].toArray();
        for(int i = 0; i < fontArray.size(); ++i)
        {
            QFileInfo pasteFontFileInfo(fontArray[i].toString());

            if(QFile::copy(pasteFontFileInfo.absoluteFilePath(), sFontMetaDir % "/" % pasteFontFileInfo.fileName()))
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

    // Repack this atlas group with imported images
    QSet<AtlasFrame *> importedFramesSet = pProject->GetAtlasModel().ImportImages(importImageList, uiAtlasGrpId, (ePasteItemType == ITEM_Font) ? ITEM_Font : ITEM_AtlasImage);
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
    QString sPrefix = itemNameFileInfo.path();
    QString sName = itemNameFileInfo.baseName();
    ProjectItem *pNewItem = AddNewItem(pProject, ePasteItemType, sPrefix, sName, false, pasteObj["src"]);
    if(pNewItem)
        pNewItem->Save();
}

void ExplorerWidget::RecursiveRemoveItem(ExplorerTreeItem *pItem)
{
    if(pItem == nullptr)
        return;

    for(int i = 0; i < pItem->GetTreeItem()->childCount(); ++i)
    {
        QVariant v = pItem->GetTreeItem()->child(i)->data(0, Qt::UserRole);
        RecursiveRemoveItem(v.value<ExplorerTreeItem *>());
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
        ExplorerTreeItem *pSelectedExplorerItem = pTreeNode->data(0, Qt::UserRole).value<ExplorerTreeItem *>();
        HyGuiItemType eSelectedItemType = pSelectedExplorerItem->GetType();
        switch(eSelectedItemType)
        {
        case ITEM_Project:
            contextMenu.addAction(FINDACTION("actionCloseProject"));
            contextMenu.addAction(FINDACTION("actionProjectSettings"));
            break;
        case DIR_Audio:
        case DIR_Particles:
        case DIR_Fonts:
        case DIR_Spine:
        case DIR_Sprites:
        case DIR_Shaders:
        case DIR_Entities:
            contextMenu.addAction(FINDACTION("actionNew" % HyGlobal::ItemName(HyGlobal::GetItemFromDir(eSelectedItemType))));
            break;
        case ITEM_Audio:
        case ITEM_Particles:
        case ITEM_Font:
        case ITEM_Spine:
        case ITEM_Sprite:
        case ITEM_Shader:
        case ITEM_Entity:
            contextMenu.addAction(FINDACTION("actionNew" % HyGlobal::ItemName(eSelectedItemType)));
            contextMenu.addSeparator();
            contextMenu.addAction(ui->actionCutItem);
            contextMenu.addAction(ui->actionCopyItem);
            contextMenu.addAction(ui->actionPasteItem);
            contextMenu.addSeparator();
        case ITEM_Prefix:
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
    
    ExplorerTreeItem *pTreeVariantItem = item->data(0, Qt::UserRole).value<ExplorerTreeItem *>();
    
    switch(pTreeVariantItem->GetType())
    {
    case DIR_Audio:
    case DIR_Particles:
    case DIR_Fonts:
    case DIR_Shaders:
    case DIR_Spine:
    case DIR_Sprites:
    case DIR_Entities:
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
    FINDACTION("actionLaunchIDE")->setEnabled(bValidItem);

    if(pCurSelected)
    {
        ExplorerTreeItem *pTreeVariantItem = pCurSelected->data(0, Qt::UserRole).value<ExplorerTreeItem *>();
        switch(pTreeVariantItem->GetType())
        {
        case ITEM_Audio:
        case ITEM_Particles:
        case ITEM_Font:
        case ITEM_Spine:
        case ITEM_Sprite:
        case ITEM_Shader:
        case ITEM_Entity:
            ui->actionCutItem->setEnabled(true);
            ui->actionCopyItem->setEnabled(true);
            break;
        default:
            ui->actionCutItem->setEnabled(false);
            ui->actionCopyItem->setEnabled(false);
            break;
        }
    }

    ui->actionPasteItem->setEnabled(sm_sInternalClipboard.isEmpty() == false);
    
    if(bValidItem)
        Harmony::SetProject(GetCurProjSelected());
}

void ExplorerWidget::on_actionRename_triggered()
{
    ExplorerTreeItem *pItem = GetCurItemSelected();
    
    switch(pItem->GetType())
    {
    case ITEM_Prefix:
        break;
        
    case ITEM_Audio:
    case ITEM_Particles:
    case ITEM_Font:
    case ITEM_Spine:
    case ITEM_Sprite:
    case ITEM_Shader:
    case ITEM_Entity:
        break;
        
    default:
        HyGuiLog("ExplorerWidget::on_actionDeleteItem_triggered was invoked on an non-item/prefix:" % QString::number(pItem->GetType()), LOGTYPE_Error);
    }
}

void ExplorerWidget::on_actionDeleteItem_triggered()
{
    ExplorerTreeItem *pItem = GetCurItemSelected();
    
    switch(pItem->GetType())
    {
    case ITEM_Prefix:
        if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the " % HyGlobal::ItemName(pItem->GetType()) % ":\n" % pItem->GetPrefix() % "\n\nAnd all of its contents? This action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
        {
            GetCurProjSelected()->DeletePrefixAndContents(GetCurSubDirSelected()->GetType(), pItem->GetPrefix());
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
        if(QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete the " % HyGlobal::ItemName(pItem->GetType()) % ":\n" % pItem->GetName(true) % "?\n\nThis action cannot be undone.", QMessageBox::Yes, QMessageBox::No))
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

void ExplorerWidget::on_actionCutItem_triggered()
{
    ExplorerTreeItem *pCurItemSelected = GetCurItemSelected();
    if(pCurItemSelected->IsProjectItem() == false)
    {
        HyGuiLog("ExplorerWidget::on_actionCutItem_triggered - Unsupported item:" % QString::number(pCurItemSelected->GetType()), LOGTYPE_Error);
        return;
    }

    ProjectItem *pProjItem = static_cast<ProjectItem *>(pCurItemSelected);
    ProjectItemMimeData *pNewMimeData = new ProjectItemMimeData(pProjItem);
    QClipboard *pClipboard = QApplication::clipboard();
    pClipboard->setText(pNewMimeData->data(HYGUI_MIMETYPE));

    HyGuiLog("Cut " % HyGlobal::ItemName(pCurItemSelected->GetType()) % " item (" % pProjItem->GetName(true) % ") to the clipboard.", LOGTYPE_Normal);
    ui->actionPasteItem->setEnabled(true);
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
    ExplorerTreeItem *pCurItemSelected = GetCurItemSelected();
    if(pCurItemSelected->IsProjectItem() == false)
    {
        HyGuiLog("ExplorerWidget::on_actionCutItem_triggered - Unsupported item:" % QString::number(pCurItemSelected->GetType()), LOGTYPE_Error);
        return;
    }

    ProjectItem *pProjItem = static_cast<ProjectItem *>(pCurItemSelected);
    ProjectItemMimeData *pNewMimeData = new ProjectItemMimeData(pProjItem);
    QClipboard *pClipboard = QApplication::clipboard();
    pClipboard->setText(pNewMimeData->data(HYGUI_MIMETYPE));

    HyGuiLog("Copied " % HyGlobal::ItemName(pCurItemSelected->GetType()) % " item (" % pProjItem->GetName(true) % ") to the clipboard.", LOGTYPE_Normal);
    ui->actionPasteItem->setEnabled(true);
}

void ExplorerWidget::on_actionPasteItem_triggered()
{
    Project *pCurProj = GetCurProjSelected();

    HyGuiLog("Paste is not implemented", LOGTYPE_Error);

    // TODO: Get rid of the sm_sInternalClipboard???

    // Don't use QClipboard because someone can just copy random text before pasting and ruin the expected json format
    PasteItemSrc(sm_sInternalClipboard, pCurProj);
}
