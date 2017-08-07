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

#include "MainWindow.h"
#include "FontItem.h"
#include "Project.h"
#include "AtlasWidget.h"
#include "HyGuiGlobal.h"
#include "IModel.h"

#include <QJsonArray>
#include <QMessageBox>
#include <QClipboard>

ExplorerWidget::ExplorerWidget(QWidget *parent) :   QWidget(parent),
                                                    ui(new Ui::ExplorerWidget)
{
    ui->setupUi(this);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->treeWidget->setDragEnabled(true);
    ui->treeWidget->setDropIndicatorShown(true);
    ui->treeWidget->setDragDropMode(QAbstractItemView::InternalMove);

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
    Project *pNewProject = new Project(sNewProjectFilePath);
    HyGuiLog("Opening project: " % pNewProject->GetAbsPath(), LOGTYPE_Info);

    QTreeWidgetItem *pProjTreeItem = pNewProject->GetTreeItem();
    ui->treeWidget->insertTopLevelItem(0, pProjTreeItem);
    ui->treeWidget->expandItem(pProjTreeItem);
    
    OnProjectLoaded(pNewProject);
    return pNewProject;

    // BELOW BREAKS QTABBAR and UNDOSTACK SIGNAL/SLOT CONNECTIONS (I guess because QObject must be created on main thread?.. fucking waste of time)
    //
    //MainWindow::StartLoading(MDI_Explorer);
    //ExplorerLoadThread *pNewLoadThread = new ExplorerLoadThread(sNewProjectFilePath, this);
    //connect(pNewLoadThread, &ExplorerLoadThread::LoadFinished, this, &ExplorerWidget::OnProjectLoaded);
    //connect(pNewLoadThread, &ExplorerLoadThread::finished, pNewLoadThread, &QObject::deleteLater);
    //pNewLoadThread->start();
}

void ExplorerWidget::AddNewItem(HyGuiItemType eNewItemType, const QString sPrefix, const QString sName)
{
    // Find the proper project tree item
    Project *pCurProj = GetCurProjSelected();
    if(pCurProj == nullptr)
    {
        HyGuiLog("Could not find associated project for item: " % sPrefix % "/" % sName, LOGTYPE_Error);
        return;
    }
    
    if(eNewItemType == ITEM_Project)
    {
        HyGuiLog("Do not use WidgetExplorer::AddItem for projects... use AddProjectItem instead", LOGTYPE_Error);
        return;
    }

    ExplorerItem *pItem = new ProjectItem(*pCurProj, eNewItemType, sPrefix, sName, QJsonValue(), true);
    
    // Get the relative path from [ProjectDir->ItemPath] e.g. "Sprites/SpritePrefix/MySprite"
    QString sRelativePath = pItem->GetPath();
    QStringList sPathSplitList = sRelativePath.split(QChar('/'));
    
    // Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
    QTreeWidgetItem *pParentTreeItem = pCurProj->GetTreeItem();
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
                return;
            }
            
            if(i != sPathSplitList.size()-1)
            {
                // Still more directories to dig thru, so this means we're at a prefix. Add the prefix TreeItem here and continue traversing down the tree
                //
                QString sPath = pParentTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>()->GetName(true) % "/" % sPathSplitList[i];
                
                ExplorerItem *pPrefixItem = new ExplorerItem(ITEM_Prefix, sPath);
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
        return;
    }

    QTreeWidgetItem *pExpandItem = pItem->GetTreeItem();
    while(pExpandItem->parent() != nullptr)
    {
        ui->treeWidget->expandItem(pExpandItem->parent());
        pExpandItem = pExpandItem->parent();
    }

    pItem->SetTreeItemSubIcon(SUBICON_New);
    MainWindow::OpenItem(static_cast<ProjectItem *>(pItem));
}

