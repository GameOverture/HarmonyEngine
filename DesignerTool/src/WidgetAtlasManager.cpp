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
        for(int i = 0; i < m_Frames.size(); ++i)
        {
            fPosY = iStartPosY - (m_Frames[i]->GetHeight() * 0.5f) - fMidRow;
            if(m_Frames[i]->pos.IsTweening() == false && m_Frames[i]->pos.Y() != fPosY)
                m_Frames[i]->pos.Tween(m_Frames[i]->pos.X(), fPosY, fTRANS_DUR, HyTween::QuadInOut);
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
        for(int i = 0; i < metaAtlasDirs.size(); ++i)
        {
            if(metaAtlasDirs[i].isDir())
            {
                bool bWorked = false;
                int iId = metaAtlasDirs[i].baseName().toInt(&bWorked);

                if(bWorked && iId >= 0)
                    AddAtlasGroup(iId);
            }
        }
    }

    m_pCmbModel = AllocateAtlasModelView();
    ui->cmbAtlasGroups->blockSignals(true);
    ui->cmbAtlasGroups->setModel(m_pCmbModel);
    ui->cmbAtlasGroups->blockSignals(false);
}

WidgetAtlasManager::~WidgetAtlasManager()
{
    while(ui->atlasGroups->currentWidget())
        delete ui->atlasGroups->currentWidget();

    delete ui;
}

WidgetAtlasGroupModel *WidgetAtlasManager::AllocateAtlasModelView()
{
    return new WidgetAtlasGroupModel(*ui->atlasGroups, this);
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

QString WidgetAtlasManager::GetSelectedAtlasGroup()
{
    return ui->cmbAtlasGroups->currentText();
}

void WidgetAtlasManager::SetSelectedAtlasGroup(QString sName)
{
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        if(ui->cmbAtlasGroups->itemText(i) == sName)
            ui->cmbAtlasGroups->setCurrentIndex(i);
    }
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

            QSet<HyGuiFrame *> newFrameSet;
            newFrameSet.insert(pFrame);
            pAtlasGroup->Repack(QSet<int>(), newFrameSet);

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

void WidgetAtlasManager::ReplaceFrame(HyGuiFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack)
{
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        WidgetAtlasGroup *pAtlasGroup = static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i));

        if(pAtlasGroup->GetId() == pFrame->GetAtlasGroupdId())
        {
            QSet<int> textureIndexToReplaceSet;
            textureIndexToReplaceSet.insert(pFrame->GetTextureIndex());

            if(0 == (pFrame->GetErrors() & GUIFRAMEERROR_Duplicate))
                m_DependencyMap.remove(pFrame->GetChecksum());

            quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
            pFrame->ReplaceImage(sName, uiChecksum, newImage, pAtlasGroup->m_MetaDir);

            if(m_DependencyMap.contains(uiChecksum))
            {
                HyGuiLog("WidgetAtlasManager::ReplaceFrame() already contains frame with this checksum: " % QString::number(uiChecksum), LOGTYPE_Info);
                pFrame->SetError(GUIFRAMEERROR_Duplicate);
            }
            else
            {
                m_DependencyMap[uiChecksum] = pFrame;
                pFrame->ClearError(GUIFRAMEERROR_Duplicate);
            }

            if(bDoAtlasGroupRepack)
                pAtlasGroup->Repack(textureIndexToReplaceSet, QSet<HyGuiFrame *>());

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

    QList<HyGuiFrame *> frameRequestList;
    for(int i = 0; i < selectedItems.size(); ++i)
    {
        HyGuiFrame *pFrame = selectedItems[i]->data(0, Qt::UserRole).value<HyGuiFrame *>();
        if(pFrame == NULL)
            continue;

        frameRequestList.append(pFrame);
    }

    if(frameRequestList.empty())
        return QList<HyGuiFrame *>();

    return RequestFrames(pItem, frameRequestList);
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem, QList<quint32> requestList)
{
    if(requestList.empty())
        return RequestFrames(pItem);
    
    QList<HyGuiFrame *> frameRequestList;
    for(int i = 0; i < requestList.size(); ++i)
    {
        QMap<quint32, HyGuiFrame *>::iterator iter = m_DependencyMap.find(requestList[i]);
        if(iter == m_DependencyMap.end())
        {
            // TODO: Support a "Yes to all" dialog functionality here
            HyGuiLog("Cannot find image with checksum: " % QString::number(requestList[i]) % "\nIt may have been removed, or is invalid in the Atlas Manager.", LOGTYPE_Warning);
        }
        else
            frameRequestList.append(iter.value());
    }

    return RequestFrames(pItem, frameRequestList);
}

