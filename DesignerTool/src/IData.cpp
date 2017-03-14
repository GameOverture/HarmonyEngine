/**************************************************************************
 *	ItemWidget.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IData.h"
#include "MainWindow.h"
#include "AtlasesWidget.h"
#include "AudioWidgetManager.h"
#include "AtlasFrame.h"

#include <QMenu>

IData::IData(Project *pItemProj,
                       eItemType eType,
                       const QString sPrefix,
                       const QString sName,
                       QJsonValue initVal) :    ExplorerItem(eType, HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType)) % "/" % sPrefix % "/" % sName),
                                                m_pItemProj(pItemProj),
                                                m_InitValue(initVal),
                                                m_pWidget(nullptr),
                                                m_pCamera(nullptr),
                                                m_bReloadDraw(false)
{
    switch(m_eTYPE)
    {
    case ITEM_Project:
    case ITEM_DirAudio:
    case ITEM_DirParticles:
    case ITEM_DirFonts:
    case ITEM_DirSpine:
    case ITEM_DirSprites:
    case ITEM_DirShaders:
    case ITEM_DirEntities:
    case ITEM_DirAtlases:
    case ITEM_Prefix:
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

IData::~IData()
{
    
}

Project *IData::GetItemProject()
{
    return m_pItemProj;
}

void IData::GiveMenuActions(QMenu *pMenu)
{
    pMenu->addAction(m_pActionUndo);
    pMenu->addAction(m_pActionRedo);
    pMenu->addSeparator();

    OnGiveMenuActions(pMenu);
}

void IData::Save()
{
    GetItemProject()->SaveGameData(m_eTYPE, GetName(true), OnSave());
    m_pUndoStack->setClean();
}

bool IData::IsSaveClean()
{
    return m_pUndoStack->isClean();
}

void IData::DiscardChanges()
{
    m_pUndoStack->clear();
}

void IData::Load(IHyApplication &hyApp)
{
    // A non NULL camera signifies that this has been loaded already
    if(m_pCamera)
        return;

    m_pCamera = hyApp.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);

    OnGuiLoad(hyApp);
    //m_HyEntity.Load();
}

void IData::Unload(IHyApplication &hyApp)
{
    // A NULL camera signifies that this has hasn't been loaded
    if(m_pCamera == NULL)
        return;

    hyApp.Window().RemoveCamera(m_pCamera);
    m_pCamera = NULL;

    OnGuiUnload(hyApp);
    //m_HyEntity.Load();
}

void IData::DrawShow(IHyApplication &hyApp)
{
    m_pCamera->SetEnabled(true);

    OnGuiShow(hyApp);
}

void IData::DrawHide(IHyApplication &hyApp)
{
    m_pCamera->SetEnabled(false);

    OnGuiHide(hyApp);
}

void IData::DrawUpdate(IHyApplication &hyApp)
{
    if(m_bReloadDraw || IsLoaded() == false)
    {
        m_bReloadDraw = false;

        if(IsLoaded())
            Unload(hyApp);

        Load(hyApp);
        DrawShow(hyApp);
    }

    OnGuiUpdate(hyApp);
}

void IData::Link(AtlasFrame *pFrame)
{
    OnLink(pFrame);
    m_Links.insert(pFrame);
}

void IData::Relink(AtlasFrame *pFrame)
{
    if(IsLoaded())
        m_bReloadDraw = true;

    OnReLink(pFrame);
    Save();
}

void IData::Unlink(AtlasFrame *pFrame)
{
    pFrame->DeleteDrawInst(this);

    OnUnlink(pFrame);
    m_Links.remove(pFrame);
}

void IData::on_undoStack_cleanChanged(bool bClean)
{
    QTabBar *pTabBar = m_pItemProj->GetTabBar();
    
    bool bCurItemDirty = false;
    bool bAnyItemDirty = false;
    
    for(int i = 0; i < pTabBar->count(); ++i)
    {
        if(pTabBar->tabData(i).value<IData *>() == this)
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
    
    m_pItemProj->SetSaveEnabled(bCurItemDirty, bAnyItemDirty);
}

