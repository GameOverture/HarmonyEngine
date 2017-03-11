/**************************************************************************
 *	WidgetAtlasManager.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAtlasManager.h"
#include "ui_WidgetAtlasManager.h"
#include "HyGlobal.h"
#include "WidgetSprite.h"
#include "Item.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QPainter>

#include "MainWindow.h"
#include "DlgInputName.h"
#include "DlgAtlasGroupSettings.h"

const float fTRANS_DUR = 0.5f;
const int iPADDING = 2;

struct PreviewRow
{
    QList<HyTexturedQuad2d *>   m_Frames;
    int                         m_iLargestHeight;

    void Clear()
    {
        m_Frames.clear();
        m_iLargestHeight = 0;
    }

    void TweenPosY(int iStartPosY)
    {
        float fMidRow = (m_iLargestHeight * 0.5f);
        float fPosY = 0.0f;
        for(int i = 0; i < m_Frames.size(); ++i)
        {
            fPosY = iStartPosY - (m_Frames[i]->GetHeight() * 0.5f) - fMidRow;
            if(m_Frames[i]->pos.IsTweening() == false && m_Frames[i]->pos.Y() != fPosY)
                m_Frames[i]->pos.Tween(m_Frames[i]->pos.X(), fPosY, fTRANS_DUR, HyTween::QuadInOut);
        }
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetAtlasGroupTreeWidget::WidgetAtlasGroupTreeWidget(QWidget *parent /*= Q_NULLPTR*/) : QTreeWidget(parent)
{
}
void WidgetAtlasGroupTreeWidget::SetOwner(WidgetAtlasManager *pOwner)
{
    m_pOwner = pOwner;
}
/*virtual*/ void WidgetAtlasGroupTreeWidget::dropEvent(QDropEvent *e)
{
    QTreeWidget::dropEvent(e);

    sortItems(0, Qt::AscendingOrder);
    m_pOwner->m_DataRef.WriteMetaSettings();
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

WidgetAtlasManager::WidgetAtlasManager(QWidget *parent) :   QWidget(parent),
                                                            ui(new Ui::WidgetAtlasManager)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    HyGuiLog("WidgetAtlasManager::WidgetAtlasManager() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasManager::WidgetAtlasManager(ItemAtlases &itemDataRef, QWidget *parent /*= 0*/) :  QWidget(parent),
                                                                                             ui(new Ui::WidgetAtlasManager),
                                                                                             m_DataRef(itemDataRef),
                                                                                             m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                                                             m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                                                             m_pMouseHoverItem(NULL)
{
    ui->setupUi(this);
    
    if(m_MetaDir.exists() == false)
    {
        HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
        m_MetaDir.mkpath(m_MetaDir.absolutePath());
    }
    if(m_DataDir.exists() == false)
    {
        HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
        m_DataDir.mkpath(m_DataDir.absolutePath());
    }




    // Set the default name, which will get overwritten if LoadSettings() is called
    m_dlgSettings.SetName("Remove this name feature");

    ui->actionDeleteImages->setEnabled(false);
    ui->actionReplaceImages->setEnabled(false);

    ui->btnDeleteImages->setDefaultAction(ui->actionDeleteImages);
    ui->btnReplaceImages->setDefaultAction(ui->actionReplaceImages);
    ui->btnAddFilter->setDefaultAction(ui->actionAddFilter);

    ui->atlasList->SetOwner(this);

    ui->atlasList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->atlasList->setDragEnabled(true);
    //ui->atlasList->viewport()->setAcceptDrops(true);
    ui->atlasList->setDropIndicatorShown(true);
    ui->atlasList->setDragDropMode(QAbstractItemView::InternalMove);

    QList<AtlasTreeItem *> atlasTreeItemList = m_pProjOwner->GetAtlasTreeItemList();
    for(int i = 0; i < atlasTreeItemList.size(); ++i)
        ui->atlasList->addTopLevelItem(atlasTreeItemList[i]);

    if(atlasTreeItemList.empty())
        WriteMetaSettings();
    else
        ui->atlasList->sortItems(0, Qt::AscendingOrder);

    ui->lcdNumTextures->display(m_DataDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot).size());
    ui->lcdTexWidth->display(m_dlgSettings.TextureWidth());
    ui->lcdTexHeight->display(m_dlgSettings.TextureHeight());

    ui->atlasList->collapseAll();
}

WidgetAtlasManager::~WidgetAtlasManager()
{
    delete ui;
}


/*friend*/ void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    for(int i = 0; i < atlasMan.m_FrameList.size(); ++i)
    {
        atlasMan.m_FrameList[i]->DrawInst(&atlasMan)->Load();
        atlasMan.m_FrameList[i]->DrawInst(&atlasMan)->SetEnabled(false);
        atlasMan.m_FrameList[i]->DrawInst(&atlasMan)->SetDisplayOrder(0);
        atlasMan.m_FrameList[i]->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
        atlasMan.m_FrameList[i]->DrawInst(&atlasMan)->alpha.Set(1.0f);
        atlasMan.m_FrameList[i]->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    }
}

