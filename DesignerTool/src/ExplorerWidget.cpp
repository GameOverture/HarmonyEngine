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

#include <QJsonArray>

ExplorerWidget::ExplorerWidget(QWidget *parent) :   QWidget(parent),
                                                    ui(new Ui::ExplorerWidget)
{
    ui->setupUi(this);

//    ui->treeWidget->addAction(new QAction("test", this));
//    ui->treeWidget->addAction(new QAction("test1", this));
//    ui->treeWidget->addAction(new QAction("test2", this));
//    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

ExplorerWidget::~ExplorerWidget()
{
    delete ui;
}

Project *ExplorerWidget::AddItemProject(const QString sNewProjectFilePath)
{
    Project *pNewProject = new Project(sNewProjectFilePath);
    if(pNewProject->HasError())
    {
        HyGuiLog("Abort opening project: " % pNewProject->GetAbsPath(), LOGTYPE_Error);
        return pNewProject;
    }
    else
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

void ExplorerWidget::AddItem(eItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd)
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

    ExplorerItem *pItem = new ProjectItem(*pCurProj, eNewItemType, sPrefix, sName, QJsonValue());
    
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
                QString sPath = pParentTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>()->GetName(true) % sPathSplitList[i];
                
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
    else if(bOpenAfterAdd)
    {
        QTreeWidgetItem *pExpandItem = pItem->GetTreeItem();
        while(pExpandItem->parent() != NULL)
        {
            ui->treeWidget->expandItem(pExpandItem->parent());
            pExpandItem = pExpandItem->parent();
        }
        
        MainWindow::OpenItem(static_cast<ProjectItem *>(pItem));
    }
}

void ExplorerWidget::RemoveItem(ExplorerItem *pItem)
{
    if(pItem == NULL)
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
    QTreeWidgetItem *pCurSelected = NULL;
    if(ui->treeWidget->selectedItems().empty() == false)
        pCurSelected = ui->treeWidget->selectedItems()[0];  // Only single selection is allowed in explorer
    
    return pCurSelected;
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

        pItemProject->SaveUserData();
    }
    
    return sListOpenProjs;
}

Project *ExplorerWidget::GetCurProjSelected()
{
    QTreeWidgetItem *pCurProjItem = GetSelectedTreeItem();
    if(pCurProjItem == NULL)
        return NULL;
    
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
    if(pCurItem == NULL)
        return NULL;
    
    QVariant v = pCurItem->data(0, Qt::UserRole);
    return v.value<ExplorerItem *>();
}

ExplorerItem *ExplorerWidget::GetCurDirSelected(bool bIncludePrefixDirs)
{
    QTreeWidgetItem *pCurTreeItem = GetSelectedTreeItem();
    if(pCurTreeItem == NULL)
        return NULL;
    
    ExplorerItem *pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>();
    while(pCurItem->GetType() != ITEM_DirAudio &&
          pCurItem->GetType() != ITEM_DirParticles &&
          pCurItem->GetType() != ITEM_DirFonts &&
          pCurItem->GetType() != ITEM_DirSpine &&
          pCurItem->GetType() != ITEM_DirSprites &&
          pCurItem->GetType() != ITEM_DirShaders &&
          pCurItem->GetType() != ITEM_DirEntities &&
          (pCurItem->GetType() != ITEM_Prefix && bIncludePrefixDirs))
    {
        pCurTreeItem = pCurItem->GetTreeItem()->parent();
        if(pCurTreeItem == NULL)
            return NULL;
        
        pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>();
    }
    
    return pCurItem;
}

//Item *WidgetExplorer::GetItemByPath(QString sItemPathAbsolute)
//{
//    QTreeWidgetItemIterator it(ui->treeWidget);
//    while(*it)
//    {
//        Item *pItem = (*it)->data(0, Qt::UserRole).value<Item *>();
        
//        if(0 == QString::compare(pItem->GetAbsPath(), sItemPathAbsolute, Qt::CaseInsensitive))
//            return pItem;
        
//        ++it;
//    }
    
//    return NULL;
//}

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
        if(pTreeNode->parent() == nullptr)
        {
            contextMenu.addAction(FINDACTION("actionCloseProject"));
            contextMenu.addAction(FINDACTION("actionProjectSettings"));
        }
        else
        {
            
        }
    }
    
    QAction* selectedItem = contextMenu.exec(globalPos);
    if (selectedItem)
    {
        // which returns a QTreeWidgetItem.
       // something was chosen, do stuff
    }
    else
    {
       // nothing was chosen
    }
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
    }
}

void ExplorerWidget::on_treeWidget_itemSelectionChanged()
{
    QTreeWidgetItem *pCurSelected = GetSelectedTreeItem();
    
    bool bValidItem = (pCurSelected != NULL);
    FINDACTION("actionCloseProject")->setEnabled(bValidItem);
    FINDACTION("actionNewAudio")->setEnabled(bValidItem);
    FINDACTION("actionNewParticle")->setEnabled(bValidItem);
    FINDACTION("actionNewFont")->setEnabled(bValidItem);
    FINDACTION("actionNewSprite")->setEnabled(bValidItem);
    FINDACTION("actionNewParticle")->setEnabled(bValidItem);
    FINDACTION("actionNewAudio")->setEnabled(bValidItem);
    FINDACTION("actionLaunchIDE")->setEnabled(bValidItem);
    
    if(bValidItem)
    {
        bValidItem = false;
        
        ExplorerItem *pItemDir = GetCurDirSelected(false);
        bValidItem = pItemDir->GetType() == ITEM_DirSprites || pItemDir->GetType() == ITEM_DirFonts;

        MainWindow::SetSelectedProj(GetCurProjSelected());
    }

    // QVariant v = current->data(0, Qt::UserRole);
    // Item *pItemVariant = v.value<Item *>();
}
