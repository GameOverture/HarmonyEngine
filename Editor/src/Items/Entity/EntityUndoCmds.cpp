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
#include "AuxDopeSheet.h"

#include <QJsonValue>

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
		m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewChildren(itemList, -1);
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
		m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewAssets(assetItemList, -1);
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
	{
		int iStateIndex = m_EntityItemRef.GetWidget()->GetCurStateIndex();
		int iFrameIndex = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
		m_pShapeTreeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewShape(iStateIndex, iFrameIndex, m_eShape, m_sData, m_bIsPrimitive, m_iIndex);
	}
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

EntityUndoCmd_Transform::EntityUndoCmd_Transform(ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, const QList<EntityTreeItemData *> &affectedItemDataList, const QList<glm::mat4> &newTransformList, const QList<glm::mat4> &oldTransformList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_iStateIndex(iStateIndex),
	m_iFrameIndex(iFrameIndex),
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
	glm::quat quatRot;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;

	glm::vec3 ptNewTranslation;
	glm::vec3 vNewScale(1.0f);
	glm::vec3 ptOldTranslation;
	glm::vec3 vOldScale(1.0f);

	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));
	QList<QUuid> affectedItemUuidList;
	m_sOldShapeDataList.clear();
	m_CreatedKeyFrameList.clear();
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		if(m_AffectedItemDataList[i]->GetType() != ITEM_BoundingVolume)
		{
			glm::decompose(m_NewTransformList[i], vNewScale, quatRot, ptNewTranslation, vSkew, vPerspective);
			double dNewRotation = glm::degrees(glm::atan(m_NewTransformList[i][0][1], m_NewTransformList[i][0][0]));
			glm::decompose(m_OldTransformList[i], vOldScale, quatRot, ptOldTranslation, vSkew, vPerspective);
			double dOldRotation = glm::degrees(glm::atan(m_OldTransformList[i][0][1], m_OldTransformList[i][0][0]));

			dOldRotation = HyMath::RoundToHundredth(dOldRotation);
			dNewRotation = HyMath::RoundToHundredth(dNewRotation);

			HySetVec(vNewScale, HyMath::RoundToThousandth(vNewScale.x), HyMath::RoundToThousandth(vNewScale.y), HyMath::RoundToThousandth(vNewScale.z));
			HySetVec(vOldScale, HyMath::RoundToThousandth(vOldScale.x), HyMath::RoundToThousandth(vOldScale.y), HyMath::RoundToThousandth(vOldScale.z));

			bool bCreatedTranslationKeyFrame = false;
			bool bCreatedRotationKeyFrame = false;
			bool bCreatedScaleKeyFrame = false;

			if(ptNewTranslation != ptOldTranslation)
			{
				QVariant tmpVariant = QPointF(ptNewTranslation.x, ptNewTranslation.y);
				bCreatedTranslationKeyFrame = pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Position", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_vec2, tmpVariant), false);
			}
			if(dNewRotation != dOldRotation)
			{
				QVariant tmpVariant = dNewRotation;
				bCreatedRotationKeyFrame = pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Rotation", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_double, tmpVariant), false);
			}
			if(vNewScale != vOldScale)
			{
				QVariant tmpVariant = QPointF(vNewScale.x, vNewScale.y);
				bCreatedScaleKeyFrame = pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Scale", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_vec2, tmpVariant), false);
			}

			m_CreatedKeyFrameList.push_back(std::make_tuple(bCreatedTranslationKeyFrame, bCreatedRotationKeyFrame, bCreatedScaleKeyFrame));

			m_sOldShapeDataList.push_back(QString());
		}
		else // ITEM_BoundingVolume
		{
			QString sShapeType = PropertiesTreeModel::ConvertJsonToVariant(PROPERTIESTYPE_ComboBoxString, pStateData->GetDopeSheetScene().GetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Type")).toString();
			QString sShapeData = PropertiesTreeModel::ConvertJsonToVariant(PROPERTIESTYPE_LineEdit, pStateData->GetDopeSheetScene().GetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Data")).toString();
			m_sOldShapeDataList.push_back(sShapeData);
			
			ShapeCtrl shapeCtrl(nullptr);
			shapeCtrl.Setup(HyGlobal::GetShapeFromString(sShapeType), HyColor::White, 0.0f, 1.0f);
			shapeCtrl.Deserialize(m_sOldShapeDataList.last(), nullptr);
			shapeCtrl.TransformSelf(m_NewTransformList[i]);
			
			pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Data", shapeCtrl.Serialize(), false);
		}

		affectedItemUuidList << m_AffectedItemDataList[i]->GetThisUuid();
	}

	pStateData->GetDopeSheetScene().RefreshAllGfxItems();

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(affectedItemUuidList);

	m_EntityItemRef.FocusWidgetState(m_iStateIndex, -1);
}

