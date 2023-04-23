/**************************************************************************
 *	EntityUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "EntityUndoCmds.h"
#include "EntityModel.h"
#include "EntityWidget.h"
#include "EntityDraw.h"
#include "IAssetItemData.h"
#include "MainWindow.h"

EntityUndoCmd_AddChildren::EntityUndoCmd_AddChildren(ProjectItemData &entityItemRef, QList<ProjectItemData *> projItemList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_EntityItemRef(entityItemRef),
	m_ChildrenList(projItemList)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

	setText("Add New Project Explorer Items(s)");
}

/*virtual*/ EntityUndoCmd_AddChildren::~EntityUndoCmd_AddChildren()
{
}

/*virtual*/ void EntityUndoCmd_AddChildren::redo() /*override*/
{
	QList<ProjectItemData *> itemList;
	for(auto *pProjItem : m_ChildrenList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pProjItem, true))
			itemList.push_back(pProjItem);
	}

	if(m_NodeList.empty())
		m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewChildren(itemList, -1);
	else
	{
		for(EntityTreeItemData *pItem : m_NodeList)
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(pItem, -1);
	}

	QList<QUuid> itemUuidList;
	for(auto *pNodeItem : m_NodeList)
		itemUuidList.push_back(pNodeItem->GetThisUuid());
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(itemUuidList);
}

/*virtual*/ void EntityUndoCmd_AddChildren::undo() /*override*/
{
	for(auto *pNodeItem : m_NodeList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pNodeItem, true))
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pNodeItem);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AddAssets::EntityUndoCmd_AddAssets(ProjectItemData &entityItemRef, QList<IAssetItemData *> assetItemList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_AssetList(assetItemList)
{
	setText("Add New Asset Node(s) as child");
}

/*virtual*/ EntityUndoCmd_AddAssets::~EntityUndoCmd_AddAssets()
{
}

/*virtual*/ void EntityUndoCmd_AddAssets::redo() /*override*/
{
	QList<IAssetItemData *> assetItemList;
	for(auto *pAssetItem : m_AssetList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pAssetItem, true))
			assetItemList.push_back(pAssetItem);
	}

	if(m_NodeList.empty())
		m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewAssets(assetItemList, -1);
	else
	{
		for(EntityTreeItemData *pItem : m_NodeList)
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(pItem, -1);
	}

	QList<QUuid> itemUuidList;
	for(auto *pNodeItem : m_NodeList)
		itemUuidList.push_back(pNodeItem->GetThisUuid());
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(itemUuidList);
}

/*virtual*/ void EntityUndoCmd_AddAssets::undo() /*override*/
{
	for(auto *pNodeItem : m_NodeList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pNodeItem, true))
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pNodeItem);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PopItems::EntityUndoCmd_PopItems(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> poppedItemList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_PoppedItemList(poppedItemList)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

	if(m_PoppedItemList.size() > 1)
		setText("Remove Items");
	else if(m_PoppedItemList.size() == 1)
		setText("Remove " % m_PoppedItemList[0]->GetText());
}

/*virtual*/ EntityUndoCmd_PopItems::~EntityUndoCmd_PopItems()
{
}

/*virtual*/ void EntityUndoCmd_PopItems::redo() /*override*/
{
	// NOTE: m_PoppedItemList has been sorted with descending row order
	m_PoppedIndexList.clear();
	for(EntityTreeItemData *pItem : m_PoppedItemList)
		m_PoppedIndexList.append(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pItem));
}

