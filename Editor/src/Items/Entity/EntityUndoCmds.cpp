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
#include "GfxShapeModel.h"

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

EntityUndoCmd_AddWidget::EntityUndoCmd_AddWidget(ProjectItemData &entityItemRef, ItemType eWidgetType, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_eWidgetType(eWidgetType),
	m_pWidgetTreeItemData(nullptr)
{
	setText("Add New " % HyGlobal::ItemName(m_eWidgetType, false) % " Widget");
}

/*virtual*/ EntityUndoCmd_AddWidget::~EntityUndoCmd_AddWidget()
{
}

/*virtual*/ void EntityUndoCmd_AddWidget::redo() /*override*/
{
	if(m_pWidgetTreeItemData == nullptr)
		m_pWidgetTreeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewWidget(m_eWidgetType, -1);
	else
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pWidgetTreeItemData, -1);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << m_pWidgetTreeItemData->GetThisUuid());
}

/*virtual*/ void EntityUndoCmd_AddWidget::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pWidgetTreeItemData);
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
	EntityTreeModel &entTreeModelRef = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel();

	for(int i = 0; i < m_SelectedItemDataList.size(); ++i)
	{
		TreeModelItemData *pDestinationParent = nullptr;
		if(m_SelectedItemDataList[i]->GetEntType() == ENTTYPE_ArrayItem)
			pDestinationParent = entTreeModelRef.GetArrayFolderTreeItemData(m_SelectedItemDataList[i]);
		else
		{
			if(m_SelectedItemDataList[i]->IsFixtureItem())
				pDestinationParent = entTreeModelRef.GetFixtureFolderTreeItemData();
			else
				pDestinationParent = entTreeModelRef.GetRootTreeItemData();
		}

		entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], pDestinationParent, m_NewItemIndexList[i]);
	}
	
	static_cast<EntityStateData *>(static_cast<EntityWidget *>(m_EntityItemRef.GetWidget())->GetCurStateData())->GetDopeSheetScene().RefreshAllGfxItems();
}

