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
{ }

ExplorerItem::ExplorerItem(Project &projectRef, HyGuiItemType eType, const QString sName) :
	m_eTYPE(eType),
	m_sName(sName),
	m_pProject(&projectRef),
	m_bIsProjectItem(false)
{ }

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
	//QString sPathWithoutExt = m_sName;
	//sPathWithoutExt.truncate(m_sName.size() - HyGlobal::ItemExt(m_eTYPE).size());
	QFileInfo itemInfo;
	itemInfo.setFile(m_sName);
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

void ExplorerItem::Rename(QString sNewName)
{
	if(m_pProject == nullptr)
	{
		HyGuiLog("ExplorerItem::Rename invoked with nullptr Project", LOGTYPE_Error);
		return;
	}

	QString sOldPath = GetName(true);
	m_sName = sNewName;
	QString sNewPath = GetName(true);

	if(m_eTYPE != ITEM_Prefix)
		m_pProject->RenamePrefix(sOldPath, sNewPath);
	else
		m_pProject->RenameItem(m_eTYPE, sOldPath, sNewPath);
}

void ExplorerItem::Rename(QString sNewPrefix, QString sNewName)
{
	if(m_pProject == nullptr)
	{
		HyGuiLog("ExplorerItem::Rename invoked with nullptr Project", LOGTYPE_Error);
		return;
	}

	QString sOldPath = GetName(true);
	m_sName = sNewName;
	QString sNewPath = sNewPrefix % "/" % GetName(true);

	if(m_eTYPE != ITEM_Prefix)
		m_pProject->RenamePrefix(sOldPath, sNewPath);
	else
		m_pProject->RenameItem(m_eTYPE, sOldPath, sNewPath);
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