/*virtual*/ void EntityUndoCmd_PopItems::undo() /*override*/
{
	// Reinsert the 'm_PoppedItemList' in reverse order (so the indices work)
	QList<QUuid> readdedItemUuidList;
	for(int32 i = m_PoppedItemList.size() - 1; i >= 0; --i)
	{
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_PoppedItemList[i], m_PoppedIndexList[i]);
		readdedItemUuidList << m_PoppedItemList[i]->GetThisUuid();
	}

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(readdedItemUuidList);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PasteItems::EntityUndoCmd_PasteItems(ProjectItemData &entityItemRef, QJsonObject pasteMimeObject, EntityTreeItemData *pArrayFolder, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_PasteMimeObject(pasteMimeObject),
	m_pArrayFolder(pArrayFolder)
{
	setText("Pasted " % QString::number(m_PasteMimeObject["itemArray"].toArray().size()) % " items");
}

/*virtual*/ EntityUndoCmd_PasteItems::~EntityUndoCmd_PasteItems()
{
}

/*virtual*/ void EntityUndoCmd_PasteItems::redo() /*override*/
{
	if(m_PastedItemList.empty())
		m_PastedItemList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewPasteItems(m_PasteMimeObject, m_pArrayFolder);
	else
	{
		for(EntityTreeItemData *pItem : m_PastedItemList)
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(pItem, -1); // NOTE: If readding back into an array, the ArrayFolder will exist because this UndoCmd cannot create a new array
	}

	// Select the pasted items
	QList<QUuid> pastedItemUuidList;
	for(EntityTreeItemData *pItem : m_PastedItemList)
		pastedItemUuidList << pItem->GetThisUuid();
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(pastedItemUuidList);
}

/*virtual*/ void EntityUndoCmd_PasteItems::undo() /*override*/
{
	for(EntityTreeItemData *pItem : m_PastedItemList)
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pItem);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AddNewShape::EntityUndoCmd_AddNewShape(ProjectItemData &entityItemRef, EditorShape eShape, QString sData, bool bIsPrimitive, int32 iRowIndex /*= -1*/, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_eShape(eShape),
	m_sData(sData),
	m_bIsPrimitive(bIsPrimitive),
	m_iIndex(iRowIndex),
	m_pShapeTreeItemData(nullptr)
{
	setText("Add New " % HyGlobal::ShapeName(m_eShape) % (m_bIsPrimitive ? "Primitive" : "") % " Shape");
}

/*virtual*/ EntityUndoCmd_AddNewShape::~EntityUndoCmd_AddNewShape()
{
}

/*virtual*/ void EntityUndoCmd_AddNewShape::redo() /*override*/
{
	if(m_pShapeTreeItemData == nullptr)
		m_pShapeTreeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewShape(m_eShape, m_sData, m_bIsPrimitive, m_iIndex);
	else
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pShapeTreeItemData, m_iIndex);
}

/*virtual*/ void EntityUndoCmd_AddNewShape::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pShapeTreeItemData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_OrderChildren::EntityUndoCmd_OrderChildren(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<int> prevItemIndexList, QList<int> newItemIndexList, bool bOrderUpwards, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_SelectedItemDataList(selectedItemDataList),
	m_PrevItemIndexList(prevItemIndexList),
	m_NewItemIndexList(newItemIndexList),
	m_bOrderUpwards(bOrderUpwards)
{
	EntityTreeModel &treeModelRef = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel();
	if(m_bOrderUpwards)
		setText(m_SelectedItemDataList.size() == 1 ? "Order Child Upwards" : "Order Children Upwards");
	else
		setText(m_SelectedItemDataList.size() == 1 ? "Order Child Downwards" : "Order Children Downwards");
}

/*virtual*/ EntityUndoCmd_OrderChildren::~EntityUndoCmd_OrderChildren()
{
}

/*virtual*/ void EntityUndoCmd_OrderChildren::redo() /*override*/
{
	EntityModel *pModel = static_cast<EntityModel *>(m_EntityItemRef.GetModel());
	EntityTreeModel &entTreeModelRef = pModel->GetTreeModel();

	//QList<QUuid> selectedItemUuidList;
	for(int i = 0; i < m_SelectedItemDataList.size(); ++i)
	{
		TreeModelItemData *pDestinationParent = nullptr;
		if(m_SelectedItemDataList[i]->GetEntType() == ENTTYPE_ArrayItem)
			pDestinationParent = entTreeModelRef.GetArrayFolderTreeItemData(m_SelectedItemDataList[i]);
		else
		{
			if(m_SelectedItemDataList[i]->GetType() == ITEM_BoundingVolume)
				pDestinationParent = entTreeModelRef.GetBvFolderTreeItemData();
			else
				pDestinationParent = entTreeModelRef.GetRootTreeItemData();
		}

		entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], pDestinationParent, m_NewItemIndexList[i]);

		//selectedItemUuidList << m_SelectedItemDataList[i]->GetThisUuid();
	}

	//EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	//if(pWidget)
	//	pWidget->RequestSelectedItems(selectedItemUuidList);
}