/*virtual*/ void EntityUndoCmd_OrderChildren::undo() /*override*/
{
	EntityTreeModel &entTreeModelRef = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel();

	for(int i = m_SelectedItemDataList.size() - 1; i >= 0; --i)
	{
		TreeModelItemData *pDestinationParent = nullptr;
		if(m_SelectedItemDataList[i]->GetEntType() == ENTTYPE_ArrayItem)
			pDestinationParent = entTreeModelRef.GetArrayFolderTreeItemData(m_SelectedItemDataList[i]);
		else
		{
			if(m_SelectedItemDataList[i]->IsFixtureItem())
				pDestinationParent = entTreeModelRef.GetFixtureFolderTreeItemData();
			else
				pDestinationParent = entTreeModelRef.GetRootTreeItemData();
		}

		entTreeModelRef.MoveTreeItem(m_SelectedItemDataList[i], pDestinationParent, m_PrevItemIndexList[i]);
	}

	static_cast<EntityStateData *>(static_cast<EntityWidget *>(m_EntityItemRef.GetWidget())->GetCurStateData())->GetDopeSheetScene().RefreshAllGfxItems();
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
	m_OldShapeDataArrayList.clear();
	m_CreatedKeyFrameList.clear();
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		if(m_AffectedItemDataList[i]->IsFixtureItem() == false)
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
				QJsonValue overwrittenValue = pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Position", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_vec2, tmpVariant), false);
				bCreatedTranslationKeyFrame = (overwrittenValue.isUndefined() || overwrittenValue.isNull());
			}
			if(dNewRotation != dOldRotation)
			{
				QVariant tmpVariant = dNewRotation;
				QJsonValue overwrittenValue = pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Rotation", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_double, tmpVariant), false);
				bCreatedRotationKeyFrame = (overwrittenValue.isUndefined() || overwrittenValue.isNull());
			}
			if(vNewScale != vOldScale)
			{
				QVariant tmpVariant = QPointF(vNewScale.x, vNewScale.y);
				QJsonValue overwrittenValue = pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Transformation", "Scale", PropertiesTreeModel::ConvertVariantToJson(PROPERTIESTYPE_vec2, tmpVariant), false);
				bCreatedScaleKeyFrame = (overwrittenValue.isUndefined() || overwrittenValue.isNull());
			}

			m_CreatedKeyFrameList.push_back(std::make_tuple(bCreatedTranslationKeyFrame, bCreatedRotationKeyFrame, bCreatedScaleKeyFrame));
		}
		//else // ITEM_FixtureShape or ITEM_FixtureChain
		//{
		//	QString sShapeType = PropertiesTreeModel::ConvertJsonToVariant(PROPERTIESTYPE_ComboBoxString, pStateData->GetDopeSheetScene().GetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Type")).toString();
		//	QJsonArray oldShapeDataArray = PropertiesTreeModel::ConvertJsonToVariant(PROPERTIESTYPE_FloatArray, pStateData->GetDopeSheetScene().GetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Data")).toJsonArray();
		//	m_OldShapeDataArrayList.append(oldShapeDataArray);

		//	QList<float> floatList;
		//	for(QJsonValue floatVal : oldShapeDataArray)
		//		floatList.push_back(static_cast<float>(floatVal.toDouble()));
		//	GfxShapeModel tmpShapeModel(HyColor::Black, HyGlobal::GetShapeFromString(sShapeType), floatList);
		//	tmpShapeModel.TransformData(m_NewTransformList[i]);

		//	QList<float> transformedFloatList = tmpShapeModel.GetData();
		//	QJsonArray transformedFloatArray;
		//	for(float fVal : transformedFloatList)
		//		transformedFloatArray.append(fVal);
		//	
		//	pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Data", transformedFloatArray, false);
		//}

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
		if(m_AffectedItemDataList[i]->IsFixtureItem() == false)
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

			m_OldShapeDataArrayList.push_back(QJsonArray()); // Blank to keep indices aligned
		}
		else
			pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_AffectedItemDataList[i], m_iFrameIndex, "Shape", "Data", m_OldShapeDataArrayList[i], false);

		affectedItemUuidList << m_AffectedItemDataList[i]->GetThisUuid();
	}

	pStateData->GetDopeSheetScene().RefreshAllGfxItems();

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(affectedItemUuidList);

	m_EntityItemRef.FocusWidgetState(m_iStateIndex, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AddPrimitive::EntityUndoCmd_AddPrimitive(ProjectItemData &entityItemRef, int32 iRowIndex /*= -1*/, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_iIndex(iRowIndex),
	m_pPrimitiveTreeItemData(nullptr)
{
	setText("Add New Primitive");
}

/*virtual*/ EntityUndoCmd_AddPrimitive::~EntityUndoCmd_AddPrimitive()
{
}

/*virtual*/ void EntityUndoCmd_AddPrimitive::redo() /*override*/
{
	if(m_pPrimitiveTreeItemData == nullptr)
		m_pPrimitiveTreeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewPrimitive(m_iIndex);
	else
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pPrimitiveTreeItemData, m_iIndex);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	pWidget->SetEditMode(m_pPrimitiveTreeItemData);
}

/*virtual*/ void EntityUndoCmd_AddPrimitive::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pPrimitiveTreeItemData);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	pWidget->SetEditMode(nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AddFixture::EntityUndoCmd_AddFixture(ProjectItemData &entityItemRef, bool bIsShape, int32 iRowIndex /*= -1*/, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_bIsShape(bIsShape),
	m_iIndex(iRowIndex),
	m_pShapeTreeItemData(nullptr)
{
	if(m_bIsShape)
		setText("Add New Shape Fixture");
	else
		setText("Add New Chain Fixture");
}

/*virtual*/ EntityUndoCmd_AddFixture::~EntityUndoCmd_AddFixture()
{
}

/*virtual*/ void EntityUndoCmd_AddFixture::redo() /*override*/
{
	if(m_pShapeTreeItemData == nullptr)
		m_pShapeTreeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewFixture(m_bIsShape, m_iIndex);
	else
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pShapeTreeItemData, m_iIndex);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	pWidget->SetEditMode(m_pShapeTreeItemData);
}

/*virtual*/ void EntityUndoCmd_AddFixture::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pShapeTreeItemData);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	pWidget->SetEditMode(nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_EditModelData::EntityUndoCmd_EditModelData(QString sText, ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, EntityTreeItemData *pEntityItemData, QString sCategoryName, QString sPropName, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_iStateIndex(iStateIndex),
	m_iFrameIndex(iFrameIndex),
	m_pEntityItemData(pEntityItemData),
	m_sCategoryName(sCategoryName),
	m_sPropName(sPropName)
{
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));
	
	KeyFrameKey shapeDataKey = std::make_tuple(m_pEntityItemData, m_iFrameIndex, m_sCategoryName % "/" % m_sPropName);
	m_bHadOldData = pStateData->GetDopeSheetScene().ContainsKeyFrameProperty(shapeDataKey);
	if(m_bHadOldData)
	{
		QJsonArray dataArray = pStateData->GetDopeSheetScene().GetKeyFrameProperty(m_pEntityItemData, m_iFrameIndex, m_sCategoryName, m_sPropName).toArray();
		for(QJsonValue val : dataArray)
			m_OldData.append(static_cast<float>(val.toDouble()));
	}

	m_NewData = m_pEntityItemData->GetEditModel()->GetActionSerialized();
	setText(sText);
}

/*virtual*/ EntityUndoCmd_EditModelData::~EntityUndoCmd_EditModelData()
{
}

/*virtual*/ void EntityUndoCmd_EditModelData::redo() /*override*/
{
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));

	QJsonArray newDataArray;
	for(float f : m_NewData)
		newDataArray.append(f);
	pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_pEntityItemData, m_iFrameIndex, m_sCategoryName, m_sPropName, newDataArray, true);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget == nullptr)
	{
		HyGuiLog("EntityUndoCmd_EditModelData::redo() - pWidget is nullptr", LOGTYPE_Error);
		return;
	}
	pWidget->RequestSelectedItems(QList<QUuid>() << m_pEntityItemData->GetThisUuid());
	pWidget->SetEditMode(m_pEntityItemData);
}

