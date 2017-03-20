/**************************************************************************
 *	ProjectItem.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ProjectItem.h"
#include "MainWindow.h"
#include "AtlasesWidget.h"
#include "AudioWidgetManager.h"
#include "AtlasFrame.h"

#include "SpriteWidget.h"
#include "FontWidget.h"
#include "FontModels.h"
#include "AudioWidget.h"
#include "AudioDraw.h"

#include <QMenu>

ProjectItem::ProjectItem(Project &projRef,
                         eItemType eType,
                         const QString sPrefix,
                         const QString sName,
                         QJsonValue initValue) :    ExplorerItem(eType, HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType)) % "/" % sPrefix % "/" % sName),
                                                    m_ProjectRef(projRef),
                                                    m_pWidget(nullptr)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pModel = new SpriteModel(this, initValue.toArray());
        break;
    case ITEM_Font:
        m_pModel = new FontModel(this, initValue.toObject());
        break;
    default:
        HyGuiLog("Improper ItemWidget type created: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
    
    m_pUndoStack = new QUndoStack(this);
    m_pActionUndo = m_pUndoStack->createUndoAction(nullptr, "&Undo");
    m_pActionUndo->setIcon(QIcon(":/icons16x16/edit-undo.png"));
    m_pActionUndo->setShortcuts(QKeySequence::Undo);
    m_pActionUndo->setShortcutContext(Qt::ApplicationShortcut);
    m_pActionUndo->setObjectName("Undo");

    m_pActionRedo = m_pUndoStack->createRedoAction(nullptr, "&Redo");
    m_pActionRedo->setIcon(QIcon(":/icons16x16/edit-redo.png"));
    m_pActionRedo->setShortcuts(QKeySequence::Redo);
    m_pActionRedo->setShortcutContext(Qt::ApplicationShortcut);
    m_pActionRedo->setObjectName("Redo");

    connect(m_pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
}

ProjectItem::~ProjectItem()
{
    
}

Project &ProjectItem::GetProject()
{
    return m_ProjectRef;
}

void ProjectItem::GiveMenuActions(QMenu *pMenu)
{
    pMenu->addAction(m_pActionUndo);
    pMenu->addAction(m_pActionRedo);
    pMenu->addSeparator();

    QJsonValue saveValue;
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
        break;
    default:
        HyGuiLog("Improper item GiveMenuActions(): " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::Save()
{
    QJsonValue saveValue;
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        saveValue = static_cast<SpriteModel *>(m_pModel)->GetSaveInfo();
        break;
    default:
        HyGuiLog("Improper item Save(): " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }

    GetProject().SaveGameData(m_eTYPE, GetName(true), saveValue);
    m_pUndoStack->setClean();
}

bool ProjectItem::IsSaveClean()
{
    return m_pUndoStack->isClean();
}

void ProjectItem::DiscardChanges()
{
    m_pUndoStack->clear();
}

void ProjectItem::RefreshWidget(QVariant param)
{
    if(m_pWidget == nullptr)
        return;

    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->Refresh(param);
        break;
    default:
        HyGuiLog("Unsupported IProjItem::RefreshWidget() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::Relink(AtlasFrame *pFrame)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteModel *>(m_pModel)->RelinkFrame(pFrame);
        break;
    default:
        HyGuiLog("Unsupported ProjectItem::Relink() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }

    RefreshWidget(-1);

    Save();
}

void ProjectItem::ProjLoad(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pWidget = new SpriteWidget(*this, hyApp);
        break;
    case ITEM_Font:
        m_pWidget = new FontWidget(*this);
        break;
    case ITEM_Audio:
        m_pWidget = new AudioWidget(*this);
        break;
    default:
        HyGuiLog("Unsupported IProjItem::ProjLoad() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::ProjUnload(IHyApplication &hyApp)
{
    delete m_pWidget;
    m_pWidget = nullptr;
}

void ProjectItem::DrawShow(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->DrawShow();
        break;
    case ITEM_Font:
        //m_pWidget = new FontWidget(*this);
        //break;
    case ITEM_Audio:
        //m_pWidget = new AudioWidget(*this);
        //break;
    default:
        HyGuiLog("Unsupported IProjItem::ProjShow() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::DrawHide(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->DrawHide();
        break;
    case ITEM_Font:
        //m_pWidget = new FontWidget(*this);
        //break;
    case ITEM_Audio:
        //m_pWidget = new AudioWidget(*this);
        //break;
    default:
        HyGuiLog("Unsupported IProjItem::ProjHide() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::DrawUpdate(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->DrawUpdate();
        break;
    case ITEM_Font:
        //m_pWidget = new FontWidget(*this);
        //break;
    case ITEM_Audio:
        //m_pWidget = new AudioWidget(*this);
        //break;
    default:
        HyGuiLog("Unsupported IProjItem::ProjUpdate() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::on_undoStack_cleanChanged(bool bClean)
{
    QTabBar *pTabBar = m_ProjectRef.GetTabBar();
    
    bool bCurItemDirty = false;
    bool bAnyItemDirty = false;
    
    for(int i = 0; i < pTabBar->count(); ++i)
    {
        if(pTabBar->tabData(i).value<ProjectItem *>() == this)
        {
            if(bClean)
                pTabBar->setTabText(i, GetName(false));
            else
                pTabBar->setTabText(i, GetName(false) + "*");
        }
        
        if(pTabBar->tabText(i).contains('*', Qt::CaseInsensitive))
        {
            bAnyItemDirty = true;
            if(pTabBar->currentIndex() == i)
                bCurItemDirty = true;
        }
    }
    
    m_ProjectRef.SetSaveEnabled(bCurItemDirty, bAnyItemDirty);
}