/*friend*/ void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
}

/*friend*/ void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
//    for(int i = 0; i < atlasMan.ui->atlasGroups->count(); ++i)
//    {
//        WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->widget(i));
        
//        QList<HyGuiFrame *> atlasGroupFrameList = atlasGrp.GetFrameList();
//        for(int j = 0; j < atlasGroupFrameList.size(); ++j)
//            atlasGroupFrameList[j]->DrawInst(&atlasMan)->SetEnabled(false);
//    }
}

/*friend*/ void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
//    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
//    static int DEBUGCNT = 0;
//    DEBUGCNT++;
//    bool bDebugPrint = false;
//    if((DEBUGCNT % 100) == 0)
//    {
//        bDebugPrint = true;
//        DEBUGCNT = 0;
//    }

//    const uint32 uiRENDERWIDTH = hyApp.Window().GetResolution().x;
//    const uint32 uiRENDERHEIGHT = hyApp.Window().GetResolution().y;
    
//    if(atlasMan.m_pMouseHoverItem && atlasMan.m_pMouseHoverItem->isSelected())
//        atlasMan.m_pMouseHoverItem = NULL;

//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    // Preview hover selection
//    QTreeWidgetItem *pHoveredItem = atlasGrp.GetTreeWidget()->itemAt(atlasGrp.GetTreeWidget()->mapFromGlobal(QCursor::pos()));
//    if(pHoveredItem && atlasMan.m_pMouseHoverItem != pHoveredItem && pHoveredItem->isSelected() == false)
//    {
//        if(atlasMan.m_pMouseHoverItem && atlasMan.m_pMouseHoverItem->isSelected() == false)
//        {
//            QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
//            HyGuiFrame *pHoverFrame = v.value<HyGuiFrame *>();
//            if(pHoverFrame)
//            {
//                pHoverFrame->DrawInst(&atlasMan)->SetEnabled(false);
//                pHoverFrame->DrawInst(&atlasMan)->SetDisplayOrder(0);
//                pHoverFrame->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
//                pHoverFrame->DrawInst(&atlasMan)->alpha.Set(1.0f);
//                pHoverFrame->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
//            }
//        }

//        atlasMan.m_pMouseHoverItem = pHoveredItem;

//        QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
//        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

//        if(pFrame)
//        {
//            pFrame->DrawInst(&atlasMan)->SetEnabled(true);
//            pFrame->DrawInst(&atlasMan)->SetDisplayOrder(100);
//            pFrame->DrawInst(&atlasMan)->pos.Set((uiRENDERWIDTH * 0.5f) + (pFrame->DrawInst(&atlasMan)->GetWidth() * -0.5f),
//                                                 (uiRENDERHEIGHT * 0.5f) + (pFrame->DrawInst(&atlasMan)->GetHeight() * -0.5f));
            
//            pFrame->DrawInst(&atlasMan)->alpha.Set(0.5f);
//        }
//    }
//    else if(atlasMan.m_pMouseHoverItem != pHoveredItem)
//        atlasMan.m_pMouseHoverItem = NULL;

//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    // Reset all frames (except hover) incase any frames got unselected. They will be enabled if selected below
//    HyGuiFrame *pHoveredFrame = NULL;

//    if(atlasMan.m_pMouseHoverItem)
//    {
//        QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
//        pHoveredFrame = v.value<HyGuiFrame *>();
//    }

//    QList<HyGuiFrame *> atlasGrpFrameList = atlasGrp.GetFrameList();
//    for(int i = 0; i < atlasGrpFrameList.size(); ++i)
//    {
//        if(pHoveredFrame != atlasGrpFrameList[i] && atlasGrpFrameList[i])
//        {
//            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetEnabled(false);
//            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetDisplayOrder(0);
//            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
//            atlasGrpFrameList[i]->DrawInst(&atlasMan)->alpha.Set(1.0f);
//            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
//        }
//    }


