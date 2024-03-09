/**************************************************************************
*	PropertiesTreeMultiModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesTreeMultiModel.h"

PropertiesTreeMultiModel::PropertiesTreeMultiModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, const QList<PropertiesTreeModel *> &multiModelListRef, QObject *pParent /*= nullptr*/) :
	PropertiesTreeModel(ownerRef, iStateIndex, subState, pParent),
	m_MultiModelList(multiModelListRef)
{
	// Construct this model only with categories/properties that are shared in all the models within 'm_MultiModelList'
	for(PropertiesTreeModel *pModel : m_MultiModelList)
	{
		//pModel->Cat
	}
}

/*virtual*/ PropertiesTreeMultiModel::~PropertiesTreeMultiModel()
{
}

/*virtual*/ void PropertiesTreeMultiModel::SetToggle(const QModelIndex &indexRef, bool bToggleOn) /*override*/
{
	//TreeModelItem *pTreeItem = GetItem(indexRef);
	//m_PropertyDefMap[pTreeItem].eAccessType = bToggleOn ? PROPERTIESACCESS_ToggleOn : PROPERTIESACCESS_ToggleOff;
}

/*virtual*/ void PropertiesTreeMultiModel::SetPropertyValue(QString sCategoryName, QString sPropertyName, const QVariant &valueRef, bool bIsProceduralObj) /*override*/
{
	//for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	//{
	//	if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
	//	{
	//		TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
	//		for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
	//		{
	//			TreeModelItem *pPropertyTreeItem = pCategoryTreeItem->GetChild(j);

	//			if(0 == pPropertyTreeItem->data(PROPERTIESCOLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
	//			{
	//				if(setData(createIndex(pPropertyTreeItem->GetIndex(), PROPERTIESCOLUMN_Value, pPropertyTreeItem), valueRef, Qt::UserRole) == false)
	//					HyGuiLog("PropertiesTreeModel::SetPropertyValue() - setData failed", LOGTYPE_Error);

	//				if(m_PropertyDefMap[pPropertyTreeItem].eAccessType == PROPERTIESACCESS_ToggleOff)
	//					m_PropertyDefMap[pPropertyTreeItem].eAccessType = PROPERTIESACCESS_ToggleOn;

	//				return;
	//			}
	//		}
	//	}
	//}
}

/*virtual*/ bool PropertiesTreeMultiModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	//if(iRole == Qt::UserRole)
	//	return ITreeModel::setData(indexRef, valueRef, iRole);

	//// NOTE: There are three cases to handle here.
	////       1) A normal setData() call that changes the data value in the PROPERTIESCOLUMN_Value When PROPERTIESACCESS_Toggle* is set here, handle it
	////       2) A category that's checkable is changed, which changes the m_PropertyDefMap[]'s 'eAccessType'
	////       3) Check/Uncheck a property that is 'togglable', which changes the m_PropertyDefMap[]'s 'eAccessType'
	//if(indexRef.column() == PROPERTIESCOLUMN_Name) // Indicates either case '2' or '3'
	//{
	//	TreeModelItem *pTreeItem = GetItem(indexRef);
	//	if((m_PropertyDefMap[pTreeItem].eAccessType == PROPERTIESACCESS_ToggleOn && valueRef == Qt::Unchecked) ||
	//		(m_PropertyDefMap[pTreeItem].eAccessType == PROPERTIESACCESS_ToggleOff && valueRef == Qt::Checked))
	//	{
	//		PropertiesUndoCmd *pUndoCmd = AllocateUndoCmd(this, indexRef, static_cast<bool>(valueRef == Qt::Checked));
	//		GetOwner().GetUndoStack()->push(pUndoCmd);
	//	}
	//}
	//else // PROPERTIESCOLUMN_Value - indicating case '1'
	//{
	//	const QVariant &origValue = GetPropertyValue(indexRef);
	//	if(origValue != valueRef)
	//	{
	//		PropertiesUndoCmd *pUndoCmd = AllocateUndoCmd(this, indexRef, valueRef);
	//		GetOwner().GetUndoStack()->push(pUndoCmd);
	//	}
	//}

	return false; // Return false because another setData() will be invoked via the UndoCmd, which actually changes the data
}

