/**************************************************************************
 *	ItemWidget.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemWidget.h"
#include "MainWindow.h"
#include "WidgetAtlasManager.h"
#include "WidgetAudioManager.h"

#include <QMenu>

ItemWidget::ItemWidget(eItemType eType,
                       const QString sPrefix,
                       const QString sName,
                       QJsonValue initVal,
                       WidgetAtlasManager &AtlasManRef,
                       WidgetAudioManager &AudioManRef) :   Item(eType, HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType)) % "/" % sPrefix % "/" % sName),
                                                            m_InitValue(initVal),
                                                            m_AtlasManRef(AtlasManRef),
                                                            m_AudioManRef(AudioManRef),
                                                            m_pWidget(NULL),
                                                            m_pEditMenu(NULL),
                                                            m_pCamera(NULL),
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
    
    m_pEditMenu = new QMenu("Edit", MainWindow::GetInstance());
    
    m_pUndoStack = new QUndoStack(this);
    QAction *pActionUndo = m_pUndoStack->createUndoAction(m_pEditMenu, "&Undo");
    pActionUndo->setIcon(QIcon(":/icons16x16/edit-undo.png"));
    pActionUndo->setShortcuts(QKeySequence::Undo);
    pActionUndo->setShortcutContext(Qt::ApplicationShortcut);
    pActionUndo->setObjectName("Undo");

    QAction *pActionRedo = m_pUndoStack->createRedoAction(m_pEditMenu, "&Redo");
    pActionRedo->setIcon(QIcon(":/icons16x16/edit-redo.png"));
    pActionRedo->setShortcuts(QKeySequence::Redo);
    pActionRedo->setShortcutContext(Qt::ApplicationShortcut);
    pActionRedo->setObjectName("Redo");

    m_pEditMenu->addAction(pActionUndo);
    m_pEditMenu->addAction(pActionRedo);
    QAction *pUndoSeparatorAction = new QAction(m_pEditMenu);
    pUndoSeparatorAction->setObjectName("UndoSeparator");
    pUndoSeparatorAction->setSeparator(true);
    m_pEditMenu->addAction(pUndoSeparatorAction);
    
    connect(m_pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
}

ItemWidget::~ItemWidget()
{
    
}

void ItemWidget::Load(IHyApplication &hyApp)
{
    // A non NULL camera signifies that this has been loaded already
    if(m_pCamera)
        return;

    m_pCamera = hyApp.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);

    OnLoad(hyApp);
}

void ItemWidget::Unload(IHyApplication &hyApp)
{
    // A NULL camera signifies that this has hasn't been loaded
    if(m_pCamera == NULL)
        return;

    hyApp.Window().RemoveCamera(m_pCamera);
    m_pCamera = NULL;

    OnUnload(hyApp);
}

void ItemWidget::DrawShow(IHyApplication &hyApp)
{
    m_pCamera->SetEnabled(true);

    OnDraw_Show(hyApp);
}

void ItemWidget::DrawHide(IHyApplication &hyApp)
{
    m_pCamera->SetEnabled(false);

    OnDraw_Hide(hyApp);
}

void ItemWidget::DrawUpdate(IHyApplication &hyApp)
{
    if(m_bReloadDraw || IsLoaded() == false)
    {
        m_bReloadDraw = false;
        
        if(IsLoaded())
            Unload(hyApp);
        
        Load(hyApp);
        DrawShow(hyApp);
    }

    OnDraw_Update(hyApp);
}

void ItemWidget::Link(HyGuiFrame *pFrame)
{
    OnLink(pFrame);
    m_Links.insert(pFrame);
}

void ItemWidget::Relink(HyGuiFrame *pFrame)
{
    if(IsLoaded())
        m_bReloadDraw = true;

    OnReLink(pFrame);
    Save();
}

void ItemWidget::Unlink(HyGuiFrame *pFrame)
{
    pFrame->DeleteDrawInst(this);

    OnUnlink(pFrame);
    m_Links.remove(pFrame);
}

ItemProject *ItemWidget::GetItemProject()
{
    return m_AtlasManRef.GetProjOwner();
}

void ItemWidget::Save()
{
    GetItemProject()->SaveGameData(m_eTYPE, GetName(true), OnSave());
    m_pUndoStack->setClean();
}

bool ItemWidget::IsSaveClean()
{
    return m_pUndoStack->isClean();
}

void ItemWidget::DiscardChanges()
{
    m_pUndoStack->clear();
}

void ItemWidget::on_undoStack_cleanChanged(bool bClean)
{
    ItemProject *pItemProj = m_AtlasManRef.GetProjOwner();
    QTabBar *pTabBar = pItemProj->GetTabBar();
    
    bool bCurItemDirty = false;
    bool bAnyItemDirty = false;
    
    for(int i = 0; i < pTabBar->count(); ++i)
    {
        if(pTabBar->tabData(i).value<ItemWidget *>() == this)
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
    
    pItemProj->SetSaveEnabled(bCurItemDirty, bAnyItemDirty);
}

