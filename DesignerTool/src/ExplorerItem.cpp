/**************************************************************************
 *	Item.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ExplorerItem.h"
#include "SpriteWidget.h"
#include "HyGuiGlobal.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "Project.h"
#include "Harmony/Utilities/HyStrManip.h"

ExplorerItem::ExplorerItem(HyGuiItemType eType, const QString sPath) : m_eTYPE(eType),
                                                                                            m_sPATH(MakeStringProperPath(sPath.toStdString().c_str(), HyGlobal::ItemExt(m_eTYPE).toStdString().c_str(), false).c_str())
{
    m_pTreeItemPtr = new QTreeWidgetItem();

    m_pTreeItemPtr->setText(0, GetName(false));
    m_pTreeItemPtr->setIcon(0, GetIcon(SUBICON_None));

    QVariant v; v.setValue(this);
    m_pTreeItemPtr->setData(0, Qt::UserRole, v);
}

ExplorerItem::~ExplorerItem()
{
    delete m_pTreeItemPtr;
}

QString ExplorerItem::GetName(bool bWithPrefix) const
{
    QString sPrefix;
    if(bWithPrefix)
        sPrefix = GetPrefix();
    
    // NOTE: We must remove the extension because dir items use "/", which doesn't work with QFileInfo::baseName()
    QString sPathWithoutExt = m_sPATH;
    sPathWithoutExt.truncate(m_sPATH.size() - HyGlobal::ItemExt(m_eTYPE).size());
    
    QFileInfo itemInfo;
    itemInfo.setFile(sPathWithoutExt);
    QString sName = sPrefix % itemInfo.baseName();
    
    return sName;
}

QString ExplorerItem::GetPrefix() const
{
    // Check to see if this item can have a valid prefix
    if(m_eTYPE == ITEM_Project)
        return QString();

    QList<HyGuiItemType> subDirItemList = HyGlobal::SubDirList();
    for(int i = 0; i < subDirItemList.size(); ++i)
    {
        if(m_eTYPE == subDirItemList[i])
            return QString();
    }
    
    // Prefix is valid for this type
    QString sPrefix;
    
    QStringList sPathSplitList = m_sPATH.split('/', QString::SkipEmptyParts);
    QStringList sSubDirList = HyGlobal::SubDirNameList();

    if(m_eTYPE == ITEM_Prefix)
    {
        for(int i = 1; i < sPathSplitList.size(); ++i)    // The 'i = 1' so we don't include the sub directory
            sPrefix += sPathSplitList[i] % "/";
    }
    else
    {
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
    
    return sPrefix;
}

void ExplorerItem::SetTreeItemSubIcon(SubIcon eSubIcon)
{
    m_pTreeItemPtr->setIcon(0, GetIcon(eSubIcon));
}