void ExplorerWidget::RemoveItem(ExplorerItem *pItem)
{
    if(pItem == nullptr)
        return;
    
    for(int i = 0; i < pItem->GetTreeItem()->childCount(); ++i)
    {
        QVariant v = pItem->GetTreeItem()->child(i)->data(0, Qt::UserRole);
        RemoveItem(v.value<ExplorerItem *>());
    }
    
    // Children are taken care of at this point, now remove self
    delete pItem;
}

void ExplorerWidget::SelectItem(ExplorerItem *pItem)
{
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

QTreeWidgetItem *ExplorerWidget::GetSelectedTreeItem()
{
    QTreeWidgetItem *pCurSelected = nullptr;
    if(ui->treeWidget->selectedItems().empty() == false)
        pCurSelected = ui->treeWidget->selectedItems()[0];  // Only single selection is allowed in explorer because two projects may be opened
    
    return pCurSelected;
}

void ExplorerWidget::PutItemOnClipboard(ProjectItem *pProjItem)
{
    QJsonValue itemValue = pProjItem->GetModel()->GetJson(false);

    QJsonObject clipboardObj;
    clipboardObj.insert(HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(pProjItem->GetType())), pProjItem->GetName(true));
    clipboardObj.insert("src", itemValue);
    QList<AtlasFrame *> atlasFrameList = pProjItem->GetModel()->GetAtlasFrames();
    QJsonArray imagesArray;
    for(int i = 0; i < atlasFrameList.size(); ++i)
    {
        QJsonObject atlasFrameObj;
        atlasFrameObj.insert("checksum", QJsonValue(static_cast<qint64>(atlasFrameList[i]->GetImageChecksum())));
        atlasFrameObj.insert("name", QJsonValue(atlasFrameList[i]->GetName()));
        atlasFrameObj.insert("url", QJsonValue(GetCurProjSelected()->GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_DirAtlases) % "/" % atlasFrameList[i]->ConstructImageFileName()));
        imagesArray.append(atlasFrameObj);
    }
    clipboardObj.insert("images", imagesArray);

    // TODO: clipboardObj.insert("audio", GetAudioWavs())

    QByteArray src = JsonValueToSrc(QJsonValue(clipboardObj));
    QClipboard *pClipboard = QApplication::clipboard();
    pClipboard->setText(src);
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
    QTreeWidgetItem *pCurProjItem = GetSelectedTreeItem();
    if(pCurProjItem == nullptr)
        return nullptr;
    
    while(pCurProjItem->parent())
        pCurProjItem = pCurProjItem->parent();
    
    QVariant v = pCurProjItem->data(0, Qt::UserRole);
    ExplorerItem *pItem = v.value<ExplorerItem *>();

    if(pItem->GetType() != ITEM_Project)
        HyGuiLog("WidgetExplorer::GetCurProjSelected() returned a non project item", LOGTYPE_Error);

    return reinterpret_cast<Project *>(pItem);
}

ExplorerItem *ExplorerWidget::GetCurItemSelected()
{
    QTreeWidgetItem *pCurItem = GetSelectedTreeItem();
    if(pCurItem == nullptr)
        return nullptr;
    
    QVariant v = pCurItem->data(0, Qt::UserRole);
    return v.value<ExplorerItem *>();
}

ExplorerItem *ExplorerWidget::GetCurSubDirSelected()
{
    QTreeWidgetItem *pCurTreeItem = GetSelectedTreeItem();
    if(pCurTreeItem == nullptr)
        return nullptr;
    
    ExplorerItem *pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>();
    while(pCurItem->GetType() != ITEM_DirAudio &&
          pCurItem->GetType() != ITEM_DirParticles &&
          pCurItem->GetType() != ITEM_DirFonts &&
          pCurItem->GetType() != ITEM_DirSpine &&
          pCurItem->GetType() != ITEM_DirSprites &&
          pCurItem->GetType() != ITEM_DirShaders &&
          pCurItem->GetType() != ITEM_DirEntities)
    {
        pCurTreeItem = pCurItem->GetTreeItem()->parent();
        if(pCurTreeItem == nullptr)
            return nullptr;
        
        pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>();
    }
    
    return pCurItem;
}

