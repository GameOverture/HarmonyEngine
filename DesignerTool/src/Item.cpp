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

Item::Item(eItemType eType, const QString sPath) :  m_eType(ITEM_Unknown),
                                                    m_pTreeItemPtr(NULL),
                                                    m_pWidget(NULL)
{
    Initialize(eType, sPath);
}

Item::Item(const Item &other)
{
    Initialize(other.GetType(), other.GetPath());
}

Item::~Item()
{
    MainWindow::CloseItem(this);
    
    delete m_pTreeItemPtr;
}

QString Item::GetName() const
{
    // NOTE: We must remove the extension because dir items use "/", which doesn't work with QFileInfo::baseName()
    QString sPathWithoutExt = m_sPath;
    sPathWithoutExt.truncate(m_sPath.size() - HyGlobal::ItemExt(m_eType).size());
    
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

void Item::Initialize(eItemType eType, const QString sPath)
{
    m_eType = eType;

    if(m_eType == ITEM_Unknown)
    {
        HyGuiLog("Setting path of unknown item (" % m_sPath % ")", LOGTYPE_Error);
        return;
    }

    m_sPath = sPath;
    m_sPath.replace(QChar('\\'), QChar('/'));

    QString sExt = HyGlobal::ItemExt(m_eType);
    if(m_sPath.right(sExt.size()) != sExt)
        m_sPath.append(sExt);
    
    switch(m_eType)
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
        
    case ITEM_Sprite:
        m_pWidget = new WidgetSprite();
        break;
        
    default:
        HyGuiLog("Item::Initialize() - Widget not specified", LOGTYPE_Error);
        break;
    }
}

