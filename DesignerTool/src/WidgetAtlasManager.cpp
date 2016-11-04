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

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>

#include "MainWindow.h"

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
        foreach(HyTexturedQuad2d *pFrame, m_Frames)
        {
            fPosY = iStartPosY - (pFrame->GetHeight() * 0.5f) - fMidRow;
            if(pFrame->pos.AnimY().IsTransforming() == false && pFrame->pos.Y() != fPosY)
                pFrame->pos.AnimY().Tween(fPosY, fTRANS_DUR, HyTween::QuadInOut);
        }
    }
};

WidgetAtlasManager::WidgetAtlasManager(QWidget *parent) :   QWidget(parent),
                                                            ui(new Ui::WidgetAtlasManager)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    HyGuiLog("WidgetAtlasManager::WidgetAtlasManager() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasManager::WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                                             ui(new Ui::WidgetAtlasManager),
                                                                                             m_pProjOwner(pProjOwner),
                                                                                             m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                                                             m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                                                             m_pMouseHoverItem(NULL)
{
    ui->setupUi(this);
    while(ui->atlasGroups->currentWidget())
        delete ui->atlasGroups->currentWidget();
    
    ui->actionDeleteAtlasGroup->setEnabled(false);
    ui->btnDeleteGroup->setDefaultAction(ui->actionDeleteAtlasGroup);
    
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

    QFileInfoList metaAtlasDirs = m_MetaDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    if(metaAtlasDirs.empty())
    {
        HyGuiLog("Empty atlas directory, creating new empty group", LOGTYPE_Info);
        AddAtlasGroup();
    }
    else
    {
        foreach(QFileInfo dir, metaAtlasDirs)
        {
            if(dir.isDir())
            {
                bool bWorked = false;
                int iId = dir.baseName().toInt(&bWorked);

                if(bWorked && iId >= 0)
                    AddAtlasGroup(iId);
            }
        }
    }

    m_pCmbModel = AllocateAtlasModelView();
    ui->cmbAtlasGroups->setModel(m_pCmbModel);
}

WidgetAtlasManager::~WidgetAtlasManager()
{
    while(ui->atlasGroups->currentWidget())
        delete ui->atlasGroups->currentWidget();

    delete ui;
}

WidgetAtlasModelView *WidgetAtlasManager::AllocateAtlasModelView()
{
    return new WidgetAtlasModelView(*ui->atlasGroups, this);
}

int WidgetAtlasManager::CurrentAtlasGroupIndex()
{
    return ui->cmbAtlasGroups->currentIndex();
}

int WidgetAtlasManager::GetAtlasIdFromIndex(int iIndex)
{
    return static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(iIndex))->GetId();
}

QSize WidgetAtlasManager::GetAtlasDimensions(int iIndex)
{
    return static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(iIndex))->GetAtlasDimensions();
}

HyGuiFrame *WidgetAtlasManager::GenerateFrame(ItemWidget *pItem, int iAtlasGroupId, QString sName, QImage &newImage, eAtlasNodeType eType)
{
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        WidgetAtlasGroup *pAtlasGroup = static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i));

        if(pAtlasGroup->GetId() == iAtlasGroupId)
        {
            // This allocates a new HyGuiFrame into the dependency map
            HyGuiFrame *pFrame = pAtlasGroup->ImportImage(sName, newImage, eType);
            pAtlasGroup->Refresh();

            // This retrieves the newly created HyGuiFrame from the dependency map
            QList<quint32> checksumList;
            checksumList.append(pFrame->GetChecksum());
            QList<HyGuiFrame *> returnList = RequestFrames(pItem, checksumList);

            if(returnList.empty() == false)
                return returnList[0];
        }
    }

    return NULL;
}

