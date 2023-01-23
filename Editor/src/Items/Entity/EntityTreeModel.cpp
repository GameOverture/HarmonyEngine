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

EntityTreeItemData::EntityTreeItemData(ProjectItemData &entityItemDataRef, QString sCodeName, HyGuiItemType eItemType, QUuid uuidOfItem) :
	TreeModelItemData(eItemType, sCodeName),
	m_Uuid(QUuid::createUuid()),
	m_ItemUuid(uuidOfItem),
	m_PropertiesTreeModel(entityItemDataRef, 0, QVariant(reinterpret_cast<qulonglong>(this))),
	m_bIsSelected(false)
{
	InitalizePropertiesTree();
}

EntityTreeItemData::EntityTreeItemData(ProjectItemData &entityItemDataRef, QJsonObject initObj) :
	TreeModelItemData(HyGlobal::GetTypeFromString(initObj["itemType"].toString()), initObj["codeName"].toString()),
	m_Uuid(initObj["UUID"].toString()),
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

QString EntityTreeItemData::GetCodeName() const
{
	return m_sName;
}

QUuid EntityTreeItemData::GetThisUuid() const
{
	return m_Uuid;
}

QUuid EntityTreeItemData::GetItemUuid() const
{
	return m_ItemUuid;
}

PropertiesTreeModel &EntityTreeItemData::GetPropertiesModel()
{
	return m_PropertiesTreeModel;
}

void EntityTreeItemData::SetSelected(bool bIsSelected)
{
	m_bIsSelected = bIsSelected;
}

void EntityTreeItemData::InsertJsonInfo(QJsonObject &childObjRef)
{
	childObjRef = m_PropertiesTreeModel.SerializeJson(); // Tree item specific stuff

	// Common stuff
	childObjRef.insert("codeName", GetCodeName());
	childObjRef.insert("itemType", HyGlobal::ItemName(m_eTYPE, false));
	childObjRef.insert("UUID", m_Uuid.toString(QUuid::WithoutBraces));
	childObjRef.insert("itemUUID", m_ItemUuid.toString(QUuid::WithoutBraces));
	childObjRef.insert("isSelected", m_bIsSelected);
}