/*virtual*/ void EntityUndoCmd_OrderChildren::undo() /*override*/
{
	EntityTreeModel &entTreeModelRef = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel();

	//QList<QUuid> selectedItemUuidList;
	for(int i = m_SelectedItemDataList.size() - 1; i >= 0; --i)
	{
		TreeModelItemData *pDestinationParent = nullptr;
		if(m_SelectedItemDataList[i]->GetEntType() == ENTTYPE_ArrayItem)
			pDestinationParent = entTreeModelRef.GetArrayFolderTreeItemData(m_SelectedItemDataList[i]);
		else
		{
			if(m_SelectedItemDataList[i]->GetType() == ITEM_BoundingVolume)
				pDestinationParent = entTreeModelRef.GetBvFolderTreeItemData();
			else
				pDestinationParent = entTreeModelRef.GetRootTreeItemData();
		}

		entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], pDestinationParent, m_PrevItemIndexList[i]);

		//selectedItemUuidList << m_SelectedItemDataList[i]->GetThisUuid();
	}

	//EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	//if(pWidget)
	//	pWidget->RequestSelectedItems(selectedItemUuidList);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_Transform::EntityUndoCmd_Transform(ProjectItemData &entityItemRef, int iStateIndex, const QList<EntityTreeItemData *> &affectedItemDataList, const QList<glm::mat4> &newTransformList, const QList<glm::mat4> &oldTransformList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_iStateIndex(iStateIndex),
	m_AffectedItemDataList(affectedItemDataList),
	m_NewTransformList(newTransformList),
	m_OldTransformList(oldTransformList)
{
	if(m_AffectedItemDataList.size() != m_NewTransformList.size() || m_AffectedItemDataList.size() != m_OldTransformList.size())
		HyGuiLog("EntityUndoCmd_Transform recieved improper sized lists", LOGTYPE_Error);

	setText("Transform " % QString::number(m_AffectedItemDataList.size()) % " Items");
}

/*virtual*/ EntityUndoCmd_Transform::~EntityUndoCmd_Transform()
{
}

/*virtual*/ void EntityUndoCmd_Transform::redo() /*override*/
{
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;

	QList<QUuid> affectedItemUuidList;
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		if(m_AffectedItemDataList[i]->GetType() != ITEM_BoundingVolume)
		{
			glm::decompose(m_NewTransformList[i], vScale, quatRot, ptTranslation, vSkew, vPerspective);
			double dRotation = glm::degrees(glm::atan(m_NewTransformList[i][0][1], m_NewTransformList[i][0][0]));

			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Transformation", "Position", QPointF(ptTranslation.x, ptTranslation.y));
			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Transformation", "Rotation", dRotation);
			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Transformation", "Scale", QPointF(vScale.x, vScale.y));
		}
		else
		{
			ShapeCtrl shapeCtrl(nullptr);
			shapeCtrl.Setup(HyGlobal::GetShapeFromString(m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).FindPropertyValue("Shape", "Type").toString()), HyColor::White, 0.0f, 1.0f);

			m_sOldShapeData = m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).FindPropertyValue("Shape", "Data").toString();
			shapeCtrl.Deserialize(m_sOldShapeData, nullptr);
			shapeCtrl.TransformSelf(m_NewTransformList[i]);
			
			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Shape", "Data", shapeCtrl.Serialize());
		}

		affectedItemUuidList << m_AffectedItemDataList[i]->GetThisUuid();
	}

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(affectedItemUuidList);

	m_EntityItemRef.FocusWidgetState(m_iStateIndex, -1);
}

