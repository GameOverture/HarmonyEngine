/**************************************************************************
 *	Item.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Item.h"

#include "MainWindow.h"
#include "WidgetRenderer.h"

#include "WidgetSprite.h"

#include <QFileInfo>

#include "Harmony/Utilities/HyStrManip.h"

Item::Item(eItemType eType, const QString sPath) :  m_eTYPE(eType),
                                                    m_sPATH(MakeStringProperPath(sPath.toStdString().c_str(), HyGlobal::ItemExt(m_eTYPE).toStdString().c_str()).c_str()),
                                                    m_pTreeItemPtr(NULL),
                                                    m_pWidget(NULL),
                                                    m_pEditMenu(NULL)
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
        m_pEditMenu = new QMenu("Edit");
        break;
    }
}

Item::~Item()
{
    MainWindow::CloseItem(this);
    
    delete m_pTreeItemPtr;
}

QString Item::GetName() const
{
    // NOTE: We must remove the extension because dir items use "/", which doesn't work with QFileInfo::baseName()
    QString sPathWithoutExt = m_sPATH;
    sPathWithoutExt.truncate(m_sPATH.size() - HyGlobal::ItemExt(m_eTYPE).size());
    
    QFileInfo itemInfo;
    itemInfo.setFile(sPathWithoutExt);
    
    return itemInfo.baseName();
}

/*virtual*/ void Item::OnDraw_Open(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Open() a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Close(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Close() a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Show(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Show() a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Hide(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Hide() a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Update(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Update() a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::Save()
{
    HyGuiLog("Tried to save a non-derived item: " % GetName(), LOGTYPE_Error);
}
