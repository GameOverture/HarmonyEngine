/**************************************************************************
 *	ExplorerItem.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ExplorerItem.h"
#include "SpriteWidget.h"
#include "Global.h"
#include "Project.h"
#include "ExplorerTreeWidget.h"
#include "Harmony/Utilities/HyStrManip.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExplorerTreeItem::ExplorerTreeItem(int type /*= Type*/) : QTreeWidgetItem(type)
{ }
ExplorerTreeItem::ExplorerTreeItem(ExplorerTreeWidget *pView, int type /*= Type*/) : QTreeWidgetItem(pView, type)
{ }
ExplorerTreeItem::ExplorerTreeItem(QTreeWidgetItem *parent, int type /*= Type*/) : QTreeWidgetItem(parent, type)
{ }

bool ExplorerTreeItem::operator<(const QTreeWidgetItem &rhs) const
{
	ExplorerItem *pLeftItem = this->data(0, Qt::UserRole).value<ExplorerItem *>();
	ExplorerItem *pRightItem = rhs.data(0, Qt::UserRole).value<ExplorerItem *>();

	if(pLeftItem->GetType() == ITEM_Prefix && pRightItem->GetType() != ITEM_Prefix)
		return true;
	if(pLeftItem->GetType() != ITEM_Prefix && pRightItem->GetType() == ITEM_Prefix)
		return false;

	return this->text(0) < rhs.text(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ExplorerItem::ExplorerItem(HyGuiItemType eType, const QString sPath, QTreeWidgetItem *pParentTreeItem) :	m_eTYPE(eType),
																													m_sPath(HyStr::MakeStringProperPath(sPath.toStdString().c_str(), HyGlobal::ItemExt(m_eTYPE).toStdString().c_str(), false).c_str()),
																													m_bIsProjectItem(false)
{
	m_pTreeItemPtr = new ExplorerTreeItem();
	m_pTreeItemPtr->setText(0, GetName(false));
	m_pTreeItemPtr->setIcon(0, GetIcon(SUBICON_None));

	QVariant v; v.setValue(this);
	m_pTreeItemPtr->setData(0, Qt::UserRole, v);

	if(pParentTreeItem)
		pParentTreeItem->addChild(m_pTreeItemPtr);
}

ExplorerItem::~ExplorerItem()
{
	delete m_pTreeItemPtr;
}

HyGuiItemType ExplorerItem::GetType() const
{
	return m_eTYPE;
}

QTreeWidgetItem *ExplorerItem::GetTreeItem() const
{
	return m_pTreeItemPtr;
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
	QStringList sPrefixParts;
	QTreeWidgetItem *pParentTreeItem = m_pTreeItemPtr->parent();
	while(pParentTreeItem)
	{
		ExplorerItem *pParentItem = pParentTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>();
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
	m_pTreeItemPtr->setText(0, GetName(false));
	QString sNewPath = GetName(true);

	HyGlobal::GetProjectFromItem(m_pTreeItemPtr)->RenamePrefix(sOldPath, sNewPath);
}

void ExplorerItem::SetTreeItemSubIcon(SubIcon eSubIcon)
{
	m_pTreeItemPtr->setIcon(0, GetIcon(eSubIcon));
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
