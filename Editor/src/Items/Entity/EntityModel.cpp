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
	IStateData(iStateIndex, modelRef, stateFileData)
{
	EntityTreeModel *pTreeModelRef = static_cast<EntityModel &>(modelRef).GetTreeModel();
	if(pTreeModelRef == nullptr)
	{
		HyGuiLog("EntityStateData::EntityStateData - pTreeModelRef was nullptr", LOGTYPE_Error);
		return;
	}

	// NOTE: If we get here, 'stateFileData' will contain valid data (since it was just copied from an existing state) and all entity tree items exist and have been created prior
	
	// Get all the items (both child and shape) and then init their properties
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	pTreeModelRef->GetTreeItemData(childList, shapeList);

	// Init 'root'
	QJsonObject propRootObj;
	if(stateFileData.m_Meta.contains("propRoot"))
		propRootObj = stateFileData.m_Meta["propRoot"].toObject();
	InsertNewPropertiesModel(pTreeModelRef->GetRootTreeItemData(), propRootObj);

	// Init 'child list'
	QJsonArray propChildListArray = stateFileData.m_Meta["propChildList"].toArray();
	//if(propChildListArray.size() != childList.size())
	//	HyGuiLog("EntityStateData::EntityStateData - stateFileData.m_Meta didn't have valid \"propChildList\"", LOGTYPE_Error);
	for(int i = 0; i < childList.size(); ++i)
	{
		QJsonObject propChildObj;
		if(propChildListArray.size() > i)
			propChildObj = propChildListArray[i].toObject();

		InsertNewPropertiesModel(childList[i], propChildObj);
	}

	// Init 'shape list'
	QJsonArray propShapeListArray = stateFileData.m_Meta["propShapeList"].toArray();
	//if(propShapeListArray.size() != shapeList.size())
	//	HyGuiLog("EntityStateData::EntityStateData - stateFileData.m_Meta didn't have valid \"propShapeList\"", LOGTYPE_Error);
	for(int i = 0; i < shapeList.size(); ++i)
	{
		QJsonObject propShapeObj;
		if(propShapeListArray.size() > i)
			propShapeObj = propShapeListArray[i].toObject();

		InsertNewPropertiesModel(shapeList[i], propShapeObj);
	}
}

/*virtual*/ EntityStateData::~EntityStateData()
{
	QList<EntityTreeItemData *> keyList = m_PropertiesMap.keys();
	for(EntityTreeItemData *pKey : keyList)
		delete m_PropertiesMap[pKey];
}

void EntityStateData::InsertNewPropertiesModel(EntityTreeItemData *pItemData, QJsonObject propObj)
{
	if(m_PropertiesMap.contains(pItemData))
	{
		HyGuiLog("EntityStateData::InsertNewPropertiesModel - item already was added", LOGTYPE_Error);
		return;
	}

	PropertiesTreeModel *pNewProperties = new PropertiesTreeModel(m_ModelRef.GetItem(), m_iINDEX, QVariant());
	InitalizePropertyModel(pItemData, *pNewProperties);
	pNewProperties->DeserializeJson(propObj);

	m_PropertiesMap.insert(pItemData, pNewProperties);
}

PropertiesTreeModel *EntityStateData::GetPropertiesTreeModel(EntityTreeItemData *pItemData) const
{
	if(m_PropertiesMap.contains(pItemData) == false)
	{
		HyGuiLog("EntityStateData::GetPropertiesTreeModel - pItemData was not found", LOGTYPE_Error);
		return nullptr;
	}
	return m_PropertiesMap[pItemData];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_pTreeModel(nullptr),
	m_bVertexEditMode(false),
	m_DopeSheetScene(this)
{
	m_DopeSheetScene.setBackgroundBrush(Qt::blue);

	// First initialize the states so they exist before we try to add properties to them
	m_pTreeModel = new EntityTreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta["UUID"].toString(), this);
	InitStates<EntityStateData>(itemFileDataRef);

	if(itemFileDataRef.m_Meta.contains("stateArray") && itemFileDataRef.m_Meta["stateArray"].toArray().size() > 0)
		InitEntityNodeTreeItems(itemFileDataRef);
}

/*virtual*/ EntityModel::~EntityModel()
{
	delete m_pTreeModel;
}