/*virtual*/ void EntityUndoCmd_Transform::undo() /*override*/
{
	glm::quat quatRot;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	
	glm::vec3 ptNewTranslation;
	glm::vec3 vNewScale(1.0f);
	glm::vec3 ptOldTranslation;
	glm::vec3 vOldScale(1.0f);

	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));
	QList<QUuid> affectedItemUuidList;
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		if(m_AffectedItemDataList[i]->GetType() != ITEM_BoundingVolume)
		{
			glm::decompose(m_OldTransformList[i], vOldScale, quatRot, ptOldTranslation, vSkew, vPerspective);
			double dOldRotation = glm::degrees(glm::atan(m_OldTransformList[i][0][1], m_OldTransformList[i][0][0]));
			glm::decompose(m_NewTransformList[i], vNewScale, quatRot, ptNewTranslation, vSkew, vPerspective);
			double dNewRotation = glm::degrees(glm::atan(m_NewTransformList[i][0][1], m_NewTransformList[i][0][0]));

			dOldRotation = HyMath::RoundToHundredth(dOldRotation);
			dNewRotation = HyMath::RoundToHundredth(dNewRotation);

			HySetVec(vNewScale, HyMath::RoundToThousandth(vNewScale.x), HyMath::RoundToThousandth(vNewScale.y), HyMath::RoundToThousandth(vNewScale.z));
			HySetVec(vOldScale, HyMath::RoundToThousandth(vOldScale.x), HyMath::RoundToThousandth(vOldScale.y), HyMath::RoundToThousandth(vOldScale.z));

			if(ptOldTranslation != ptNewTranslation)
			{
				if(std::get<0>(m_CreatedKeyFrameList[i]))
					pStateData->GetDopeSheetScene().RemoveKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Position", false);
				else
				{
					QVariant tmpVariant = QPointF(ptOldTranslation.x, ptOldTranslation.y);
					pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Position", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_vec2, tmpVariant), false);
				}
			}
			if(dOldRotation != dNewRotation)
			{
				if(std::get<1>(m_CreatedKeyFrameList[i]))
					pStateData->GetDopeSheetScene().RemoveKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Rotation", false);
				else
				{
					QVariant tmpVariant = dOldRotation;
					pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Rotation", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_double, tmpVariant), false);
				}
			}
			if(vOldScale != vNewScale)
			{
				if(std::get<2>(m_CreatedKeyFrameList[i]))
					pStateData->GetDopeSheetScene().RemoveKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Scale", false);
				else
				{
					QVariant tmpVariant = QPointF(vOldScale.x, vOldScale.y);
					pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Scale", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_vec2, tmpVariant), false);
				}
			}

			m_sOldShapeDataList.push_back(QString());
		}
		else
			pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Data", m_sOldShapeDataList[i], false);

		affectedItemUuidList << m_AffectedItemDataList[i]->GetThisUuid();
	}

	pStateData->GetDopeSheetScene().RefreshAllGfxItems();

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(affectedItemUuidList);

	m_EntityItemRef.FocusWidgetState(m_iStateIndex, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_ShapeData::EntityUndoCmd_ShapeData(ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, EntityTreeItemData *pShapeItemData, ShapeCtrl::VemAction eVemAction, QString sNewData, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_iStateIndex(iStateIndex),
	m_iFrameIndex(iFrameIndex),
	m_pShapeItemData(pShapeItemData),
	m_eVemAction(eVemAction),
	m_sNewData(sNewData)
{
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));
	m_sPrevData = PropertiesTreeModel::ConvertJsonToVariant(PROPERTIESTYPE_LineEdit, pStateData->GetDopeSheetScene().GetKeyFrameProperty(m_pShapeItemData, m_iFrameIndex, "Shape", "Data")).toString();

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
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));
	pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_pShapeItemData, m_iFrameIndex, "Shape", "Data", m_sNewData, true);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pShapeItemData->GetThisUuid());

	EntityDraw *pDraw = static_cast<EntityDraw *>(m_EntityItemRef.GetDraw());
	if(pDraw)
		pDraw->ActivateVemOnNextJsonMeta();
}

