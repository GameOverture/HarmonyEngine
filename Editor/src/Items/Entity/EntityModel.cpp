/**************************************************************************
*	EntityModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityModel.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "MainWindow.h"
#include "EntityDraw.h"

EntityStateData::EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData)
{
}
/*virtual*/ EntityStateData::~EntityStateData()
{
}
/*virtual*/ QVariant EntityStateData::OnLinkAsset(AssetItemData *pAsset) /*override*/
{
	return 0;
}
/*virtual*/ void EntityStateData::OnUnlinkAsset(AssetItemData *pAsset) /*override*/
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_EntityTypeMapper(this),
	m_TreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta["UUID"].toString(), this)
{
	InitStates<EntityStateData>(itemFileDataRef);

	// Insert all the items into the nodeTree
	QJsonArray childArray = itemFileDataRef.m_Meta["childList"].toArray();
	for(int i = 0; i < childArray.size(); ++i)
	{
		QJsonObject childObj = childArray[i].toObject();
		
		HyGuiItemType eGuiType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());
		if(eGuiType == ITEM_Primitive)
			Cmd_AddNewChild(nullptr, childObj, i);
		else
		{
			QUuid uuid(childObj["itemUUID"].toString());
			ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(uuid);
			if(pProjItem)
				Cmd_AddNewChild(nullptr, childObj, i);
			else
				HyGuiLog("Null project item for UUID: " % uuid.toString() % " type: " % HyGlobal::ItemName(eGuiType, false), LOGTYPE_Error);
		}
	}
}

/*virtual*/ EntityModel::~EntityModel()
{
}

void EntityModel::RegisterWidgets(QComboBox &cmbEntityTypeRef)
{
	m_EntityTypeMapper.AddComboBoxMapping(&cmbEntityTypeRef);
}

EntityTreeModel &EntityModel::GetTreeModel()
{
	return m_TreeModel;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	for(auto *pItem : projItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_InsertNewChild(pItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_AddNewChildren could not insert a child: " % pItem->GetName(true), LOGTYPE_Error);
	}

	m_ItemRef.GetProject().RegisterItems(&m_ItemRef, projItemList);
	
	return treeNodeList;
}

EntityTreeItemData *EntityModel::Cmd_AddNewChild(ProjectItemData *pProjItemData, QJsonObject initObj, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_InsertNewChild(initObj, iRow);
	if(pProjItemData)
		m_ItemRef.GetProject().RegisterItems(&m_ItemRef, QList<ProjectItemData *>() << pProjItemData);

	return pTreeItemData;
}

EntityTreeItemData *EntityModel::Cmd_AddNewPrimitive(int iRow)
{
	return nullptr;
}

EntityTreeItemData *EntityModel::Cmd_AddShape(int iRow)
{
	return nullptr;
}

void EntityModel::Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList)
{
	EntityDraw *pDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pDraw)
		pDraw->OnSelectionChange(selectedList, deselectedList);
}

int32 EntityModel::Cmd_RemoveTreeItem(EntityTreeItemData *pItem)
{
	if(pItem == nullptr)
		return -1;

	int32 iRow = m_TreeModel.Cmd_PopChild(pItem);
	if(iRow < 0)
		return iRow;

	ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(pItem->GetItemUuid());
	if(pProjItem)
		m_ItemRef.GetProject().RelinquishItems(&m_ItemRef, QList<ProjectItemData *>() << pProjItem);

	return iRow;
}

bool EntityModel::Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow)
{
	if(m_TreeModel.Cmd_InsertChild(pNodeItem, iRow) == false)
		return false;

	ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(pNodeItem->GetItemUuid());
	if(pProjItem)
		m_ItemRef.GetProject().RegisterItems(&m_ItemRef, QList<ProjectItemData *>() << pProjItem);

	return true;
}

/*virtual*/ void EntityModel::OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) /*override*/
{
	EntityTreeItemData *pEntityTreeData = reinterpret_cast<EntityTreeItemData *>(propertiesModelRef.GetSubstate().toLongLong());
	

}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	return true;
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	itemSpecificFileDataOut.m_Meta.insert("codeName", m_TreeModel.GetEntityTreeItemData()->GetCodeName());
	itemSpecificFileDataOut.m_Meta.insert("entityType", m_EntityTypeMapper.GetCurrentItem());
	
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	m_TreeModel.GetTreeItemData(childList, shapeList);

	QJsonArray childArray;
	for(EntityTreeItemData *pChild : childList)
	{
		QJsonObject childObj;
		pChild->InsertJsonInfo(childObj);
		childArray.append(childObj);
	}
	itemSpecificFileDataOut.m_Meta.insert("childList", childArray);

	QJsonArray shapeArray;
	for(EntityTreeItemData *pShape : shapeList)
	{
		QJsonObject childObj;
		pShape->InsertJsonInfo(childObj);
		childArray.append(childObj);
	}	
	itemSpecificFileDataOut.m_Meta.insert("shapeList", shapeArray);
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
}

/*virtual*/ QList<AssetItemData *> EntityModel::GetAssets(AssetType eAssetType) const /*override*/
{
	// TODO: EntityModel::GetAssets
	return QList<AssetItemData *>();
}