void EntityModel::InitEntityNodeTreeItems(const FileDataPair &itemFileData)
{
	// Each element in QList<> represents a state's properties for all the children or shapes
	QList<QJsonObject> propRootObjectList;
	QList<QJsonArray> propChildArrayList;
	QList<QJsonArray> propShapeArrayList;
	QJsonArray stateArray = itemFileData.m_Meta["stateArray"].toArray();
	for(int i = 0; i < /*m_StateList.size()*/stateArray.size(); ++i)
	{
		QJsonObject stateObj = stateArray[i].toObject();
		propRootObjectList.push_back(stateObj["propRoot"].toObject());
		propChildArrayList.push_back(stateObj["propChildList"].toArray());
		propShapeArrayList.push_back(stateObj["propShapeList"].toArray());
	}
	if(GetNumStates() == 0 ||
		propChildArrayList.size() != GetNumStates() ||
		propShapeArrayList.size() != GetNumStates() ||
		propRootObjectList.size() != GetNumStates())
	{
		HyGuiLog("EntityModel::EntityModel - invalid number of states when parsing properties", LOGTYPE_Error);
	}

	// Initialize root
	for(int iStateIndex = 0; iStateIndex < GetNumStates(); ++iStateIndex)
	{
		QJsonObject propRootObj = propRootObjectList[iStateIndex];
		m_pTreeModel->GetRootTreeItemData()->GetPropertiesModel(iStateIndex)->DeserializeJson(propRootObj);
	}

	std::function<void(const QJsonArray &, const QList<QJsonArray> &)> fpPopulateNodeTreeItems =
		[&](const QJsonArray &itemListArray, const QList<QJsonArray> &propItemArrayList)
	{
		for(int i = 0; i < itemListArray.size(); ++i)
		{
			if(itemListArray[i].isObject())
			{
				QJsonObject descObj = itemListArray[i].toObject();

				QJsonArray propsArray;
				for(int iStateIndex = 0; iStateIndex < GetNumStates(); ++iStateIndex)
					propsArray.push_back(propItemArrayList[iStateIndex][i].toObject());

				Cmd_AddNewItem(descObj, propsArray, false, i);
			}
			else if(itemListArray[i].isArray())
			{
				QJsonArray subItemArray = itemListArray[i].toArray();
				for(int j = 0; j < subItemArray.size(); ++j)
				{
					QJsonObject descObj = subItemArray[j].toObject();

					QJsonArray propsArray;
					for(int iStateIndex = 0; iStateIndex < GetNumStates(); ++iStateIndex)
					{
						QJsonArray subPropItemArray = propItemArrayList[iStateIndex][i].toArray();
						propsArray.push_back(subPropItemArray[j].toObject());
					}

					Cmd_AddNewItem(descObj, propsArray, true, j == 0 ? i : j);
				}
			}
			else
				HyGuiLog("EntityModel::EntityModel invalid childlist", LOGTYPE_Error);
		}
	};
	// Insert all the 'child' items into the nodeTree
	fpPopulateNodeTreeItems(itemFileData.m_Meta["descChildList"].toArray(), propChildArrayList);
	fpPopulateNodeTreeItems(itemFileData.m_Meta["descShapeList"].toArray(), propShapeArrayList);
}

EntityTreeModel *EntityModel::GetTreeModel()
{
	return m_pTreeModel;
}

