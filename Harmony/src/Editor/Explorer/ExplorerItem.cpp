/**************************************************************************
 *	ExplorerItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ExplorerItem.h"
#include "SpriteWidget.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "Harmony/Utilities/HyStrManip.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

ExplorerItem::ExplorerItem() :
	m_eTYPE(ITEM_Unknown),
	m_pProject(nullptr),
	m_bIsProjectItem(false)
{
}

ExplorerItem::ExplorerItem(Project &projectRef, HyGuiItemType eType, const QString sPath) :
	m_eTYPE(eType),
	m_sPath(HyStr::MakeStringProperPath(sPath.toStdString().c_str(), HyGlobal::ItemExt(m_eTYPE).toStdString().c_str(), false).c_str()),
	m_pProject(&projectRef),
	m_bIsProjectItem(false)
{
	// GetPrefix() doesn't work until we attach a parent. Manually grab the prefix from 'm_sPath'
	QStringList sPathParts = m_sPath.split('/');
	QString sPrefix;
	if(sPathParts.size() == 1)
		sPrefix = sPathParts[0];
	else
	{
		for(int32 i = 0; i < sPathParts.size() - 1; ++i)
			sPrefix += sPathParts[i];
	}
	sPrefix += '/';
}

ExplorerItem::~ExplorerItem()
{
}

HyGuiItemType ExplorerItem::GetType() const
{
	return m_eTYPE;
}

Project &ExplorerItem::GetProject() const
{
	return *m_pProject;
}

bool ExplorerItem::IsProjectItem() const
{
	return m_bIsProjectItem;
}

QString ExplorerItem::GetName(bool bWithPrefix) const
{
	QString sPrefix;
	if(bWithPrefix)
		sPrefix = GetPrefix();
	
	// NOTE: We must remove the extension because dir items use "/", which doesn't work with QFileInfo::baseName()
	QString sPathWithoutExt = m_sPath;
	sPathWithoutExt.truncate(m_sPath.size() - HyGlobal::ItemExt(m_eTYPE).size());
	QFileInfo itemInfo;
	itemInfo.setFile(sPathWithoutExt);
	QString sName = sPrefix % itemInfo.baseName();
	
	return sName;
}

// Ends with a '/'
QString ExplorerItem::GetPrefix() const
{
	return m_pProject->GetExplorerModel().AssemblePrefix(const_cast<ExplorerItem *>(this));
}

QIcon ExplorerItem::GetIcon(SubIcon eSubIcon) const
{
	return HyGlobal::ItemIcon(m_eTYPE, eSubIcon);
}

/*virtual*/ void ExplorerItem::Rename(QString sNewName)
{
	if(m_eTYPE != ITEM_Prefix)
	{
		HyGuiLog("ExplorerItem::Rename on improper item type", LOGTYPE_Error);
		return;
	}

	QString sOldPath = GetName(true);
	m_sPath = sNewName;
	QString sNewPath = GetName(true);

	if(m_pProject)
		m_pProject->RenamePrefix(sOldPath, sNewPath);
}

/*virtual*/ void ExplorerItem::DeleteFromProject()
{
	if(m_pProject)
	{
		if(m_eTYPE == ITEM_Prefix)
			m_pProject->DeletePrefixAndContents(GetName(true));
		else if(IsProjectItem())
			m_pProject->DeleteGameData(m_eTYPE, GetName(true));
	}
}

/*virtual*/ QString ExplorerItem::GetToolTip() const /*override*/
{
	return GetName(true);
}

QDataStream &operator<<(QDataStream &out, ExplorerItem *const &rhs)
{
	out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
	return out;
}

QDataStream &operator>>(QDataStream &in, ExplorerItem *rhs)
{
	in.readRawData(reinterpret_cast<char *>(rhs), sizeof(rhs));
	return in;
}
