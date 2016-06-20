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

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "MainWindow.h"

const float fTRANS_DUR = 0.5f;
const int iPADDING = 2;

struct PreviewRow
{
    QList<HyGuiFrame *> m_Frames;
    int                 m_iLargestHeight;

    void Clear()
    {
        m_Frames.clear();
        m_iLargestHeight = 0;
    }

    void TweenPosY(int iStartPosY)
    {
        float fMidRow = (m_iLargestHeight * 0.5f);
        float fPosY = 0.0f;
        foreach(HyGuiFrame *pFrame, m_Frames)
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

    m_pFrameImportActionGroup = new QActionGroup(this);
    m_pFrameImportActionGroup->setExclusive(false);
    m_pFrameImportActionGroup->setEnabled(false);

    m_pFrameRelinquishActionGroup = new QActionGroup(this);
    m_pFrameRelinquishActionGroup->setExclusive(false);
    m_pFrameRelinquishActionGroup->setEnabled(true);
    
    while(ui->atlasGroups->currentWidget())
        delete ui->atlasGroups->currentWidget();
    
    if(m_MetaDir.exists() == false)
        HyGuiLog("Meta atlas directory is missing!", LOGTYPE_Error);

    if(m_DataDir.exists() == false)
        HyGuiLog("Data atlas directory is missing!", LOGTYPE_Error);

    Reload();
}

WidgetAtlasManager::~WidgetAtlasManager()
{
    delete ui;
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

    QFile atlasInfoFile(m_DataDir.absolutePath() % "/" % HYGUIPATH_DataAtlasFileName);
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

void WidgetAtlasManager::PreviewAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_AtlasManager);
}

void WidgetAtlasManager::HideAtlasGroup()
{
    m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_Nothing);
}

/*friend*/ void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
    {
        pFrame->Load();
        pFrame->Reset();
    }

    atlasGrp.ResizeAtlasListColumns();
}

