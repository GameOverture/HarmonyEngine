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

#include <QMenu>

ItemWidget::ItemWidget(eItemType eType, const QString sPath, WidgetAtlasManager *pAtlasMan) :   Item(eType, sPath),
                                                                                                m_pAtlasMan(pAtlasMan),
                                                                                                m_pWidget(NULL),
                                                                                                m_pEditMenu(NULL),
                                                                                                m_pCamera(NULL)
{
    if(m_pAtlasMan == NULL && m_eTYPE != ITEM_Project)
        HyGuiLog("ItemWidget ctor of type [" % QString::number(m_eTYPE) % "] was passed a NULL WidgetAtlasManager", LOGTYPE_Error);

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
        break;
    case ITEM_Audio:
    case ITEM_Particles:
    case ITEM_Font:
    case ITEM_Spine:
    case ITEM_Shader:
    case ITEM_Entity:
    case ITEM_Sprite:
        m_pEditMenu = new QMenu("Edit", MainWindow::GetInstance());
        break;
    }
}

ItemWidget::~ItemWidget()
{
    
}

void ItemWidget::DrawOpen(IHyApplication &hyApp)
{
    if(m_pCamera == NULL)
        m_pCamera = hyApp.Window().CreateCamera2d();

    m_pCamera->SetEnabled(false);

    OnDraw_Open(hyApp);
}

void ItemWidget::DrawClose(IHyApplication &hyApp)
{
    if(m_pCamera)
        hyApp.Window().RemoveCamera(m_pCamera);

    OnDraw_Close(hyApp);
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
    OnDraw_Update(hyApp);
}

///*virtual*/ void ItemWidget::OnDraw_Open(IHyApplication &hyApp)
//{
//    HyGuiLog("Tried to OnDraw_Open() a non-derived item: " % GetName(true), LOGTYPE_Error);
//}

///*virtual*/ void ItemWidget::OnDraw_Close(IHyApplication &hyApp)
//{
//    HyGuiLog("Tried to OnDraw_Close() a non-derived item: " % GetName(true), LOGTYPE_Error);
//}

///*virtual*/ void ItemWidget::OnDraw_Show(IHyApplication &hyApp)
//{
//    HyGuiLog("Tried to OnDraw_Show() a non-derived item: " % GetName(true), LOGTYPE_Error);
//}

///*virtual*/ void ItemWidget::OnDraw_Hide(IHyApplication &hyApp)
//{
//    HyGuiLog("Tried to OnDraw_Hide() a non-derived item: " % GetName(true), LOGTYPE_Error);
//}

///*virtual*/ void ItemWidget::OnDraw_Update(IHyApplication &hyApp)
//{
//    HyGuiLog("Tried to OnDraw_Update() a non-derived item: " % GetName(true), LOGTYPE_Error);
//}

/*virtual*/ void ItemWidget::Link(HyGuiFrame *pFrame, QVariant param)
{
    m_Links.insert(pFrame);
}

/*virtual*/ void ItemWidget::UnLink(HyGuiFrame *pFrame)
{
    m_Links.remove(pFrame);
}
