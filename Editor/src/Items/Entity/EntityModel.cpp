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
#include "EntityItemMimeData.h"
#include "IAssetItemData.h"
#include "SourceModel.h"

EntityStateData::EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData),
	m_DopeSheetScene(this, stateFileData.m_Meta)
{
}

/*virtual*/ EntityStateData::~EntityStateData()
{
}

EntityDopeSheetScene &EntityStateData::GetDopeSheetScene()
{
	return m_DopeSheetScene;
}

//void EntityStateData::InsertNewPropertiesModel(EntityTreeItemData *pItemData, QJsonObject propObj)
//{
//	if(m_PropertiesMap.contains(pItemData))
//	{
//		HyGuiLog("EntityStateData::InsertNewPropertiesModel - item already was added", LOGTYPE_Error);
//		return;
//	}
//
//	PropertiesTreeModel *pNewProperties = new PropertiesTreeModel(m_ModelRef.GetItem(), m_iINDEX, QVariant());
//	InitalizePropertyModel(pItemData, *pNewProperties);
//	pNewProperties->DeserializeJson(propObj);
//
//	m_PropertiesMap.insert(pItemData, pNewProperties);
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_TreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta, this),
	m_bVertexEditMode(false)
{
	// The EntityTreeModel ('m_TreeModel') was initialized first so that all the EntityTreeItemData's exist.
	// InitStates will look them up using their UUID when initializing its Key Frames map within the state's DopeSheetScene
	InitStates<EntityStateData>(itemFileDataRef);
}

/*virtual*/ EntityModel::~EntityModel()
{
}

EntityTreeModel &EntityModel::GetTreeModel()
{
	return m_TreeModel;
}

