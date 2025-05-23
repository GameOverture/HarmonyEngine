/**************************************************************************
*	SpineSkinTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "SpineSkinTreeModel.h"
#include "SpineModel.h"
#include "SpineUndoCmds.h"

SpineSkinTreeItem::SpineSkinTreeItem(const QString &data, SpineSkinTreeItem *parent /*= nullptr*/) :
	m_data(data),
	m_parent(parent),
	m_checkState(Qt::Unchecked)
{
}

SpineSkinTreeItem::~SpineSkinTreeItem()
{
	qDeleteAll(m_children);
}

SpineSkinTreeItem *SpineSkinTreeItem::appendChild(const QString &data)
{
	for(SpineSkinTreeItem *child : m_children)
	{
		if(child->m_data == data)
			return child;
	}

	SpineSkinTreeItem *newChild = new SpineSkinTreeItem(data, this);
	m_children.append(newChild);

	return newChild;
}

SpineSkinTreeItem *SpineSkinTreeItem::child(int row) const
{
	return m_children.value(row);
}

int SpineSkinTreeItem::childCount() const
{
	return m_children.size();
}

int SpineSkinTreeItem::row() const
{ 
	return m_parent ? m_parent->m_children.indexOf(const_cast<SpineSkinTreeItem *>(this)) : 0;
}

SpineSkinTreeItem *SpineSkinTreeItem::parent() const
{
	return m_parent;
}

QString SpineSkinTreeItem::data() const
{
	return m_data;
}

Qt::CheckState SpineSkinTreeItem::checkState() const
{
	return m_checkState;
}

void SpineSkinTreeItem::setCheckState(Qt::CheckState state)
{
	m_checkState = state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpineSkinTreeModel::SpineSkinTreeModel(SpineStateData &stateDataRef, QStringList sSkinPathsList, QObject *parent /*= nullptr*/) :
	QAbstractItemModel(parent),
	m_StateDataRef(stateDataRef),
	m_pRoot(new SpineSkinTreeItem("root"))
{
	for(const auto &str : sSkinPathsList)
	{
		QStringList parts = str.split("/");
		SpineSkinTreeItem *current = m_pRoot;
		for(const QString &part : parts)
			current = current->appendChild(part);
	}
}

/*virtual*/ SpineSkinTreeModel::~SpineSkinTreeModel()
{
	delete m_pRoot;
}

/*virtual*/ QModelIndex SpineSkinTreeModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	if(!hasIndex(row, column, parent))
		return QModelIndex();
	SpineSkinTreeItem *parentItem = parent.isValid() ? static_cast<SpineSkinTreeItem *>(parent.internalPointer()) : m_pRoot;
	SpineSkinTreeItem *childItem = parentItem->child(row);

	return childItem ? createIndex(row, column, childItem) : QModelIndex();
}

/*virutal*/ QModelIndex SpineSkinTreeModel::parent(const QModelIndex &index) const /*override*/
{
	if(!index.isValid())
		return QModelIndex();
	SpineSkinTreeItem *childItem = static_cast<SpineSkinTreeItem *>(index.internalPointer());
	SpineSkinTreeItem *parentItem = childItem->parent();
	if(parentItem == m_pRoot || parentItem == nullptr)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

QVariant SpineSkinTreeModel::headerData(int iSection, Qt::Orientation orientation, int eRole /*= Qt::DisplayRole*/) const /*override*/
{
	if(orientation == Qt::Horizontal && eRole == Qt::DisplayRole)
	{
		if(iSection == 0)
			return QString("Set Enabled Skins");
	}
	return QVariant();
}

/*virtual*/ int SpineSkinTreeModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	SpineSkinTreeItem *parentItem = parent.isValid() ? static_cast<SpineSkinTreeItem *>(parent.internalPointer()) : m_pRoot;
	return parentItem->childCount();
}

/*virtual*/ int SpineSkinTreeModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return 1;
}

/*virtual*/ QVariant SpineSkinTreeModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const /*override*/
{
	if(!index.isValid())
		return QVariant();

	SpineSkinTreeItem *pItem = static_cast<SpineSkinTreeItem *>(index.internalPointer());

	if(role == Qt::DisplayRole)
		return pItem->data();
	else if(role == Qt::DecorationRole)
	{
		if(pItem->childCount() > 0)
			return QIcon(":/icons16x16/items/Prefix.png");
		else
			return QIcon(":/icons16x16/spine-skin.png");
	}
	else if(role == Qt::CheckStateRole)
		return pItem->checkState();

	return QVariant();
}

