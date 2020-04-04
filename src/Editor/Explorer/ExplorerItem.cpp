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
	// I think a default ctor is required for QObject to work?
	HyGuiLog("Invalid ExplorerItem ctor used", LOGTYPE_Error);
}

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

/*virtual*/ QString ExplorerItem::GetName(bool bWithPrefix) const
{
	return bWithPrefix ? GetPrefix() % m_sName : m_sName;
}

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
	Rename(GetPrefix(), sNewName);
}

void ExplorerItem::Rename(QString sNewPrefix, QString sNewName)
{
	if(m_pProject == nullptr)
	{
		HyGuiLog("ExplorerItem::Rename invoked with nullptr Project", LOGTYPE_Error);
		return;
	}

	if(sNewPrefix.isEmpty() == false && sNewPrefix[sNewPrefix.length() - 1] != '/')
		sNewPrefix += '/';

	QString sOldPath = GetName(true);
	QString sNewPath = sNewPrefix % sNewName;

	if(m_eTYPE == ITEM_Prefix)
		m_sName = m_pProject->RenamePrefix(sOldPath, sNewPath);
	else
		m_sName = m_pProject->RenameItem(m_eTYPE, sOldPath, sNewPath);
}

/*virtual*/ void ExplorerItem::DeleteFromProject()
{
	if(m_pProject == nullptr)
		return;

	if(m_eTYPE == ITEM_Prefix)
		m_pProject->DeletePrefixAndContents(GetName(true), true);
	else if(IsProjectItem())
		m_pProject->DeleteItemData(m_eTYPE, GetName(true), true);
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