/*virtual*/ void EntityUndoCmd_ShapeData::undo() /*override*/
{
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));
	pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_pShapeItemData, m_iFrameIndex, "Shape", "Data", m_sPrevData, true);

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
		QJsonObject descObj;
		m_pPrevShapeItemData->InsertJsonInfo_Desc(descObj);
		descObj["Type"] = m_pPrevShapeItemData->GetType() == ITEM_BoundingVolume ? HyGlobal::ItemName(ITEM_Primitive, false) : HyGlobal::ItemName(ITEM_BoundingVolume, false);
		m_pNewShapeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddExistingItem(descObj, m_pPrevShapeItemData->GetEntType() == ENTTYPE_ArrayItem, -1);
	}
	else
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pNewShapeItemData, -1);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pNewShapeItemData->GetThisUuid());
}

/*virtual*/ void EntityUndoCmd_ConvertShape::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pNewShapeItemData);
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pPrevShapeItemData, m_iPoppedIndex);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pPrevShapeItemData->GetThisUuid());
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PasteKeyFrames::EntityUndoCmd_PasteKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, EntityTreeItemData *pItemData, int iFrameStart, QJsonArray pasteDataArray, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_pItemData(pItemData),
	m_iFrameStart(iFrameStart),
	m_PasteDataArray(pasteDataArray)
{
	setText("Paste Key Frames");
}

/*virtual*/ EntityUndoCmd_PasteKeyFrames::~EntityUndoCmd_PasteKeyFrames()
{
}


/*virtual*/ void EntityUndoCmd_PasteKeyFrames::redo() /*override*/
{
	m_DopeSheetSceneRef.PasteFrameData(m_pItemData, m_iFrameStart, m_PasteDataArray);
}

/*virtual*/ void EntityUndoCmd_PasteKeyFrames::undo() /*override*/
{
	m_DopeSheetSceneRef.UnpasteFrameData(m_pItemData, m_iFrameStart, m_PasteDataArray);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_NudgeSelectedKeyFrames::EntityUndoCmd_NudgeSelectedKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameOffset, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_iFrameOffset(iFrameOffset)
{
	QList<QGraphicsItem *> selectedItemsList = m_DopeSheetSceneRef.selectedItems();
	setText("Offset " % QString::number(selectedItemsList.size()) % " Key Frame(s)");

	for(QGraphicsItem *pSelectedGfxItem : selectedItemsList)
	{
		GraphicsKeyFrameItem *pSelectedGfxKeyFrameItem = static_cast<GraphicsKeyFrameItem *>(pSelectedGfxItem);
		std::tuple<EntityTreeItemData *, int, QString> tupleKey = pSelectedGfxKeyFrameItem->GetKey();
		QString sCategory = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[0];
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sProperty); // If Tween key frame found, this will be the 'TweenProperty'

		// Store the original/old key frame data, before 'nudge' takes place
		if(pSelectedGfxKeyFrameItem->IsTweenKeyFrame() == false)
		{
			m_Prop_SelectedDataMap[tupleKey] = m_DopeSheetSceneRef.GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
																					   std::get<GFXDATAKEY_FrameIndex>(tupleKey),
																					   sCategory,
																					   sProperty);
		}
		else
		{
			m_Tween_SelectedDataMap[tupleKey] = m_DopeSheetSceneRef.GetTweenJsonValues(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
																					   std::get<GFXDATAKEY_FrameIndex>(tupleKey),
																					   eTweenProp);
		}
		
		// Update 'tupleKey' to what it will be after the 'nudge' operation
		tupleKey = std::make_tuple(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
								   HyMath::Max(0, std::get<GFXDATAKEY_FrameIndex>(tupleKey) + m_iFrameOffset),
								   std::get<GFXDATAKEY_CategoryPropString>(tupleKey));

		// Check if this key frame will be overwritten by the 'nudge' operation
		if(m_DopeSheetSceneRef.ContainsKeyFrameProperty(tupleKey))
		{
			m_Prop_OverwrittenDataMap[tupleKey] = m_DopeSheetSceneRef.GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
																						  std::get<GFXDATAKEY_FrameIndex>(tupleKey),
																						  sCategory,
																						  sProperty);
		}
		if(m_DopeSheetSceneRef.ContainsKeyFrameTween(tupleKey))
		{
			m_Tween_OverwrittenDataMap[tupleKey] = m_DopeSheetSceneRef.GetTweenJsonValues(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
																						  std::get<GFXDATAKEY_FrameIndex>(tupleKey),
																						  eTweenProp);
		}
	}
}