void EntityTreeItemData::InitalizePropertiesTree()
{
	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	m_PropertiesTreeModel.AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
	m_PropertiesTreeModel.AppendProperty("Common", "UUID", PROPERTIESTYPE_LineEdit, m_Uuid.toString(QUuid::WithoutBraces), "The universally unique identifier of the Project Item this node represents", true);

	if(m_eTYPE != ITEM_Shape)
	{
		if(m_eTYPE != ITEM_Audio)
		{
			m_PropertiesTreeModel.AppendProperty("Common", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated");
			m_PropertiesTreeModel.AppendProperty("Common", "Display Order", PROPERTIESTYPE_int, 0, "Higher display orders get drawn above other items with less. Undefined ordering when equal", false, -iRANGE, iRANGE, 1);
		}
		m_PropertiesTreeModel.AppendProperty("Common", "Update During Paused", PROPERTIESTYPE_bool, Qt::Unchecked, "Only items with this checked will receive updates when the game/application is paused");
		m_PropertiesTreeModel.AppendProperty("Common", "User Tag", PROPERTIESTYPE_int, 0, "Not used by Harmony. You can set it to anything you like", false, -iRANGE, iRANGE, 1);

		m_PropertiesTreeModel.AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
		m_PropertiesTreeModel.AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", false, -fRANGE, fRANGE, 1.0, "[", "]");
		m_PropertiesTreeModel.AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", false, -fRANGE, fRANGE, 0.01, "[", "]");
		m_PropertiesTreeModel.AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", false, 0.0, 360.0, 0.1, "", "°");
	}

	switch(m_eTYPE)
	{
	case ITEM_Entity:
		m_PropertiesTreeModel.AppendCategory("Physics", QVariant(), true, false, "Optionally create a physics component that can affect the transformation of this entity");
		m_PropertiesTreeModel.AppendProperty("Physics", "Start Activated", PROPERTIESTYPE_bool, Qt::Checked, "This entity will start its physics simulation upon creation");
		m_PropertiesTreeModel.AppendProperty("Physics", "Type", PROPERTIESTYPE_ComboBox, 0, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Static" << "Kinematic" << "Dynamic");
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
		m_PropertiesTreeModel.AppendCategory("Primitive", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc.");
		m_PropertiesTreeModel.AppendProperty("Primitive", "Shape Type", PROPERTIESTYPE_ComboBox, 0, "The type of shape this is", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Nothing" << "Box" << "Circle" << "Polygon" << "Line" << "Line Chain" << "Line Loop");
		m_PropertiesTreeModel.AppendProperty("Primitive", "Shape Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", true);
		m_PropertiesTreeModel.AppendProperty("Primitive", "Wireframe", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to render only the wireframe of the shape type");
		m_PropertiesTreeModel.AppendProperty("Primitive", "Line Thickness", PROPERTIESTYPE_double, 1.0, "When applicable, how thick to render lines", false, 1.0, 100.0, 1.0);
		break;

	case ITEM_Shape:
		m_PropertiesTreeModel.AppendCategory("Shape", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc.");
		m_PropertiesTreeModel.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBox, 0, "The type of shape this is", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Nothing" << "Box" << "Circle" << "Polygon" << "Line" << "Line Chain" << "Line Loop");
		m_PropertiesTreeModel.AppendProperty("Shape", "Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", true);
		m_PropertiesTreeModel.AppendProperty("Shape", "Density", PROPERTIESTYPE_double, 0.0, "Usually in kg / m^2. A shape should have a non-zero density when the entity's physics is dynamic", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Shape", "Friction", PROPERTIESTYPE_double, 0.2, "The friction coefficient, usually in the range [0,1]", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Shape", "Restitution", PROPERTIESTYPE_double, 0.0, "The restitution (elasticity) usually in the range [0,1]", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Shape", "Restitution Threshold", PROPERTIESTYPE_double, 1.0, "Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)", false, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_PropertiesTreeModel.AppendProperty("Shape", "Sensor", PROPERTIESTYPE_bool, Qt::Unchecked, "A sensor shape collects contact information but never generates a collision response");
		m_PropertiesTreeModel.AppendProperty("Shape", "Filter: Category Mask", PROPERTIESTYPE_int, 0x0001, "The collision category bits for this shape. Normally you would just set one bit", false, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_PropertiesTreeModel.AppendProperty("Shape", "Filter: Collision Mask", PROPERTIESTYPE_int, 0xFFFF, "The collision mask bits. This states the categories that this shape would accept for collision", false, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_PropertiesTreeModel.AppendProperty("Shape", "Filter: Group Override", PROPERTIESTYPE_int, 0, "Collision overrides allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision override", false, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max(), 1, QString(), QString(), QVariant());
		break;

	case ITEM_AtlasImage:
		m_PropertiesTreeModel.AppendCategory("Textured Quad");
		break;

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
	EntityTreeItemData *pThisEntityItem = new EntityTreeItemData(m_ModelRef.GetItem(), sEntityCodeName, ITEM_Entity, uuidOfEntity);
	QVariant v;
	v.setValue<EntityTreeItemData *>(pThisEntityItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(0, iCol, QModelIndex()), v, Qt::UserRole) == false)
			HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);
	}
}

/*virtual*/ EntityTreeModel::~EntityTreeModel()
{
}

TreeModelItem *EntityTreeModel::GetEntityTreeItem() const
{
	if(m_pRootItem->GetNumChildren() == 0)
		return nullptr;

	return m_pRootItem->GetChild(0);
}

EntityTreeItemData *EntityTreeModel::GetEntityTreeItemData() const
{
	if(m_pRootItem->GetNumChildren() == 0)
		return nullptr;

	return m_pRootItem->GetChild(0)->data(0).value<EntityTreeItemData *>();
}

void EntityTreeModel::GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &shapeListOut) const
{
	TreeModelItem *pThisEntity = GetEntityTreeItem();
	for(int i = 0; i < pThisEntity->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pThisEntity->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;
		
		if(pCurItem->GetType() != ITEM_Shape)
			childListOut.push_back(pCurItem);
		else
			shapeListOut.push_back(pCurItem);
	}
}

EntityTreeItemData *EntityTreeModel::FindTreeItemData(QUuid uuid) const
{
	TreeModelItem *pThisEntity = GetEntityTreeItem();
	for(int i = 0; i < pThisEntity->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pThisEntity->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;

		if(pCurItem->GetThisUuid() == uuid)
			return pCurItem;
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
	if(&m_ModelRef.GetItem() == pItem)
	{
		if(bShowDialogsOnFail)
			HyGuiLog("Entity cannot add itself as a child", LOGTYPE_Error);
		return false;
	}
	if(HyGlobal::GetProjItemTypeList().contains(pItem->GetType()) == false)
	{
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is not a valid child type: " % QString::number(pItem->GetType()), LOGTYPE_Error);
		return false;
	}
	if(pItem->GetType() == ITEM_Entity)
	{
		// TODO: Ensure that this child entity doesn't contain this as child
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is invalid to be added. This Entity contains itself as a child", LOGTYPE_Error);

		return false;
	}

	return true;
}

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	TreeModelItem *pParentTreeItem = GetItem(index(0, 0, QModelIndex()));
	QModelIndex parentIndex = FindIndex<EntityTreeItemData *>(pParentTreeItem->data(0).value<EntityTreeItemData *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);
	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityTreeModel::InsertNewChild() - insertRow failed", LOGTYPE_Error);
		return nullptr;
	}

	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pProjItem->GetName(false));
	
	// Allocate and store the new item in the tree model
	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef.GetItem(), sCodeName, pProjItem->GetType(), pProjItem->GetUuid());
	QVariant v;
	v.setValue<EntityTreeItemData *>(pNewItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(iRow, iCol, parentIndex), v, Qt::UserRole) == false)
			HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);
	}

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewChild(QJsonObject initObj, int iRow /*= -1*/)
{
	HyGuiItemType eGuiType = HyGlobal::GetTypeFromString(initObj["itemType"].toString());
	QString sCodeName = initObj["codeName"].toString();

	TreeModelItem *pParentTreeItem = GetItem(index(0, 0, QModelIndex()));
	QModelIndex parentIndex = FindIndex<EntityTreeItemData *>(pParentTreeItem->data(0).value<EntityTreeItemData *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);
	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityTreeModel::InsertNewChild() - insertRow failed", LOGTYPE_Error);
		return nullptr;
	}

	// Allocate and store the new item in the tree model
	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef.GetItem(), initObj);
	QVariant v;
	v.setValue<EntityTreeItemData *>(pNewItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(iRow, iCol, parentIndex), v, Qt::UserRole) == false)
			HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);
	}

	return pNewItem;
}