QGraphicsScene *EntityModel::GetDopeSheetScene()
{
	return &m_DopeSheetScene;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pItem : projItemList)
	{
		EntityTreeItemData *pAddedItem = m_pTreeModel->Cmd_InsertNewChild(pItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_AddNewChildren could not insert a child: " % pItem->GetName(true), LOGTYPE_Error);

		registerList.push_back(pItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);
	
	return treeNodeList;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewAssets(QList<IAssetItemData *> assetItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pAssetItem : assetItemList)
	{
		EntityTreeItemData *pAddedItem = m_pTreeModel->Cmd_InsertNewAsset(pAssetItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_AddNewChildren could not insert an asset child: " % pAssetItem->GetName(), LOGTYPE_Error);

		registerList.push_back(pAssetItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);

	return treeNodeList;
}

EntityTreeItemData *EntityModel::Cmd_AddNewItem(QJsonObject descObj, QJsonArray propsArray, bool bIsArrayItem, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_pTreeModel->Cmd_InsertNewItem(descObj, propsArray, bIsArrayItem, iRow);

	QUuid uuidToRegister(descObj["itemUUID"].toString());
	if(uuidToRegister.isNull() == false)
		m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << uuidToRegister);

	return pTreeItemData;
}

EntityTreeItemData *EntityModel::Cmd_AddNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_pTreeModel->Cmd_InsertNewShape(eShape, sData, bIsPrimitive, "m_", iRow);
	
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
		QJsonArray propArray = itemObj["propArray"].toArray();

		QUuid newUuid = QUuid::createUuid();
		descObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
		descObj.insert("isSelected", false);
		
		for(int j = 0; j < propArray.size(); ++j)
		{
			QJsonObject propObj = propArray[j].toObject();
			QJsonObject commonObj = propObj["Common"].toObject();
			commonObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
			propObj.insert("Common", commonObj); // Reinsert "Common" with new UUID

			propArray[j] = propObj;
		}

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

		EntityTreeItemData *pPastedTreeItemData = Cmd_AddNewItem(descObj, propArray, bIsArrayItem, -1);
		pastedItemList.push_back(pPastedTreeItemData);
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

	// Parse info from 'itemArray' and invoke Cmd_AddNewItem on each item
	QList<EntityTreeItemData *> newItemDataList;
	for(int i = 0; i < itemArray.size(); ++i)
	{
		QJsonObject itemObj = itemArray[i].toObject();

		QJsonObject descObj = itemObj["descObj"].toObject();
		QJsonArray propArray = itemObj["propArray"].toArray();

		QUuid newUuid = QUuid::createUuid();
		descObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
		descObj.insert("codeName", sArrayName);
		descObj.insert("isSelected", false);

		for(int j = 0; j < propArray.size(); ++j)
		{
			QJsonObject propObj = propArray[j].toObject();
			QJsonObject commonObj = propObj["Common"].toObject();
			commonObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
			propObj.insert("Common", commonObj); // Reinsert "Common" with new UUID

			propArray[j] = propObj;
		}

		EntityTreeItemData *pDuplicateItem = Cmd_AddNewItem(descObj, propArray, true, i == 0 ? iArrayFolderRow : -1);
		newItemDataList.push_back(pDuplicateItem);
	}

	return newItemDataList;
}

void EntityModel::Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList)
{
	for(EntityTreeItemData *pTreeItem : selectedList)
		pTreeItem->SetSelected(true);
	for(EntityTreeItemData *pTreeItem : deselectedList)
		pTreeItem->SetSelected(false);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ApplyJsonData();
}

int32 EntityModel::Cmd_RemoveTreeItem(EntityTreeItemData *pItem)
{
	if(pItem == nullptr)
		return -1;

	m_ItemRef.GetProject().DecrementDependencies(&m_ItemRef, QList<QUuid>() << pItem->GetReferencedItemUuid());

	int32 iRow = m_pTreeModel->Cmd_PopChild(pItem);
	if(iRow < 0)
		return iRow;

	ClearShapeEdit();

	return iRow;
}

bool EntityModel::Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow)
{
	if(m_pTreeModel->Cmd_ReaddChild(pNodeItem, iRow) == false)
		return false;

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << pNodeItem->GetReferencedItemUuid());

	return true;
}

