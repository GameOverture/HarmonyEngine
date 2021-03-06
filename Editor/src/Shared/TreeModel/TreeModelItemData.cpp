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
	m_bIsProjectItem(false)
{
	// I think a default ctor is required for QObject to work?
	HyGuiLog("Invalid TreeModelItemData ctor used", LOGTYPE_Error);
}

TreeModelItemData::TreeModelItemData(HyGuiItemType eType, const QString sText) :
	m_eTYPE(eType),
	m_sName(sText),
	m_bIsProjectItem(false)
{ }

/*virtual*/ TreeModelItemData::~TreeModelItemData()
{
}

HyGuiItemType TreeModelItemData::GetType() const
{
	return m_eTYPE;
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