/*virtual*/ void EntityUndoCmd_Transform::undo() /*override*/
{
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	QList<QUuid> affectedItemUuidList;
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		if(m_AffectedItemDataList[i]->GetType() != ITEM_BoundingVolume)
		{
			glm::decompose(m_OldTransformList[i], vScale, quatRot, ptTranslation, vSkew, vPerspective);

			double dRotation = glm::degrees(glm::atan(m_OldTransformList[i][0][1], m_OldTransformList[i][0][0]));
			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Transformation", "Position", QPointF(ptTranslation.x, ptTranslation.y));
			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Transformation", "Rotation", dRotation);
			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Transformation", "Scale", QPointF(vScale.x, vScale.y));
		}
		else
			m_AffectedItemDataList[i]->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Shape", "Data", m_sOldShapeData);

		affectedItemUuidList << m_AffectedItemDataList[i]->GetThisUuid();
	}

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(affectedItemUuidList);

	m_EntityItemRef.FocusWidgetState(m_iStateIndex, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_ShapeData::EntityUndoCmd_ShapeData(ProjectItemData &entityItemRef, int iStateIndex, EntityTreeItemData *pShapeItemData, ShapeCtrl::VemAction eVemAction, QString sNewData, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_iStateIndex(iStateIndex),
	m_pShapeItemData(pShapeItemData),
	m_eVemAction(eVemAction),
	m_sNewData(sNewData),
	m_sPrevData(m_pShapeItemData->GetPropertiesModel(iStateIndex).FindPropertyValue("Shape", "Data").toString())
{
	switch(m_eVemAction)
	{
	case ShapeCtrl::VEMACTION_Translate:
	case ShapeCtrl::VEMACTION_GrabPoint:
		setText("Translate vert(s) on " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_RadiusHorizontal:
	case ShapeCtrl::VEMACTION_RadiusVertical:
		setText("Adjust circle radius on " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_Add:
		setText("Add vertex to " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_RemoveSelected:
		setText("Remove vert(s) from " % pShapeItemData->GetCodeName());
		break;

	case ShapeCtrl::VEMACTION_Invalid:
	case ShapeCtrl::VEMACTION_None:
	default:
		HyGuiLog("EntityUndoCmd_ShapeData ctor - Invalid ShapeCtrl::VemAction", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ EntityUndoCmd_ShapeData::~EntityUndoCmd_ShapeData()
{
}

/*virtual*/ void EntityUndoCmd_ShapeData::redo() /*override*/
{
	m_pShapeItemData->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Shape", "Data", m_sNewData);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pShapeItemData->GetThisUuid());

	EntityDraw *pDraw = static_cast<EntityDraw *>(m_EntityItemRef.GetDraw());
	if(pDraw)
		pDraw->ActivateVemOnNextJsonMeta();
}

/*virtual*/ void EntityUndoCmd_ShapeData::undo() /*override*/
{
	m_pShapeItemData->GetPropertiesModel(m_iStateIndex).SetPropertyValue("Shape", "Data", m_sPrevData);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pShapeItemData->GetThisUuid());

	EntityDraw *pDraw = static_cast<EntityDraw *>(m_EntityItemRef.GetDraw());
	if(pDraw)
		pDraw->ActivateVemOnNextJsonMeta();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_ConvertShape::EntityUndoCmd_ConvertShape(ProjectItemData &entityItemRef, EntityTreeItemData *pShapeItemData, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_pNewShapeItemData(nullptr),
	m_pPrevShapeItemData(pShapeItemData),
	m_iPoppedIndex(-1)
{
	if(m_pPrevShapeItemData->GetType() == ITEM_BoundingVolume)
		setText("Convert shape to Primitive");
	else
		setText("Convert shape to Bounding Volume");
}

/*virtual*/ EntityUndoCmd_ConvertShape::~EntityUndoCmd_ConvertShape()
{
}

/*virtual*/ void EntityUndoCmd_ConvertShape::redo() /*override*/
{
	m_iPoppedIndex = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pPrevShapeItemData);

	if(m_pNewShapeItemData == nullptr)
	{
		EditorShape eShape = HyGlobal::GetShapeFromString(m_pPrevShapeItemData->GetPropertiesModel(0).FindPropertyValue("Shape", "Type").toString());
		QString sData = m_pPrevShapeItemData->GetPropertiesModel(0).FindPropertyValue("Shape", "Data").toString();
		bool bConvertingToPrimitive = m_pPrevShapeItemData->GetType() == ITEM_BoundingVolume;

		m_pNewShapeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewShape(eShape, sData, bConvertingToPrimitive, -1);
		for(int iStateIndex = 0; iStateIndex < m_EntityItemRef.GetModel()->GetNumStates(); ++iStateIndex)
		{
			QString sPrevShapeType = m_pPrevShapeItemData->GetPropertiesModel(iStateIndex).FindPropertyValue("Shape", "Type").toString();
			QString sPrevShapeData = m_pPrevShapeItemData->GetPropertiesModel(iStateIndex).FindPropertyValue("Shape", "Data").toString();

			m_pNewShapeItemData->GetPropertiesModel(iStateIndex).SetPropertyValue("Shape", "Type", sPrevShapeType);
			m_pNewShapeItemData->GetPropertiesModel(iStateIndex).SetPropertyValue("Shape", "Data", sPrevShapeData);
		}
	}
	else
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pNewShapeItemData, -1);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pNewShapeItemData->GetThisUuid());

	//EntityDraw *pDraw = static_cast<EntityDraw *>(m_EntityItemRef.GetDraw());
	//if(pDraw)
	//	pDraw->ActivateVemOnNextJsonMeta();
}

/*virtual*/ void EntityUndoCmd_ConvertShape::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pNewShapeItemData);
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pPrevShapeItemData, m_iPoppedIndex);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pPrevShapeItemData->GetThisUuid());

	//EntityDraw *pDraw = static_cast<EntityDraw *>(m_EntityItemRef.GetDraw());
	//if(pDraw)
	//	pDraw->ActivateVemOnNextJsonMeta();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_RenameItem::EntityUndoCmd_RenameItem(ProjectItemData &entityItemRef, EntityTreeItemData *pItemData, QString sNewName, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_EntityItemRef(entityItemRef),
	m_pItemData(pItemData),
	m_sNewName(sNewName),
	m_sOldName(m_pItemData->GetCodeName())
{
	setText("Rename entity item to " % sNewName);
}

/*virtual*/ EntityUndoCmd_RenameItem::~EntityUndoCmd_RenameItem()
{
}

/*virtual*/ void EntityUndoCmd_RenameItem::redo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RenameItem(m_pItemData, m_sNewName);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pItemData->GetThisUuid());
}

/*virtual*/ void EntityUndoCmd_RenameItem::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RenameItem(m_pItemData, m_sOldName);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pItemData->GetThisUuid());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_DuplicateToArray::EntityUndoCmd_DuplicateToArray(ProjectItemData &entityItemRef, EntityTreeItemData *pItemData, int iArraySize, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_pItemData(pItemData),
	m_iPoppedIndex(-1),
	m_iArraySize(iArraySize)
{
	if(pItemData->GetEntType() != ENTTYPE_Item)
		HyGuiLog("EntityUndoCmd_DuplicateToArray - invalid pItemData passed to ctor, was not ENTTYPE_Item", LOGTYPE_Error);
	
	setText("Duplicate " % pItemData->GetCodeName() % " into " % HyGlobal::ItemName(pItemData->GetType(), false) % " array[" % QString::number(iArraySize) % "]");
}

/*virtual*/ EntityUndoCmd_DuplicateToArray::~EntityUndoCmd_DuplicateToArray()
{
}

/*virtual*/ void EntityUndoCmd_DuplicateToArray::redo() /*override*/
{
	// First remove the item
	m_iPoppedIndex = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pItemData);
	
	if(m_ArrayItemList.empty())
	{
		QList<EntityTreeItemData *> mimeItemList;
		for(int i = 0; i < m_iArraySize; ++i)
			mimeItemList << m_pItemData;

		m_ArrayItemList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewArray(mimeItemList, m_pItemData->GetCodeName(), m_iPoppedIndex);
	}
	else
	{
		for(int i = 0; i < m_ArrayItemList.size(); ++i)
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_ArrayItemList[i], (i == 0) ? m_iPoppedIndex : -1);
	}

	QList<QUuid> arrayItemUuidList;
	for(EntityTreeItemData *pArrayItem : m_ArrayItemList)
		arrayItemUuidList.push_back(pArrayItem->GetThisUuid());
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(arrayItemUuidList);
}