QList<EntityTreeItemData *> EntityModel::Cmd_CreateNewChildren(QList<ProjectItemData *> projItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pItem : projItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_AllocChildTreeItem(pItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_CreateNewChildren could not insert a child: " % pItem->GetName(true), LOGTYPE_Error);

		registerList.push_back(pItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);
	
	return treeNodeList;
}

QList<EntityTreeItemData *> EntityModel::Cmd_CreateNewAssets(QList<IAssetItemData *> assetItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pAssetItem : assetItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_AllocAssetTreeItem(pAssetItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_CreateNewAssets could not insert an asset child: " % pAssetItem->GetName(), LOGTYPE_Error);

		registerList.push_back(pAssetItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);

	return treeNodeList;
}

EntityTreeItemData *EntityModel::Cmd_AddExistingItem(QJsonObject descObj, bool bIsArrayItem, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_AllocExistingTreeItem(descObj, bIsArrayItem, iRow);

	QUuid uuidToRegister(descObj["itemUUID"].toString());
	if(uuidToRegister.isNull() == false)
		m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << uuidToRegister);

	return pTreeItemData;
}

EntityTreeItemData *EntityModel::Cmd_CreateNewShape(int iStateIndex, int iFrameIndex, EditorShape eShape, QString sData, bool bIsPrimitive, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_AllocShapeTreeItem(eShape, sData, bIsPrimitive, "m_", iRow);
	static_cast<EntityStateData *>(GetStateData(iStateIndex))->GetDopeSheetScene().SetKeyFrameProperty(pTreeItemData, iFrameIndex, "Shape", "Type", QJsonValue(HyGlobal::ShapeName(eShape)), false);
	static_cast<EntityStateData *>(GetStateData(iStateIndex))->GetDopeSheetScene().SetKeyFrameProperty(pTreeItemData, iFrameIndex, "Shape", "Data", QJsonValue(sData), true);
	
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << pTreeItemData->GetThisUuid());

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ActivateVemOnNextJsonMeta();

	return pTreeItemData;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewPasteItems(QJsonObject mimeObject, EntityTreeItemData *pArrayFolder)
{
	QList<EntityTreeItemData *> pastedItemList;

	QJsonArray itemArray = mimeObject["itemArray"].toArray();
	for(int i = 0; i < itemArray.size(); ++i)
	{
		QJsonObject itemObj = itemArray[i].toObject();

		QJsonObject descObj = itemObj["descObj"].toObject();

		QUuid newUuid = QUuid::createUuid();
		descObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
		descObj.insert("isSelected", false);

		bool bIsArrayItem = false;
		if(pArrayFolder)
		{
			ItemType eGuiType = HyGlobal::GetTypeFromString(descObj["itemType"].toString());
			if(eGuiType != pArrayFolder->GetType())
			{
				HyGuiLog("EntityUndoCmd_PasteItems::redo - pasted array item (" % descObj["codeName"].toString() % ") " % descObj["itemType"].toString() % " did match array type", LOGTYPE_Error);
				continue;
			}

			descObj["codeName"] = pArrayFolder->GetCodeName();
			bIsArrayItem = true;
		}

		// NOTE: Error checking the pasted item's states to match current entity should have already been done in EntityWidget::on_actionPasteEntityItems_triggered
		EntityTreeItemData *pPastedTreeItemData = Cmd_AddExistingItem(descObj, bIsArrayItem, -1);
		pastedItemList.push_back(pPastedTreeItemData);

		// Merge the pasted key frame data into the states for the new item
		QJsonArray stateKeyFramesArray = itemObj["stateKeyFramesArray"].toArray();
		for(int iStateIndex = 0; iStateIndex < GetNumStates(); ++iStateIndex)
		{
			if(stateKeyFramesArray.size() <= iStateIndex)
				break;

			EntityStateData *pStateData = static_cast<EntityStateData *>(GetStateData(iStateIndex));

			QJsonObject stateKeyFramesObj = stateKeyFramesArray[iStateIndex].toObject();
			QJsonArray keyFramesArray = stateKeyFramesObj["keyFrames"].toArray();

			for(int iKeyFrameIndex = 0; iKeyFrameIndex < keyFramesArray.size(); ++iKeyFrameIndex)
			{
				QJsonObject keyFrameObj = keyFramesArray[iKeyFrameIndex].toObject();
				pStateData->GetDopeSheetScene().SetKeyFrameProperties(pPastedTreeItemData, keyFrameObj["frame"].toInt(), keyFrameObj["props"].toObject());
			}
		}
	}

	return pastedItemList;
}

// It is assumed that the items within 'itemDataList' have been removed/popped prior
QList<EntityTreeItemData *> EntityModel::Cmd_CreateNewArray(QList<EntityTreeItemData *> itemDataList, QString sArrayName, int iArrayFolderRow)
{
	sArrayName = GenerateCodeName(sArrayName);

	// Create temporary EntityItemMimeData to generate JSON object that contains an "itemList" JSON array of that represents 'itemDataList'
	EntityItemMimeData *pMimeData = new EntityItemMimeData(m_ItemRef, itemDataList);
	QByteArray jsonData = pMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_EntityItems));
	delete pMimeData;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
	QJsonObject mimeObject = jsonDocument.object();
	QJsonArray itemArray = mimeObject["itemArray"].toArray();

	// Parse info from 'itemArray' and invoke Cmd_AddExistingItem on each item
	QList<EntityTreeItemData *> newItemDataList;
	for(int i = 0; i < itemArray.size(); ++i)
	{
		QJsonObject itemObj = itemArray[i].toObject();

		QJsonObject descObj = itemObj["descObj"].toObject();
		QJsonArray stateKeyFramesArray = itemObj["stateKeyFramesArray"].toArray();

		QUuid newUuid = QUuid::createUuid();
		descObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
		descObj.insert("codeName", sArrayName);
		descObj.insert("isSelected", false);

		EntityTreeItemData *pDuplicateItem = Cmd_AddExistingItem(descObj, true, i == 0 ? iArrayFolderRow : -1);
		newItemDataList.push_back(pDuplicateItem);

		// Copy all its property/key frames into the newly created item
		for(int j = 0; j < stateKeyFramesArray.size(); ++j)
		{
			EntityStateData *pStateData = static_cast<EntityStateData *>(m_StateList[j]);

			QJsonObject stateKeyFramesObj = stateKeyFramesArray[j].toObject();
			if(pStateData->GetName() != stateKeyFramesObj["name"].toString() || pStateData->GetDopeSheetScene().GetFramesPerSecond() != stateKeyFramesObj["framesPerSecond"].toInt())
				HyGuiLog("EntityModel::Cmd_CreateNewArray - states mismatch", LOGTYPE_Error);
			
			QJsonArray keyFramesArray = stateKeyFramesObj["keyFrames"].toArray();
			for(int iKeyFrameIndex = 0; iKeyFrameIndex < keyFramesArray.size(); ++iKeyFrameIndex)
			{
				QJsonObject keyFrameObj = keyFramesArray[iKeyFrameIndex].toObject();
				pStateData->GetDopeSheetScene().SetKeyFrameProperties(pDuplicateItem, keyFrameObj["frame"].toInt(), keyFrameObj["props"].toObject());
			}
		}
	}

	return newItemDataList;
}