/*virtual*/ void EntityUndoCmd_EditModelData::undo() /*override*/
{
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityItemRef.GetModel()->GetStateData(m_iStateIndex));

	QJsonArray oldDataArray;
	for(float f : m_OldData)
		oldDataArray.append(f);
	pStateData->GetDopeSheetScene().SetKeyFrameProperty(m_pEntityItemData, m_iFrameIndex, m_sCategoryName, m_sPropName, oldDataArray, true);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
	if(pWidget == nullptr)
	{
		HyGuiLog("EntityUndoCmd_EditModelData::undo() - pWidget is nullptr", LOGTYPE_Error);
		return;
	}
	pWidget->RequestSelectedItems(QList<QUuid>() << m_pEntityItemData->GetThisUuid());
	pWidget->SetEditMode(m_pEntityItemData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//EntityUndoCmd_ConvertShape::EntityUndoCmd_ConvertShape(ProjectItemData &entityItemRef, EntityTreeItemData *pShapeItemData, QUndoCommand *pParent /*= nullptr*/) :
//	m_EntityItemRef(entityItemRef),
//	m_pNewShapeItemData(nullptr),
//	m_pPrevShapeItemData(pShapeItemData),
//	m_iPoppedIndex(-1)
//{
//	if(m_pPrevShapeItemData->IsFixtureItem())
//		setText("Convert Fixture to Primitive");
//	else if(m_pPrevShapeItemData->GetType() == ITEM_Primitive)
//		setText("Convert Primitive to Fixture");
//	else
//		HyGuiLog("EntityUndoCmd_ConvertShape::EntityUndoCmd_ConvertShape() - Wrong type", LOGTYPE_Error);
//}
//
///*virtual*/ EntityUndoCmd_ConvertShape::~EntityUndoCmd_ConvertShape()
//{
//}
//
///*virtual*/ void EntityUndoCmd_ConvertShape::redo() /*override*/
//{
//	m_iPoppedIndex = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pPrevShapeItemData);
//
//	if(m_pNewShapeItemData == nullptr)
//	{
//		QJsonObject descObj;
//		m_pPrevShapeItemData->InsertJsonInfo_Desc(descObj);
//
//		if(m_pPrevShapeItemData->IsFixtureItem())
//			descObj["Type"] = HyGlobal::ItemName(ITEM_Primitive, false);
//		else
//			descObj["Type"] = HyGlobal::ItemName(ITEM_FixtureShape, false);
//
//		m_pNewShapeItemData = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddExistingItem(descObj, m_pPrevShapeItemData->GetEntType() == ENTTYPE_ArrayItem, -1);
//	}
//	else
//		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pNewShapeItemData, -1);
//
//	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
//	if(pWidget)
//		pWidget->RequestSelectedItems(QList<QUuid>() << m_pNewShapeItemData->GetThisUuid());
//}
//
///*virtual*/ void EntityUndoCmd_ConvertShape::undo() /*override*/
//{
//	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pNewShapeItemData);
//	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pPrevShapeItemData, m_iPoppedIndex);
//
//	EntityWidget *pWidget = static_cast<EntityWidget *>(m_EntityItemRef.GetWidget());
//	if(pWidget)
//		pWidget->RequestSelectedItems(QList<QUuid>() << m_pPrevShapeItemData->GetThisUuid());
//}

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
	if(m_ArrayItemList.empty())
		m_ArrayItemList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_CreateNewArray(m_PackItemList, m_sArrayName, m_iArrayFolderRow);
	else
	{
		for(int i = 0; i < m_ArrayItemList.size(); ++i)
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_ArrayItemList[i], (i == 0) ? m_iArrayFolderRow : -1);
	}

	// NOTE: m_PackItemList has been sorted to be in row descending order
	m_PoppedIndexList.clear();
	for(EntityTreeItemData *pItem : m_PackItemList)
		m_PoppedIndexList.append(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pItem));

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

EntityUndoCmd_ReplaceItems::EntityUndoCmd_ReplaceItems(ProjectItemData &projItemRef, QList<EntityTreeItemData *> replaceItemList, QUndoCommand *pParent /*= nullptr*/) :
	m_ProjItemRef(projItemRef),
	m_ReplaceItemList(replaceItemList)
{
	setText("Replace items");

	for(EntityTreeItemData *pItem : m_ReplaceItemList)
		m_OriginalItemUuidList.push_back(pItem->GetReferencedItemUuid());
}

/*virtual*/ EntityUndoCmd_ReplaceItems::~EntityUndoCmd_ReplaceItems()
{
}

/*virtual*/ void EntityUndoCmd_ReplaceItems::redo() /*override*/
{
	QList<QUuid> replaceItemUuidList;
	for(EntityTreeItemData *pItem : m_ReplaceItemList)
	{
		pItem->SetReferencedItemUuid(m_ProjItemRef.GetUuid());
		pItem->SetReallocateDrawItem(true);
	}
}