void WidgetAtlasManager::ReplaceFrame(HyGuiFrame *pFrame, QString sName, QImage &newImage)
{
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        WidgetAtlasGroup *pAtlasGroup = static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i));

        if(pAtlasGroup->GetId() == pFrame->GetAtlasGroupdId())
        {
            pFrame->ReplaceImage(sName, newImage, pAtlasGroup->m_MetaDir);
            pAtlasGroup->Refresh();
            return;
        }
    }
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget());
    QList<QTreeWidgetItem *> selectedItems = atlasGrp.GetTreeWidget()->selectedItems();
    qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());

    atlasGrp.GetTreeWidget()->clearSelection();

    QList<quint32> checksumRequestList;
    for(int i = 0; i < selectedItems.size(); ++i)
    {
        HyGuiFrame *pFrame = selectedItems[i]->data(0, Qt::UserRole).value<HyGuiFrame *>();
        if(pFrame == NULL)
            continue;

        checksumRequestList.append(pFrame->GetChecksum());
    }

    if(checksumRequestList.empty())
        return QList<HyGuiFrame *>();

    return RequestFrames(pItem, checksumRequestList);
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem, QList<HyGuiFrame *> requestList)
{
    if(requestList.empty())
        return RequestFrames(pItem);
    
    QList<quint32> checksumRequestList;
    for(int i = 0; i < requestList.size(); ++i)
        checksumRequestList.append(requestList[i]->GetChecksum());

    return RequestFrames(pItem, checksumRequestList);
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem, QList<quint32> requestList)
{
    if(requestList.empty())
        return RequestFrames(pItem);
    
    QList<HyGuiFrame *> returnList;
    for(int i = 0; i < requestList.size(); ++i)
    {
        QMap<quint32, HyGuiFrame *>::iterator iter = m_DependencyMap.find(requestList[i]);
        
        if(iter == m_DependencyMap.end())
        {
            // TODO: Support a "Yes to all" dialog functionality here
            HyGuiLog("Cannot find image with checksum: " % QString::number(requestList[i]) % "\nIt may have been removed, or is invalid in the Atlas Manager.", LOGTYPE_Warning);
        }
        else
        {
            SetDependency(iter.value(), pItem);
            returnList.append(iter.value());
        }
    }
    
    return returnList;
}

void WidgetAtlasManager::RelinquishFrames(ItemWidget *pItem, QList<HyGuiFrame *> relinquishList)
{
    for(int i = 0; i < relinquishList.size(); ++i)
        RemoveDependency(relinquishList[i], pItem);
}

/*friend*/ void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
    {
        pFrame->DrawInst(&atlasMan)->Load();
        pFrame->DrawInst(&atlasMan)->SetEnabled(false);
        pFrame->DrawInst(&atlasMan)->SetDisplayOrder(0);
        pFrame->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
        pFrame->DrawInst(&atlasMan)->SetTransparency(1.0f);
        pFrame->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    }

    atlasGrp.ResizeAtlasListColumns();
}

/*friend*/ void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
}

/*friend*/ void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    for(int i = 0; i < atlasMan.ui->atlasGroups->count(); ++i)
    {
        WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->widget(i));
        
        foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
            pFrame->DrawInst(&atlasMan)->SetEnabled(false);
    }
}