QList<HyGuiFrame *> WidgetAtlasManager::RequestFrames(ItemWidget *pItem, QList<HyGuiFrame *> requestList)
{
    if(requestList.empty())
        return RequestFrames(pItem);
    
    QList<HyGuiFrame *> returnList;
    for(int i = 0; i < requestList.size(); ++i)
    {
        SetDependency(requestList[i], pItem);
        returnList.append(requestList[i]);
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
    
    QList<HyGuiFrame *> atlasFrameList = atlasGrp.GetFrameList();
    for(int i = 0; i < atlasFrameList.size(); ++i)
    {
        atlasFrameList[i]->DrawInst(&atlasMan)->Load();
        atlasFrameList[i]->DrawInst(&atlasMan)->SetEnabled(false);
        atlasFrameList[i]->DrawInst(&atlasMan)->SetDisplayOrder(0);
        atlasFrameList[i]->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
        atlasFrameList[i]->DrawInst(&atlasMan)->alpha.Set(1.0f);
        atlasFrameList[i]->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
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
        
        QList<HyGuiFrame *> atlasGroupFrameList = atlasGrp.GetFrameList();
        for(int j = 0; j < atlasGroupFrameList.size(); ++j)
            atlasGroupFrameList[j]->DrawInst(&atlasMan)->SetEnabled(false);
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
                pHoverFrame->DrawInst(&atlasMan)->alpha.Set(1.0f);
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
            
            pFrame->DrawInst(&atlasMan)->alpha.Set(0.5f);
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

    QList<HyGuiFrame *> atlasGrpFrameList = atlasGrp.GetFrameList();
    for(int i = 0; i < atlasGrpFrameList.size(); ++i)
    {
        if(pHoveredFrame != atlasGrpFrameList[i] && atlasGrpFrameList[i])
        {
            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetEnabled(false);
            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetDisplayOrder(0);
            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetTint(1.0f, 1.0f, 1.0f);
            atlasGrpFrameList[i]->DrawInst(&atlasMan)->alpha.Set(1.0f);
            atlasGrpFrameList[i]->DrawInst(&atlasMan)->SetCoordinateType(HYCOORDTYPE_Screen, NULL);
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
        pFrame->DrawInst(&atlasMan)->alpha.Set(1.0f);

        float fFrameWidth = pFrame->DrawInst(&atlasMan)->GetWidth();
        float fFrameHeight = pFrame->DrawInst(&atlasMan)->GetHeight();

        // Will it fit in this row
        if(ptDrawPos.x() + fFrameWidth > uiRENDERWIDTH)
        {
            curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());
            
            ptDrawPos.setX(0);
            ptDrawPos.setY(ptDrawPos.y() + curRow.m_iLargestHeight + iPADDING);
            
            curRow.Clear();
        }
        
        float fPosX = ptDrawPos.x();
        
        if(pFrame->DrawInst(&atlasMan)->pos.IsTweening() == false && pFrame->DrawInst(&atlasMan)->pos.X() != ptDrawPos.x())
            pFrame->DrawInst(&atlasMan)->pos.Tween(fPosX, pFrame->DrawInst(&atlasMan)->pos.Y(), fTRANS_DUR, HyTween::QuadInOut);

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
        for(int i = 0; i < atlasDirs.size(); ++i)
        {
            if(atlasDirs[i].isDir() && atlasDirs[i].baseName().toInt() == iId)
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
    
    WidgetAtlasGroup *pNewAtlas = new WidgetAtlasGroup(newMetaAtlasDir, newDataAtlasDir, this, this);
    ui->atlasGroups->setCurrentIndex(ui->atlasGroups->addWidget(pNewAtlas));
    ui->cmbAtlasGroups->setCurrentIndex(ui->atlasGroups->currentIndex());
}

void WidgetAtlasManager::PreviewAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_AtlasManager);
}

void WidgetAtlasManager::HideAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_Nothing);
}

HyGuiFrame *WidgetAtlasManager::CreateFrame(quint32 uiChecksum, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, eAtlasNodeType eType, int iW, int iH, int iTexIndex, int iX, int iY, uint uiErrors)
{
    HyGuiFrame *pNewFrame = NULL;

    if(m_DependencyMap.contains(uiChecksum))
    {
        HyGuiFrame *pExistingFrame = m_DependencyMap.find(uiChecksum).value();
        HyGuiLog("'" % sN % "' is a duplicate of '" % pExistingFrame->GetName() % "' with the checksum: " % QString::number(uiChecksum), LOGTYPE_Info);

        pNewFrame = new HyGuiFrame(uiChecksum, sN, rAlphaCrop, uiAtlasGroupId, eType, iW, iH, iTexIndex, iX, iY, uiErrors);

        pNewFrame->SetError(GUIFRAMEERROR_Duplicate);
        pExistingFrame->SetError(GUIFRAMEERROR_Duplicate);
    }
    else
    {
        pNewFrame = new HyGuiFrame(uiChecksum, sN, rAlphaCrop, uiAtlasGroupId, eType, iW, iH, iTexIndex, iX, iY, uiErrors);
        m_DependencyMap[uiChecksum] = pNewFrame;
    }

    return pNewFrame;
}

void WidgetAtlasManager::RemoveImage(HyGuiFrame *pFrame, QDir metaDir)
{
    m_DependencyMap.remove(pFrame->GetChecksum());
    pFrame->DeleteMetaImage(metaDir);

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
        static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->GetAtlasInfoForGameData(atlasGroupObj);
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
    
    QList<HyGuiFrame *> atlasGrpFrameList = atlasGrp.GetFrameList();
    for(int i = 0; i < atlasGrpFrameList.size(); ++i)
         atlasGrpFrameList[i]->DrawInst(this)->Unload();

    m_pProjOwner->Reset();

    // Then set the new atlas group
    ui->atlasGroups->setCurrentIndex(index);
}

void WidgetAtlasManager::on_actionDeleteAtlasGroup_triggered()
{
    
}

WidgetAtlasGroupModel::WidgetAtlasGroupModel(QStackedWidget &atlasGroupsRef, QObject *pParent) :  QStringListModel(pParent),
                                                                                                m_AtlasGroupsRef(atlasGroupsRef)
{ }

/*virtual*/ QVariant WidgetAtlasGroupModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
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

/*virtual*/ int	WidgetAtlasGroupModel::rowCount(const QModelIndex & parent /*= QModelIndex()*/) const
{
    return m_AtlasGroupsRef.count();
}