//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    // Display all selected

//    QPoint ptDrawPos(0, 0);
//    PreviewRow curRow;
//    curRow.Clear();

//    QList<QTreeWidgetItem *> selectedItems = atlasGrp.GetTreeWidget()->selectedItems();
//    qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());
    
//    for(int i = 0; i < selectedItems.size(); ++i)
//    {
//        QVariant v = selectedItems[i]->data(0, Qt::UserRole);
//        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

//        if(pFrame == NULL)
//            continue;

//        pFrame->DrawInst(&atlasMan)->SetEnabled(true);
//        pFrame->DrawInst(&atlasMan)->alpha.Set(1.0f);

//        float fFrameWidth = pFrame->DrawInst(&atlasMan)->GetWidth();
//        float fFrameHeight = pFrame->DrawInst(&atlasMan)->GetHeight();

//        // Will it fit in this row
//        if(ptDrawPos.x() + fFrameWidth > uiRENDERWIDTH)
//        {
//            curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());
            
//            ptDrawPos.setX(0);
//            ptDrawPos.setY(ptDrawPos.y() + curRow.m_iLargestHeight + iPADDING);
            
//            curRow.Clear();
//        }
        
//        float fPosX = ptDrawPos.x();
        
//        if(pFrame->DrawInst(&atlasMan)->pos.IsTweening() == false && pFrame->DrawInst(&atlasMan)->pos.X() != ptDrawPos.x())
//            pFrame->DrawInst(&atlasMan)->pos.Tween(fPosX, pFrame->DrawInst(&atlasMan)->pos.Y(), fTRANS_DUR, HyTween::QuadInOut);

//        ptDrawPos.setX(ptDrawPos.x() + fFrameWidth + iPADDING);

//        if(curRow.m_iLargestHeight < fFrameHeight)
//            curRow.m_iLargestHeight = fFrameHeight;

//        curRow.m_Frames.append(pFrame->DrawInst(&atlasMan));
//    }
    
//    curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //QPointF ptCamPos(uiCurWidth * 0.5f, uiCurHeight * 0.5f);

    // Pan camera over previewed
    //HyGuiLog("Hover: " % QString::number(atlasGrp.m_pMouseHoverFrame->pos.X()) % ", " % QString::number(atlasGrp.m_pMouseHoverFrame->pos.Y()), LOGTYPE_Normal);
    //HyGuiLog("Cam  : " % QString::number(pProj->m_pCamera->pos.X()) % ", " % QString::number(pProj->m_pCamera->pos.Y()), LOGTYPE_Normal);
    //if(pProj->m_pCamera && pProj->m_pCamera->pos.IsTweening() == false)
        //pProj->m_pCamera->pos.Animate(iFrameCount * 12, iFrameCount * 12, 1.0f, HyEase::quadInOut);
}

void WidgetAtlasManager::PreviewAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_AtlasManager);
}

void WidgetAtlasManager::HideAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_Nothing);
}


void WidgetAtlasManager::on_btnAddImages_clicked()
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

    if(sImportImgList.empty() == false)
        Repack(QSet<int>(), ImportImages(sImportImgList));
}

void WidgetAtlasManager::on_btnAddDir_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);

    if(dlg.exec() == QDialog::Rejected)
        return;

    QStringList sDirs = dlg.selectedFiles();
    QStringList sImportImgList;
    for(int iDirIndex = 0; iDirIndex < sDirs.size(); ++iDirIndex)
    {
        QDir dirEntry(sDirs[iDirIndex]);
        HyGlobal::RecursiveFindOfFileExt("png", sImportImgList, dirEntry);
    }

    if(sImportImgList.empty() == false)
        Repack(QSet<int>(), ImportImages(sImportImgList));
}

void WidgetAtlasManager::on_btnSettings_clicked()
{
    if(m_dlgSettings.GetName().isEmpty())
        m_dlgSettings.SetName("Atlas Group");

    m_dlgSettings.DataToWidgets();
    if(QDialog::Accepted == m_dlgSettings.exec())
    {
        m_dlgSettings.WidgetsToData();  // Save the changes

        if(m_dlgSettings.IsSettingsDirty())
            RepackAll();
        else if(m_dlgSettings.IsNameChanged())
            WriteMetaSettings();
    }
    else
        m_dlgSettings.DataToWidgets();  // Reverts changes made
}