/*virtual*/ QVariant PropertiesTreeMultiModel::data(const QModelIndex &indexRef, int iRole) const /*override*/
{
	//if(indexRef.isValid() == false)
	//	return QVariant();

	//if(iRole == Qt::UserRole)
	//	return ITreeModel::data(indexRef, iRole);

	//TreeModelItem *pTreeItem = GetItem(indexRef);
	//const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	//switch(iRole)
	//{
	//case Qt::DisplayRole:
	//	if(indexRef.column() == PROPERTIESCOLUMN_Name)
	//		return pTreeItem->data(PROPERTIESCOLUMN_Name);
	//	else
	//		return ConvertValueToString(pTreeItem);

	//case Qt::TextAlignmentRole:
	//	return Qt::AlignLeft;
	//	//if(propDefRef.IsCategory())
	//	//	return Qt::AlignHCenter;

	//case Qt::ToolTipRole:
	//	return propDefRef.sToolTip;

	//case Qt::BackgroundRole:
	//	if(propDefRef.IsCategory())
	//		return QBrush(propDefRef.GetColor());
	//	else if(indexRef.column() == PROPERTIESCOLUMN_Value && propDefRef.eType == PROPERTIESTYPE_Color && propDefRef.eAccessType != PROPERTIESACCESS_ToggleOff)
	//		return QBrush(QColor(pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().left(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().top(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().width()));
	//	//	return QBrush((0 == (pTreeItem->GetIndex() & 1)) ? propDefRef.GetColor() : propDefRef.GetColor().lighter());

	//case Qt::ForegroundRole:
	//	if(propDefRef.IsCategory())
	//		return QBrush(QColor::fromRgb(255, 255, 255));
	//	else if(indexRef.column() == PROPERTIESCOLUMN_Value && propDefRef.eType == PROPERTIESTYPE_Color && propDefRef.eAccessType != PROPERTIESACCESS_ToggleOff)
	//	{
	//		QColor clr(pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().left(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().top(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().width());
	//		double a = 1 - (0.299 * clr.redF() + 0.587 * clr.greenF() + 0.114 * clr.blueF()) / 255;
	//		if(a < 0.5)
	//		{
	//			QBrush bgColorBrush(Qt::black);
	//			return QVariant(bgColorBrush);
	//		}
	//		else
	//		{
	//			QBrush bgColorBrush(Qt::white);
	//			return QVariant(bgColorBrush);
	//		}
	//	}

	//case Qt::FontRole:
	//	if(propDefRef.IsCategory())
	//	{
	//		QFont font;
	//		font.setBold(true);
	//		return font;
	//	}

	//case Qt::CheckStateRole:
	//	if(indexRef.column() == 0)
	//	{
	//		if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn || propDefRef.eAccessType == PROPERTIESACCESS_ToggleOff)
	//			return propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn ? Qt::Checked : Qt::Unchecked;
	//	}
	//	else if(propDefRef.eType == PROPERTIESTYPE_bool)
	//		return pTreeItem->data(PROPERTIESCOLUMN_Value);
	//}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags PropertiesTreeMultiModel::flags(const QModelIndex &indexRef) const /*override*/
{
	Qt::ItemFlags returnFlags = Qt::NoItemFlags;

	//if(indexRef.isValid() == false)
	//	return returnFlags;

	//TreeModelItem *pTreeItem = GetItem(indexRef);
	//const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	//if(propDefRef.IsCategory())
	//{
	//	if(propDefRef.eAccessType != PROPERTIESACCESS_ReadOnly)
	//		returnFlags |= Qt::ItemIsEnabled;

	//	if(propDefRef.IsTogglable())
	//		returnFlags |= Qt::ItemIsUserCheckable;
	//}
	//else
	//{
	//	TreeModelItem *pCategoryTreeItem = pTreeItem->GetParent();
	//	const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];
	//	if(categoryPropDefRef.IsCategory() == false)
	//		HyGuiLog("PropertiesTreeModel::flags() passed in index is not a category and its parent is not one either", LOGTYPE_Error);

	//	if(categoryPropDefRef.IsTogglable() == false ||
	//	   (categoryPropDefRef.IsTogglable() && categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOn))
	//	{
	//		if(propDefRef.eAccessType != PROPERTIESACCESS_ReadOnly)
	//		{
	//			if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn || propDefRef.eAccessType == PROPERTIESACCESS_ToggleOff)
	//			{
	//				if(indexRef.column() == PROPERTIESCOLUMN_Name)
	//					returnFlags |= (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	//				else // column is PROPERTIESCOLUMN_Value
	//				{
	//					if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn)
	//						returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	//				}
	//			}
	//			else // PROPERTIESACCESS_Mutable
	//				returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);

	//			if(propDefRef.eType == PROPERTIESTYPE_bool)
	//				returnFlags |= Qt::ItemIsUserCheckable;
	//		}
	//	}
	//}

	return returnFlags;
}