/*virtual*/ EntityUndoCmd_NudgeSelectedKeyFrames::~EntityUndoCmd_NudgeSelectedKeyFrames()
{
}

/*virtual*/ void EntityUndoCmd_NudgeSelectedKeyFrames::redo() /*override*/
{
	// First sort the contents of 'm_Prop_SelectedDataMap' by frame index, ordered based on the direction of the nudge
	std::function<bool(const KeyFrameKey &, const KeyFrameKey &)> fpSortPredicate = 
	[this](const KeyFrameKey &lhs, const KeyFrameKey &rhs) -> bool
	{
		return (m_iFrameOffset > 0) ?
			   (std::get<GFXDATAKEY_FrameIndex>(lhs) < std::get<GFXDATAKEY_FrameIndex>(rhs)) :
			   (std::get<GFXDATAKEY_FrameIndex>(lhs) > std::get<GFXDATAKEY_FrameIndex>(rhs));
	};

	// Sort and nudge the 'property' key frames
	QList<KeyFrameKey> sortedKeyList = m_Prop_SelectedDataMap.keys();
	std::sort(sortedKeyList.begin(), sortedKeyList.end(), fpSortPredicate);
	for(KeyFrameKey tupleKey : sortedKeyList)
	{
		QString sCategory = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[0];
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];

		m_DopeSheetSceneRef.NudgeKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												  std::get<GFXDATAKEY_FrameIndex>(tupleKey),
												  sCategory,
												  sProperty,
												  m_iFrameOffset,
												  false);
	}

	// Sort and nudge the 'tween' key frames
	sortedKeyList = m_Tween_SelectedDataMap.keys();
	std::sort(sortedKeyList.begin(), sortedKeyList.end(), fpSortPredicate);
	for(KeyFrameKey tupleKey : sortedKeyList)
	{
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sProperty);

		m_DopeSheetSceneRef.NudgeKeyFrameTween(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
											   std::get<GFXDATAKEY_FrameIndex>(tupleKey),
											   eTweenProp,
											   m_iFrameOffset,
											   false);
	}
	
	m_DopeSheetSceneRef.RefreshAllGfxItems();
}

