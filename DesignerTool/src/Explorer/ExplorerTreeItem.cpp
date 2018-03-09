/**************************************************************************
 *	ExplorerTreeItem.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ExplorerTreeItem.h"
#include "SpriteWidget.h"
#include "Global.h"
#include "Project.h"
#include "Harmony/Utilities/HyStrManip.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

ExplorerTreeItem::ExplorerTreeItem(HyGuiItemType eType, const QString sPath, QTreeWidgetItem *pParentTreeItem) :	m_eTYPE(eType),
																													m_sPath(HyStr::MakeStringProperPath(sPath.toStdString().c_str(), HyGlobal::ItemExt(m_eTYPE).toStdString().c_str(), false).c_str()),
																													m_bIsProjectItem(false)
{
	m_pTreeItemPtr = new QTreeWidgetItem();
	m_pTreeItemPtr->setText(0, GetName(false));
	m_pTreeItemPtr->setIcon(0, GetIcon(SUBICON_None));

	QVariant v; v.setValue(this);
	m_pTreeItemPtr->setData(0, Qt::UserRole, v);

	if(pParentTreeItem)
		pParentTreeItem->addChild(m_pTreeItemPtr);
}

ExplorerTreeItem::~ExplorerTreeItem()
{
	delete m_pTreeItemPtr;
}

HyGuiItemType ExplorerTreeItem::GetType() const
{
	return m_eTYPE;
}

QTreeWidgetItem *ExplorerTreeItem::GetTreeItem() const
{
	return m_pTreeItemPtr;
}

bool ExplorerTreeItem::IsProjectItem() const
{
	return m_bIsProjectItem;
}

QString ExplorerTreeItem::GetName(bool bWithPrefix) const
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
QString ExplorerTreeItem::GetPrefix() const
{
	QStringList sPrefixParts;
	QTreeWidgetItem *pParentTreeItem = m_pTreeItemPtr->parent();
	while(pParentTreeItem)
	{
		ExplorerTreeItem *pParentItem = pParentTreeItem->data(0, Qt::UserRole).value<ExplorerTreeItem *>();
		if(pParentItem->GetType() == ITEM_Prefix)
			sPrefixParts.prepend(pParentTreeItem->text(0));

		pParentTreeItem = pParentTreeItem->parent();
	}

	QString sPrefix;
	for(int i = 0; i < sPrefixParts.size(); ++i)
	{
		sPrefix += sPrefixParts[i];
		sPrefix += "/";
	}
	return sPrefix;
}

QIcon ExplorerTreeItem::GetIcon(SubIcon eSubIcon) const
{
	return HyGlobal::ItemIcon(m_eTYPE, eSubIcon);
}

/*virtual*/ void ExplorerTreeItem::Rename(QString sNewName)
{
	if(m_eTYPE != ITEM_Prefix)
	{
		HyGuiLog("ExplorerTreeItem::Rename on improper item type", LOGTYPE_Error);
		return;
	}

	QString sOldPath = GetName(true);
	m_sPath = sNewName;
	m_pTreeItemPtr->setText(0, GetName(false));
	QString sNewPath = GetName(true);

	HyGlobal::GetProjectFromItem(m_pTreeItemPtr)->RenamePrefix(sOldPath, sNewPath);
}

void ExplorerTreeItem::SetTreeItemSubIcon(SubIcon eSubIcon)
{
	m_pTreeItemPtr->setIcon(0, GetIcon(eSubIcon));
}

QDataStream &operator<<(QDataStream &out, ExplorerTreeItem *const &rhs)
{
	out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
	return out;
}

QDataStream &operator>>(QDataStream &in, ExplorerTreeItem *rhs)
{
	in.readRawData(reinterpret_cast<char *>(rhs), sizeof(rhs));
	return in;
}