/*friend*/ void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
    static int DEBUGCNT = 0;
    DEBUGCNT++;
    bool bDebugPrint = false;
    if((DEBUGCNT % 100) == 0)
    {
        bDebugPrint = true;
        DEBUGCNT = 0;
    }

    const uint32 uiRENDERWIDTH = hyApp.Window().GetResolution().x;
    const uint32 uiRENDERHEIGHT = hyApp.Window().GetResolution().y;
    
    if(atlasMan.m_pMouseHoverItem && atlasMan.m_pMouseHoverItem->isSelected())
        atlasMan.m_pMouseHoverItem = NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Preview hover selection
    QTreeWidgetItem *pHoveredItem = atlasGrp.GetTreeWidget()->itemAt(atlasGrp.GetTreeWidget()->mapFromGlobal(QCursor::pos()));
    if(pHoveredItem && atlasMan.m_pMouseHoverItem != pHoveredItem && pHoveredItem->isSelected() == false)
    {
        if(atlasMan.m_pMouseHoverItem && atlasMan.m_pMouseHoverItem->isSelected() == false)
        {
            QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
            HyGuiFrame *pHoverFrame = v.value<HyGuiFrame *>();
            if(pHoverFrame)
            {
                pHoverFrame->DrawInst(&atlasMan)->SetEnabled(false);
                pHoverFrame->DrawInst(&atlasMan)->SetDisplayOrder(0);
                pHoverFrame->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
                pHoverFrame->DrawInst(&atlasMan)->SetTransparency(1.0f);
                pHoverFrame->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
            }
        }

        atlasMan.m_pMouseHoverItem = pHoveredItem;

        QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        if(pFrame)
        {
            pFrame->DrawInst(&atlasMan)->SetEnabled(true);
            pFrame->DrawInst(&atlasMan)->SetDisplayOrder(100);
            pFrame->DrawInst(&atlasMan)->pos.Set((uiRENDERWIDTH * 0.5f) + (pFrame->DrawInst(&atlasMan)->GetWidth() * -0.5f),
                                                 (uiRENDERHEIGHT * 0.5f) + (pFrame->DrawInst(&atlasMan)->GetHeight() * -0.5f));
            pFrame->DrawInst(&atlasMan)->SetTransparency(0.5f);
        }
    }
    else if(atlasMan.m_pMouseHoverItem != pHoveredItem)
        atlasMan.m_pMouseHoverItem = NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Reset all frames (except hover) incase any frames got unselected. They will be enabled if selected below
    HyGuiFrame *pHoveredFrame = NULL;

    if(atlasMan.m_pMouseHoverItem)
    {
        QVariant v = atlasMan.m_pMouseHoverItem->data(0, Qt::UserRole);
        pHoveredFrame = v.value<HyGuiFrame *>();
    }

    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
    {
        if(pHoveredFrame != pFrame && pFrame)
        {
            pFrame->DrawInst(&atlasMan)->SetEnabled(false);
            pFrame->DrawInst(&atlasMan)->SetDisplayOrder(0);
            pFrame->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
            pFrame->DrawInst(&atlasMan)->SetTransparency(1.0f);
            pFrame->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Display all selected

    QPoint ptDrawPos(0, 0);
    PreviewRow curRow;
    curRow.Clear();

    QList<QTreeWidgetItem *> selectedItems = atlasGrp.GetTreeWidget()->selectedItems();
    qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());
    
    for(int i = 0; i < selectedItems.size(); ++i)
    {
        QVariant v = selectedItems[i]->data(0, Qt::UserRole);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        if(pFrame == NULL)
            continue;

        pFrame->DrawInst(&atlasMan)->SetEnabled(true);
        pFrame->DrawInst(&atlasMan)->SetTransparency(1.0f);
        //pFrame->DrawInst(&atlasMan)->SetDisplayOrder(-0x0FFFFFFF + i);

        float fFrameWidth = pFrame->IsRotated() ? pFrame->DrawInst(&atlasMan)->GetHeight() : pFrame->DrawInst(&atlasMan)->GetWidth();
        float fFrameHeight = pFrame->IsRotated() ? pFrame->DrawInst(&atlasMan)->GetWidth() : pFrame->DrawInst(&atlasMan)->GetHeight();

        // Will it fit in this row
        if(ptDrawPos.x() + fFrameWidth > uiRENDERWIDTH)
        {
            curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());
            
            ptDrawPos.setX(0);
            ptDrawPos.setY(ptDrawPos.y() + curRow.m_iLargestHeight + iPADDING);
            
            curRow.Clear();
        }
        
        float fPosX = ptDrawPos.x() + (pFrame->IsRotated() ? ((fFrameWidth * 0.5f) - (fFrameHeight * 0.5f)) : 0);
        
        if(pFrame->DrawInst(&atlasMan)->pos.AnimX().IsTransforming() == false && pFrame->DrawInst(&atlasMan)->pos.X() != ptDrawPos.x())
            pFrame->DrawInst(&atlasMan)->pos.AnimX().Tween(fPosX, fTRANS_DUR, HyTween::QuadInOut);

        ptDrawPos.setX(ptDrawPos.x() + fFrameWidth + iPADDING);

        if(curRow.m_iLargestHeight < fFrameHeight)
            curRow.m_iLargestHeight = fFrameHeight;

        curRow.m_Frames.append(pFrame->DrawInst(&atlasMan));
    }
    
    curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //QPointF ptCamPos(uiCurWidth * 0.5f, uiCurHeight * 0.5f);

    // Pan camera over previewed
    //HyGuiLog("Hover: " % QString::number(atlasGrp.m_pMouseHoverFrame->pos.X()) % ", " % QString::number(atlasGrp.m_pMouseHoverFrame->pos.Y()), LOGTYPE_Normal);
    //HyGuiLog("Cam  : " % QString::number(pProj->m_pCamera->pos.X()) % ", " % QString::number(pProj->m_pCamera->pos.Y()), LOGTYPE_Normal);
    //if(pProj->m_pCamera && pProj->m_pCamera->pos.IsTweening() == false)
        //pProj->m_pCamera->pos.Animate(iFrameCount * 12, iFrameCount * 12, 1.0f, HyEase::quadInOut);
}

