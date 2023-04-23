/**************************************************************************
 *	ExplorerItemData.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ExplorerItemData.h"
#include "SpriteWidget.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "MainWindow.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

ExplorerItemData::ExplorerItemData(Project &projectRef, ItemType eType, const QUuid &uuid, const QString sName) :
	TreeModelItemData(eType, uuid, sName),
	m_pProject(&projectRef)
{ }

ExplorerItemData::~ExplorerItemData()
{
}

Project &ExplorerItemData::GetProject() const
{
	return *m_pProject;
}

/*virtual*/ QString ExplorerItemData::GetName(bool bWithPrefix) const
{
	return bWithPrefix ? GetPrefix() % m_sName : m_sName;
}

QString ExplorerItemData::GetPrefix() const
{
	return MainWindow::GetExplorerModel().AssemblePrefix(const_cast<ExplorerItemData *>(this));
}

void ExplorerItemData::Rename(QString sNewName)
{
	Rename(GetPrefix(), sNewName);
}

void ExplorerItemData::Rename(QString sNewPrefix, QString sNewName)
{
	if(m_pProject == nullptr)
	{
		HyGuiLog("ExplorerItemData::Rename invoked with nullptr Project", LOGTYPE_Error);
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

/*virtual*/ void ExplorerItemData::DeleteFromProject()
{
	RelinquishDependees();

	if(m_pProject == nullptr)
		return;

	if(m_eTYPE == ITEM_Prefix)
		m_pProject->DeletePrefixAndContents(GetName(true), true);
	else if(IsProjectItem())
		m_pProject->DeleteItemData(m_eTYPE, GetName(true), true);
}

/*virtual*/ QString ExplorerItemData::GetToolTip() const /*override*/
{
	return GetName(true);
}
