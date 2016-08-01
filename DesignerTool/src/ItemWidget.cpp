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

ItemWidget::ItemWidget(eItemType eType, const QString sPath, WidgetAtlasManager &AtlasManRef) : Item(eType, sPath),
                                                                                                m_AtlasManRef(AtlasManRef),
                                                                                                m_pWidget(NULL),
                                                                                                m_pEditMenu(NULL),
                                                                                                m_pCamera(NULL)
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

void ItemWidget::DrawLoad(IHyApplication &hyApp)
{
    // A non NULL camera signifies that this has been loaded already
    if(m_pCamera)
        return;

    m_pCamera = hyApp.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);

    OnDraw_Load(hyApp);
}

void ItemWidget::DrawUnload(IHyApplication &hyApp)
{
    // A NULL camera signifies that this has hasn't been loaded
    if(m_pCamera == NULL)
        return;

    hyApp.Window().RemoveCamera(m_pCamera);
    m_pCamera = NULL;

    OnDraw_Unload(hyApp);
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

void ItemWidget::Link(HyGuiFrame *pFrame)
{
    OnLink(pFrame);
    m_Links.insert(pFrame);
}

void ItemWidget::Unlink(HyGuiFrame *pFrame)
{
    OnUnlink(pFrame);
    m_Links.remove(pFrame);
}