void WidgetAtlasManager::on_actionDeleteImages_triggered()
{
    QSet<int> affectedTextureIndexSet;

    QList<QTreeWidgetItem *> selectedImageList = ui->atlasList->selectedItems();
    for(int i = 0; i < selectedImageList.count(); ++i)
    {
        HyGuiFrame *pFrame = selectedImageList[i]->data(0, Qt::UserRole).value<HyGuiFrame *>();
        QSet<ItemWidget *> sLinks = pFrame->GetLinks();
        if(sLinks.empty() == false)
        {
            QString sMessage = "'" % pFrame->GetName() % "' image cannot be deleted because it is in use by the following items: \n\n";
            for(QSet<ItemWidget *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
                sMessage.append(HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem((*LinksIter)->GetType())) % "/" % (*LinksIter)->GetName(true) % "\n");

            HyGuiLog(sMessage, LOGTYPE_Warning);
            continue;
        }

        affectedTextureIndexSet.insert(pFrame->GetTextureIndex());

        m_FrameList.removeOne(pFrame);
        RemoveImage(pFrame, m_MetaDir);
        delete selectedImageList[i];
    }

    Repack(affectedTextureIndexSet, QSet<HyGuiFrame *>());
}

void WidgetAtlasManager::on_actionReplaceImages_triggered()
{
    QSet<int> affectedTextureIndexSet;

    QList<QTreeWidgetItem *> atlasSelectedImageList = ui->atlasList->selectedItems();

    // Store a list of the frames, since 'atlasSelectedImageList' will become invalid within Refresh()
    QList<HyGuiFrame *> selectedImageList;
    for(int i = 0; i < atlasSelectedImageList.count(); ++i)
        selectedImageList.append(atlasSelectedImageList[i]->data(0, Qt::UserRole).value<HyGuiFrame *>());

    QFileDialog dlg(this);

    if(selectedImageList.count() == 1)
    {
        dlg.setFileMode(QFileDialog::ExistingFile);
        dlg.setWindowTitle("Select an image as the replacement");
    }
    else
    {
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setWindowTitle("Select " % QString::number(selectedImageList.count()) % " images as replacements");
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

        if(sImportImgList.count() != selectedImageList.count())
            HyGuiLog("You must select " % QString::number(selectedImageList.count()) % " images", LOGTYPE_Warning);
    }
    while(sImportImgList.count() != selectedImageList.count());

    for(int i = 0; i < selectedImageList.count(); ++i)
    {
        HyGuiLog("Replacing: " % selectedImageList[i]->GetName() % " -> " % sImportImgList[i], LOGTYPE_Info);

        QFileInfo fileInfo(sImportImgList[i]);
        QImage newImage(fileInfo.absoluteFilePath());

        affectedTextureIndexSet.insert(selectedImageList[i]->GetTextureIndex());

        ReplaceFrame(selectedImageList[i], fileInfo.fileName(), newImage, false);
    }

    Repack(affectedTextureIndexSet, QSet<HyGuiFrame *>());

    for(int i = 0; i < selectedImageList.count(); ++i)
    {
        QSet<ItemWidget *> sLinks = selectedImageList[i]->GetLinks();
        for(QSet<ItemWidget *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
            (*LinksIter)->Relink(selectedImageList[i]);
    }
}

void WidgetAtlasManager::on_actionAddFilter_triggered()
{
    AtlasTreeItem *pNewTreeItem = new AtlasTreeItem(ui->atlasList);

    DlgInputName *pDlg = new DlgInputName("Enter Atlas Group Filter Name", "New Filter");
    if(pDlg->exec() == QDialog::Accepted)
        pNewTreeItem->setText(0, pDlg->GetName());
    else
    {
        delete pDlg;
        return;
    }

    delete pDlg;

    pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Prefix));
    pNewTreeItem->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));

    ui->atlasList->sortItems(0, Qt::AscendingOrder);
    WriteMetaSettings();
}

/*virtual*/ void WidgetAtlasManager::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if(ui->atlasList == NULL)
        return;

    int iTotalWidth = ui->atlasList->size().width();
    ui->atlasList->setColumnWidth(0, iTotalWidth - 60);
}

void WidgetAtlasManager::on_atlasList_itemSelectionChanged()
{
    int iNumSelected = ui->atlasList->selectedItems().count();

    ui->actionDeleteImages->setEnabled(iNumSelected != 0);
    ui->actionReplaceImages->setEnabled(iNumSelected != 0);
}
