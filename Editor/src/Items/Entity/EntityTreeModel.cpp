/**************************************************************************
*	EntityTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityTreeModel.h"
#include "EntityModel.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "MainWindow.h"

#include <QVariant>

EntityTreeItemData::EntityTreeItemData(ProjectItemData &entityItemDataRef, bool bIsForwardDeclared, QString sCodeName, HyGuiItemType eItemType, EntityItemType eEntType, QUuid uuidOfItem, QUuid uuidOfThis) :
	TreeModelItemData(eItemType, uuidOfThis, sCodeName),
	m_eEntType(eEntType),
	m_bIsForwardDeclared(bIsForwardDeclared),
	m_ItemUuid(uuidOfItem),
	m_PropertiesTreeModel(entityItemDataRef, 0, QVariant(reinterpret_cast<qulonglong>(this))),
	m_bIsSelected(false)
{
	InitalizePropertiesTree();
}

EntityTreeItemData::EntityTreeItemData(ProjectItemData &entityItemDataRef, bool bIsForwardDeclared, QJsonObject initObj, bool bIsArrayItem) :
	TreeModelItemData(HyGlobal::GetTypeFromString(initObj["itemType"].toString()), initObj["Common"].toObject()["UUID"].toString(), initObj["codeName"].toString()),
	m_eEntType(bIsArrayItem ? ENTTYPE_ArrayItem : ENTTYPE_Item),
	m_bIsForwardDeclared(bIsForwardDeclared),
	m_ItemUuid(initObj["itemUUID"].toString()),
	m_PropertiesTreeModel(entityItemDataRef, 0, QVariant(reinterpret_cast<qulonglong>(this))),
	m_bIsSelected(initObj["isSelected"].toBool())
{
	InitalizePropertiesTree();
	m_PropertiesTreeModel.DeserializeJson(initObj);
}

/*virtual*/ EntityTreeItemData::~EntityTreeItemData()
{
}

EntityItemType EntityTreeItemData::GetEntType() const
{
	return m_eEntType;
}

QString EntityTreeItemData::GetCodeName() const
{
	return m_sName;
}

const QUuid &EntityTreeItemData::GetThisUuid() const
{
	return GetUuid();
}

const QUuid &EntityTreeItemData::GetItemUuid() const
{
	return m_ItemUuid;
}

bool EntityTreeItemData::IsForwardDeclared() const
{
	return m_bIsForwardDeclared;
}

PropertiesTreeModel &EntityTreeItemData::GetPropertiesModel()
{
	return m_PropertiesTreeModel;
}

bool EntityTreeItemData::IsSelected() const
{
	return m_bIsSelected;
}

void EntityTreeItemData::SetSelected(bool bIsSelected)
{
	m_bIsSelected = bIsSelected;
}

void EntityTreeItemData::InsertJsonInfo(QJsonObject &childObjRef)
{
	childObjRef = m_PropertiesTreeModel.SerializeJson(); // The UUID is serialized among this in: category "Common"; property "UUID"

	// Common stuff
	childObjRef.insert("codeName", GetCodeName());
	childObjRef.insert("itemType", HyGlobal::ItemName(m_eTYPE, false));
	childObjRef.insert("itemUUID", m_ItemUuid.toString(QUuid::WithoutBraces));
	childObjRef.insert("isSelected", m_bIsSelected);
}

