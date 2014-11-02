#include "WidgetExplorer.h"
#include "ui_WidgetExplorer.h"

#include "MainWindow.h"
#include "ItemSprite.h"
#include "ItemFont.h"

#include <QDirIterator>

WidgetExplorer::WidgetExplorer(QWidget *parent) :
    QWidget(parent),
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

void WidgetExplorer::AddItem(eItemType eNewItemType, const QString sNewItemPath, bool bOpenAfterAdd)
{
    if(eNewItemType == ITEM_Unknown)
    {
        HYLOG("Invalid item passed to WidgetExplorer::AddItem()", LOGTYPE_Error);
        return;
    }
    
    Item *pItem;
    switch(eNewItemType)
    {
    case ITEM_Project:
    case ITEM_DirAudio:
    case ITEM_DirFonts:
    case ITEM_DirShaders:
    case ITEM_DirSpine:
    case ITEM_DirSprites:
    case ITEM_Prefix:
        pItem = new Item();
    case ITEM_Sprite:
        pItem = new ItemSprite();
        break;
    case ITEM_Font:
        pItem = new ItemFont();
        break;
    default:
        HYLOG("Item: " % sNewItemPath % " is not handled in WidgetExplorer::AddItem()", LOGTYPE_Error);
        return;
    }
    pItem->Set(eNewItemType, sNewItemPath);
    
    if(pItem->GetType() == ITEM_Project)
    {
        if(HyGlobal::IsWorkspaceValid(QDir(pItem->GetPath())) == false)
        {
            
            HYLOG("Could not open project: " % pItem->GetPath(), LOGTYPE_Warning);
            return;
        }
        
        HYLOG("Opening project: " % pItem->GetPath(), LOGTYPE_Info);
        
        QTreeWidgetItem *pProjTreeItem = CreateTreeItem(NULL, pItem);
        
        QList<eItemType> subDirList = HyGlobal::SubDirList();
        foreach(eItemType eType, subDirList)
        {
            QString sSubDirPath = pItem->GetPath() % "data/" % HyGlobal::ItemName(eType) % HyGlobal::ItemExt(eType);
            Item *pSubDirItem = new Item();
            pSubDirItem->Set(eType, sSubDirPath);
            
            QTreeWidgetItem *pSubDirTreeItem = CreateTreeItem(pProjTreeItem, pSubDirItem);
            QTreeWidgetItem *pCurParentTreeItem = pSubDirTreeItem;
            
            QDirIterator dirIter(sSubDirPath, QDirIterator::Subdirectories);
            while(dirIter.hasNext())
            {
                QString sCurPath = dirIter.next();
                
                if(sCurPath.endsWith(QChar('.')))
                    continue;
                
                // Ensure pCurParentTreeItem is correct. If not keep moving up the tree until found.
                QFileInfo curFileInfo(sCurPath);
                QString sCurFileParentBaseName = curFileInfo.dir().dirName();
                QString sCurTreeItemName = pCurParentTreeItem->text(0);
                while(QString::compare(sCurFileParentBaseName, sCurTreeItemName, Qt::CaseInsensitive) != 0)
                {
                    pCurParentTreeItem = pCurParentTreeItem->parent();
                    sCurTreeItemName = pCurParentTreeItem->text(0);
                }
                
                Item *pPrefixItem;
                if(dirIter.fileInfo().isDir())
                {
                    pPrefixItem = new Item();
                    pPrefixItem->Set(ITEM_Prefix, sCurPath);
                    pCurParentTreeItem = CreateTreeItem(pCurParentTreeItem, pPrefixItem);
                }
                else if(dirIter.fileInfo().isFile())
                {
                    eItemType eType;
                    for(int i = 0; i < NUMITEM; ++i)
                    {
                        if(sCurPath.endsWith(HyGlobal::ItemExt(i)))
                        {
                            eType = static_cast<eItemType>(i);
                            break;
                        }
                    }
                    
                    switch(eType)
                    {
                    case ITEM_Sprite:   pPrefixItem = new ItemSprite(); break;
                    }
                    
                    pPrefixItem->Set(eType, sCurPath);
                    CreateTreeItem(pCurParentTreeItem, pPrefixItem);
                }
            }
        }
        
        if(bOpenAfterAdd)
        {
            ui->treeWidget->expandItem(pProjTreeItem);
        }
    }
    else
    {
        // Find parent tree item
        QTreeWidgetItem *pParentTreeItem = NULL;
        QDir projDir;
        for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
        {
            QVariant v = ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole);
            Item *pTopLevelItem = v.value<Item *>();
            
            if(pItem->GetPath().contains(pTopLevelItem->GetPath(), Qt::CaseInsensitive))
            {
                projDir.setPath(pTopLevelItem->GetPath());
                pParentTreeItem = pTopLevelItem->GetTreeItem();
                break;
            }
        }
        
        if(pParentTreeItem == NULL)
        {
            HYLOG("Could not find associated project for item: " % pItem->GetPath(), LOGTYPE_Error);
            return;
        }
        
        QString sRelativePath = projDir.relativeFilePath(pItem->GetPath());
        
        QStringList sPathSplitList = sRelativePath.split(QChar('/'));
        if(QString::compare(sPathSplitList[0], "data", Qt::CaseInsensitive) != 0)
        {
            HYLOG("Project path does not begin inside 'data' directory", LOGTYPE_Error);
            return;
        }
        
//        for(int i = 0; i < pParentTreeItem->childCount(); ++i)
//        {
//            if(QString::compare(sPathSplitList[1], pParentTreeItem->child(i)->text(0), Qt::CaseInsensitive) == 0)
//            {
//                pParentTreeItem = pParentTreeItem->child(i);
//                break;
//            }
//        }
        
        bool bSucceeded = false;
        for(int i = 1; i < sPathSplitList.size(); ++i)
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
                if(i == 1)
                {
                    HYLOG("Cannot find valid sub directory: " % sPathSplitList[i], LOGTYPE_Error);
                    return;
                }
                
                if(i != sPathSplitList.size()-1)
                {
                    QString sPath = pParentTreeItem->data(0, Qt::UserRole).value<Item *>()->GetPath() % "/" % sPathSplitList[i];
                    
                    Item *pPrefixItem = new Item();
                    pPrefixItem->Set(ITEM_Prefix, sPath);
                    
                    CreateTreeItem(pParentTreeItem, pPrefixItem);
                }
                else
                {
                    CreateTreeItem(pParentTreeItem, pItem);
                    
                    bSucceeded = true;
                    break;
                }
            }
        }
        
        if(bSucceeded == false)
        {
            HYLOG("Did not add item: " % pItem->GetName() % " successfully", LOGTYPE_Error);
            return;
        }
        else if(bOpenAfterAdd)
        {
            // TODO: expand doesn't work, might need to expand every parent of pItem
            QTreeWidgetItem *pExpandItem = pItem->GetTreeItem();
            while(pExpandItem->parent() != NULL)
            {
                ui->treeWidget->expandItem(pExpandItem->parent());
                pExpandItem = pExpandItem->parent();
            }
            
            MainWindow::OpenItem(pItem);
        }
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