void EntityModel::Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList)
{
	for(EntityTreeItemData *pTreeItem : selectedList)
		pTreeItem->SetSelected(true);
	for(EntityTreeItemData *pTreeItem : deselectedList)
		pTreeItem->SetSelected(false);

	static_cast<EntityStateData *>(m_StateList[static_cast<EntityWidget *>(m_ItemRef.GetWidget())->GetCurStateIndex()])->GetDopeSheetScene().RefreshAllGfxItems();

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ApplyJsonData();
}

int32 EntityModel::Cmd_RemoveTreeItem(EntityTreeItemData *pItem)
{
	if(pItem == nullptr)
		return -1;

	m_ItemRef.GetProject().DecrementDependencies(&m_ItemRef, QList<QUuid>() << pItem->GetReferencedItemUuid());

	int32 iRow = m_TreeModel.Cmd_PopChild(pItem);
	if(iRow < 0)
		return iRow;

	ClearShapeEdit();

	return iRow;
}

bool EntityModel::Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow)
{
	if(m_TreeModel.Cmd_ReaddChild(pNodeItem, iRow) == false)
		return false;

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << pNodeItem->GetReferencedItemUuid());

	return true;
}

void EntityModel::Cmd_RenameItem(EntityTreeItemData *pItemData, QString sNewName)
{
	if(pItemData->GetEntType() == ENTTYPE_ArrayFolder)
	{
		pItemData->SetText(sNewName);

		QList<TreeModelItemData *> arrayChildrenList = m_TreeModel.GetItemsRecursively(m_TreeModel.FindIndex<EntityTreeItemData *>(pItemData, 0));
		for(TreeModelItemData *pItemData : arrayChildrenList)
			pItemData->SetText(sNewName);
	}
	else if(pItemData->GetEntType() == ENTTYPE_ArrayItem)
	{
		QModelIndex arrayFolderIndex = m_TreeModel.parent(m_TreeModel.FindIndex<EntityTreeItemData *>(pItemData, 0));
		EntityTreeItemData *pArrayFolderItemData = m_TreeModel.data(arrayFolderIndex, Qt::UserRole).value<EntityTreeItemData *>();

		pArrayFolderItemData->SetText(sNewName);

		QList<TreeModelItemData *> arrayChildrenList = m_TreeModel.GetItemsRecursively(arrayFolderIndex);
		for(TreeModelItemData *pItemData : arrayChildrenList)
			pItemData->SetText(sNewName);
	}
	else
		pItemData->SetText(sNewName);
}

void EntityModel::SetShapeEditDrag(EditorShape eShapeType, bool bAsPrimitive)
{
	QString sStatusMsg("Drawing new ");
	sStatusMsg += bAsPrimitive ? "primitive " : "";
	sStatusMsg += HyGlobal::ShapeName(eShapeType);
	MainWindow::SetStatus(sStatusMsg, 0);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->CheckShapeAddBtn(eShapeType, bAsPrimitive);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetShapeEditDrag(eShapeType, bAsPrimitive);
}

