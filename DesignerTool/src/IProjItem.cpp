/**************************************************************************
 *	ItemWidget.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IProjItem.h"
#include "MainWindow.h"
#include "AtlasesWidget.h"
#include "AudioWidgetManager.h"
#include "AtlasFrame.h"

#include "SpriteWidget.h"
#include "SpriteDraw.h"
#include "SpriteItem.h"
#include "FontWidget.h"
#include "FontDraw.h"
#include "AudioWidget.h"
#include "AudioDraw.h"

#include <QMenu>

IProjItem::IProjItem(Project *pItemProj,
                       eItemType eType,
                       const QString sPrefix,
                       const QString sName) :   ExplorerItem(eType, HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType)) % "/" % sPrefix % "/" % sName),
                                                m_pItemProj(pItemProj),
                                                m_pDraw(nullptr),
                                                m_pWidget(nullptr)
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

IProjItem::~IProjItem()
{
    
}

Project *IProjItem::GetProject()
{
    return m_pItemProj;
}

void IProjItem::GiveMenuActions(QMenu *pMenu)
{
    pMenu->addAction(m_pActionUndo);
    pMenu->addAction(m_pActionRedo);
    pMenu->addSeparator();

    OnGiveMenuActions(pMenu);
}

void IProjItem::Save()
{
    // TODO: Save must work with item not opened/loaded
    GetProject()->SaveGameData(m_eTYPE, GetName(true), OnSave());
    m_pUndoStack->setClean();
}

bool IProjItem::IsSaveClean()
{
    return m_pUndoStack->isClean();
}

void IProjItem::DiscardChanges()
{
    m_pUndoStack->clear();
}

void IProjItem::ProjLoad(IHyApplication &hyApp)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pWidget = new SpriteWidget(static_cast<SpriteItem *>(this));
        m_pDraw = new SpriteDraw(static_cast<SpriteItem *>(this));
        break;
    case ITEM_Font:
        m_pWidget = new FontWidget(static_cast<FontItem *>(this));
        m_pDraw = new FontDraw(static_cast<FontItem *>(this));
        break;
    case ITEM_Audio:
        m_pWidget = new AudioWidget(static_cast<AudioItem *>(this));
        break;
    case ITEM_Project:
    case ITEM_DirAudio:
    case ITEM_DirParticles:
    case ITEM_DirFonts:
    case ITEM_DirSpine:
    case ITEM_DirSprites:
    case ITEM_DirShaders:
    case ITEM_DirEntities:
    case ITEM_DirAtlases:
    case ITEM_DirAudioBanks:
    case ITEM_Prefix:
    case ITEM_Particles:
    case ITEM_Spine:
    case ITEM_Shader:
    case ITEM_Entity:
        HyGuiLog("Unsupported IProjItem::ProjLoad() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }

    m_pDraw->ProjLoad(hyApp);
}

void IProjItem::ProjUnload(IHyApplication &hyApp)
{
    delete m_pDraw;
    delete m_pWidget;
    
    m_pDraw = nullptr;
    m_pWidget = nullptr;
}

void IProjItem::ProjShow(IHyApplication &hyApp)
{
    m_pDraw->ProjShow(hyApp);
}

void IProjItem::ProjHide(IHyApplication &hyApp)
{
    m_pDraw->ProjHide(hyApp);
}

void IProjItem::ProjUpdate(IHyApplication &hyApp)
{
    m_pDraw->ProjUpdate(hyApp);
}

void IProjItem::Link(AtlasFrame *pFrame)
{
    OnLink(pFrame);
    m_Links.insert(pFrame);
}

void IProjItem::Relink(AtlasFrame *pFrame)
{
    if(m_pDraw)
        m_pDraw->Relink(pFrame);

    if(m_pWidget)
    {
        switch(m_eTYPE)
        {
        case ITEM_Sprite:   static_cast<SpriteWidget *>(m_pWidget)->Relink(pFrame);     break;
        case ITEM_Font:     /*static_cast<FontWidget *>(m_pWidget)->Relink(pFrame);*/   break;
        case ITEM_Audio:    /*static_cast<AudioWidget *>(m_pWidget)->Relink(pFrame);*/  break;
        case ITEM_Project:
        case ITEM_DirAudio:
        case ITEM_DirParticles:
        case ITEM_DirFonts:
        case ITEM_DirSpine:
        case ITEM_DirSprites:
        case ITEM_DirShaders:
        case ITEM_DirEntities:
        case ITEM_DirAtlases:
        case ITEM_DirAudioBanks:
        case ITEM_Prefix:
        case ITEM_Particles:
        case ITEM_Spine:
        case ITEM_Shader:
        case ITEM_Entity:
        default:
            HyGuiLog("Unsupported IProjItem::Relink() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
            break;
        }
    }
    
    Save();
}

void IProjItem::Unlink(AtlasFrame *pFrame)
{
    OnUnlink(pFrame);
    m_Links.remove(pFrame);
}

void IProjItem::on_undoStack_cleanChanged(bool bClean)
{
    QTabBar *pTabBar = m_pItemProj->GetTabBar();
    
    bool bCurItemDirty = false;
    bool bAnyItemDirty = false;
    
    for(int i = 0; i < pTabBar->count(); ++i)
    {
        if(pTabBar->tabData(i).value<IProjItem *>() == this)
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

