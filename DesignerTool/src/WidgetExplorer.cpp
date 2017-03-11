/**************************************************************************
 *	WidgetExplorer.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetExplorer.h"
#include "ui_WidgetExplorer.h"

#include "MainWindow.h"
#include "ItemAudio.h"
#include "ItemSprite.h"
#include "ItemFont.h"
#include "ItemProject.h"
#include "WidgetAtlasManager.h"
#include "HyGlobal.h"

WidgetExplorer::WidgetExplorer(QWidget *parent) :   QWidget(parent),
                                                    ui(new Ui::WidgetExplorer)
{
    ui->setupUi(this);

//    ui->treeWidget->addAction(new QAction("test", this));
//    ui->treeWidget->addAction(new QAction("test1", this));
//    ui->treeWidget->addAction(new QAction("test2", this));
//    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

WidgetExplorer::~WidgetExplorer()
{
    delete ui;
}

void WidgetExplorer::AddItemProject(const QString sNewProjectFilePath)
{
    //m_LoadProjectThread
    //QThread *pNewThread = new QThread();



    //ItemProject *pItemProject = new ItemProject(sNewProjectFilePath);



    //pItemProject->moveToThread(pNewThread);
    //connect(this, &WidgetExplorer::LoadItemProject, pItemProject, &ItemProject::OnLoadThread);
    //connect(pItemProject, &ItemProject::LoadFinished, this, &WidgetExplorer::OnProjectLoaded);
    //connect(pNewThread, &QThread::finished, pNewThread, &QObject::deleteLater);



    //OnProjectLoaded(pItemProject);



    //Q_EMIT LoadItemProject();
    //MainWindow::StartLoading(MDI_Explorer);
    //pNewThread->start();






    MainWindow::StartLoading(MDI_Explorer);
    WidgetExplorerLoadThread *pNewLoadThread = new WidgetExplorerLoadThread(sNewProjectFilePath, this);
    connect(pNewLoadThread, &WidgetExplorerLoadThread::LoadFinished, this, &WidgetExplorer::OnProjectLoaded);
    connect(pNewLoadThread, &WidgetExplorerLoadThread::finished, pNewLoadThread, &QObject::deleteLater);
    pNewLoadThread->start();
}

void WidgetExplorer::AddItem(eItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd)
{
    // Find the proper project tree item
    ItemProject *pCurProj = GetCurProjSelected();
    if(pCurProj == nullptr)
    {
        HyGuiLog("Could not find associated project for item: " % sPrefix % "/" % sName, LOGTYPE_Error);
        return;
    }
    
    Item *pItem;
    switch(eNewItemType)
    {
    case ITEM_Project:
        HyGuiLog("Do not use WidgetExplorer::AddItem for projects... use AddProjectItem instead", LOGTYPE_Error);
        return;
    case ITEM_DirAudio:
    case ITEM_DirFonts:
    case ITEM_DirShaders:
    case ITEM_DirSpine:
    case ITEM_DirSprites:
    case ITEM_Prefix:
        HyGuiLog("Do not use WidgetExplorer::AddItem for Sub dirs or prefixes", LOGTYPE_Error);
        return;
    case ITEM_Audio:
        pItem = new ItemAudio(pCurProj, sPrefix, sName, QJsonValue());
        break;
    case ITEM_Sprite:
        pItem = new ItemSprite(pCurProj, sPrefix, sName, QJsonValue());
        break;
    case ITEM_Font:
        pItem = new ItemFont(pCurProj, sPrefix, sName, QJsonValue());
        break;
    default:
        HyGuiLog("Item: " % sPrefix % "/" % sName % " is not handled in WidgetExplorer::AddItem()", LOGTYPE_Error);
        return;
    }
    
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
                QString sPath = pParentTreeItem->data(0, Qt::UserRole).value<Item *>()->GetName(true) % sPathSplitList[i];
                
                Item *pPrefixItem = new Item(ITEM_Prefix, sPath);
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
        
        MainWindow::OpenItem(static_cast<ItemWidget *>(pItem));
    }
}

void WidgetExplorer::RemoveItem(Item *pItem)
{
    if(pItem == NULL)
        return;
    
    for(int i = 0; i < pItem->GetTreeItem()->childCount(); ++i)
    {
        QVariant v = pItem->GetTreeItem()->child(i)->data(0, Qt::UserRole);
        RemoveItem(v.value<Item *>());
    }
    
    // Children are taken care of at this point, now remove self
    delete pItem;
}

void WidgetExplorer::SelectItem(Item *pItem)
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

void WidgetExplorer::ProcessItem(Item *pItem)
{
}

QTreeWidgetItem *WidgetExplorer::GetSelectedTreeItem()
{
    QTreeWidgetItem *pCurSelected = NULL;
    if(ui->treeWidget->selectedItems().empty() == false)
        pCurSelected = ui->treeWidget->selectedItems()[0];  // Only single selection is allowed in explorer
    
    return pCurSelected;
}

QStringList WidgetExplorer::GetOpenProjectPaths()
{
    QStringList sListOpenProjs;
    sListOpenProjs.clear();
    
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        Item *pItem = ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<Item *>();
        ItemProject *pItemProject = static_cast<ItemProject *>(pItem);
        sListOpenProjs.append(pItemProject->GetAbsPath());

        pItemProject->SaveUserData();
    }
    
    return sListOpenProjs;
}

ItemProject *WidgetExplorer::GetCurProjSelected()
{
    QTreeWidgetItem *pCurProjItem = GetSelectedTreeItem();
    if(pCurProjItem == NULL)
        return NULL;
    
    while(pCurProjItem->parent())
        pCurProjItem = pCurProjItem->parent();
    
    QVariant v = pCurProjItem->data(0, Qt::UserRole);
    Item *pItem = v.value<Item *>();

    if(pItem->GetType() != ITEM_Project)
        HyGuiLog("WidgetExplorer::GetCurProjSelected() returned a non project item", LOGTYPE_Error);

    return reinterpret_cast<ItemProject *>(pItem);
}

Item *WidgetExplorer::GetCurItemSelected()
{
    QTreeWidgetItem *pCurItem = GetSelectedTreeItem();
    if(pCurItem == NULL)
        return NULL;
    
    QVariant v = pCurItem->data(0, Qt::UserRole);
    return v.value<Item *>();
}

Item *WidgetExplorer::GetCurDirSelected(bool bIncludePrefixDirs)
{
    QTreeWidgetItem *pCurTreeItem = GetSelectedTreeItem();
    if(pCurTreeItem == NULL)
        return NULL;
    
    Item *pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<Item *>();
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
        
        pCurItem = pCurTreeItem->data(0, Qt::UserRole).value<Item *>();
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

void WidgetExplorer::OnProjectLoaded(ItemProject *pLoadedProj)
{
    pLoadedProj->LoadWidgets();

    if(pLoadedProj->HasError())
    {
        HyGuiLog("Abort opening project: " % pLoadedProj->GetAbsPath(), LOGTYPE_Error);
        return;
    }
    else
        HyGuiLog("Opening project: " % pLoadedProj->GetAbsPath(), LOGTYPE_Info);

    QTreeWidgetItem *pProjTreeItem = pLoadedProj->GetTreeItem();
    ui->treeWidget->insertTopLevelItem(0, pProjTreeItem);
    ui->treeWidget->expandItem(pProjTreeItem);

    MainWindow::StopLoading(MDI_Explorer);
}

void WidgetExplorer::OnContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    QTreeWidgetItem *pTreeNode = ui->treeWidget->itemAt(pos);
    
    QMenu contextMenu;

    if(pTreeNode == NULL)
    {
        contextMenu.addAction(FINDACTION("actionNewProject"));
        contextMenu.addAction(FINDACTION("actionOpenProject"));
    }
    else
    {
        if(pTreeNode->parent() == NULL)
            contextMenu.addAction(FINDACTION("actionCloseProject"));
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

void WidgetExplorer::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    // setCurrentItem() required if this function is manually invoked. E.g. AddItem()
    ui->treeWidget->setCurrentItem(item);
    
    Item *pTreeVariantItem = item->data(0, Qt::UserRole).value<Item *>();
    
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
        MainWindow::OpenItem(static_cast<ItemWidget *>(pTreeVariantItem));
        break;
    }
}

void WidgetExplorer::on_treeWidget_itemSelectionChanged()
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
        
        Item *pItemDir = GetCurDirSelected(false);
        bValidItem = pItemDir->GetType() == ITEM_DirSprites || pItemDir->GetType() == ITEM_DirFonts;

        MainWindow::SetSelectedProj(GetCurProjSelected());
    }

    // QVariant v = current->data(0, Qt::UserRole);
    // Item *pItemVariant = v.value<Item *>();
}