/*virtual*/ void EntityUndoCmd_NudgeSelectedKeyFrames::undo() /*override*/
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// First remove the key frames that were nudged
	for(KeyFrameKey tupleKey : m_Prop_SelectedDataMap.keys())
	{
		QString sCategory = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[0];
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		m_DopeSheetSceneRef.RemoveKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												   HyMath::Max(0, std::get<GFXDATAKEY_FrameIndex>(tupleKey) + m_iFrameOffset),
												   sCategory,
												   sProperty,
												   false);
	}
	for(KeyFrameKey tupleKey : m_Tween_SelectedDataMap.keys())
	{
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sProperty);

		m_DopeSheetSceneRef.RemoveKeyFrameTween(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												HyMath::Max(0, std::get<GFXDATAKEY_FrameIndex>(tupleKey) + m_iFrameOffset),
												eTweenProp,
												false);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Then restore the original data
	for(KeyFrameKey tupleKey : m_Prop_SelectedDataMap.keys())
	{
		QString sCategory = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[0];
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		m_DopeSheetSceneRef.SetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												std::get<GFXDATAKEY_FrameIndex>(tupleKey),
												sCategory,
												sProperty,
												m_Prop_SelectedDataMap[tupleKey],
												false);
	}
	for(KeyFrameKey tupleKey : m_Tween_SelectedDataMap.keys())
	{
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sProperty);

		m_DopeSheetSceneRef.SetKeyFrameTween(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
											 std::get<GFXDATAKEY_FrameIndex>(tupleKey),
											 eTweenProp,
											 m_Tween_SelectedDataMap[tupleKey],
											 false);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// As well as the data that was overwritten
	for(KeyFrameKey tupleKey : m_Prop_OverwrittenDataMap.keys())
	{
		QString sCategory = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[0];
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		m_DopeSheetSceneRef.SetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												std::get<GFXDATAKEY_FrameIndex>(tupleKey),
												sCategory,
												sProperty,
												m_Prop_OverwrittenDataMap[tupleKey],
												false);
	}
	for(KeyFrameKey tupleKey : m_Tween_OverwrittenDataMap.keys())
	{
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sProperty);
		m_DopeSheetSceneRef.SetKeyFrameTween(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
											 std::get<GFXDATAKEY_FrameIndex>(tupleKey),
											 eTweenProp,
											 m_Tween_OverwrittenDataMap[tupleKey],
											 false);
	}

	m_DopeSheetSceneRef.RefreshAllGfxItems();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_NudgeTweenDuration::EntityUndoCmd_NudgeTweenDuration(EntityDopeSheetScene &entityDopeSheetSceneRef, EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, double dNewDuration, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_pItemData(pItemData),
	m_iFrameIndex(iFrameIndex),
	m_eTweenProp(eTweenProp),
	m_dNewDuration(dNewDuration)
{
	setText("Adjust " % m_pItemData->GetCodeName() % "'s Tween " % HyGlobal::TweenPropName(m_eTweenProp) % " Duration");
	m_dOldDuration = m_DopeSheetSceneRef.GetKeyFrameProperty(m_pItemData, m_iFrameIndex, "Tween " % HyGlobal::TweenPropName(m_eTweenProp), "Duration").toDouble();
};

/*virtual*/ EntityUndoCmd_NudgeTweenDuration::~EntityUndoCmd_NudgeTweenDuration()
{
}

/*virtual*/ void EntityUndoCmd_NudgeTweenDuration::redo() /*override*/
{
	m_DopeSheetSceneRef.SetKeyFrameProperty(m_pItemData, m_iFrameIndex, "Tween " % HyGlobal::TweenPropName(m_eTweenProp), "Duration", m_dNewDuration, true);
}

/*virtual*/ void EntityUndoCmd_NudgeTweenDuration::undo() /*override*/
{
	m_DopeSheetSceneRef.SetKeyFrameProperty(m_pItemData, m_iFrameIndex, "Tween " % HyGlobal::TweenPropName(m_eTweenProp), "Duration", m_dOldDuration, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_ConvertToTween::EntityUndoCmd_ConvertToTween(EntityDopeSheetScene &entityDopeSheetSceneRef, EntityTreeItemData *pItemData, TweenProperty eTweenProp, int iStartFrameIndex, int iEndFrameIndex, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_pItemData(pItemData),
	m_eTweenProp(eTweenProp),
	m_iStartFrameIndex(iStartFrameIndex),
	m_iEndFrameIndex(iEndFrameIndex)
{
	setText("Create " % HyGlobal::TweenPropName(m_eTweenProp) % " Tween");
	QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(m_eTweenProp);

	if(m_DopeSheetSceneRef.ContainsKeyFrameProperty(KeyFrameKey(m_pItemData, m_iEndFrameIndex, propPair.first % '/' % propPair.second)) == false)
	{
		HyGuiLog("EntityUndoCmd_ConvertToTween::EntityUndoCmd_ConvertToTween() - Destination key frame must be a valid (non-tween) property", LOGTYPE_Error);
		return;
	}

	m_DestinationValue = m_DopeSheetSceneRef.GetKeyFrameProperty(m_pItemData, m_iEndFrameIndex, propPair.first, propPair.second);
}

/*virtual*/ EntityUndoCmd_ConvertToTween::~EntityUndoCmd_ConvertToTween()
{
}

/*virtual*/ void EntityUndoCmd_ConvertToTween::redo() /*override*/
{
	QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(m_eTweenProp);
	m_DopeSheetSceneRef.RemoveKeyFrameProperty(m_pItemData, m_iEndFrameIndex, propPair.first, propPair.second, false);

	double dDuration = (m_iEndFrameIndex - m_iStartFrameIndex) * (1.0 / static_cast<EntityModel &>(m_DopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond());
	TweenJsonValues tweenValues(m_DestinationValue, QJsonValue(dDuration), QJsonValue(HyGlobal::TweenFuncName(TWEENFUNC_Linear)));
	m_DopeSheetSceneRef.SetKeyFrameTween(m_pItemData, m_iStartFrameIndex, m_eTweenProp, tweenValues, true);
}

/*virtual*/ void EntityUndoCmd_ConvertToTween::undo() /*override*/
{
	m_DopeSheetSceneRef.RemoveKeyFrameTween(m_pItemData, m_iStartFrameIndex, m_eTweenProp, false);

	QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(m_eTweenProp);
	m_DopeSheetSceneRef.SetKeyFrameProperty(m_pItemData, m_iEndFrameIndex, propPair.first, propPair.second, m_DestinationValue, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_SetCallback::EntityUndoCmd_SetCallback(EntityDopeSheetScene &entityDopeSheetSceneRef, QString sCallback, int iFrameIndex, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_sNewCallback(sCallback),
	m_iFrameIndex(iFrameIndex)
{
	m_sOldCallback = m_DopeSheetSceneRef.GetCallback(m_iFrameIndex);

	if(m_sNewCallback.isEmpty())
		setText("Remove " % m_sOldCallback % " Callback");
	else
	{
		if(m_sOldCallback.isEmpty())
			setText("Create " % m_sNewCallback % " Callback");
		else
			setText("Rename Callback To " % m_sNewCallback);
	}
}

/*virtual*/ EntityUndoCmd_SetCallback::~EntityUndoCmd_SetCallback()
{
}

/*virtual*/ void EntityUndoCmd_SetCallback::redo() /*override*/
{
	if(m_sNewCallback.isEmpty())
		m_DopeSheetSceneRef.RemoveCallback(m_iFrameIndex);
	else
	{
		if(m_sOldCallback.isEmpty())
			m_DopeSheetSceneRef.CreateCallback(m_iFrameIndex, m_sNewCallback);
		else
			m_DopeSheetSceneRef.RenameCallback(m_iFrameIndex, m_sNewCallback);
	}

	static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet))->UpdateWidgets();
}

/*virtual*/ void EntityUndoCmd_SetCallback::undo() /*override*/
{
	if(m_sNewCallback.isEmpty())
		m_DopeSheetSceneRef.CreateCallback(m_iFrameIndex, m_sOldCallback);
	else
	{
		if(m_sOldCallback.isEmpty())
			m_DopeSheetSceneRef.RemoveCallback(m_iFrameIndex);
		else
			m_DopeSheetSceneRef.RenameCallback(m_iFrameIndex, m_sOldCallback);
	}

	static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet))->UpdateWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_FramesPerSecond::EntityUndoCmd_FramesPerSecond(EntityModel &entityModelRef, int iNewFPS, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_EntityModelRef(entityModelRef),
	m_iNewFPS(iNewFPS)
{
	setText("Change Frames Per Second");
	m_iOldFPS = m_EntityModelRef.GetFramesPerSecond();
}

/*virtual*/ EntityUndoCmd_FramesPerSecond::~EntityUndoCmd_FramesPerSecond()
{
}

/*virtual*/ void EntityUndoCmd_FramesPerSecond::redo() /*override*/
{
	m_EntityModelRef.GetAuxWidgetsModel()->setData(m_EntityModelRef.GetAuxWidgetsModel()->index(0, AUXDOPEWIDGETSECTION_FramesPerSecond), m_iNewFPS, Qt::UserRole);
}

/*virtual*/ void EntityUndoCmd_FramesPerSecond::undo() /*override*/
{
	m_EntityModelRef.GetAuxWidgetsModel()->setData(m_EntityModelRef.GetAuxWidgetsModel()->index(0, AUXDOPEWIDGETSECTION_FramesPerSecond), m_iOldFPS, Qt::UserRole);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AutoInitialize::EntityUndoCmd_AutoInitialize(EntityModel &entityModelRef, bool bNewValue, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_EntityModelRef(entityModelRef),
	m_bNewValue(bNewValue)
{
	setText("Toggle Auto Initialize");
	m_bOldValue = m_EntityModelRef.IsAutoInitialize();
}

/*virtual*/ EntityUndoCmd_AutoInitialize::~EntityUndoCmd_AutoInitialize()
{
}

/*virtual*/ void EntityUndoCmd_AutoInitialize::redo() /*override*/
{
	m_EntityModelRef.GetAuxWidgetsModel()->setData(m_EntityModelRef.GetAuxWidgetsModel()->index(0, AUXDOPEWIDGETSECTION_AutoInitialize), m_bNewValue, Qt::UserRole);
}

/*virtual*/ void EntityUndoCmd_AutoInitialize::undo() /*override*/
{
	m_EntityModelRef.GetAuxWidgetsModel()->setData(m_EntityModelRef.GetAuxWidgetsModel()->index(0, AUXDOPEWIDGETSECTION_AutoInitialize), m_bOldValue, Qt::UserRole);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PropertyModified::EntityUndoCmd_PropertyModified(PropertiesTreeModel *pModel, const QModelIndex &index, const QVariant &newData, QUndoCommand *pParent /*= nullptr*/) :
	PropertiesUndoCmd(pModel, index, newData, pParent),
	m_iStateIndex(-1),
	m_iFrameIndex(-1)
{
	// Don't want base class (PropertiesUndoCmd) to invoke FocusWidgetState() before we set our models
	m_bDoFocusWidgetState = false;

	if(m_pModel->GetOwner().GetWidget() == nullptr)
	{
		HyGuiLog("EntityUndoCmd_PropertyModified - EntityWidget was nullptr", LOGTYPE_Error);
		return;
	}
	
	EntityDopeSheetScene &dopeSheetSceneRef = static_cast<EntityStateData *>(m_pModel->GetOwner().GetWidget()->GetCurStateData())->GetDopeSheetScene();
	EntityTreeItemData *pEntityTreeData = m_pModel->GetSubstate().value<EntityTreeItemData *>();

	m_iStateIndex = m_pModel->GetOwner().GetWidget()->GetCurStateIndex();
	m_iFrameIndex = dopeSheetSceneRef.GetCurrentFrame();

	// If unchecking this item, remember its value for when undo() is invoked
	if(m_ModelIndex.column() == PROPERTIESCOLUMN_Name && m_NewData.toBool() == false)
	{
		QString sCategory = m_pModel->GetCategoryName(m_ModelIndex);
		if(m_pModel->IsCategory(m_ModelIndex))
		{
			if(sCategory.startsWith("Tween "))
			{
				TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategory.mid(6));
				m_OldTweenData = dopeSheetSceneRef.GetTweenJsonValues(pEntityTreeData, m_iFrameIndex, eTweenProp);
			}
			else
				HyGuiLog("EntityUndoCmd_PropertyModified - Unhandled logic: Need generic container of old values", LOGTYPE_Error);
		}
		else
			m_OldPropertyValue = m_pModel->GetPropertyJsonValue(m_ModelIndex);
	}
}

/*virtual*/ EntityUndoCmd_PropertyModified::~EntityUndoCmd_PropertyModified()
{
}

/*virtual*/ void EntityUndoCmd_PropertyModified::redo() /*override*/
{
	// PropertiesUndoCmd::redo() will set m_pModel to the appropriate value, which can be queried with m_ModelIndex
	PropertiesUndoCmd::redo();
	UpdateEntityModel(true);

	m_pModel->GetOwner().FocusWidgetState(m_iStateIndex, m_pModel->GetSubstate());
}

/*virtual*/ void EntityUndoCmd_PropertyModified::undo() /*override*/
{
	// PropertiesUndoCmd::undo() will set m_pModel to the appropriate value, which can be queried with m_ModelIndex
	PropertiesUndoCmd::undo();
	UpdateEntityModel(false);

	m_pModel->GetOwner().FocusWidgetState(m_iStateIndex, m_pModel->GetSubstate());
}

void EntityUndoCmd_PropertyModified::UpdateEntityModel(bool bIsRedo)
{
	// Set/Remove Key Frame Property in DopeSheetScene
	EntityDopeSheetScene &dopeSheetSceneRef = static_cast<EntityStateData *>(m_pModel->GetOwner().GetModel()->GetStateData(m_iStateIndex))->GetDopeSheetScene();
	EntityTreeItemData *pEntityTreeData = m_pModel->GetSubstate().value<EntityTreeItemData *>();

	bool bRemoveProperties;
	if(bIsRedo)
		bRemoveProperties = (m_ModelIndex.column() == PROPERTIESCOLUMN_Name && m_NewData.toBool() == false);
	else
		bRemoveProperties = (m_ModelIndex.column() == PROPERTIESCOLUMN_Name && m_NewData.toBool());

	if(m_pModel->IsCategory(m_ModelIndex))
	{
		// Special Case: Tween Categories, when enabled need to write all its properties at once, since they are 'PROPERTIESACCESS_Mutable' (aka not toggle)
		QString sCategory = m_pModel->GetCategoryName(m_ModelIndex);
		if(sCategory.startsWith("Tween "))
		{
			TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategory.mid(6));
			if(bRemoveProperties)
			{
				dopeSheetSceneRef.RemoveKeyFrameTween(pEntityTreeData, m_iFrameIndex, eTweenProp, true);
			}
			else
			{
				if(m_OldTweenData.m_Destination.isUndefined() || m_OldTweenData.m_Destination.isNull())
				{
					QJsonValue destinationValue = m_pModel->FindPropertyJsonValue(sCategory, "Destination");
					QJsonValue durationValue = m_pModel->FindPropertyJsonValue(sCategory, "Duration");
					QJsonValue tweenTypeValue = m_pModel->FindPropertyJsonValue(sCategory, "Tween Type");
					dopeSheetSceneRef.SetKeyFrameTween(pEntityTreeData, m_iFrameIndex, eTweenProp, TweenJsonValues(destinationValue, durationValue, tweenTypeValue), true);
				}
				else
					dopeSheetSceneRef.SetKeyFrameTween(pEntityTreeData, m_iFrameIndex, eTweenProp, m_OldTweenData, true);
			}
		}
	}
	else // Modifying a property
	{
		QString sCategory = m_pModel->GetCategoryName(m_ModelIndex);
		QString sProperty = m_pModel->GetPropertyName(m_ModelIndex);

		if(bRemoveProperties)
			dopeSheetSceneRef.RemoveKeyFrameProperty(pEntityTreeData, m_iFrameIndex, sCategory, sProperty, true);
		else
		{
			if(bIsRedo == false && m_OldPropertyValue.isUndefined() == false && m_OldPropertyValue.isNull() == false)
				dopeSheetSceneRef.SetKeyFrameProperty(pEntityTreeData, m_iFrameIndex, sCategory, sProperty, m_OldPropertyValue, true);
			else
				dopeSheetSceneRef.SetKeyFrameProperty(pEntityTreeData, m_iFrameIndex, sCategory, sProperty, m_pModel->GetPropertyJsonValue(m_ModelIndex), true);
		}
	}
}