QTreeWidgetItem *WidgetExplorer::CreateTreeItem(QTreeWidgetItem *pParent, Item *pItem)
{
    QTreeWidgetItem *pNewTreeItem;
    if(pParent == NULL)
        pNewTreeItem = new QTreeWidgetItem(ui->treeWidget);
    else
        pNewTreeItem = new QTreeWidgetItem();
    
    pNewTreeItem->setText(0, pItem->GetName());
    pNewTreeItem->setIcon(0, pItem->GetIcon());
//    pNewTreeItem->setFlags(pNewItem->flags() | Qt::ItemIsEditable);
    
    QVariant v; v.setValue(pItem);
    pNewTreeItem->setData(0, Qt::UserRole, v);
    
    if(pParent)
        pParent->addChild(pNewTreeItem);
    
    pItem->SetTreeItem(pNewTreeItem);
    
    return pNewTreeItem;
}

QStringList WidgetExplorer::GetOpenProjectPaths()
{
    QStringList sListOpenProjs;
    sListOpenProjs.clear();
    
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        Item *pItemVariant = ui->treeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<Item *>();
        sListOpenProjs.append(pItemVariant->GetPath());
    }
    
    return sListOpenProjs;
}

Item *WidgetExplorer::GetCurProjSelected()
{
    QTreeWidgetItem *pCurProjItem = ui->treeWidget->currentItem();
    if(pCurProjItem == NULL)
        return NULL;
    
    while(pCurProjItem->parent())
        pCurProjItem = pCurProjItem->parent();
    
    QVariant v = pCurProjItem->data(0, Qt::UserRole);
    return v.value<Item *>();
}

Item *WidgetExplorer::GetCurItemSelected()
{
    QTreeWidgetItem *pCurItem = ui->treeWidget->currentItem();
    if(pCurItem == NULL)
        return NULL;
    
    QVariant v = pCurItem->data(0, Qt::UserRole);
    return v.value<Item *>();
}

Item *WidgetExplorer::GetCurDirSelected(bool bIncludePrefixDirs)
{
    QTreeWidgetItem *pCurTreeItem = ui->treeWidget->currentItem();
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

void WidgetExplorer::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    bool bValidItem = (current != NULL);
    FINDACTION("actionCloseProject")->setEnabled(bValidItem);
    FINDACTION("actionNewAudio")->setEnabled(bValidItem);
    FINDACTION("actionNewParticle")->setEnabled(bValidItem);
    FINDACTION("actionNewFont")->setEnabled(bValidItem);
    FINDACTION("actionNewSprite")->setEnabled(bValidItem);
    FINDACTION("actionNewParticle")->setEnabled(bValidItem);
    FINDACTION("actionNewAudio")->setEnabled(bValidItem);
    
    if(bValidItem)
    {
        bValidItem = false;
        
        Item *pItemDir = GetCurDirSelected(false);
        bValidItem = pItemDir->GetType() == ITEM_DirSprites || pItemDir->GetType() == ITEM_DirFonts;
    }
    FINDACTION("actionNewAtlas")->setEnabled(bValidItem);
        
//    QVariant v = current->data(0, Qt::UserRole);
//    Item *pItemVariant = v.value<Item *>();
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
        MainWindow::OpenItem(pTreeVariantItem);
        break;
    }
}