bool EntityTreeModel::Cmd_InsertChild(EntityTreeItemData *pItem, int iRow)
{
	TreeModelItem *pParentTreeItem = GetItem(index(0, 0, QModelIndex()));
	QModelIndex parentIndex = FindIndex<EntityTreeItemData *>(pParentTreeItem->data(0).value<EntityTreeItemData *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);
	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityTreeModel::InsertChild() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(pItem->GetCodeName());

	QVariant v;
	v.setValue<EntityTreeItemData *>(pItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(iRow, iCol, parentIndex), v, Qt::UserRole) == false)
			HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);
	}

	return true;
}

int32 EntityTreeModel::Cmd_PopChild(EntityTreeItemData *pItem)
{
	TreeModelItem *pTreeItem = GetItem(FindIndex<EntityTreeItemData *>(pItem, 0));
	TreeModelItem *pParentTreeItem = pTreeItem->GetParent();

	int32 iRow = pTreeItem->GetIndex();
	if(removeRow(iRow, createIndex(pParentTreeItem->GetIndex(), 0, pParentTreeItem)) == false)
	{
		HyGuiLog("ExplorerModel::PopChild() - removeRow failed", LOGTYPE_Error);
		return -1;
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
			return pItem->GetCodeName();
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

			return QVariant(pItem->GetIcon(SUBICON_None));
		}
		else
			return QVariant();


	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant(pItem->GetThisUuid().toString());

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
