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
#include "AtlasWidget.h"
#include "AudioWidgetManager.h"
#include "AtlasFrame.h"
#include "IModel.h"

#include "SpriteWidget.h"
#include "FontWidget.h"
#include "FontModels.h"
#include "AudioWidget.h"
#include "AudioDraw.h"

#include <QMenu>

ProjectItem::ProjectItem(Project &projRef,
                         HyGuiItemType eType,
                         const QString sPrefix,
                         const QString sName,
                         QJsonValue initValue) :    ExplorerItem(eType, HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType)) % "/" % sPrefix % "/" % sName),
                                                    m_ProjectRef(projRef),
                                                    m_SaveValue(initValue),
                                                    m_CurValue(initValue),
                                                    m_pModel(nullptr),
                                                    m_pWidget(nullptr),
                                                    m_pDraw(nullptr)
{
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

    LoadModel();

    connect(m_pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
    connect(m_pUndoStack, SIGNAL(indexChanged(int)), this, SLOT(on_undoStack_indexChanged(int)));
}

ProjectItem::~ProjectItem()
{
    
}

void ProjectItem::LoadModel()
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pModel = new SpriteModel(this, m_SaveValue.toArray());
        break;
    case ITEM_Font:
        m_pModel = new FontModel(this, m_SaveValue.toObject());
        break;
    default:
        HyGuiLog("Improper ItemWidget type created: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
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
    case ITEM_Font:
        static_cast<FontWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
        break;
    default:
        HyGuiLog("Improper item GiveMenuActions(): " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::Save()
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_SaveValue = static_cast<SpriteModel *>(m_pModel)->GetSaveInfo();
        break;
    case ITEM_Font:
        m_SaveValue = static_cast<FontModel *>(m_pModel)->GetSaveInfo();
        break;
    default:
        HyGuiLog("Improper item Save(): " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }

    GetProject().SaveGameData(m_eTYPE, GetName(true), m_SaveValue);
    m_pUndoStack->setClean();
}

bool ProjectItem::IsSaveClean()
{
    return m_pUndoStack->isClean();
}

void ProjectItem::DiscardChanges()
{
    m_pUndoStack->clear();

    delete m_pModel;
    LoadModel();
}

void ProjectItem::WidgetRefreshDraw(IHyApplication &hyApp)
{
    m_pModel->Refresh();
    
    delete m_pDraw;
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pDraw = new SpriteDraw(this, hyApp);
        break;
    case ITEM_Font:
        m_pDraw = new FontDraw(this, hyApp);
        break;
    default:
        HyGuiLog("Unimplemented WidgetRefreshDraw() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
    
    m_pDraw->Load();
    m_pDraw->SetEnabled(false);
}

void ProjectItem::WidgetLoad(IHyApplication &hyApp)
{
    delete m_pWidget;
    delete m_pDraw;
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_pWidget = new SpriteWidget(*this);
        m_pDraw = new SpriteDraw(this, hyApp);
        break;
    case ITEM_Font:
        m_pWidget = new FontWidget(*this);
        m_pDraw = new FontDraw(this, hyApp);
        break;
    case ITEM_Audio:
        m_pWidget = new AudioWidget(*this);
        //m_pDraw = new AudioDraw(*static_cast<AudioModel *>(m_pModel), hyApp);
        break;
    default:
        HyGuiLog("Unimplemented WidgetLoad() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
    
    m_pDraw->Load();
    m_pDraw->SetEnabled(false);
}

void ProjectItem::WidgetUnload(IHyApplication &hyApp)
{
    delete m_pWidget;
    m_pWidget = nullptr;
    
    delete m_pDraw;
    m_pDraw = nullptr;
}

void ProjectItem::WidgetShow(IHyApplication &hyApp)
{
    m_pDraw->Show();
}

void ProjectItem::WidgetHide(IHyApplication &hyApp)
{
    m_pDraw->Hide();
}

//void ProjectItem::WidgetUpdate(IHyApplication &hyApp)
//{
//    m_pDraw->WidgetUpdate(m_pWidget);
//}

void ProjectItem::BlockAllWidgetSignals(bool bBlock)
{
    if(m_pWidget == nullptr)
        return;
    
    QList<QWidget *> widgetList = m_pWidget->findChildren<QWidget *>();
    for(auto iter = widgetList.begin(); iter != widgetList.end(); ++iter)
        (*iter)->blockSignals(bBlock);
}

void ProjectItem::WidgetRefreshData(QVariant param)
{
    if(m_pWidget == nullptr)
        return;

    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        static_cast<SpriteWidget *>(m_pWidget)->RefreshData(param);
        break;
    case ITEM_Font:
        static_cast<FontWidget *>(m_pWidget)->RefreshData(param);
        break;
    default:
        HyGuiLog("Unsupported ProjectItem::RefreshWidget() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }
}

void ProjectItem::DeleteFromProject()
{
    m_pUndoStack->setClean();
    GetProject().DeleteGameData(m_eTYPE, GetName(true));
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

void ProjectItem::on_undoStack_indexChanged(int iIndex)
{
    switch(m_eTYPE)
    {
    case ITEM_Sprite:
        m_CurValue = static_cast<SpriteModel *>(m_pModel)->GetSaveInfo();
        break;
    case ITEM_Font:
        m_CurValue = static_cast<FontModel *>(m_pModel)->GetSaveInfo();
        break;
    default:
        HyGuiLog("Improper item in on_undoStack_indexChanged(): " % QString::number(m_eTYPE), LOGTYPE_Error);
        break;
    }

    //GetProject().SaveGameData(m_eTYPE, GetName(true), m_SaveValue);
}