void EntityModel::SetShapeEditVemMode(bool bEnable)
{
	m_bVertexEditMode = bEnable;

	if(m_bVertexEditMode)
		MainWindow::SetStatus("Vertex Edit Mode", 0);
	else
		MainWindow::ClearStatus();

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->CheckVertexEditMode(m_bVertexEditMode);

	EntityDraw *pEntityDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntityDraw)
	{
		if(m_bVertexEditMode)
			pEntityDraw->SetShapeEditVertex();
		else
			pEntityDraw->ClearShapeEdit();
	}
}

void EntityModel::ClearShapeEdit()
{
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->UncheckAll();

	EntityDraw *pEntityDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntityDraw)
		pEntityDraw->ClearShapeEdit();
}

QString EntityModel::GenerateCodeName(QString sDesiredName) const
{
	return m_TreeModel.GenerateCodeName(sDesiredName);
}

QString EntityModel::GenerateSrc_FileIncludes() const
{
	QString sSrc;

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	for(EntityTreeItemData *pItem : itemList)
	{
		if(pItem->GetType() != ITEM_Entity)
			continue;

		QUuid referencedItemUuid = pItem->GetReferencedItemUuid();
		ProjectItemData *pReferencedItemData = static_cast<ProjectItemData *>(GetItem().GetProject().FindItemData(referencedItemUuid));
		if(pReferencedItemData == nullptr)
			HyGuiLog("Could not find referenced item data from Sub-Entity's UUID: " + referencedItemUuid.toString(), LOGTYPE_Error);
		sSrc += "#include \"" + pReferencedItemData->GetName(false);
		sSrc += ".h\"\n";
	}

	return sSrc;
}

QString EntityModel::GenerateSrc_MemberVariables() const
{
	QString sSrc;

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	EntityTreeItemData *pCurArray = nullptr;
	for(EntityTreeItemData *pItem : itemList)
	{
		if(pCurArray)
		{
			if(pCurArray->GetCodeName() == pItem->GetCodeName())
				continue;
			pCurArray = nullptr;
		}

		sSrc += "\t";
		if(pItem->GetType() == ITEM_Entity)
		{
			QUuid referencedItemUuid = pItem->GetReferencedItemUuid();
			ProjectItemData *pReferencedItemData = static_cast<ProjectItemData *>(GetItem().GetProject().FindItemData(referencedItemUuid));
			if(pReferencedItemData == nullptr)
				HyGuiLog("Could not find referenced item data from Sub-Entity's UUID: " + referencedItemUuid.toString(), LOGTYPE_Error);
			sSrc += pReferencedItemData->GetName(false);
		}
		else
			sSrc += pItem->GetHyNodeTypeName();
		sSrc += " " + pItem->GetCodeName();
		if(pItem->GetEntType() != ENTTYPE_ArrayItem)
			sSrc += ";\n";
		else
		{
			sSrc += "\t[" + QString::number(m_TreeModel.GetArrayFolderTreeItem(pItem)->GetNumChildren()) + "];\n";
			pCurArray = pItem;
		}
	}

	return sSrc;
}