/*friend*/ void AtlasManager_DrawClose(IHyApplication &hyApp, WidgetAtlasManager &atlasMan)
{
    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(atlasMan.ui->atlasGroups->currentWidget());
    
    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
         pFrame->Unload();
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
            pFrame->SetEnabled(false);
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
            QVariant v = atlasMan.m_pMouseHoverItem->data(0, QTreeWidgetItem::UserType);
            HyGuiFrame *pHoverFrame = v.value<HyGuiFrame *>();
            if(pHoverFrame)
                pHoverFrame->Reset();
        }

        atlasMan.m_pMouseHoverItem = pHoveredItem;

        QVariant v = atlasMan.m_pMouseHoverItem->data(0, QTreeWidgetItem::UserType);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        pFrame->SetEnabled(true);
        pFrame->SetDisplayOrder(100);
        pFrame->pos.Set((uiRENDERWIDTH * 0.5f) + (pFrame->GetWidth() * -0.5f),
                        (uiRENDERHEIGHT * 0.5f) + (pFrame->GetHeight() * -0.5f));
        pFrame->color.A(0.5f);
    }
    else if(atlasMan.m_pMouseHoverItem != pHoveredItem)
        atlasMan.m_pMouseHoverItem = NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Reset all frames (except hover) incase any frames got unselected. They will be enabled if selected below
    HyGuiFrame *pHoveredFrame = NULL;

    if(atlasMan.m_pMouseHoverItem)
    {
        QVariant v = atlasMan.m_pMouseHoverItem->data(0, QTreeWidgetItem::UserType);
        pHoveredFrame = v.value<HyGuiFrame *>();
    }

    foreach(HyGuiFrame *pFrame, atlasGrp.GetFrameList())
    {
        if(pHoveredFrame != pFrame && pFrame)
            pFrame->Reset();
    }
    
    if(bDebugPrint)
        HyGuiLog(QString::number(reinterpret_cast<qulonglong>(atlasMan.m_pMouseHoverItem)), LOGTYPE_Normal);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Display all selected

    QPoint ptDrawPos(0, 0);
    PreviewRow curRow;
    curRow.Clear();

    QList<QTreeWidgetItem *> selectedItems = atlasGrp.GetTreeWidget()->selectedItems();
    qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());
    
    for(int i = 0; i < selectedItems.size(); ++i)
    {
        QVariant v = selectedItems[i]->data(0, QTreeWidgetItem::UserType);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        if(pFrame == NULL)
            continue;

        pFrame->SetEnabled(true);
        pFrame->color.A(1.0f);
        pFrame->SetDisplayOrder(-0x0FFFFFFF + i);

        float fFrameWidth = pFrame->IsRotated() ? pFrame->GetHeight() : pFrame->GetWidth();
        float fFrameHeight = pFrame->IsRotated() ? pFrame->GetWidth() : pFrame->GetHeight();

        // Will it fit in this row
        if(ptDrawPos.x() + fFrameWidth > uiRENDERWIDTH)
        {
            curRow.TweenPosY(uiRENDERHEIGHT - ptDrawPos.y());
            
            ptDrawPos.setX(0);
            ptDrawPos.setY(ptDrawPos.y() + curRow.m_iLargestHeight + iPADDING);
            
            curRow.Clear();
        }
        
        float fPosX = ptDrawPos.x() + (pFrame->IsRotated() ? ((fFrameWidth * 0.5f) - (fFrameHeight * 0.5f)) : 0);
        
        if(pFrame->pos.AnimX().IsTransforming() == false && pFrame->pos.X() != ptDrawPos.x())
            pFrame->pos.AnimX().Tween(fPosX, fTRANS_DUR, HyTween::QuadInOut);

        ptDrawPos.setX(ptDrawPos.x() + fFrameWidth + iPADDING);

        if(curRow.m_iLargestHeight < fFrameHeight)
            curRow.m_iLargestHeight = fFrameHeight;

        curRow.m_Frames.append(pFrame);
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

void WidgetAtlasManager::Reload()
{
    m_pMouseHoverItem = NULL;
    
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
}

QAction *WidgetAtlasManager::CreateRequestFramesAction(Item *pRequester)
{
    QObject *pParent = NULL;
    switch(pRequester->GetType())
    {
    case ITEM_Sprite:
        pParent = pRequester->GetWidget();
        break;

    default:
        HyGuiLog("Unsupported item type in WidgetAtlasManager::CreateImportFrameAction", LOGTYPE_Error);
        return NULL;
    }

    QAction *pNewAction = new QAction(pParent);
    pNewAction->setIcon(QIcon(":/icons16x16/generic-add.png"));
    pNewAction->setData(QVariant(pRequester->GetType()));
    pNewAction->setObjectName("actionAddFrames");
    //pNewAction->setShortcuts(QKeySequence::Undo);
    //pNewAction->setShortcutContext(Qt::ApplicationShortcut);
    connect(pNewAction, SIGNAL(triggered()), this, SLOT(on_actionImportFrames_triggered()));
    pNewAction->setActionGroup(m_pFrameImportActionGroup);

    return pNewAction;
}

QAction *WidgetAtlasManager::CreateRelinquishFramesAction(Item *pRequester)
{
    QObject *pParent = NULL;
    switch(pRequester->GetType())
    {
    case ITEM_Sprite:
        pParent = pRequester->GetWidget();
        break;

    default:
        HyGuiLog("Unsupported item type in WidgetAtlasManager::CreateRelinquishFrameAction", LOGTYPE_Error);
        return NULL;
    }

    QAction *pNewAction = new QAction(pParent);
    pNewAction->setIcon(QIcon(":/icons16x16/edit-delete.png"));
    pNewAction->setData(QVariant(pRequester->GetName(true)));
    pNewAction->setObjectName("actionRemoveFrame");
    //pNewAction->setShortcuts(QKeySequence::Undo);
    //pNewAction->setShortcutContext(Qt::ApplicationShortcut);
    connect(pNewAction, SIGNAL(triggered()), this, SLOT(on_actionRelinqishFrames_triggered()));
    pNewAction->setActionGroup(m_pFrameRelinquishActionGroup);

    return pNewAction;
}

void WidgetAtlasManager::SetFramesAvailableForImport()
{
    WidgetAtlasGroup *pAtlasGrp = static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget());
    if(pAtlasGrp)
    {
        QList<QTreeWidgetItem *> selectedItems = pAtlasGrp->GetTreeWidget()->selectedItems();
        m_pFrameImportActionGroup->setEnabled(selectedItems.count() != 0);
    }
    else
        m_pFrameImportActionGroup->setEnabled(false);
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
    for(int i = 0; i < ui->atlasGroups->count(); ++i)
    {
        if(static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->IsMatching(newMetaAtlasDir, newDataAtlasDir))
        {
            static_cast<WidgetAtlasGroup *>(ui->atlasGroups->widget(i))->Reload();
            
            bGroupAlreadyExists = true;
            break;
        }
    }
    
    if(bGroupAlreadyExists == false)
    {
        WidgetAtlasGroup *pNewAtlas = new WidgetAtlasGroup(newMetaAtlasDir, newDataAtlasDir, this);
        ui->atlasGroups->setCurrentIndex(ui->atlasGroups->addWidget(pNewAtlas));
    }
}

void WidgetAtlasManager::on_atlasGroups_currentChanged(int iIndex)
{
    SetFramesAvailableForImport();
}

void WidgetAtlasManager::on_btnAddGroup_clicked()
{

}

void WidgetAtlasManager::on_actionImportFrames_triggered()
{
    QAction* pAction = qobject_cast<QAction*>(sender());
    Q_ASSERT(pAction);
    
    pAction->property(

    switch(pAction->data().toInt())
    {
    case ITEM_Sprite:
        static_cast<WidgetSprite *>(pAction->parentWidget())->OnRequestFrames(
        pParent = pRequester->GetWidget();
        break;

    default:
        HyGuiLog("Unsupported item type in WidgetAtlasManager::on_actionImportFrames_triggered", LOGTYPE_Error);
        return NULL;
    }

    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget());
    QList<QTreeWidgetItem *> selectedItems = atlasGrp.GetTreeWidget()->selectedItems();
    qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());

    for(int i = 0; i < selectedItems.size(); ++i)
    {
        QVariant v = selectedItems[i]->data(0, QTreeWidgetItem::UserType);
        HyGuiFrame *pFrame = v.value<HyGuiFrame *>();

        if(pFrame == NULL)
            continue;

        //frameListOut.append(QPair<int, int>(pFrame->GetTextureIndex(), pFrame->Get
    }
}

void WidgetAtlasManager::on_actionRelinqishFrames_triggered()
{
    QAction* pAction = qobject_cast<QAction*>(sender());
    Q_ASSERT(pAction);

    WidgetAtlasGroup &atlasGrp = *static_cast<WidgetAtlasGroup *>(ui->atlasGroups->currentWidget());
    //atlasGrp.GetTreeWidget()->selectedItems().count() != 0;
}
