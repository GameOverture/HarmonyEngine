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
#include "WidgetSprite.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "ItemProject.h"
#include "Harmony/Utilities/HyStrManip.h"

Item::Item(eItemType eType, const QString sPath) :  m_eTYPE(eType),
                                                    m_sPATH(MakeStringProperPath(sPath.toStdString().c_str(), HyGlobal::ItemExt(m_eTYPE).toStdString().c_str(), false).c_str())
{
    m_pTreeItemPtr = new QTreeWidgetItem();

    m_pTreeItemPtr->setText(0, GetName(false));
    m_pTreeItemPtr->setIcon(0, GetIcon());

    QVariant v; v.setValue(this);
    m_pTreeItemPtr->setData(0, Qt::UserRole, v);
}

Item::~Item()
{
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
        else
        {
            QFileInfo itemInfo;
            itemInfo.setFile(m_sPATH);
            sPrefix = itemInfo.path() % "/";
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

//QString Item::GetRelPath() const
//{
//    QString sRelPath;

//    QString sFullPath = m_sPATH;
//    if(m_eTYPE == ITEM_Sprite || m_eTYPE == ITEM_Font)
//        sFullPath.remove(HyGlobal::ItemExt(m_eTYPE));

//    QStringList sPathSplitList = sFullPath.split('/', QString::SkipEmptyParts);
//    QStringList sSubDirList = HyGlobal::SubDirNameList();

//    int iSplitIndex = sPathSplitList.size() - 1;
//    bool bSubDirFound = false;
//    for(; iSplitIndex >= 0; --iSplitIndex)
//    {
//        for(int i = 0; i < sSubDirList.size(); ++i)
//        {
//            if(sSubDirList[i].compare(sPathSplitList[iSplitIndex], Qt::CaseInsensitive) == 0)
//            {
//                bSubDirFound = true;
//                break;
//            }
//        }

//        if(bSubDirFound)
//            break;
//    }

//    if(bSubDirFound)
//    {
//        for(int i = iSplitIndex; i < sPathSplitList.size(); ++i)
//        {
//            if(i != iSplitIndex)
//                sRelPath += "/";

//            sRelPath += sPathSplitList[i];
//        }
//    }

//    return sRelPath;
//}