// NOTE: These properties get set to the proper harmony node within EntityDrawItem::RefreshJson
//		 Updates here should reflect to the function above
void EntityTreeItemData::InitalizePropertiesTree()
{
	if(m_eTYPE == ITEM_Prefix) // aka Shapes folder
		return;

	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	m_PropertiesTreeModel.AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
	m_PropertiesTreeModel.AppendProperty("Common", "UUID", PROPERTIESTYPE_LineEdit, GetThisUuid().toString(QUuid::WithoutBraces), "The universally unique identifier of the Project Item this node represents", true);

	if(m_eTYPE != ITEM_Shape)
	{
		m_PropertiesTreeModel.AppendProperty("Common", "Update During Paused", PROPERTIESTYPE_bool, Qt::Unchecked, "Only items with this checked will receive updates when the game/application is paused");
		m_PropertiesTreeModel.AppendProperty("Common", "User Tag", PROPERTIESTYPE_int, 0, "Not used by Harmony. You can set it to anything you like", false, -iRANGE, iRANGE, 1);

		m_PropertiesTreeModel.AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
		m_PropertiesTreeModel.AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", false, -fRANGE, fRANGE, 1.0, "[", "]");
		m_PropertiesTreeModel.AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", false, -fRANGE, fRANGE, 0.01, "[", "]");
		m_PropertiesTreeModel.AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", false, 0.0, 360.0, 0.1, "", "°");

		if(m_eTYPE != ITEM_Audio)
		{
			m_PropertiesTreeModel.AppendCategory("Body", HyGlobal::ItemColor(ITEM_Prefix));
			m_PropertiesTreeModel.AppendProperty("Body", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated");
			m_PropertiesTreeModel.AppendProperty("Body", "Color Tint", PROPERTIESTYPE_Color, QRect(255, 255, 255, 255), "A color to alpha blend this item with");
			m_PropertiesTreeModel.AppendProperty("Body", "Alpha", PROPERTIESTYPE_double, 1.0, "A value from 0.0 to 1.0 that indicates how opaque/transparent this item is", false, 0.0, 1.0, 0.05);
			m_PropertiesTreeModel.AppendProperty("Body", "Display Order", PROPERTIESTYPE_int, 0, "Higher display orders get drawn above other items with less. Undefined ordering when equal", false, -iRANGE, iRANGE, 1);
		}
	}

	switch(m_eTYPE)
	{
	case ITEM_Entity:
		m_PropertiesTreeModel.AppendCategory("Physics", QVariant(), true, false, "Optionally create a physics component that can affect the transformation of this entity");
		m_PropertiesTreeModel.AppendProperty("Physics", "Start Activated", PROPERTIESTYPE_bool, Qt::Checked, "This entity will start its physics simulation upon creation");
		m_PropertiesTreeModel.AppendProperty("Physics", "Type", PROPERTIESTYPE_ComboBoxInt, 0, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Static" << "Kinematic" << "Dynamic");
		m_PropertiesTreeModel.AppendProperty("Physics", "Fixed Rotation", PROPERTIESTYPE_bool, Qt::Unchecked, "Prevents this body from rotating if checked. Useful for characters");
		m_PropertiesTreeModel.AppendProperty("Physics", "Initially Awake", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to make body initially awake. Start sleeping otherwise");
		m_PropertiesTreeModel.AppendProperty("Physics", "Allow Sleep", PROPERTIESTYPE_bool, Qt::Checked, "Uncheck this if this body should never fall asleep. This increases CPU usage");
		m_PropertiesTreeModel.AppendProperty("Physics", "Gravity Scale", PROPERTIESTYPE_double, 1.0, "Adjusts the gravity on this single body. Negative values will reverse gravity. Increased gravity can decrease stability", false, -100.0, 100.0, 0.1);
		m_PropertiesTreeModel.AppendProperty("Physics", "Dynamic CCD", PROPERTIESTYPE_bool, Qt::Unchecked, "Continuous collision detection for other dynamic moving bodies. Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies. You should use this flag sparingly since it increases processing time");
		m_PropertiesTreeModel.AppendProperty("Physics", "Linear Damping", PROPERTIESTYPE_double, 0.0, "Reduces the world linear velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		m_PropertiesTreeModel.AppendProperty("Physics", "Angular Damping", PROPERTIESTYPE_double, 0.01, "Reduces the world angular velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		m_PropertiesTreeModel.AppendProperty("Physics", "Linear Velocity", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Starting Linear velocity of the body's origin in scene coordinates", false, -fRANGE, fRANGE, 1.0, "[", "]");
		m_PropertiesTreeModel.AppendProperty("Physics", "Angular Velocity", PROPERTIESTYPE_double, 0.0, "Starting Angular velocity of the body", false, 0.0, 100.0, 0.01);
		break;

	case ITEM_Primitive:
		m_PropertiesTreeModel.AppendCategory("Primitive", QVariant(), false, false, "A visible shape that can be drawn to the screen");
		m_PropertiesTreeModel.AppendProperty("Primitive", "Wireframe", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to render only the wireframe of the shape type");
		m_PropertiesTreeModel.AppendProperty("Primitive", "Line Thickness", PROPERTIESTYPE_double, 1.0, "When applicable, how thick to render lines", false, 1.0, 100.0, 1.0);
		m_PropertiesTreeModel.AppendCategory("Shape", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc");
		m_PropertiesTreeModel.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", false, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetShapeNameList());
		m_PropertiesTreeModel.AppendProperty("Shape", "Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", true);
		break;

	case ITEM_Shape:
		m_PropertiesTreeModel.AppendCategory("Shape", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc");
		m_PropertiesTreeModel.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", false, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetShapeNameList());
		m_PropertiesTreeModel.AppendProperty("Shape", "Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", true);
		m_PropertiesTreeModel.AppendCategory("Fixture", QVariant(), true, true, "Become a fixture used in physics simulations and collision");
		m_PropertiesTreeModel.AppendProperty("Fixture", "Density", PROPERTIESTYPE_double, 0.0, "Usually in kg / m^2. A shape should have a non-zero density when the entity's physics is dynamic", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Fixture", "Friction", PROPERTIESTYPE_double, 0.2, "The friction coefficient, usually in the range [0,1]", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Fixture", "Restitution", PROPERTIESTYPE_double, 0.0, "The restitution (elasticity) usually in the range [0,1]", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Fixture", "Restitution Threshold", PROPERTIESTYPE_double, 1.0, "Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Fixture", "Sensor", PROPERTIESTYPE_bool, Qt::Unchecked, "A sensor shape collects contact information but never generates a collision response");
		m_PropertiesTreeModel.AppendProperty("Fixture", "Filter: Category Mask", PROPERTIESTYPE_int, 0x0001, "The collision category bits for this shape. Normally you would just set one bit", false, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_PropertiesTreeModel.AppendProperty("Fixture", "Filter: Collision Mask", PROPERTIESTYPE_int, 0xFFFF, "The collision mask bits. This states the categories that this shape would accept for collision", false, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_PropertiesTreeModel.AppendProperty("Fixture", "Filter: Group Override", PROPERTIESTYPE_int, 0, "Collision overrides allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision override", false, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max(), 1, QString(), QString(), QVariant());
		break;

	//case ITEM_AtlasImage:
	//	m_PropertiesTreeModel.AppendCategory("Textured Quad");
	//	break;

	case ITEM_Text:
		m_PropertiesTreeModel.AppendCategory("Text", m_ItemUuid.toString(QUuid::WithoutBraces));
		m_PropertiesTreeModel.AppendProperty("Text", "State", PROPERTIESTYPE_StatesComboBox, 0, "The text state to be displayed");
		m_PropertiesTreeModel.AppendProperty("Text", "Text", PROPERTIESTYPE_LineEdit, "Text123", "What UTF-8 string to be displayed", false);
		break;

	case ITEM_Sprite:
		m_PropertiesTreeModel.AppendCategory("Sprite", m_ItemUuid.toString(QUuid::WithoutBraces));
		m_PropertiesTreeModel.AppendProperty("Sprite", "State", PROPERTIESTYPE_StatesComboBox, 0, "The sprite state to be displayed");
		m_PropertiesTreeModel.AppendProperty("Sprite", "Frame", PROPERTIESTYPE_SpriteFrames, 0, "The sprite frame index to start on");
		break;

	default:
		HyGuiLog(QString("EntityTreeItem::InitalizePropertiesTree - unsupported type: ") % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EntityTreeModel::EntityTreeModel(EntityModel &modelRef, QString sEntityCodeName, QUuid uuidOfEntity, QObject *pParent /*= nullptr*/) :
	ITreeModel(NUMCOLUMNS, QStringList(), pParent),
	m_ModelRef(modelRef)
{
	// Insert self as root node
	if(insertRow(0, QModelIndex()) == false)
	{
		HyGuiLog("EntityTreeModel::EntityTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pThisEntityItem = new EntityTreeItemData(m_ModelRef.GetItem(), false, sEntityCodeName, ITEM_Entity, ENTTYPE_Root, uuidOfEntity, uuidOfEntity);
	QVariant v;
	v.setValue<EntityTreeItemData *>(pThisEntityItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(0, iCol, QModelIndex()), v, Qt::UserRole) == false)
			HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);
	}

	// Insert 'folder' to hold bounding volumes (shapes)
	if(insertRow(1, QModelIndex()) == false)
	{
		HyGuiLog("EntityTreeModel::EntityTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pShapeFolderItem = new EntityTreeItemData(m_ModelRef.GetItem(), false, "Bounding Volumes", ITEM_Prefix, ENTTYPE_BvFolder, QUuid(), QUuid());
	QVariant shapeData;
	shapeData.setValue<EntityTreeItemData *>(pShapeFolderItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(1, iCol, QModelIndex()), shapeData, Qt::UserRole) == false)
			HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);
	}
}

/*virtual*/ EntityTreeModel::~EntityTreeModel()
{
}

TreeModelItem *EntityTreeModel::GetRootTreeItem() const
{
	if(m_pRootItem->GetNumChildren() == 0)
		return nullptr;

	return m_pRootItem->GetChild(0);
}

EntityTreeItemData *EntityTreeModel::GetRootTreeItemData() const
{
	if(m_pRootItem->GetNumChildren() == 0)
		return nullptr;

	return m_pRootItem->GetChild(0)->data(0).value<EntityTreeItemData *>();
}

TreeModelItem *EntityTreeModel::GetBvFolderTreeItem() const
{
	if(m_pRootItem->GetNumChildren() < 2)
		return nullptr;

	return m_pRootItem->GetChild(1);
}

EntityTreeItemData *EntityTreeModel::GetBvFolderTreeItemData() const
{
	if(m_pRootItem->GetNumChildren() < 2)
		return nullptr;

	return m_pRootItem->GetChild(1)->data(0).value<EntityTreeItemData *>();
}

EntityTreeItemData *EntityTreeModel::GetArrayFolderTreeItemData(EntityTreeItemData *pArrayItem) const
{
	if(pArrayItem->GetEntType() != ENTTYPE_ArrayItem)
	{
		HyGuiLog("EntityTreeModel::GetArrayFolderTreeItemData was passed a non-array item", LOGTYPE_Error);
		return nullptr;
	}

	TreeModelItem *pParentFolderItem = (pArrayItem->GetType() == ITEM_Shape) ? GetBvFolderTreeItem() : GetRootTreeItem();
	for(int i = 0; i < pParentFolderItem->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pSubItem = pParentFolderItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pArrayItem->GetCodeName() == pSubItem->GetCodeName() && pSubItem->GetEntType() == ENTTYPE_ArrayFolder)
			return pSubItem;
	}

	HyGuiLog("EntityTreeModel::GetArrayFolderTreeItemData array folder was not found", LOGTYPE_Error);
	return nullptr;
}

void EntityTreeModel::GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &shapeListOut) const
{
	TreeModelItem *pThisEntity = GetRootTreeItem();
	for(int i = 0; i < pThisEntity->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pThisEntity->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;

		if(pCurItem->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisEntity->GetChild(i);
			for(int j = 0; j < pArrayFolder->GetNumChildren(); ++j)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(j)->data(0).value<EntityTreeItemData *>();
				childListOut.push_back(pArrayItem);
			}
		}
		else
			childListOut.push_back(pCurItem);
	}

	TreeModelItem *pThisShapesFolder = GetBvFolderTreeItem();
	for(int i = 0; i < pThisShapesFolder->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurShape = pThisShapesFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurShape == nullptr)
			continue;

		if(pCurShape->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisShapesFolder->GetChild(i);
			for(int j = 0; j < pArrayFolder->GetNumChildren(); ++j)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(j)->data(0).value<EntityTreeItemData *>();
				shapeListOut.push_back(pArrayItem);
			}
		}
		else
			shapeListOut.push_back(pCurShape);
	}
}

EntityTreeItemData *EntityTreeModel::FindTreeItemData(QUuid uuid) const
{
	TreeModelItem *pThisEntity = GetRootTreeItem();
	for(int i = 0; i < pThisEntity->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pThisEntity->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;

		if(pCurItem->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisEntity->GetChild(i);
			for(int i = 0; i < pArrayFolder->GetNumChildren(); ++i)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
				if(pArrayItem->GetThisUuid() == uuid)
					return pArrayItem;
			}
		}

		if(pCurItem->GetThisUuid() == uuid)
			return pCurItem;
	}

	TreeModelItem *pThisShapeFolder = GetBvFolderTreeItem();
	for(int i = 0; i < pThisShapeFolder->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurShape = pThisShapeFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurShape == nullptr)
			continue;

		if(pCurShape->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisShapeFolder->GetChild(i);
			for(int i = 0; i < pArrayFolder->GetNumChildren(); ++i)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
				if(pArrayItem->GetThisUuid() == uuid)
					return pArrayItem;
			}
		}

		if(pCurShape->GetThisUuid() == uuid)
			return pCurShape;
	}

	return nullptr;
}

bool EntityTreeModel::IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const
{
	if(pItem == nullptr)
	{
		if(bShowDialogsOnFail)
			HyGuiLog("Entity tried to add a null item", LOGTYPE_Error);
		return false;
	}

	if(HyGlobal::GetProjItemTypeList().contains(pItem->GetType()) == false)
	{
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is not a valid child type: " % QString::number(pItem->GetType()), LOGTYPE_Error);
		return false;
	}

	return true;
}

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pProjItem->GetName(false));

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef.GetItem(), ShouldForwardDeclare(pProjItem), sCodeName, pProjItem->GetType(), ENTTYPE_Item, pProjItem->GetUuid(), QUuid::createUuid());
	InsertTreeItem(pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewChild(AssetItemData *pAssetItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pAssetItem->GetName());

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef.GetItem(), false, sCodeName, pAssetItem->GetManagerAssetType() == ASSET_Atlas ? ITEM_AtlasImage : ITEM_Audio, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewItem(QJsonObject initObj, bool bIsArrayItem, int iRow /*= -1*/)
{
	HyGuiItemType eGuiType = HyGlobal::GetTypeFromString(initObj["itemType"].toString());
	QString sCodeName = initObj["codeName"].toString();
	if(bIsArrayItem == false)
		sCodeName = GenerateCodeName(sCodeName);

	TreeModelItem *pParentTreeItem = nullptr;
	if(eGuiType != ITEM_Shape)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetBvFolderTreeItem();

	bool bFoundArrayFolder = false;
	if(bIsArrayItem)
		bFoundArrayFolder = FindOrCreateArrayFolder(pParentTreeItem, sCodeName, eGuiType, iRow);

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef.GetItem(), ShouldForwardDeclare(initObj), initObj, bIsArrayItem);
	iRow = (iRow < 0 || (bIsArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	InsertTreeItem(pNewItem, pParentTreeItem, iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + (bIsPrimitive ? "Prim" : "") + HyGlobal::ShapeName(eShape).simplified().remove(' '));

	TreeModelItem *pParentTreeItem = nullptr;
	if(bIsPrimitive)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetBvFolderTreeItem();

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef.GetItem(), false, sCodeName, bIsPrimitive ? ITEM_Primitive : ITEM_Shape, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	pNewItem->GetPropertiesModel().SetPropertyValue("Shape", "Type", HyGlobal::ShapeName(eShape));
	pNewItem->GetPropertiesModel().SetPropertyValue("Shape", "Data", sData);
	InsertTreeItem(pNewItem, pParentTreeItem, iRow);

	return pNewItem;
}

bool EntityTreeModel::Cmd_ReaddChild(EntityTreeItemData *pItem, int iRow)
{
	// Generate a unique code name for this new item
	//QString sCodeName = GenerateCodeName(pItem->GetCodeName());

	TreeModelItem *pParentTreeItem = nullptr;
	if(pItem->GetType() != ITEM_Shape)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetBvFolderTreeItem();

	bool bFoundArrayFolder = false;
	if(pItem->GetEntType() == ENTTYPE_ArrayItem)
		bFoundArrayFolder = FindOrCreateArrayFolder(pParentTreeItem, pItem->GetCodeName(), pItem->GetType(), iRow);

	iRow = (iRow < 0 || (pItem->GetEntType() == ENTTYPE_ArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	return InsertTreeItem(pItem, pParentTreeItem, iRow);
}

int32 EntityTreeModel::Cmd_PopChild(EntityTreeItemData *pItem)
{
	QModelIndex itemIndex = FindIndex<EntityTreeItemData *>(pItem, 0);
	if(itemIndex.isValid() == false)
	{
		HyGuiLog("EntityTreeModel::Cmd_PopChild could not find index for pItem", LOGTYPE_Error);
		return -1;
	}

	TreeModelItem *pTreeItem = GetItem(itemIndex);
	TreeModelItem *pParentTreeItem = pTreeItem->GetParent();

	int32 iRow = pTreeItem->GetIndex();
	QModelIndex parentIndex = createIndex(pParentTreeItem->GetIndex(), 0, pParentTreeItem);
	if(removeRow(iRow, parentIndex) == false)
	{
		HyGuiLog("ExplorerModel::PopChild() - removeRow failed", LOGTYPE_Error);
		return -1;
	}

	// If item removed was apart of an array, check if its array folder is now empty. If so, remove the array folder too
	EntityTreeItemData *pParentTreeItemData = pParentTreeItem->data(0).value<EntityTreeItemData *>();
	if(pParentTreeItemData->GetEntType() == ENTTYPE_ArrayFolder && pParentTreeItem->GetNumChildren() == 0)
	{
		TreeModelItem *pArrayFolderParent = pParentTreeItem->GetParent();
		
		int iArrayFolderRow = pParentTreeItem->GetIndex();
		if(removeRow(iArrayFolderRow, createIndex(pArrayFolderParent->GetIndex(), 0, pArrayFolderParent)) == false)
			HyGuiLog("ExplorerModel::PopChild() - removeRow failed for array folder", LOGTYPE_Error);

		iRow = iArrayFolderRow; // Return the ArrayFolder row so if the item gets readded, it'll recreate the ArrayFolder on the same row
	}

	return iRow;
}

QVariant EntityTreeModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	EntityTreeItemData *pItem = pTreeItem->data(0).value<EntityTreeItemData *>();
	ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(pItem->GetItemUuid());

	switch(iRole)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		if(indexRef.column() == COLUMN_CodeName)
		{
			if(pItem->GetEntType() == ENTTYPE_ArrayFolder)
				return pItem->GetCodeName() % "[" % QString::number(pTreeItem->GetNumChildren()) % "]";
			else if(pItem->GetEntType() == ENTTYPE_ArrayItem)
				return "[" % QString::number(pTreeItem->GetIndex()) % "] - " % pItem->GetThisUuid().toString(QUuid::StringFormat::WithoutBraces).split('-')[0];
			else
				return pItem->GetCodeName();
		}
		else // COLUMN_ItemPath
		{
			if(pProjItem)
				return pProjItem->GetName(true);
			else
				return QVariant();
		}

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(indexRef.column() == COLUMN_CodeName)
		{
			if(pProjItem && pProjItem->IsExistencePendingSave())
				return QVariant(pItem->GetIcon(SUBICON_New));
			else if(pProjItem && pProjItem->IsSaveClean() == false)
				return QVariant(pItem->GetIcon(SUBICON_Dirty));

			if(pItem->GetEntType() == ENTTYPE_ArrayFolder)
				return HyGlobal::ItemIcon(pItem->GetType(), SUBICON_Open);

			if(pItem->GetType() == ITEM_Primitive || pItem->GetType() == ITEM_Shape)
			{
				QIcon icon;
				QString sIconUrl = ":/icons16x16/shapes/" % QString(pItem->GetType() == ITEM_Primitive ? "primitive_" : "shapes_");
				switch(HyGlobal::GetShapeFromString(pItem->GetPropertiesModel().FindPropertyValue("Shape", "Type").toString()))
				{
				default:
				case SHAPE_None:
					return QVariant();
					
				case SHAPE_Box:			sIconUrl += "box.png"; break;
				case SHAPE_Circle:		sIconUrl += "circle.png"; break;
				case SHAPE_Polygon:		sIconUrl += "polygon.png"; break;
				case SHAPE_LineSegment:	sIconUrl += "lineSeg.png"; break;
				case SHAPE_LineChain:	sIconUrl += "lineChain.png"; break;
				case SHAPE_LineLoop:	sIconUrl += "lineLoop.png"; break;
				}

				icon.addFile(sIconUrl);
				return QVariant(icon);
			}
			
			return QVariant(pItem->GetIcon(SUBICON_None));
		}
		else
			return QVariant();


	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant();// QVariant(pItem->GetThisUuid().toString());

	default:
		return QVariant();
	}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags EntityTreeModel::flags(const QModelIndex &indexRef) const /*override*/
{
	return QAbstractItemModel::flags(indexRef);
}

/*virtual*/ void EntityTreeModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*override*/
{
}

QString EntityTreeModel::GenerateCodeName(QString sDesiredName) const
{
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	GetTreeItemData(childList, shapeList);
	childList += shapeList; // Just combine the two since they all need to be unique

	uint uiConflictCount = 0;
	bool bIsUnique = false;
	do
	{
		QString sFullCodeName = sDesiredName;
		if(uiConflictCount > 0)
			sFullCodeName += QString::number(uiConflictCount);

		int i = 0;
		for(; i < childList.size(); ++i)
		{
			if(childList[i] == nullptr)
				continue;

			if(sFullCodeName.compare(childList[i]->GetCodeName()) == 0)
			{
				uiConflictCount++;
				break;
			}
		}
		if(i == childList.size())
		{
			sDesiredName = sFullCodeName;
			bIsUnique = true;
		}

	} while(!bIsUnique);

	return sDesiredName;
}

bool EntityTreeModel::ShouldForwardDeclare(ProjectItemData *pProjItem)
{
	if(m_ModelRef.GetUuid() == pProjItem->GetUuid())
		return true;

	if(pProjItem->GetType() != ITEM_Entity)
		return false;
	
	FileDataPair projItemFileData;
	pProjItem->GetSavedFileData(projItemFileData);

	return false;
	//std::function<bool(const QJsonObject &entItemObj, const QUuid &thisUuid)> fpCheckForSelf =
	//	[](const QJsonObject &entItemObj, const QUuid &thisUuid) -> bool
	//{
	//	if(
	//	return QUuid::fromString(entItemObj["itemUUID"].toString()) == thisUuid;

	//};

	//QJsonArray childListArray = projItemFileData.m_Meta["childList"].toArray();
	//for(int i = 0; i < childListArray.size(); ++i)
	//{
	//	if(childListArray[i].isObject())
	//	{

	//	}
	//	else if(childListArray[i].isArray())
	//	{
	//		QJsonArray arrayFolder = childListArray[i].toArray();
	//		for(int j = 0; j < arrayFolder.size(); ++j)
	//		{
	//			arrayFolder[j].toObject()
	//		}
	//	}
	//}
	
}

bool EntityTreeModel::ShouldForwardDeclare(const QJsonObject &initObj)
{
	if(QUuid::fromString(initObj["itemUUID"].toString()) == m_ModelRef.GetUuid())
		return true;

	if(HyGlobal::GetTypeFromString(initObj["itemType"].toString()) == ITEM_Entity)
	{

	}

	return false;
}

bool EntityTreeModel::FindOrCreateArrayFolder(TreeModelItem *&pParentTreeItemOut, QString sCodeName, HyGuiItemType eItemType, int iRowToCreateAt)
{
	bool bFoundArrayFolder = false;

	// Find array tree item parent or create it
	for(int i = 0; i < pParentTreeItemOut->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pSubItem = pParentTreeItemOut->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(sCodeName == pSubItem->GetCodeName() && pSubItem->GetEntType() == ENTTYPE_ArrayFolder)
		{
			pParentTreeItemOut = pParentTreeItemOut->GetChild(i);
			bFoundArrayFolder = true;
			break;
		}
	}

	if(bFoundArrayFolder == false)
	{
		QModelIndex parentIndex = FindIndex<EntityTreeItemData *>(pParentTreeItemOut->data(0).value<EntityTreeItemData *>(), 0);
		int iArrayFolderRow = (iRowToCreateAt == -1 ? pParentTreeItemOut->GetNumChildren() : iRowToCreateAt);

		if(insertRow(iArrayFolderRow, parentIndex) == false)
		{
			HyGuiLog("EntityTreeModel::Cmd_InsertNewChild() - ArrayFolder insertRow failed", LOGTYPE_Error);
			return nullptr;
		}
		// Allocate and store the new array folder item in the tree model
		EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef.GetItem(), false, sCodeName, eItemType, ENTTYPE_ArrayFolder, QUuid(), QUuid());
		QVariant v;
		v.setValue<EntityTreeItemData *>(pNewItem);
		for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
		{
			if(setData(index(iArrayFolderRow, iCol, parentIndex), v, Qt::UserRole) == false)
				HyGuiLog("ExplorerModel::Cmd_InsertNewChild() - setData failed", LOGTYPE_Error);
		}

		pParentTreeItemOut = pParentTreeItemOut->GetChild(iArrayFolderRow);
	}

	return bFoundArrayFolder;
}