QString EntityModel::GenerateSrc_MemberInitializerList() const
{
	QString sSrc = " :\n\tHyEntity2d(pParent)";

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	EntityTreeItemData *pCurArray = nullptr;
	for(EntityTreeItemData *pItem : itemList)
	{
		if(pCurArray)
		{
			if(pCurArray->GetCodeName() != pItem->GetCodeName())
			{
				sSrc += "},\n\t" + pItem->GetCodeName();
				pCurArray = nullptr;
			}
		}
		else
			sSrc += ",\n\t" + pItem->GetCodeName();

		TreeModelItemData *pReferencedItemData = GetItem().GetProject().FindItemData(pItem->GetReferencedItemUuid());
		QString sInitialization;
		switch(pItem->GetType())
		{
		case ITEM_Primitive:
		case ITEM_BoundingVolume:
		case ITEM_Entity:
			sInitialization = "(this)";
			break;

		case ITEM_Audio:
		case ITEM_Text:
		case ITEM_Spine:
		case ITEM_Sprite:
			if(pReferencedItemData == nullptr || pReferencedItemData->IsProjectItem() == false)
				HyGuiLog("EntityModel::GenerateSrc_MemberInitializerList() - Could not find referenced project item for: " % pItem->GetCodeName(), LOGTYPE_Error);
			else
			{
				QString sItemPath = static_cast<ProjectItemData *>(pReferencedItemData)->GetName(true);
				int iLastIndex = sItemPath.lastIndexOf('/');
				QString sPrefix = sItemPath.left(iLastIndex);
				QString sName = sItemPath.mid(iLastIndex + 1);

				sInitialization = "(\"" + sPrefix + "\", \"" + sName + "\", this)";
			}
			break;

		case ITEM_AtlasFrame:
			if(pReferencedItemData == nullptr || pReferencedItemData->IsAssetItem() == false)
			{
				HyGuiLog("EntityModel::GenerateSrc_MemberInitializerList() - Could not find referenced asset item for: " % pItem->GetCodeName(), LOGTYPE_Error);
				break;
			}
			sInitialization = "(" + QString::number(static_cast<IAssetItemData *>(pReferencedItemData)->GetChecksum()) + ", " + QString::number(static_cast<IAssetItemData *>(pReferencedItemData)->GetBankId()) + ", this)";
			break;

		default:
			HyGuiLog("EntityModel::GenerateSrc_MemberInitializerList() - Unhandled item type: " % HyGlobal::ItemName(pItem->GetType(), false), LOGTYPE_Error);
			break;
		}
		
		if(pItem->GetEntType() != ENTTYPE_ArrayItem)
			sSrc += sInitialization;
		else
		{
			if(pCurArray == nullptr)
				sSrc += "{";
			else
				sSrc += ", ";

			sSrc += pItem->GetHyNodeTypeName() + sInitialization;
			pCurArray = pItem;
		}
	}

	if(pCurArray)
		sSrc += "}";

	return sSrc;
}

QString EntityModel::GenerateSrc_Ctor() const
{
	return "SetState(0);";
}

QString EntityModel::GenerateSrc_SetStates() const
{
	QString sSrc = "switch(uiStateIndex)\n\t{\n\t";

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	itemList.prepend(m_TreeModel.GetRootTreeItemData());

	bool bActivatePhysics = false;
	uint32 uiMaxVertListSize = 0;

	for(int i = 0; i < GetNumStates(); ++i)
	{
		sSrc += "case " + QString::number(i) + ":\n\t\t";

		for(EntityTreeItemData *pItem : itemList)
		{
			sSrc += pItem->GenerateStateSrc(i, "\n\t\t", bActivatePhysics, uiMaxVertListSize);
			sSrc += "\n\t\t";
		}

		if(bActivatePhysics)
			sSrc += "physics.Activate();\n\n\t";

		sSrc += "break;\n\n\t";
	}
	sSrc += "default:\n\t\tHyLogWarning(\"" + QString(HySrcEntityNamespace) + "::" + GetItem().GetName(false) + "::SetState() was passed an invalid state: \" << uiStateIndex);\n\t\treturn false;\n\t}";
	sSrc += "\n\n\treturn true;";

	if(uiMaxVertListSize > 0)
		sSrc.prepend("std::vector<glm::vec2> vertList;\n\tvertList.reserve(" + QString::number(uiMaxVertListSize) + ");\n\t");

	return sSrc;
}

