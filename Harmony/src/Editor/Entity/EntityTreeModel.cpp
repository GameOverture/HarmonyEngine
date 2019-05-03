/**************************************************************************
*	EntityTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityTreeModel.h"
#include "EntityModel.h"

EntityTreeModel::EntityTreeModel(EntityModel *pEntityModel, QObject *parent) :
	ITreeModel(QStringList(), parent),
	m_pEntityModel(pEntityModel)
{
}

/*virtual*/ EntityTreeModel::~EntityTreeModel()
{
}

bool EntityTreeModel::AddChildItem(ExplorerItem *pItem)
{
	if(pItem == nullptr || &m_pEntityModel->GetItem() == pItem)
		return false;

	if(pItem->GetType() == ITEM_Entity)
	{
		// TODO: Ensure that this child entity doesn't contain this as child
	}

	if(insertRow(m_pRootItem->childCount(), createIndex(m_pRootItem->childNumber(), 0, m_pRootItem)) == false)
	{
		HyGuiLog("EntityTreeModel::AddChildItem() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	QVariant v;
	v.setValue<ExplorerItem *>(pItem);
	if(setData(index(m_pRootItem->childCount() - 1, 0, createIndex(m_pRootItem->childNumber(), 0, m_pRootItem)), v) == false)
		HyGuiLog("EntityTreeModel::AddChildItem() - setData failed", LOGTYPE_Error);
}

bool EntityTreeModel::RemoveChild(ExplorerItem *pItem)
{
	TreeModelItem *pTreeItem = GetItem(FindIndex<ExplorerItem *>(pItem, 0));
	TreeModelItem *pParentTreeItem = pTreeItem->parent();
	return removeRow(pTreeItem->childNumber(), createIndex(pParentTreeItem->childNumber(), 0, pParentTreeItem));
}

QVariant EntityTreeModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	ExplorerItem *pItem = pTreeItem->data(0).value<ExplorerItem *>();
	switch(iRole)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
		return QVariant(pItem->GetName(false));

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(pItem->IsProjectItem())
		{
			ProjectItem *pProjItem = static_cast<ProjectItem *>(pItem);
			if(pProjItem->IsExistencePendingSave())
				return QVariant(pItem->GetIcon(SUBICON_New));
			else if(pProjItem->IsSaveClean() == false)
				return QVariant(pItem->GetIcon(SUBICON_Dirty));
		}
		return QVariant(pItem->GetIcon(SUBICON_None));

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant(pItem->GetName(true));

	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		return QVariant(pItem->GetName(true));

	default:
		return QVariant();
	}
}

/*virtual*/ Qt::ItemFlags EntityTreeModel::flags(const QModelIndex &indexRef) const /*override*/
{
	return QAbstractItemModel::flags(indexRef);
}