/*virtual*/ void EntityUndoCmd_DuplicateToArray::undo() /*override*/
{
	for(EntityTreeItemData *pItem : m_ArrayItemList)
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pItem);

	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pItemData, m_iPoppedIndex);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pItemData->GetThisUuid());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PackToArray::EntityUndoCmd_PackToArray(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> packItemList, QString sArrayName, int iArrayFolderRow, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_PackItemList(packItemList),
	m_sArrayName(sArrayName),
	m_iArrayFolderRow(iArrayFolderRow)
{
	setText("Pack items into " % HyGlobal::ItemName(m_PackItemList[0]->GetType(), false) % " array");
}

/*virtual*/ EntityUndoCmd_PackToArray::~EntityUndoCmd_PackToArray()
{
}

/*virtual*/ void EntityUndoCmd_PackToArray::redo() /*override*/
{
	// NOTE: m_PackItemList has been sorted to be in row descending order
	m_PoppedIndexList.clear();
	for(EntityTreeItemData *pItem : m_PackItemList)
		m_PoppedIndexList.append(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pItem));

	if(m_ArrayItemList.empty())
		m_ArrayItemList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewArray(m_PackItemList, m_sArrayName, m_iArrayFolderRow);
	else
	{
		for(int i = 0; i < m_ArrayItemList.size(); ++i)
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_ArrayItemList[i], (i == 0) ? m_iArrayFolderRow : -1);
	}

	QList<QUuid> arrayItemUuidList;
	for(EntityTreeItemData *pArrayItem : m_ArrayItemList)
		arrayItemUuidList.push_back(pArrayItem->GetThisUuid());
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(arrayItemUuidList);
}

/*virtual*/ void EntityUndoCmd_PackToArray::undo() /*override*/
{
	for(EntityTreeItemData *pItem : m_ArrayItemList)
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pItem);

	// Reinsert the 'm_PackItemList' in reverse order (so the indices work)
	QList<QUuid> readdedItemUuidList;
	for(int32 i = m_PackItemList.size() - 1; i >= 0; --i)
	{
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_PackItemList[i], m_PoppedIndexList[i]);
		readdedItemUuidList << m_PackItemList[i]->GetThisUuid();
	}

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(readdedItemUuidList);
}
