/**************************************************************************
*	TreeModelItemData.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TreeModelItemData.h"
#include "Project.h"
#include "ExplorerModel.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

TreeModelItemData::TreeModelItemData() :
	m_eTYPE(ITEM_Unknown),
	m_UUID(), // Invalid zeroed out UUID
	m_bIsProjectItem(false),
	m_bIsAssetItem(false)
{
	// I think a default ctor is required for QObject to work?
	HyGuiLog("Invalid TreeModelItemData ctor used", LOGTYPE_Error);
}

TreeModelItemData::TreeModelItemData(ItemType eType, const QUuid &uuid, const QString sText) :
	m_eTYPE(eType),
	m_UUID(uuid),
	m_sName(sText),
	m_bIsProjectItem(false),
	m_bIsAssetItem(false)
{ }

/*virtual*/ TreeModelItemData::~TreeModelItemData()
{
}

ItemType TreeModelItemData::GetType() const
{
	return m_eTYPE;
}

const QUuid &TreeModelItemData::GetUuid() const
{
	return m_UUID;
}

QString TreeModelItemData::GetText() const
{
	return m_sName;
}

void TreeModelItemData::SetText(QString sText)
{
	m_sName = sText;
}

QIcon TreeModelItemData::GetIcon(SubIcon eSubIcon) const
{
	return HyGlobal::ItemIcon(m_eTYPE, eSubIcon);
}

bool TreeModelItemData::IsProjectItem() const
{
	return m_bIsProjectItem;
}

bool TreeModelItemData::IsAssetItem() const
{
	return m_bIsAssetItem;
}

QList<TreeModelItemData *> TreeModelItemData::GetDependants() const
{
	return m_DependantMap.keys();
}

QList<TreeModelItemData *> TreeModelItemData::GetDependees() const
{
	return m_DependeeMap.keys();
}

void TreeModelItemData::AddDependantRef(TreeModelItemData *pDependant)
{
	if(pDependant == nullptr)
		return;
	pDependant->AddDependeeRef(this);

	if(m_DependantMap.contains(pDependant))
		m_DependantMap[pDependant]++;
	else
		m_DependantMap.insert(pDependant, 1);
}

void TreeModelItemData::SubtractDependantRef(TreeModelItemData *pDependant)
{
	if(pDependant == nullptr)
		return;
	pDependant->SubtractDependeeRef(this);

	if(m_DependantMap.contains(pDependant) == false)
	{
		HyGuiLog("TreeModelItemData::SubtractDependantRef invoked and not found in m_DependantMap", LOGTYPE_Error);
		return;
	}

	m_DependantMap[pDependant]--;
	if(m_DependantMap[pDependant] == 0)
		m_DependantMap.remove(pDependant);
}

void TreeModelItemData::RelinquishDependees()
{
	while(m_DependeeMap.empty() == false)
	{
		TreeModelItemData *pKey = m_DependeeMap.lastKey();
		int iValue = m_DependeeMap.last();
		
		for(int i = 0; i < iValue; ++i)
			pKey->SubtractDependantRef(this);
	}

	if(m_DependantMap.empty() == false)
		HyGuiLog("TreeModelItemData::RelinquishDependees did not deplete", LOGTYPE_Error);
}

void TreeModelItemData::AddDependeeRef(TreeModelItemData *pDependee)
{
	if(m_DependeeMap.contains(pDependee))
		m_DependeeMap[pDependee]++;
	else
		m_DependeeMap.insert(pDependee, 1);
}

void TreeModelItemData::SubtractDependeeRef(TreeModelItemData *pDependee)
{
	if(m_DependeeMap.contains(pDependee) == false)
	{
		HyGuiLog("TreeModelItemData::SubtractDependeeRef invoked and not found in m_DependeeMap", LOGTYPE_Error);
		return;
	}

	m_DependeeMap[pDependee]--;
	if(m_DependeeMap[pDependee] == 0)
		m_DependeeMap.remove(pDependee);
}

QDataStream &operator<<(QDataStream &out, TreeModelItemData *const &rhs)
{
	out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
	return out;
}

QDataStream &operator>>(QDataStream &in, TreeModelItemData *rhs)
{
	in.readRawData(reinterpret_cast<char *>(rhs), sizeof(rhs));
	return in;
}