void ExplorerWidget::OnProjectLoaded(Project *pLoadedProj)
{
    //pLoadedProj->moveToThread(QApplication::instance()->thread());
    MainWindow::StopLoading(MDI_Explorer);
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
        case ITEM_Project:
            contextMenu.addAction(FINDACTION("actionCloseProject"));
            contextMenu.addAction(FINDACTION("actionProjectSettings"));
            break;
        case ITEM_DirAudio:
        case ITEM_DirParticles:
        case ITEM_DirFonts:
        case ITEM_DirSpine:
        case ITEM_DirSprites:
        case ITEM_DirShaders:
        case ITEM_DirEntities:
            contextMenu.addAction(FINDACTION("actionNew" % HyGlobal::ItemName(HyGlobal::GetCorrespondingItemFromDir(eSelectedItemType))));
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
    
    ExplorerItem *pTreeVariantItem = item->data(0, Qt::UserRole).value<ExplorerItem *>();
    
    switch(pTreeVariantItem->GetType())
    {
    case ITEM_Project:
    case ITEM_DirAudio:
    case ITEM_DirParticles:
    case ITEM_DirFonts:
    case ITEM_DirShaders:
    case ITEM_DirSpine:
    case ITEM_DirSprites:
    case ITEM_DirEntities:
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
        ui->actionCutItem->setEnabled(true);
        ui->actionCopyItem->setEnabled(true);
        //ui->actionPaste
        break;
    default:
        ui->actionCutItem->setEnabled(false);
        ui->actionCopyItem->setEnabled(false);
        break;
    }
    
    if(bValidItem)
        MainWindow::SetSelectedProj(GetCurProjSelected());
}

void ExplorerWidget::on_actionRename_triggered()
{
    ExplorerItem *pItem = GetCurItemSelected();
    
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
    ExplorerItem *pItem = GetCurItemSelected();
    
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
    ExplorerItem *pCurItemSelected = GetCurItemSelected();
    switch(pCurItemSelected->GetType())
    {
        case ITEM_Audio:
        case ITEM_Particles:
        case ITEM_Font:
        case ITEM_Spine:
        case ITEM_Sprite:
        case ITEM_Shader:
        case ITEM_Entity: {
            ProjectItem *pProjItem = static_cast<ProjectItem *>(pProjItem);
            PutItemOnClipboard(pProjItem);
            HyGuiLog("Cut " % HyGlobal::ItemName(pCurItemSelected->GetType()) % " item to the clipboard.", LOGTYPE_Normal);

            ui->actionPasteItem->setEnabled(true);
        } break;

        default: {
            HyGuiLog("ExplorerWidget::on_actionCutItem_triggered - Unsupported item:" % QString::number(pCurItemSelected->GetType()), LOGTYPE_Error);
        } break;
    }
}

void ExplorerWidget::on_actionCopyItem_triggered()
{
    ExplorerItem *pCurItemSelected = GetCurItemSelected();
    switch(pCurItemSelected->GetType())
    {
        case ITEM_Audio:
        case ITEM_Particles:
        case ITEM_Font:
        case ITEM_Spine:
        case ITEM_Sprite:
        case ITEM_Shader:
        case ITEM_Entity: {
            ProjectItem *pProjItem = static_cast<ProjectItem *>(pCurItemSelected);
            PutItemOnClipboard(pProjItem);
            HyGuiLog("Copied " % HyGlobal::ItemName(pCurItemSelected->GetType()) % " item to the clipboard.", LOGTYPE_Normal);

            ui->actionPasteItem->setEnabled(true);
        } break;

        default: {
            HyGuiLog("ExplorerWidget::on_actionCutItem_triggered - Unsupported item:" % QString::number(pCurItemSelected->GetType()), LOGTYPE_Error);
        } break;
    }
}

void ExplorerWidget::on_actionPasteItem_triggered()
{

}