void EntityModel::Cmd_RenameItem(EntityTreeItemData *pItemData, QString sNewName)
{
	if(pItemData->GetEntType() == ENTTYPE_ArrayFolder)
	{
		pItemData->SetText(sNewName);

		QList<TreeModelItemData *> arrayChildrenList = m_pTreeModel->GetItemsRecursively(m_pTreeModel->FindIndex<EntityTreeItemData *>(pItemData, 0));
		for(TreeModelItemData *pItemData : arrayChildrenList)
			pItemData->SetText(sNewName);
	}
	else if(pItemData->GetEntType() == ENTTYPE_ArrayItem)
	{
		QModelIndex arrayFolderIndex = m_pTreeModel->parent(m_pTreeModel->FindIndex<EntityTreeItemData *>(pItemData, 0));
		EntityTreeItemData *pArrayFolderItemData = m_pTreeModel->data(arrayFolderIndex, Qt::UserRole).value<EntityTreeItemData *>();

		pArrayFolderItemData->SetText(sNewName);

		QList<TreeModelItemData *> arrayChildrenList = m_pTreeModel->GetItemsRecursively(arrayFolderIndex);
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
	return m_pTreeModel->GenerateCodeName(sDesiredName);
}

QString EntityModel::GenerateSrc_FileIncludes() const
{
	QString sSrc;

	QList<EntityTreeItemData *> itemList, shapeList;
	m_pTreeModel->GetTreeItemData(itemList, shapeList);
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
	m_pTreeModel->GetTreeItemData(itemList, shapeList);
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
			sSrc += "\t[" + QString::number(m_pTreeModel->GetArrayFolderTreeItem(pItem)->GetNumChildren()) + "];\n";
			pCurArray = pItem;
		}
	}

	return sSrc;
}

QString EntityModel::GenerateSrc_MemberInitializerList() const
{
	QString sSrc = " :\n\tHyEntity2d(pParent)";

	QList<EntityTreeItemData *> itemList, shapeList;
	m_pTreeModel->GetTreeItemData(itemList, shapeList);
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
	m_pTreeModel->GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	itemList.prepend(m_pTreeModel->GetRootTreeItemData());

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

/*virtual*/ void EntityModel::OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) /*override*/
{
	EntityTreeItemData *pEntityTreeData = reinterpret_cast<EntityTreeItemData *>(propertiesModelRef.GetSubstate().toLongLong());
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
	itemSpecificFileDataOut.m_Meta.insert("codeName", m_pTreeModel->GetRootTreeItemData()->GetCodeName());
	
	InsertChildAndShapeList(-1, itemSpecificFileDataOut);
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
	InsertChildAndShapeList(uiIndex, stateFileDataOut);
}

// Pass -1 for 'iStateIndex' when saving the common "desc" data
void EntityModel::InsertChildAndShapeList(int iStateIndex, FileDataPair &fileDataPairOut) const
{
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	m_pTreeModel->GetTreeItemData(childList, shapeList);

	if(iStateIndex >= 0)
	{
		QJsonObject propRootObject = m_pTreeModel->GetRootTreeItemData()->GetPropertiesModel(iStateIndex)->SerializeJson();
		fileDataPairOut.m_Meta.insert("propRoot", propRootObject);
	}

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
				if(iStateIndex == -1)
					childList[i]->InsertJsonInfo_Desc(arrayItemObj);
				else
					arrayItemObj = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesTreeModel(childList[i])->SerializeJson();

				packedArray.append(arrayItemObj);
				++i;

			} while(i < childList.size() && childList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(childList[i]->GetCodeName()) == 0);

			childArray.append(packedArray);
		}
		else
		{
			QJsonObject childObj;
			if(iStateIndex == -1)
				childList[i]->InsertJsonInfo_Desc(childObj);
			else
				childObj = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesTreeModel(childList[i])->SerializeJson();

			childArray.append(childObj);
			++i;
		}
	}
	if(iStateIndex == -1)
		fileDataPairOut.m_Meta.insert("descChildList", childArray);
	else
		fileDataPairOut.m_Meta.insert("propChildList", childArray);

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
				if(iStateIndex == -1)
					shapeList[i]->InsertJsonInfo_Desc(arrayItemObj);
				else
					arrayItemObj = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesTreeModel(shapeList[i])->SerializeJson();

				packedArray.append(arrayItemObj);
				++i;

			} while(i < shapeList.size() && shapeList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(shapeList[i]->GetCodeName()) == 0);

			shapeArray.append(packedArray);
		}
		else
		{
			QJsonObject shapeObj;
			if(iStateIndex == -1)
				shapeList[i]->InsertJsonInfo_Desc(shapeObj);
			else
				shapeObj = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesTreeModel(shapeList[i])->SerializeJson();

			shapeArray.append(shapeObj);
			++i;
		}
	}
	if(iStateIndex == -1)
		fileDataPairOut.m_Meta.insert("descShapeList", shapeArray);
	else
		fileDataPairOut.m_Meta.insert("propShapeList", shapeArray);
}
