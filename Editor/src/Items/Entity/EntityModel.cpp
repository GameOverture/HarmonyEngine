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
#include "EntityWidget.h"

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
	m_TreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta["UUID"].toString(), this),
	m_bVertexEditMode(false)
{
	InitStates<EntityStateData>(itemFileDataRef);

	// Insert all the items into the nodeTree
	QJsonArray childArray = itemFileDataRef.m_Meta["childList"].toArray();
	for(int i = 0; i < childArray.size(); ++i)
	{
		QJsonObject childObj = childArray[i].toObject();
		Cmd_AddNewChild(childObj, i);
	}

	QJsonArray shapeArray = itemFileDataRef.m_Meta["shapeList"].toArray();
	for(int i = 0; i < shapeArray.size(); ++i)
	{
		QJsonObject shapeObj = shapeArray[i].toObject();
		Cmd_AddNewChild(shapeObj, i);
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
	QList<QUuid> registerList;
	for(auto *pItem : projItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_InsertNewChild(pItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_AddNewChildren could not insert a child: " % pItem->GetName(true), LOGTYPE_Error);

		registerList.push_back(pItem->GetUuid());
	}

	m_ItemRef.GetProject().RegisterItems(GetUuid(), registerList);
	
	return treeNodeList;
}

EntityTreeItemData *EntityModel::Cmd_AddNewChild(QJsonObject initObj, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_InsertNewChild(initObj, iRow);

	QUuid uuidToRegister(initObj["itemUUID"].toString());
	if(uuidToRegister.isNull() == false)
		m_ItemRef.GetProject().RegisterItems(GetUuid(), QList<QUuid>() << uuidToRegister);

	return pTreeItemData;
}

EntityTreeItemData *EntityModel::Cmd_AddNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_InsertNewShape(eShape, sData, bIsPrimitive, "m_", iRow);
	
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << pTreeItemData->GetThisUuid(), false);

	return pTreeItemData;
}

void EntityModel::Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList)
{
	for(EntityTreeItemData *pTreeItem : selectedList)
		pTreeItem->SetSelected(true);
	for(EntityTreeItemData *pTreeItem : deselectedList)
		pTreeItem->SetSelected(false);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->SetSelectedItems(selectedList, deselectedList);
}

int32 EntityModel::Cmd_RemoveTreeItem(EntityTreeItemData *pItem)
{
	if(pItem == nullptr)
		return -1;

	int32 iRow = m_TreeModel.Cmd_PopChild(pItem);
	if(iRow < 0)
		return iRow;

	m_ItemRef.GetProject().RelinquishItems(GetUuid(), QList<QUuid>() << pItem->GetItemUuid());

	return iRow;
}

bool EntityModel::Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow)
{
	if(m_TreeModel.Cmd_InsertChild(pNodeItem, iRow) == false)
		return false;

	m_ItemRef.GetProject().RegisterItems(GetUuid(), QList<QUuid>() << pNodeItem->GetItemUuid());

	return true;
}

void EntityModel::ToggleVemMode()
{
	m_bVertexEditMode = !m_bVertexEditMode;

	if(m_bVertexEditMode)
		MainWindow::SetStatus("Vertex Edit Mode", 0);
	else
		MainWindow::ClearStatus();

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->SetShapeEditVertex(m_bVertexEditMode);
}

void EntityModel::SetVemMode(bool bEnable)
{
	m_bVertexEditMode = bEnable;
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
		shapeArray.append(childObj);
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