/*virtual*/ void EntityModel::OnPropertyModified(PropertiesTreeModel &propertiesModelRef, const QModelIndex &indexRef) /*override*/
{
	EntityWidget *pEntWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pEntWidget == nullptr)
	{
		HyGuiLog("EntityModel::OnPropertyModified() - EntityWidget was nullptr", LOGTYPE_Error);
		return;
	}
	int iStateIndex = pEntWidget->GetCurStateIndex();
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_StateList[iStateIndex]);

	int iFrameIndex = pStateData->GetDopeSheetScene().GetCurrentFrame();
	QString sCategory = propertiesModelRef.GetCategoryName(indexRef);
	QString sProperty = propertiesModelRef.GetPropertyName(indexRef);

	EntityTreeItemData *pEntityTreeData = propertiesModelRef.GetSubstate().value<EntityTreeItemData *>();

	if(propertiesModelRef.GetPropertyDefinition(indexRef).eAccessType == PROPERTIESACCESS_ToggleOff)
		pStateData->GetDopeSheetScene().RemoveKeyFrameProperty(pEntityTreeData, iFrameIndex, sCategory, sProperty, true);
	else
		pStateData->GetDopeSheetScene().SetKeyFrameProperty(pEntityTreeData, iFrameIndex, sCategory, sProperty, propertiesModelRef.GetPropertyJsonValue(indexRef), true);
}

/*virtual*/ void EntityModel::OnPopState(int iPoppedStateIndex) /*override*/
{
}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	// Save generated C++ class in a .h
	SourceModel &sourceModelRef = m_ItemRef.GetProject().GetSourceModel();
	return sourceModelRef.GenerateEntitySrcFiles(*this);
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	itemSpecificFileDataOut.m_Meta.insert("codeName", m_TreeModel.GetRootTreeItemData()->GetCodeName());
	
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	m_TreeModel.GetTreeItemData(childList, shapeList);

	QJsonArray childArray;
	QString sCurrentArrayCodeName = "";
	for(int i = 0; i < childList.size(); ) // 'i' is incremented inside the loop
	{
		if(childList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(childList[i]->GetCodeName()) != 0)
		{
			sCurrentArrayCodeName = childList[i]->GetCodeName();
			QJsonArray packedArray;
			do
			{
				QJsonObject arrayItemObj;
				childList[i]->InsertJsonInfo_Desc(arrayItemObj);

				packedArray.append(arrayItemObj);
				++i;

			} while(i < childList.size() && childList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(childList[i]->GetCodeName()) == 0);

			childArray.append(packedArray);
		}
		else
		{
			QJsonObject childObj;
			childList[i]->InsertJsonInfo_Desc(childObj);
			
			childArray.append(childObj);
			++i;
		}
	}
	itemSpecificFileDataOut.m_Meta.insert("descChildList", childArray);

	QJsonArray shapeArray;
	sCurrentArrayCodeName = "";
	for(int i = 0; i < shapeList.size(); )
	{
		if(shapeList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(shapeList[i]->GetCodeName()) != 0)
		{
			sCurrentArrayCodeName = shapeList[i]->GetCodeName();
			QJsonArray packedArray;
			do
			{
				QJsonObject arrayItemObj;
				shapeList[i]->InsertJsonInfo_Desc(arrayItemObj);
				
				packedArray.append(arrayItemObj);
				++i;

			} while(i < shapeList.size() && shapeList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(shapeList[i]->GetCodeName()) == 0);

			shapeArray.append(packedArray);
		}
		else
		{
			QJsonObject shapeObj;
			shapeList[i]->InsertJsonInfo_Desc(shapeObj);
			
			shapeArray.append(shapeObj);
			++i;
		}
	}
	itemSpecificFileDataOut.m_Meta.insert("descShapeList", shapeArray);
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_StateList[uiIndex]);

	stateFileDataOut.m_Meta.insert("framesPerSecond", pStateData->GetDopeSheetScene().GetFramesPerSecond());

	// Combine all items (root, children, and shapes) into a single list 'itemList'
	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	itemList.prepend(m_TreeModel.GetRootTreeItemData());

	// Serialize all key frames for each item that has them
	QJsonObject stateKeyFramesObj;
	for(EntityTreeItemData *pItem : itemList)
	{
		QJsonArray keyFramesArray = pStateData->GetDopeSheetScene().SerializeAllKeyFrames(pItem);
		if(keyFramesArray.empty())
			continue;
		
		QString sUuid = pItem->GetThisUuid().toString(QUuid::StringFormat::WithoutBraces);
		stateKeyFramesObj.insert(sUuid, keyFramesArray);
	}
	stateFileDataOut.m_Meta.insert("keyFrames", stateKeyFramesObj);
}