void WidgetAtlasManager::AddAtlasGroup(int iId /*= -1*/)
{
    if(iId == -1)
    {
        // Find first available directory name
        iId = 0;
        QFileInfoList atlasDirs = m_MetaDir.entryInfoList(QDir::Dirs, QDir::Name);
        foreach(QFileInfo info, atlasDirs)
        {
            if(info.isDir() && info.baseName().toInt() == iId)
                iId++;
        }
        
        // Make new atlas group in both meta and data directories
        if(false == m_MetaDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HyGuiLog("Failed to create new meta-atlas directory", LOGTYPE_Error)
        else
            HyGuiLog("Created new meta-atlas group: " + iId, LOGTYPE_Info)

        if(false == m_DataDir.mkdir(HyGlobal::MakeFileNameFromCounter(iId)))
            HyGuiLog("Failed to create new data-atlas directory", LOGTYPE_Error)
        else
            HyGuiLog("Created new data-atlas group: " + iId, LOGTYPE_Info)
    }
    
    QDir newMetaAtlasDir(m_MetaDir);
    if(newMetaAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId)) == false)
    {
        HyGuiLog("Failed to 'cd' into meta-atlas group directory: " + iId, LOGTYPE_Error);
        return;
    }

    QDir newDataAtlasDir(m_DataDir);
    if(newDataAtlasDir.cd(HyGlobal::MakeFileNameFromCounter(iId)) == false)
    {
        HyGuiLog("Failed to 'cd' into data-atlas group directory: " + iId, LOGTYPE_Error);
        return;
    }
    
    bool bGroupAlreadyExists = false;
//    for(int i = 0; i < ui->atlasGroups->count(); ++i)
//    {
//        if(static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->IsMatching(newMetaAtlasDir, newDataAtlasDir))
//        {
//            //static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->Reload();
            
//            bGroupAlreadyExists = true;
//            break;
//        }
//    }
    
    if(bGroupAlreadyExists == false)
    {
        WidgetAtlasGroup *pNewAtlas = new WidgetAtlasGroup(newMetaAtlasDir, newDataAtlasDir, this, this);
        ui->atlasGroups->setCurrentIndex(ui->atlasGroups->addWidget(pNewAtlas));
        
//        ui->cmbAtlasGroups->addItem(pNewAtlas->GetName(), QVariant(pNewAtlas->GetId()));
        ui->cmbAtlasGroups->setCurrentIndex(ui->atlasGroups->currentIndex());
    }
}

void WidgetAtlasManager::PreviewAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_AtlasManager);
}

void WidgetAtlasManager::HideAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_Nothing);
}

HyGuiFrame *WidgetAtlasManager::CreateFrame(quint32 uiChecksum, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, eAtlasNodeType eType, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY, uint uiErrors)
{
    HyGuiFrame *pNewFrame = NULL;

    if(m_DependencyMap.contains(uiChecksum))
    {
        HyGuiLog("WidgetAtlasManager::CreateFrame() already contains frame with this checksum: " % QString::number(uiChecksum), LOGTYPE_Error);

        pNewFrame = new HyGuiFrame(uiChecksum, sN, rAlphaCrop, uiAtlasGroupId, eType, iW, iH, iTexIndex, bRot, iX, iY, uiErrors);
        pNewFrame->SetError(GUIFRAMEERROR_Duplicate);
    }
    else
    {
        pNewFrame = new HyGuiFrame(uiChecksum, sN, rAlphaCrop, uiAtlasGroupId, eType, iW, iH, iTexIndex, bRot, iX, iY, uiErrors);
        m_DependencyMap[uiChecksum] = pNewFrame;
    }

    return pNewFrame;
}