/*virtual*/ bool SpineSkinTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) /*override*/
{
	if(!index.isValid())
		return false;

	SpineSkinTreeItem *pItem = static_cast<SpineSkinTreeItem *>(index.internalPointer());
	if(role == Qt::CheckStateRole)
	{
		Qt::CheckState eState = static_cast<Qt::CheckState>(value.toInt());
		pItem->setCheckState(eState);

		// Recursively propagate to all items
		UpdateChildrenCheckState(pItem, eState);
		UpdateParentCheckState(pItem->parent());

		// Preserve the old skin list, then update enabled skins
		QStringList sPrevSkinList = m_sEnabledSkinList;
		m_sEnabledSkinList.clear();
		QStack<SpineSkinTreeItem *> treeItemStack;
		treeItemStack.push(m_pRoot);
		while(!treeItemStack.isEmpty())
		{
			SpineSkinTreeItem *pItem = treeItemStack.pop();
			if(pItem->childCount() == 0 && pItem->checkState() == Qt::Checked)
			{
				// This is a leaf item, so add it to the list
				// Get the full path of the skin
				QString sSkinPath = pItem->data();
				SpineSkinTreeItem *pParent = pItem->parent();
				while(pParent && pParent != m_pRoot)
				{
					sSkinPath = pParent->data() + "/" + sSkinPath;
					pParent = pParent->parent();
				}
				m_sEnabledSkinList.push_back(sSkinPath);
			}

			for(int i = 0; i < pItem->childCount(); ++i)
				treeItemStack.push(pItem->child(i));
		}

		SpineUndoCmd_SkinsChanged *pCmd = new SpineUndoCmd_SkinsChanged(m_StateDataRef.GetModel().GetItem(), m_StateDataRef.GetIndex(), m_sEnabledSkinList, sPrevSkinList);
		m_StateDataRef.GetModel().GetItem().GetUndoStack()->push(pCmd);

		Q_EMIT dataChanged(index, index, { Qt::CheckStateRole });
		//Q_EMIT CheckStatesUpdated(); // This sends out the Undo/Redo command
		return true;
	}

	return false;
}

/*virtual*/ Qt::ItemFlags SpineSkinTreeModel::flags(const QModelIndex &index) const /*override*/
{
	if(!index.isValid())
		return Qt::NoItemFlags;

	SpineSkinTreeItem *pItem = static_cast<SpineSkinTreeItem *>(index.internalPointer());
	if(pItem->childCount() > 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsTristate;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

QStringList SpineSkinTreeModel::GetEnabledSkinList() const
{
	return m_sEnabledSkinList;
}

void SpineSkinTreeModel::Cmd_SetEnabledSkins(const QStringList &sSkinPathsList)
{
	m_sEnabledSkinList = sSkinPathsList;

	QStack<SpineSkinTreeItem *> treeItemStack;
	treeItemStack.push(m_pRoot);
	while(!treeItemStack.isEmpty())
	{
		SpineSkinTreeItem *pItem = treeItemStack.pop();
		if(pItem->childCount() == 0)
		{
			// This is a leaf item, check if it's in the list
			QString sSkinPath = pItem->data();
			SpineSkinTreeItem *pParent = pItem->parent();
			while(pParent && pParent != m_pRoot)
			{
				sSkinPath = pParent->data() + "/" + sSkinPath;
				pParent = pParent->parent();
			}

			if(m_sEnabledSkinList.contains(sSkinPath))
				pItem->setCheckState(Qt::Checked);
			else
				pItem->setCheckState(Qt::Unchecked);

			UpdateParentCheckState(pItem->parent());

			QModelIndex index = createIndex(pItem->row(), 0, pItem);
			Q_EMIT dataChanged(index, index, { Qt::CheckStateRole });
		}

		for(int i = 0; i < pItem->childCount(); ++i)
			treeItemStack.push(pItem->child(i));
	}
}

void SpineSkinTreeModel::UpdateChildrenCheckState(SpineSkinTreeItem *pItem, Qt::CheckState eState)
{
	for(int i = 0; i < pItem->childCount(); ++i)
	{
		SpineSkinTreeItem *pChild = pItem->child(i);
		pChild->setCheckState(eState);
		QModelIndex childIndex = createIndex(i, 0, pChild);
		Q_EMIT dataChanged(childIndex, childIndex, { Qt::CheckStateRole });
		
		UpdateChildrenCheckState(pChild, eState);
	}
}

void SpineSkinTreeModel::UpdateParentCheckState(SpineSkinTreeItem *pItem)
{
	if(pItem == nullptr)
		return;

	int iCheckedCount = 0;
	int iUncheckedCount = 0;

	for(int i = 0; i < pItem->childCount(); ++i)
	{
		Qt::CheckState cs = pItem->child(i)->checkState();
		if(cs == Qt::Checked)
			iCheckedCount++;
		else if(cs == Qt::Unchecked)
			iUncheckedCount++;
	}

	Qt::CheckState eNewState;
	if(iCheckedCount == pItem->childCount())
		eNewState = Qt::Checked;
	else if(iUncheckedCount == pItem->childCount())
		eNewState = Qt::Unchecked;
	else
		eNewState = Qt::PartiallyChecked;

	if(pItem->checkState() != eNewState)
	{
		pItem->setCheckState(eNewState);
		QModelIndex index = createIndex(pItem->row(), 0, pItem);
		Q_EMIT dataChanged(index, index, { Qt::CheckStateRole });

		if(pItem->parent() != m_pRoot && pItem->parent() != nullptr)
			UpdateParentCheckState(pItem->parent());
	}
}
