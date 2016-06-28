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
        m_pEditMenu = new QMenu("Edit", MainWindow::GetInstance());
        break;
    }
}

Item::~Item()
{
    MainWindow::CloseItem(this);
    
    delete m_pTreeItemPtr;
}

QString Item::GetName(bool bWithPrefix) const
{
    // Check to see if this item can have a valid prefix, otherwise force 'bWithPrefix' to false
    if(bWithPrefix)
    {
        if(m_eTYPE == ITEM_Project)
            bWithPrefix = false;
        
        QList<eItemType> subDirItemList = HyGlobal::SubDirList();
        for(int i = 0; i < subDirItemList.size() && bWithPrefix; ++i)
        {
            if(m_eTYPE == subDirItemList[i])
                bWithPrefix = false;
        }
    }
    
    QString sPrefix; sPrefix.clear();
    if(bWithPrefix)
    {
        QStringList sPathSplitList = m_sPATH.split('/', QString::SkipEmptyParts);
        QStringList sSubDirList = HyGlobal::SubDirNameList();
        
        int iSplitIndex = sPathSplitList.size() - 1;
        bool bSubDirFound = false;
        for(; iSplitIndex >= 0; --iSplitIndex)
        {
            for(int i = 0; i < sSubDirList.size(); ++i)
            {
                if(sSubDirList[i].compare(sPathSplitList[iSplitIndex], Qt::CaseInsensitive) == 0)
                {
                    bSubDirFound = true;
                    break;
                }
            }
            
            if(bSubDirFound)
                break;
        }
        
        if(bSubDirFound)
        {
            for(int i = iSplitIndex + 1; i < sPathSplitList.size() - 1; ++i)    // The '+ 1' so we don't include the sub directory, and the '- 1' is so we don't include the name
                sPrefix += sPathSplitList[i] % "/";
        }
    }
    
    // NOTE: We must remove the extension because dir items use "/", which doesn't work with QFileInfo::baseName()
    QString sPathWithoutExt = m_sPATH;
    sPathWithoutExt.truncate(m_sPATH.size() - HyGlobal::ItemExt(m_eTYPE).size());
    
    QFileInfo itemInfo;
    itemInfo.setFile(sPathWithoutExt);
    QString sName = sPrefix % itemInfo.baseName();
    
    return sName;
}

QString Item::GetRelPath() const
{
    QString sRelPath;

    QStringList sPathSplitList = m_sPATH.split('/', QString::SkipEmptyParts);
    QStringList sSubDirList = HyGlobal::SubDirNameList();

    int iSplitIndex = sPathSplitList.size() - 1;
    bool bSubDirFound = false;
    for(; iSplitIndex >= 0; --iSplitIndex)
    {
        for(int i = 0; i < sSubDirList.size(); ++i)
        {
            if(sSubDirList[i].compare(sPathSplitList[iSplitIndex], Qt::CaseInsensitive) == 0)
            {
                bSubDirFound = true;
                break;
            }
        }

        if(bSubDirFound)
            break;
    }

    if(bSubDirFound)
    {
        for(int i = iSplitIndex; i < sPathSplitList.size(); ++i)
            sRelPath += sPathSplitList[i] % "/";
    }

    return sRelPath;
}

/*virtual*/ void Item::OnDraw_Open(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Open() a non-derived item: " % GetName(true), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Close(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Close() a non-derived item: " % GetName(true), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Show(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Show() a non-derived item: " % GetName(true), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Hide(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Hide() a non-derived item: " % GetName(true), LOGTYPE_Error);
}

/*virtual*/ void Item::OnDraw_Update(IHyApplication &hyApp)
{
    HyGuiLog("Tried to OnDraw_Update() a non-derived item: " % GetName(true), LOGTYPE_Error);
}

/*virtual*/ void Item::Save()
{
    HyGuiLog("Tried to save a non-derived item: " % GetName(true), LOGTYPE_Error);
}

/*virtual*/ void Item::Link(HyGuiFrame *pFrame, QVariant param)
{
    m_Links.insert(pFrame);
}

/*virtual*/ void Item::UnLink(HyGuiFrame *pFrame)
{
    m_Links.remove(pFrame);
}