/*virtual*/ void EntityUndoCmd_ReplaceItems::undo() /*override*/
{
	for(int i = 0; i < m_ReplaceItemList.size(); ++i)
	{
		m_ReplaceItemList[i]->SetReferencedItemUuid(m_OriginalItemUuidList[i]);
		m_ReplaceItemList[i]->SetReallocateDrawItem(true);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PasteKeyFrames::EntityUndoCmd_PasteKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, QList<QPair<EntityTreeItemData *, QJsonArray>> pasteKeyFramesPairList, int iStartFrameIndex, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_PasteKeyFramesPairList(pasteKeyFramesPairList),
	m_iStartFrameIndex(iStartFrameIndex)
{
	if(m_PasteKeyFramesPairList.empty())
		HyGuiLog("EntityUndoCmd_PasteKeyFrames::EntityUndoCmd_PasteKeyFrames m_PasteKeyFramesPairList is empty", LOGTYPE_Error);

	if(m_iStartFrameIndex < 0)
		setText("Paste Key Frames at constructor");
	else
		setText("Paste Key Frames at frame " % QString::number(m_iStartFrameIndex));
}

/*virtual*/ EntityUndoCmd_PasteKeyFrames::~EntityUndoCmd_PasteKeyFrames()
{
}

/*virtual*/ void EntityUndoCmd_PasteKeyFrames::redo() /*override*/
{
	m_PoppedKeyFramesPairList = m_DopeSheetSceneRef.PasteSerializedKeyFrames(m_PasteKeyFramesPairList, m_iStartFrameIndex);
}

/*virtual*/ void EntityUndoCmd_PasteKeyFrames::undo() /*override*/
{
	m_DopeSheetSceneRef.UnpasteSerializedKeyFrames(m_PasteKeyFramesPairList, m_PoppedKeyFramesPairList, m_iStartFrameIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PopKeyFrames::EntityUndoCmd_PopKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, const QJsonObject &KeyFrameObj, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_KeyFramesObject(KeyFrameObj)
{
	setText("Delete Key Frames");
}

/*virtual*/ EntityUndoCmd_PopKeyFrames::~EntityUndoCmd_PopKeyFrames()
{
}

/*virtual*/ void EntityUndoCmd_PopKeyFrames::redo() /*override*/
{
	m_DopeSheetSceneRef.RemoveSerializedKeyFrames(m_KeyFramesObject);
}

/*virtual*/ void EntityUndoCmd_PopKeyFrames::undo() /*override*/
{
	m_DopeSheetSceneRef.InsertSerializedKeyFrames(m_KeyFramesObject);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_NudgeSelectedKeyFrames::EntityUndoCmd_NudgeSelectedKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameOffset, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_iFrameOffset(iFrameOffset)
{
	QList<QGraphicsItem *> selectedItemsList = m_DopeSheetSceneRef.selectedItems();
	for(QGraphicsItem *pSelectedGfxItem : selectedItemsList)
	{
		bool bAcquiredDataType = false;
		DopeSheetGfxItemType eItemType = static_cast<DopeSheetGfxItemType>(pSelectedGfxItem->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
		if(bAcquiredDataType == false)
			continue;

		if(eItemType == GFXITEM_TweenKnob)
		{
			GraphicsTweenKnobItem *pTweenKnobItem = static_cast<GraphicsTweenKnobItem *>(pSelectedGfxItem);
			KeyFrameKey tweenKnobKey = pTweenKnobItem->GetKey();

			// Determine if only the tween knob is selected (ignore this knob selection if the corresponding tween keyframe is also selected)
			bool bOnlyTweenKnobSelected = true;
			for(QGraphicsItem *pCheckingGfxItem : selectedItemsList)
			{
				DopeSheetGfxItemType eCheckingItemType = static_cast<DopeSheetGfxItemType>(pCheckingGfxItem->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
				if(pCheckingGfxItem == pTweenKnobItem || bAcquiredDataType == false || eCheckingItemType != GFXITEM_TweenKeyFrame)
					continue;

				if(tweenKnobKey == static_cast<GraphicsKeyFrameItem *>(pCheckingGfxItem)->GetKey())
				{
					bOnlyTweenKnobSelected = false;
					break;
				}
			}

			if(bOnlyTweenKnobSelected)
			{
				QString sCategoryProp = std::get<GFXDATAKEY_CategoryPropString>(tweenKnobKey);
				TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategoryProp.split('/')[1]);

				double dOldDuration = m_DopeSheetSceneRef.GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tweenKnobKey),
																			  std::get<GFXDATAKEY_FrameIndex>(tweenKnobKey),
																			  "Tween " % HyGlobal::TweenPropName(eTweenProp),
																			  "Duration").toDouble();

				// Store the original/old duration, before 'nudge' takes place
				m_TweenKnobs_SelectedDataMap.insert(pTweenKnobItem->GetKey(), dOldDuration);
			}
		}
		else // GFXITEM_PropertyKeyFrame or GFXITEM_TweenKeyFrame
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
									   HyMath::Max(-1, std::get<GFXDATAKEY_FrameIndex>(tupleKey) + m_iFrameOffset),
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

	// Sort and nudge the tween knobs
	sortedKeyList = m_TweenKnobs_SelectedDataMap.keys();
	std::sort(sortedKeyList.begin(), sortedKeyList.end(), fpSortPredicate);
	for(KeyFrameKey tupleKey : sortedKeyList)
	{
		QString sCategoryProp = std::get<GFXDATAKEY_CategoryPropString>(tupleKey);
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategoryProp.split('/')[1]);

		int iFramesPerSec = static_cast<EntityModel &>(m_DopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond();
		int iTweenStartKeyFrame = std::get<GFXDATAKEY_FrameIndex>(tupleKey);

		GraphicsKeyFrameItem *pGfxTweenFrame = m_DopeSheetSceneRef.FindTweenKeyFrameItem(tupleKey);
		if(pGfxTweenFrame == nullptr)
		{
			HyGuiLog("EntityUndoCmd_NudgeSelectedKeyFrames::redo() - pGfxTweenFrame is nullptr", LOGTYPE_Error);
			continue;
		}
		int iTweenEndKeyFrame = pGfxTweenFrame->GetTweenFramesDuration() + iTweenStartKeyFrame + m_iFrameOffset;
		iTweenEndKeyFrame = HyMath::Max(iTweenStartKeyFrame, iTweenEndKeyFrame);

		double dNewDuration = (iTweenEndKeyFrame - iTweenStartKeyFrame) * (1.0 / iFramesPerSec);
		m_DopeSheetSceneRef.SetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												std::get<GFXDATAKEY_FrameIndex>(tupleKey),
												"Tween " % HyGlobal::TweenPropName(eTweenProp),
												"Duration",
												dNewDuration,
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
												   HyMath::Max(-1, std::get<GFXDATAKEY_FrameIndex>(tupleKey) + m_iFrameOffset),
												   sCategory,
												   sProperty,
												   false);
	}
	for(KeyFrameKey tupleKey : m_Tween_SelectedDataMap.keys())
	{
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sProperty);

		m_DopeSheetSceneRef.RemoveKeyFrameTween(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												HyMath::Max(-1, std::get<GFXDATAKEY_FrameIndex>(tupleKey) + m_iFrameOffset),
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
	for(KeyFrameKey tupleKey : m_TweenKnobs_SelectedDataMap.keys())
	{
		QString sCategoryProp = std::get<GFXDATAKEY_CategoryPropString>(tupleKey);
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategoryProp.split('/')[1]);
		m_DopeSheetSceneRef.SetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey),
												std::get<GFXDATAKEY_FrameIndex>(tupleKey),
												"Tween " % HyGlobal::TweenPropName(eTweenProp),
												"Duration",
												m_TweenKnobs_SelectedDataMap[tupleKey],
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

EntityUndoCmd_ConvertToTween::EntityUndoCmd_ConvertToTween(EntityDopeSheetScene &entityDopeSheetSceneRef, QList<ContextTweenData> contextTweenDataList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_ContextTweenDataList(contextTweenDataList)
{
	bool bMultiPropertyType = std::any_of(m_ContextTweenDataList.begin(), m_ContextTweenDataList.end(), [this](const ContextTweenData &contextTweenData) { return contextTweenData.m_eTweenProperty != m_ContextTweenDataList[0].m_eTweenProperty; });

	if(bMultiPropertyType)
		setText("Convert to Multiple Tweens");
	else if(contextTweenDataList.size() == 1)
		setText("Convert to " % HyGlobal::TweenPropName(contextTweenDataList[0].m_eTweenProperty) % " Tween");
	else
		setText("Convert to Multiple " % HyGlobal::TweenPropName(contextTweenDataList[0].m_eTweenProperty) % " Tweens");
		
	//// Populate m_DestinationValuesList to be used with undo
	//for(const ContextTweenData &contextTweenData : m_ContextTweenDataList)
	//{
	//	QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(contextTweenData.m_eTweenProperty);
	//	if(m_DopeSheetSceneRef.ContainsKeyFrameProperty(KeyFrameKey(contextTweenData.m_pTreeItemData, contextTweenData.m_iEndFrame, propPair.first % '/' % propPair.second)) == false)
	//		HyGuiLog("EntityUndoCmd_ConvertToTween::EntityUndoCmd_ConvertToTween() - Destination key frame must be a valid (non-tween) property", LOGTYPE_Error);
	//	else
	//		m_DestinationValuesList.push_back(m_DopeSheetSceneRef.GetKeyFrameProperty(contextTweenData.m_pTreeItemData, contextTweenData.m_iEndFrame, propPair.first, propPair.second));
	//}
}

/*virtual*/ EntityUndoCmd_ConvertToTween::~EntityUndoCmd_ConvertToTween()
{
}

/*virtual*/ void EntityUndoCmd_ConvertToTween::redo() /*override*/
{
	for(int i = 0; i < m_ContextTweenDataList.size(); ++i)
	{
		const ContextTweenData &contextTweenData = m_ContextTweenDataList[i];

		QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(contextTweenData.m_eTweenProperty);
		m_DopeSheetSceneRef.RemoveKeyFrameProperty(contextTweenData.m_pTreeItemData, contextTweenData.m_iEndFrame, propPair.first, propPair.second, false);

		double dDuration = (contextTweenData.m_iEndFrame - contextTweenData.m_iStartFrame) * (1.0 / static_cast<EntityModel &>(m_DopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond());
		TweenJsonValues tweenValues(contextTweenData.m_EndValue, QJsonValue(dDuration), QJsonValue(HyGlobal::TweenFuncName(TWEENFUNC_Linear)));
		m_DopeSheetSceneRef.SetKeyFrameTween(contextTweenData.m_pTreeItemData, contextTweenData.m_iStartFrame, contextTweenData.m_eTweenProperty, tweenValues, i == (m_ContextTweenDataList.size() - 1));
	}
}

/*virtual*/ void EntityUndoCmd_ConvertToTween::undo() /*override*/
{
	for(int i = 0; i < m_ContextTweenDataList.size(); ++i)
	{
		const ContextTweenData &contextTweenData = m_ContextTweenDataList[i];

		m_DopeSheetSceneRef.RemoveKeyFrameTween(contextTweenData.m_pTreeItemData, contextTweenData.m_iStartFrame, contextTweenData.m_eTweenProperty, false);
		
		QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(contextTweenData.m_eTweenProperty);
		m_DopeSheetSceneRef.SetKeyFrameProperty(contextTweenData.m_pTreeItemData, contextTweenData.m_iEndFrame, propPair.first, propPair.second, contextTweenData.m_EndValue, i == (m_ContextTweenDataList.size() - 1));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_BreakTween::EntityUndoCmd_BreakTween(EntityDopeSheetScene &entityDopeSheetSceneRef, QList<ContextTweenData> breakTweenDataList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_BreakTweenDataList(breakTweenDataList)
{
	if(m_BreakTweenDataList.size() == 1)
		setText("Break Tween");
	else
		setText("Break Tweens");

	// Store the original tween types in m_TweenFuncValueList to be used with undo
	for(const ContextTweenData &contextTweenData : m_BreakTweenDataList)
		m_TweenFuncValueList.push_back(m_DopeSheetSceneRef.GetTweenJsonValues(contextTweenData.m_pTreeItemData, contextTweenData.m_iStartFrame, contextTweenData.m_eTweenProperty).m_TweenFuncType);
}

/*virtual*/ EntityUndoCmd_BreakTween::~EntityUndoCmd_BreakTween()
{
}

/*virtual*/ void EntityUndoCmd_BreakTween::redo() /*override*/
{
	for(int i = 0; i < m_BreakTweenDataList.size(); ++i)
	{
		const ContextTweenData &contextTweenData = m_BreakTweenDataList[i];
		m_DopeSheetSceneRef.RemoveKeyFrameTween(contextTweenData.m_pTreeItemData, contextTweenData.m_iStartFrame, contextTweenData.m_eTweenProperty, false);
		
		QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(contextTweenData.m_eTweenProperty);
		if(contextTweenData.m_StartValue.isUndefined() == false && contextTweenData.m_StartValue.isNull() == false)
			m_DopeSheetSceneRef.SetKeyFrameProperty(contextTweenData.m_pTreeItemData, contextTweenData.m_iStartFrame, propPair.first, propPair.second, contextTweenData.m_StartValue, false);
		m_DopeSheetSceneRef.SetKeyFrameProperty(contextTweenData.m_pTreeItemData, contextTweenData.m_iEndFrame, propPair.first, propPair.second, contextTweenData.m_EndValue, i == (m_BreakTweenDataList.size() - 1));
	}
}

/*virtual*/ void EntityUndoCmd_BreakTween::undo() /*override*/
{
	for(int i = 0; i < m_BreakTweenDataList.size(); ++i)
	{
		const ContextTweenData &contextTweenData = m_BreakTweenDataList[i];
		
		QPair<QString, QString> propPair = HyGlobal::ConvertTweenPropToRegularPropPair(contextTweenData.m_eTweenProperty);
		m_DopeSheetSceneRef.RemoveKeyFrameProperty(contextTweenData.m_pTreeItemData, contextTweenData.m_iStartFrame, propPair.first, propPair.second, false);
		m_DopeSheetSceneRef.RemoveKeyFrameProperty(contextTweenData.m_pTreeItemData, contextTweenData.m_iEndFrame, propPair.first, propPair.second, false);

		double dDuration = (contextTweenData.m_iEndFrame - contextTweenData.m_iStartFrame) * (1.0 / static_cast<EntityModel &>(m_DopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond());
		TweenJsonValues tweenValues(contextTweenData.m_EndValue, QJsonValue(dDuration), m_TweenFuncValueList[i]);
		m_DopeSheetSceneRef.SetKeyFrameTween(contextTweenData.m_pTreeItemData, contextTweenData.m_iStartFrame, contextTweenData.m_eTweenProperty, tweenValues, i == (m_BreakTweenDataList.size() - 1));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_AddCallback::EntityUndoCmd_AddCallback(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameIndex, QString sCallback, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_iFrameIndex(iFrameIndex),
	m_sCallback(sCallback)
{
	if(m_sCallback.isEmpty())
		HyGuiLog("EntityUndoCmd_AddEvent::EntityUndoCmd_AddCallback() - sCallback name cannot be empty", LOGTYPE_Error);

	setText("Create " % m_sCallback % " callback");
}

/*virtual*/ EntityUndoCmd_AddCallback::~EntityUndoCmd_AddCallback()
{
}

/*virtual*/ void EntityUndoCmd_AddCallback::redo() /*override*/
{
	m_DopeSheetSceneRef.SetCallback(m_iFrameIndex, m_sCallback);
	static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet))->UpdateWidgets();
}

/*virtual*/ void EntityUndoCmd_AddCallback::undo() /*override*/
{
	m_DopeSheetSceneRef.RemoveCallback(m_iFrameIndex, m_sCallback);
	static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet))->UpdateWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_RemoveCallback::EntityUndoCmd_RemoveCallback(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameIndex, QString sCallback, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_iFrameIndex(iFrameIndex),
	m_sCallback(sCallback)
{
	setText("Remove " % m_sCallback % " Callback");
}

/*virtual*/ EntityUndoCmd_RemoveCallback::~EntityUndoCmd_RemoveCallback()
{
}

/*virtual*/ void EntityUndoCmd_RemoveCallback::redo() /*override*/
{
	m_DopeSheetSceneRef.RemoveCallback(m_iFrameIndex, m_sCallback);
	static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet))->UpdateWidgets();
}

/*virtual*/ void EntityUndoCmd_RemoveCallback::undo() /*override*/
{
	m_DopeSheetSceneRef.SetCallback(m_iFrameIndex, m_sCallback);
	static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet))->UpdateWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_RenameCallback::EntityUndoCmd_RenameCallback(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameIndex, QString sOldCallback, QString sNewCallback, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_DopeSheetSceneRef(entityDopeSheetSceneRef),
	m_iFrameIndex(iFrameIndex),
	m_sOldCallback(sOldCallback),
	m_sNewCallback(sNewCallback)
{
	if(m_sOldCallback.isEmpty() || m_sNewCallback.isEmpty())
		HyGuiLog("EntityUndoCmd_RenameCallback::EntityUndoCmd_RenameCallback() - Callback name cannot be empty", LOGTYPE_Error);

	setText("Rename " % m_sOldCallback % " Callback");
}

/*virtual*/ EntityUndoCmd_RenameCallback::~EntityUndoCmd_RenameCallback()
{
}

/*virtual*/ void EntityUndoCmd_RenameCallback::redo() /*override*/
{
	m_DopeSheetSceneRef.RenameCallback(m_iFrameIndex, m_sOldCallback, m_sNewCallback);
	static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet))->UpdateWidgets();
}

/*virtual*/ void EntityUndoCmd_RenameCallback::undo() /*override*/
{
	m_DopeSheetSceneRef.RenameCallback(m_iFrameIndex, m_sNewCallback, m_sOldCallback);
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

//EntityUndoCmd_AutoInitialize::EntityUndoCmd_AutoInitialize(EntityModel &entityModelRef, bool bNewValue, QUndoCommand *pParent /*= nullptr*/) :
//	QUndoCommand(pParent),
//	m_EntityModelRef(entityModelRef),
//	m_bNewValue(bNewValue)
//{
//	setText("Toggle Auto Initialize");
//	m_bOldValue = m_EntityModelRef.IsAutoInitialize();
//}
//
///*virtual*/ EntityUndoCmd_AutoInitialize::~EntityUndoCmd_AutoInitialize()
//{
//}
//
///*virtual*/ void EntityUndoCmd_AutoInitialize::redo() /*override*/
//{
//	m_EntityModelRef.GetAuxWidgetsModel()->setData(m_EntityModelRef.GetAuxWidgetsModel()->index(0, AUXDOPEWIDGETSECTION_AutoInitialize), m_bNewValue, Qt::UserRole);
//}
//
///*virtual*/ void EntityUndoCmd_AutoInitialize::undo() /*override*/
//{
//	m_EntityModelRef.GetAuxWidgetsModel()->setData(m_EntityModelRef.GetAuxWidgetsModel()->index(0, AUXDOPEWIDGETSECTION_AutoInitialize), m_bOldValue, Qt::UserRole);
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PropertyModified::EntityUndoCmd_PropertyModified(PropertiesTreeModel *pModel, const QModelIndex &index, const QVariant &newData, QUndoCommand *pParent /*= nullptr*/) :
	PropertiesUndoCmd(pModel, index, newData, pParent),
	m_ePropTypeModified(pModel->GetIndexDefinition(index).eType),
	m_iFrameIndex(-1)
{
	if(pModel->GetProjItem()->GetWidget() == nullptr)
	{
		HyGuiLog("EntityUndoCmd_PropertyModified - EntityWidget was nullptr", LOGTYPE_Error);
		return;
	}
	
	EntityDopeSheetScene &dopeSheetSceneRef = static_cast<EntityStateData *>(pModel->GetProjItem()->GetWidget()->GetCurStateData())->GetDopeSheetScene();

	if(pModel->GetSubstate().value<EntityTreeItemData *>() == nullptr) // Indicates a multi-model
	{
		QList<PropertiesTreeModel *> multiModelList = static_cast<PropertiesTreeMultiModel *>(pModel)->GetMultiModelList();
		for(PropertiesTreeModel *pMultiModel : multiModelList)
		{
			EntityTreeItemData *pMultiEntityTreeData = pMultiModel->GetSubstate().value<EntityTreeItemData *>();
			if(pMultiEntityTreeData == nullptr)
			{
				HyGuiLog("EntityUndoCmd_PropertyModified::UpdateEntityModel - pMultiEntityTreeData was nullptr", LOGTYPE_Error);
				continue;
			}
			m_EntityTreeItemDataList.push_back(pMultiEntityTreeData);
		}
	}
	else
		m_EntityTreeItemDataList.push_back(pModel->GetSubstate().value<EntityTreeItemData *>());

	m_iStateIndex = pModel->GetProjItem()->GetWidget()->GetCurStateIndex();
	m_iFrameIndex = dopeSheetSceneRef.GetCurrentFrame();

	for(EntityTreeItemData *pEntityTreeData : m_EntityTreeItemDataList)
	{
		// Special case for when checking to enable/disable categories or properties
		if(m_bIsColumnNameToggle && m_NewData.value<Qt::CheckState>() == Qt::Unchecked)
		{
			// Unchecking this item, remember its value for when undo() is invoked
			if(m_CatPropPair.second.isEmpty()) // Is Category
			{
				if(m_CatPropPair.first.startsWith("Tween "))
				{
					TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(m_CatPropPair.first.mid(6));
					m_OverrideTweenDataList.push_back(dopeSheetSceneRef.GetTweenJsonValues(pEntityTreeData, m_iFrameIndex, eTweenProp));
				}
				else
					HyGuiLog("EntityUndoCmd_PropertyModified - Unhandled logic: Need generic container of old values", LOGTYPE_Error);
			}
			else
				m_OverridePropertyValueList.push_back(pModel->FindPropertyJsonValue(m_CatPropPair.first, m_CatPropPair.second)); // NOTE: This is the old value before redo() is invoked
		}
		else if(m_bIsColumnNameToggle && m_NewData.value<Qt::CheckState>() == Qt::Checked)
		{
			// Checking this item, extrapolate what the new data should be if possible
			if(m_CatPropPair.second.isEmpty() == false) // Is NOT category (is property)
			{
				if(m_iFrameIndex > -1)
				{
					QList<IDrawExItem *> drawItemList = static_cast<EntityDraw *>(pModel->GetProjItem()->GetDraw())->GetDrawItemList();
					for(IDrawExItem *pDrawItem : drawItemList)
					{
						EntityDrawItem *pEntDrawItem = static_cast<EntityDrawItem *>(pDrawItem);
						if(pEntDrawItem->GetEntityTreeItemData() == pEntityTreeData)
						{
							m_OverridePropertyValueList.push_back(pEntDrawItem->ExtractPropertyData(m_CatPropPair.first, m_CatPropPair.second));
							break;
						}
					}
				}
				else // This is the ctor, so just get the default property value
				{
					PropertiesDef propDef = m_pModel->GetDefinition(m_CatPropPair.first, m_CatPropPair.second);
					if(propDef.IsValid())
						m_OverridePropertyValueList.push_back(PropertiesTreeModel::ConvertVariantToJson(propDef.eType, propDef.defaultData));
				}
			}
		}
	}
}

/*virtual*/ EntityUndoCmd_PropertyModified::~EntityUndoCmd_PropertyModified()
{
}

/*virtual*/ void EntityUndoCmd_PropertyModified::OnRedo() /*override*/
{
	// HACK: Don't invoke PropertiesUndoCmd::OnRedo() because m_pModel can be deleted (when it's a multi-model) - instead of querying m_pModel, directly use m_NewData instead
	UpdateEntityModel(true);
}

/*virtual*/ void EntityUndoCmd_PropertyModified::OnUndo() /*override*/
{
	// HACK: Don't invoke PropertiesUndoCmd::OnUndo() because m_pModel can be deleted (when it's a multi-model) - instead of querying m_pModel, directly use m_OldData instead
	UpdateEntityModel(false);
}

void EntityUndoCmd_PropertyModified::UpdateEntityModel(bool bIsRedo)
{
	EntityDopeSheetScene &dopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(m_iStateIndex))->GetDopeSheetScene();

	for(int i = 0; i < m_EntityTreeItemDataList.size(); ++i)
	{
		EntityTreeItemData *pEntityTreeData = m_EntityTreeItemDataList[i];

		bool bRemoveProperties; // Is unchecking box
		if(bIsRedo)
			bRemoveProperties = (m_bIsColumnNameToggle && m_NewData == Qt::Unchecked);
		else
			bRemoveProperties = (m_bIsColumnNameToggle && m_NewData == Qt::Checked);

		if(m_CatPropPair.second.isEmpty()) // Is Category
		{
			// Special Case: Tween Categories, when enabled need to write all its properties at once, since they are 'PROPERTIESACCESS_Mutable' (aka not toggle)
			if(m_CatPropPair.first.startsWith("Tween "))
			{
				TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(m_CatPropPair.first.mid(6));
				if(bRemoveProperties)
				{
					dopeSheetSceneRef.RemoveKeyFrameTween(pEntityTreeData, m_iFrameIndex, eTweenProp, true);
				}
				else
				{
					if(m_OverrideTweenDataList.isEmpty() || m_OverrideTweenDataList[i].m_Destination.isUndefined() || m_OverrideTweenDataList[i].m_Destination.isNull())
					{
						// TODO: When code reaches here and it was a multi-model Redo cmd, when later doing Undo, the original (varying) tween properties can possibly be lost

						// Best determine the tween property values. Otherwise use default values
						TweenJsonValues tweenJsonValues(eTweenProp);

						for(int iTween = 0; iTween < m_EntityTreeItemDataList.size(); ++iTween)
						{
							EntityTreeItemData *pTweenTreeData = m_EntityTreeItemDataList[iTween];
							TweenJsonValues checkTweenJsonValues = dopeSheetSceneRef.GetTweenJsonValues(pTweenTreeData, m_iFrameIndex, eTweenProp);
							if(checkTweenJsonValues.m_Destination.isUndefined() == false && checkTweenJsonValues.m_Destination.isNull() == false)
							{
								tweenJsonValues = checkTweenJsonValues;
								break;
							}
						}

						dopeSheetSceneRef.SetKeyFrameTween(pEntityTreeData, m_iFrameIndex, eTweenProp, tweenJsonValues, true);
					}
					else
						dopeSheetSceneRef.SetKeyFrameTween(pEntityTreeData, m_iFrameIndex, eTweenProp, m_OverrideTweenDataList[i], true);
				}
			}
		}
		else // Modifying a property
		{
			if(bRemoveProperties)
				dopeSheetSceneRef.RemoveKeyFrameProperty(pEntityTreeData, m_iFrameIndex, m_CatPropPair.first, m_CatPropPair.second, true);
			else
			{
				if(m_OverridePropertyValueList.isEmpty() == false && m_OverridePropertyValueList[i].isUndefined() == false && m_OverridePropertyValueList[i].isNull() == false)
					dopeSheetSceneRef.SetKeyFrameProperty(pEntityTreeData, m_iFrameIndex, m_CatPropPair.first, m_CatPropPair.second, m_OverridePropertyValueList[i], true);
				else
					dopeSheetSceneRef.SetKeyFrameProperty(pEntityTreeData, m_iFrameIndex, m_CatPropPair.first, m_CatPropPair.second, PropertiesTreeModel::ConvertVariantToJson(m_ePropTypeModified, m_NewData), true);
			}
		}
	}
}
