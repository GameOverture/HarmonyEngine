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

EntityStateData::EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData)
{
	// m_PropertiesMap will be filled in by the EntityModel::Cmd_AddItemDataProperties() function
}

/*virtual*/ EntityStateData::~EntityStateData()
{
	QList<EntityTreeItemData *> keyList = m_PropertiesMap.keys();
	for(EntityTreeItemData *pKey : keyList)
		delete m_PropertiesMap[pKey];
}

PropertiesTreeModel *EntityStateData::GetPropertiesTreeModel(EntityTreeItemData *pItemData)
{
	if(m_PropertiesMap.contains(pItemData) == false)
	{
		HyGuiLog("EntityStateData::GetPropertiesTreeModel - pItemData was not found", LOGTYPE_Error);
		return nullptr;
	}
	return m_PropertiesMap[pItemData];
}

void EntityStateData::Cmd_AddItemDataProperties(EntityTreeItemData *pItemData, QJsonObject propObj)
{
	if(m_PropertiesMap.contains(pItemData))
	{
		HyGuiLog("EntityStateData::Cmd_AddItemDataProperties - item already was added", LOGTYPE_Error);
		return;
	}

	PropertiesTreeModel *pNewProperties = new PropertiesTreeModel(m_ModelRef.GetItem(), m_iINDEX, QVariant());
	InitalizePropertyModel(pItemData, *pNewProperties);
	pNewProperties->DeserializeJson(propObj);

	m_PropertiesMap.insert(pItemData, pNewProperties);
}

void EntityStateData::Cmd_RemoveItemDataProperties(EntityTreeItemData *pItemData)
{
	if(m_PropertiesMap.contains(pItemData) == false)
	{
		HyGuiLog("EntityStateData::Cmd_RemoveItemDataProperties - pItemData was not found", LOGTYPE_Error);
		return;
	}

	delete m_PropertiesMap[pItemData];
	m_PropertiesMap.remove(pItemData);
}