void WidgetAtlasManager::RemoveImage(HyGuiFrame *pFrame)
{
    m_DependencyMap.remove(pFrame->GetChecksum());
    delete pFrame;

    // In case the removed image happened to be the current 'm_pMouseHoverItem'
    m_pMouseHoverItem = NULL;
}

void WidgetAtlasManager::SaveData()
{
    QJsonArray atlasGroupArray;
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        QJsonObject atlasGroupObj;
        static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->GetAtlasInfo(atlasGroupObj);
        atlasGroupArray.append(atlasGroupObj);
    }

    QJsonDocument atlasInfoDoc;
    atlasInfoDoc.setArray(atlasGroupArray);

    QFile atlasInfoFile(m_DataDir.absolutePath() % "/" % HYGUIPATH_DataAtlases);
    if(atlasInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
    }
    else
    {
        qint64 iBytesWritten = atlasInfoFile.write(atlasInfoDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to atlas settings file: " % atlasInfoFile.errorString(), LOGTYPE_Error);
        }

        atlasInfoFile.close();
    }
}

void WidgetAtlasManager::SetDependency(HyGuiFrame *pFrame, ItemWidget *pItem)
{
    pFrame->m_Links.insert(pItem);
    pItem->Link(pFrame);
}

void WidgetAtlasManager::RemoveDependency(HyGuiFrame *pFrame, ItemWidget *pItem)
{
    pFrame->m_Links.remove(pItem);
    pItem->Unlink(pFrame);
}

void WidgetAtlasManager::on_atlasGroups_currentChanged(int iIndex)
{
    ui->actionDeleteAtlasGroup->setEnabled(ui->atlasGroups->currentWidget() != NULL);
}

void WidgetAtlasManager::on_btnAddGroup_clicked()
{
    AddAtlasGroup();
}

void WidgetAtlasManager::on_cmbAtlasGroups_currentIndexChanged(int index)
{
//    for(int i = 0; i < ui->atlasGroups->count(); ++i)
//    {
//        if(static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->GetId() == ui->cmbAtlasGroups->currentData().toInt())
//        {
//            // Unload the old atlas group draw instances
//            WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget());
//            foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
//                 pFrame->DrawInst(this)->Unload();
            
//            m_pProjOwner->Reset();
            
//            // Then set the new atlas group
//            ui->atlasGroups->setCurrentIndex(i);
//        }
//    }

    if(ui->atlasGroups->count() == 0)
        return;

    // Unload the old atlas group draw instances
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget());
    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
         pFrame->DrawInst(this)->Unload();

    m_pProjOwner->Reset();

    // Then set the new atlas group
    ui->atlasGroups->setCurrentIndex(index);
}

void WidgetAtlasManager::on_actionDeleteAtlasGroup_triggered()
{
    
}

WidgetAtlasModelView::WidgetAtlasModelView(QStackedWidget &atlasGroupsRef, QObject *pParent) :  QStringListModel(pParent),
                                                                                                m_AtlasGroupsRef(atlasGroupsRef)
{ }

/*virtual*/ QVariant WidgetAtlasModelView::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
    if(role == Qt::DisplayRole)
    {
        if(m_AtlasGroupsRef.count() == 0)
            return "";
        else
            return static_cast<WidgetAtlasGroup *>(m_AtlasGroupsRef.widget(index.row()))->GetName();
    }
    else
        return QStringListModel::data(index, role);
}

/*virtual*/ int	WidgetAtlasModelView::rowCount(const QModelIndex & parent /*= QModelIndex()*/) const
{
    return m_AtlasGroupsRef.count();
}