// NOTE: These properties get set to the proper harmony node within EntityDrawItem::RefreshJson
//		 Updates here should reflect to the function above
void EntityStateData::InitalizePropertyModel(EntityTreeItemData *pItemData, PropertiesTreeModel &propertiesTreeModelRef)
{
	if(pItemData->GetType() == ITEM_Prefix) // aka Shapes folder
		return;

	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	propertiesTreeModelRef.AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
	propertiesTreeModelRef.AppendProperty("Common", "UUID", PROPERTIESTYPE_LineEdit, pItemData->GetThisUuid().toString(QUuid::WithoutBraces), "The universally unique identifier of the Project Item this node represents", true);

	if(pItemData->GetType() != ITEM_BoundingVolume)
	{
		if(pItemData->IsAssetItem() == false)
			propertiesTreeModelRef.AppendProperty("Common", "State", PROPERTIESTYPE_StatesComboBox, 0, "The " % HyGlobal::ItemName(pItemData->GetType(), false) % "'s state to be displayed", false, QVariant(), QVariant(), QVariant(), QString(), QString(), pItemData->GetReferencedItemUuid());
		propertiesTreeModelRef.AppendProperty("Common", "Update During Paused", PROPERTIESTYPE_bool, Qt::Unchecked, "Only items with this checked will receive updates when the game/application is paused");
		propertiesTreeModelRef.AppendProperty("Common", "User Tag", PROPERTIESTYPE_int, 0, "Not used by Harmony. You can set it to anything you like", false, -iRANGE, iRANGE, 1);

		propertiesTreeModelRef.AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
		propertiesTreeModelRef.AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", false, -fRANGE, fRANGE, 1.0, "[", "]");
		propertiesTreeModelRef.AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", false, -fRANGE, fRANGE, 0.01, "[", "]");
		propertiesTreeModelRef.AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", false, 0.0, 360.0, 0.1, "", "°");

		if(pItemData->GetType() != ITEM_Audio)
		{
			propertiesTreeModelRef.AppendCategory("Body", HyGlobal::ItemColor(ITEM_Prefix));
			propertiesTreeModelRef.AppendProperty("Body", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated");
			propertiesTreeModelRef.AppendProperty("Body", "Color Tint", PROPERTIESTYPE_Color, QRect(255, 255, 255, 255), "A color to alpha blend this item with");
			propertiesTreeModelRef.AppendProperty("Body", "Alpha", PROPERTIESTYPE_double, 1.0, "A value from 0.0 to 1.0 that indicates how opaque/transparent this item is", false, 0.0, 1.0, 0.05);
			propertiesTreeModelRef.AppendProperty("Body", "Display Order", PROPERTIESTYPE_int, 0, "Higher display orders get drawn above other items with less. Undefined ordering when equal", false, -iRANGE, iRANGE, 1);
		}
	}

	switch(pItemData->GetType())
	{
	case ITEM_Entity:
		propertiesTreeModelRef.AppendCategory("Physics", QVariant(), true, false, "Optionally create a physics component that can affect the transformation of this entity");
		propertiesTreeModelRef.AppendProperty("Physics", "Start Activated", PROPERTIESTYPE_bool, Qt::Checked, "This entity will start its physics simulation upon creation");
		propertiesTreeModelRef.AppendProperty("Physics", "Type", PROPERTIESTYPE_ComboBoxInt, 0, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Static" << "Kinematic" << "Dynamic");
		propertiesTreeModelRef.AppendProperty("Physics", "Fixed Rotation", PROPERTIESTYPE_bool, Qt::Unchecked, "Prevents this body from rotating if checked. Useful for characters");
		propertiesTreeModelRef.AppendProperty("Physics", "Initially Awake", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to make body initially awake. Start sleeping otherwise");
		propertiesTreeModelRef.AppendProperty("Physics", "Allow Sleep", PROPERTIESTYPE_bool, Qt::Checked, "Uncheck this if this body should never fall asleep. This increases CPU usage");
		propertiesTreeModelRef.AppendProperty("Physics", "Gravity Scale", PROPERTIESTYPE_double, 1.0, "Adjusts the gravity on this single body. Negative values will reverse gravity. Increased gravity can decrease stability", false, -100.0, 100.0, 0.1);
		propertiesTreeModelRef.AppendProperty("Physics", "Dynamic CCD", PROPERTIESTYPE_bool, Qt::Unchecked, "Continuous collision detection for other dynamic moving bodies. Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies. You should use this flag sparingly since it increases processing time");
		propertiesTreeModelRef.AppendProperty("Physics", "Linear Damping", PROPERTIESTYPE_double, 0.0, "Reduces the world linear velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		propertiesTreeModelRef.AppendProperty("Physics", "Angular Damping", PROPERTIESTYPE_double, 0.01, "Reduces the world angular velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		propertiesTreeModelRef.AppendProperty("Physics", "Linear Velocity", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Starting Linear velocity of the body's origin in scene coordinates", false, -fRANGE, fRANGE, 1.0, "[", "]");
		propertiesTreeModelRef.AppendProperty("Physics", "Angular Velocity", PROPERTIESTYPE_double, 0.0, "Starting Angular velocity of the body", false, 0.0, 100.0, 0.01);
		break;

	case ITEM_Primitive:
		propertiesTreeModelRef.AppendCategory("Primitive", QVariant(), false, false, "A visible shape that can be drawn to the screen");
		propertiesTreeModelRef.AppendProperty("Primitive", "Wireframe", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to render only the wireframe of the shape type");
		propertiesTreeModelRef.AppendProperty("Primitive", "Line Thickness", PROPERTIESTYPE_double, 1.0, "When applicable, how thick to render lines", false, 1.0, 100.0, 1.0);
		propertiesTreeModelRef.AppendCategory("Shape", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc");
		propertiesTreeModelRef.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", false, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetShapeNameList());
		propertiesTreeModelRef.AppendProperty("Shape", "Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", true);
		break;

	case ITEM_BoundingVolume:
		propertiesTreeModelRef.AppendCategory("Shape", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc");
		propertiesTreeModelRef.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", false, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetShapeNameList());
		propertiesTreeModelRef.AppendProperty("Shape", "Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", true);
		propertiesTreeModelRef.AppendCategory("Fixture", QVariant(), true, true, "Become a fixture used in physics simulations and collision");
		propertiesTreeModelRef.AppendProperty("Fixture", "Density", PROPERTIESTYPE_double, 0.0, "Usually in kg / m^2. A shape should have a non-zero density when the entity's physics is dynamic", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Friction", PROPERTIESTYPE_double, 0.2, "The friction coefficient, usually in the range [0,1]", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Restitution", PROPERTIESTYPE_double, 0.0, "The restitution (elasticity) usually in the range [0,1]", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Restitution Threshold", PROPERTIESTYPE_double, 1.0, "Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Sensor", PROPERTIESTYPE_bool, Qt::Unchecked, "A sensor shape collects contact information but never generates a collision response");
		propertiesTreeModelRef.AppendProperty("Fixture", "Filter: Category Mask", PROPERTIESTYPE_int, 0x0001, "The collision category bits for this shape. Normally you would just set one bit", false, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		propertiesTreeModelRef.AppendProperty("Fixture", "Filter: Collision Mask", PROPERTIESTYPE_int, 0xFFFF, "The collision mask bits. This states the categories that this shape would accept for collision", false, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		propertiesTreeModelRef.AppendProperty("Fixture", "Filter: Group Override", PROPERTIESTYPE_int, 0, "Collision overrides allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision override", false, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max(), 1, QString(), QString(), QVariant());
		break;

	case ITEM_AtlasFrame:
		// No texture quad specific properties
		break;

	case ITEM_Text:
		propertiesTreeModelRef.AppendCategory("Text", pItemData->GetReferencedItemUuid().toString(QUuid::WithoutBraces));
		propertiesTreeModelRef.AppendProperty("Text", "Text", PROPERTIESTYPE_LineEdit, "Text123", "What UTF-8 string to be displayed", false);
		propertiesTreeModelRef.AppendProperty("Text", "Style", PROPERTIESTYPE_ComboBoxString, HyGlobal::GetTextStyleNameList()[TEXTSTYLE_Line], "The style of how the text is shown", false, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTextStyleNameList());
		propertiesTreeModelRef.AppendProperty("Text", "Style Dimensions", PROPERTIESTYPE_vec2, QPointF(200.0f, 50.0f), "Text box size used when required by the style (like ScaleBox or Column)", false, 0.0f, fRANGE, 1.0f);
		propertiesTreeModelRef.AppendProperty("Text", "Alignment", PROPERTIESTYPE_ComboBoxString, HyGlobal::GetAlignmentNameList()[HYALIGN_Left], "The alignment of the text", false, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetAlignmentNameList());
		propertiesTreeModelRef.AppendProperty("Text", "Monospaced Digits", PROPERTIESTYPE_bool, false, "Check to use monospaced digits, which ensures all digits use the same width", false);
		propertiesTreeModelRef.AppendProperty("Text", "Text Indent", PROPERTIESTYPE_int, 0, "The number of pixels to indent the text", false, 0, iRANGE, 1);
		break; 

	case ITEM_Sprite:
		propertiesTreeModelRef.AppendCategory("Sprite", pItemData->GetReferencedItemUuid().toString(QUuid::WithoutBraces));
		propertiesTreeModelRef.AppendProperty("Sprite", "Frame", PROPERTIESTYPE_SpriteFrames, 0, "The sprite frame index to start on", false, QVariant(), QVariant(), QVariant(), QString(), QString(), pItemData->GetReferencedItemUuid());
		propertiesTreeModelRef.AppendProperty("Sprite", "Anim Rate", PROPERTIESTYPE_double, 1.0, "The animation rate modifier", false, 0.0, fRANGE, 0.1);
		propertiesTreeModelRef.AppendProperty("Sprite", "Anim Paused", PROPERTIESTYPE_bool, false, "The current state's animation starts paused");
		break;

	default:
		HyGuiLog(QString("EntityTreeItem::InitalizePropertiesTree - unsupported type: ") % QString::number(pItemData->GetType()), LOGTYPE_Error);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_TreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta["UUID"].toString(), this),
	m_bVertexEditMode(false)
{
	// First initialize the states so they exist before we try to add properties to them
	InitStates<EntityStateData>(itemFileDataRef);

	// Each element in QList<> represents a state's properties for all the children or shapes
	QList<QJsonArray> propChildArrayList;
	QList<QJsonArray> propShapeArrayList;
	QJsonArray stateArray = itemFileDataRef.m_Meta["stateArray"].toArray();
	for(int i = 0; i < stateArray.size(); ++i)
	{
		QJsonObject stateObj = stateArray[i].toObject();
		propChildArrayList.push_back(stateObj["propChildList"].toArray());
		propShapeArrayList.push_back(stateObj["propShapeList"].toArray());
	}
	if(stateArray.size() != 0 && (propChildArrayList.size() != GetNumStates() || propShapeArrayList.size() != GetNumStates()))
		HyGuiLog("EntityModel::EntityModel - invalid number of states when parsing properties", LOGTYPE_Error);
	
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
	fpPopulateNodeTreeItems(itemFileDataRef.m_Meta["childList"].toArray(), propChildArrayList);
	fpPopulateNodeTreeItems(itemFileDataRef.m_Meta["shapeList"].toArray(), propShapeArrayList);
	
	//QJsonArray shapeListArray = itemFileDataRef.m_Meta["shapeList"].toArray();
	//for(int i = 0; i < shapeListArray.size(); ++i)
	//{
	//	if(shapeListArray[i].isObject())
	//	{
	//		QJsonObject shapeObj = shapeListArray[i].toObject();
	//		Cmd_AddNewItem(shapeObj, false, i);
	//	}
	//	else if(shapeListArray[i].isArray())
	//	{
	//		QJsonArray shapeArray = shapeListArray[i].toArray();
	//		for(int j = 0; j < shapeArray.size(); ++j)
	//			Cmd_AddNewItem(shapeArray[j].toObject(), true, j == 0 ? i : j);
	//	}
	//	else
	//		HyGuiLog("EntityModel::EntityModel invalid shapeList", LOGTYPE_Error);
	//}
}

/*virtual*/ EntityModel::~EntityModel()
{
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

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);
	
	return treeNodeList;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewAssets(QList<IAssetItemData *> assetItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pAssetItem : assetItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_InsertNewAsset(pAssetItem, "m_", iRow);
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
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_InsertNewItem(descObj, propsArray, bIsArrayItem, iRow);

	QUuid uuidToRegister(descObj["itemUUID"].toString());
	if(uuidToRegister.isNull() == false)
		m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << uuidToRegister);

	return pTreeItemData;
}

EntityTreeItemData *EntityModel::Cmd_AddNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_InsertNewShape(eShape, sData, bIsPrimitive, "m_", iRow);
	
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

/*virtual*/ void EntityModel::OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) /*override*/
{
	EntityTreeItemData *pEntityTreeData = reinterpret_cast<EntityTreeItemData *>(propertiesModelRef.GetSubstate().toLongLong());
}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	// Save generated C++ class in a .h
	//m_ItemRef.GetProject().GetSourceAbsPath()

	return true;
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	itemSpecificFileDataOut.m_Meta.insert("codeName", m_TreeModel.GetRootTreeItemData()->GetCodeName());
	
	InsertChildAndShapeList(-1, itemSpecificFileDataOut);
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
	InsertChildAndShapeList(uiIndex, stateFileDataOut);
}

void EntityModel::InsertChildAndShapeList(int iStateIndex, FileDataPair &fileDataPairOut) const
{
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	m_TreeModel.GetTreeItemData(childList, shapeList);

	QJsonArray childArray;
	QString sCurrentArrayCodeName = "";
	for(int i = 0; i < childList.size(); )
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
		fileDataPairOut.m_Meta.insert("childList", childArray);
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
		fileDataPairOut.m_Meta.insert("shapeList", shapeArray);
	else
		fileDataPairOut.m_Meta.insert("propShapeList", shapeArray);
}
