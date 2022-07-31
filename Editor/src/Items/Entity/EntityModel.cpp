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

#include <QVariant>

EntityNodeItem::EntityNodeItem(ProjectItemData &entityItemDataRef, QString sCodeName, HyGuiItemType eItemType, QUuid uuidOfItem) :
	TreeModelItemData(eItemType, sCodeName),
	m_Uuid(uuidOfItem),
	m_PropertiesTreeModel(entityItemDataRef, 0, QVariant(reinterpret_cast<qulonglong>(this)))
{
	InitalizePropertiesTree();
}

EntityNodeItem::EntityNodeItem(ProjectItemData &entityItemDataRef, QJsonObject initObj) :
	TreeModelItemData(HyGlobal::GetTypeFromString(initObj["itemType"].toString()), initObj["codeName"].toString()),
	m_Uuid(initObj["itemUUID"].toString()),
	m_PropertiesTreeModel(entityItemDataRef, 0, QVariant(reinterpret_cast<qulonglong>(this)))
{
	InitalizePropertiesTree();
	m_PropertiesTreeModel.DeserializeJson(initObj);
}

/*virtual*/ EntityNodeItem::~EntityNodeItem()
{
}

QString EntityNodeItem::GetCodeName() const
{
	return m_sName;
}

QUuid EntityNodeItem::GetUuid() const
{
	return m_Uuid;
}

PropertiesTreeModel &EntityNodeItem::GetPropertiesModel()
{
	return m_PropertiesTreeModel;
}

void EntityNodeItem::InsertJsonInfo(QJsonObject &childObjRef)
{
	childObjRef = m_PropertiesTreeModel.SerializeJson();

	childObjRef.insert("codeName", GetCodeName());
	childObjRef.insert("itemType", HyGlobal::ItemName(m_eTYPE, false));
	childObjRef.insert("itemUUID", m_Uuid.toString(QUuid::WithoutBraces));

	//QPointF ptPos = m_PropertiesTreeModel.FindPropertyValue("Transformation", "Position").toPointF();
	//QPointF vScale = m_PropertiesTreeModel.FindPropertyValue("Transformation", "Scale").toPointF();

	//childObjRef.insert("pos", QJsonArray() << ptPos.x() << ptPos.y());
	//childObjRef.insert("rot", m_PropertiesTreeModel.FindPropertyValue("Transformation", "Rotation").toFloat());
	//childObjRef.insert("scale", QJsonArray() << vScale.x() << vScale.y());
	//childObjRef.insert("visible", m_PropertiesTreeModel.FindPropertyValue("Common", "Visible").toBool());
	//childObjRef.insert("pauseUpdate", m_PropertiesTreeModel.FindPropertyValue("Common", "Update During Paused").toBool());
	//childObjRef.insert("tag", m_PropertiesTreeModel.FindPropertyValue("Common", "User Tag").toInt());
	//childObjRef.insert("displayOrder", m_PropertiesTreeModel.FindPropertyValue("Common", "Display Order").toInt());

	//// Type specific key/values
	//switch(m_eTYPE)
	//{
	//case ITEM_Sprite:
	//	childObjRef.insert("stateIndex", m_PropertiesTreeModel.FindPropertyValue("Sprite", "State").toInt());
	//	childObjRef.insert("animFrame", m_PropertiesTreeModel.FindPropertyValue("Sprite", "Frame").toInt());
	//}
}

void EntityNodeItem::InitalizePropertiesTree()
{
	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	m_PropertiesTreeModel.AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
	m_PropertiesTreeModel.AppendProperty("Common", "UUID", PROPERTIESTYPE_LineEdit, m_Uuid.toString(QUuid::WithoutBraces), "The universally unique identifier of the Project Item this node represents", true);
	if(m_eTYPE != ITEM_Audio)
	{
		m_PropertiesTreeModel.AppendProperty("Common", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated");
		m_PropertiesTreeModel.AppendProperty("Common", "Display Order", PROPERTIESTYPE_int, 0, "Higher display orders get drawn above other items with less. Undefined ordering when equal", false, -iRANGE, iRANGE, 1);
	}
	m_PropertiesTreeModel.AppendProperty("Common", "Update During Paused", PROPERTIESTYPE_bool, Qt::Unchecked, "Only items with this checked will receive updates when the game/application is paused");
	m_PropertiesTreeModel.AppendProperty("Common", "User Tag", PROPERTIESTYPE_int, 0, "Not used by Harmony. You can set it to anything you like", false, -iRANGE, iRANGE, 1);

	m_PropertiesTreeModel.AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
	m_PropertiesTreeModel.AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", false, QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 1.0, "[", "]");
	m_PropertiesTreeModel.AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", false, QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 0.01, "[", "]");
	m_PropertiesTreeModel.AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", false, 0.0, 360.0, 0.1, "", "°");

	switch(m_eTYPE)
	{
	case ITEM_Entity:
		break;

	case ITEM_Primitive:
		break;

	case ITEM_AtlasImage:
		m_PropertiesTreeModel.AppendCategory("Textured Quad");
		break;

	case ITEM_Text:
		m_PropertiesTreeModel.AppendCategory("Text", m_Uuid.toString(QUuid::WithoutBraces));
		m_PropertiesTreeModel.AppendProperty("Text", "State", PROPERTIESTYPE_StatesComboBox, 0, "The text state to be displayed");
		break;

	case ITEM_Sprite:
		m_PropertiesTreeModel.AppendCategory("Sprite", m_Uuid.toString(QUuid::WithoutBraces));
		m_PropertiesTreeModel.AppendProperty("Sprite", "State", PROPERTIESTYPE_StatesComboBox, 0, "The sprite state to be displayed");
		m_PropertiesTreeModel.AppendProperty("Sprite", "Frame", PROPERTIESTYPE_SpriteFrames, 0, "The sprite frame index to start on");
		break;

	default:
		HyGuiLog(QString("EntityNodeItem::InitalizePropertiesTree - unsupported type: ") % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}

	if(m_eTYPE != ITEM_Audio)
	{
		m_PropertiesTreeModel.AppendCategory("Shape", QVariant(), true, false, "Specify this node's bounding volume");
		m_PropertiesTreeModel.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBox, 0, "The type of shape this bounding volume will be", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Box" << "Circle" << "Polygon" << "Line" << "Line Chain" << "Line Loop");

		m_PropertiesTreeModel.AppendCategory("Physics", QVariant(), true, false, "Optionally enable physics to be applied to above shape");
		m_PropertiesTreeModel.AppendProperty("Physics", "Type", PROPERTIESTYPE_ComboBox, 0, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Static" << "Kinematic" << "Dynamic");
		m_PropertiesTreeModel.AppendProperty("Physics", "Gravity Scale", PROPERTIESTYPE_double, 1.0, "Adjusts the gravity on this single body. Negative values will reverse gravity. Increased gravity can decrease stability", false, -100.0, 100.0, 0.1);
		m_PropertiesTreeModel.AppendProperty("Physics", "Linear Damping", PROPERTIESTYPE_double, 0.0, "Reduces the world linear velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		m_PropertiesTreeModel.AppendProperty("Physics", "Angular Damping", PROPERTIESTYPE_double, 0.01, "Reduces the world angular velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		m_PropertiesTreeModel.AppendProperty("Physics", "Dynamic CCD", PROPERTIESTYPE_bool, Qt::Unchecked, "Continuous collision detection for other dynamic moving bodies. Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies. You should use this flag sparingly since it increases processing time");
		m_PropertiesTreeModel.AppendProperty("Physics", "Fixed Rotation", PROPERTIESTYPE_bool, Qt::Unchecked, "Prevents this body from rotating if checked. Useful for characters");
		m_PropertiesTreeModel.AppendProperty("Physics", "Initially Awake", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to make body initially awake. Start sleeping otherwise");
		m_PropertiesTreeModel.AppendProperty("Physics", "Allow Sleep", PROPERTIESTYPE_bool, Qt::Checked, "Uncheck this if this body should never fall asleep. This increases CPU usage");
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

	EntityNodeItem *pNewItem = new EntityNodeItem(m_ModelRef.GetItem(), sEntityCodeName, ITEM_Entity, uuidOfEntity);

	QVariant v;
	v.setValue<EntityNodeItem *>(pNewItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(0, iCol, QModelIndex()), v, Qt::UserRole) == false)
			HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);
	}
}

/*virtual*/ EntityTreeModel::~EntityTreeModel()
{
}

QList<EntityNodeItem *> EntityTreeModel::GetChildrenNodes() const
{
	QList<EntityNodeItem *> nodeList;
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
		nodeList.push_back(m_pRootItem->GetChild(i)->data(0).value<EntityNodeItem *>());

	return nodeList;
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

EntityNodeItem *EntityTreeModel::InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	TreeModelItem *pParentTreeItem = GetItem(index(0, 0, QModelIndex()));
	QModelIndex parentIndex = FindIndex<EntityNodeItem *>(pParentTreeItem->data(0).value<EntityNodeItem *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);
	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityTreeModel::InsertNewChild() - insertRow failed", LOGTYPE_Error);
		return nullptr;
	}

	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pProjItem->GetName(false));
	
	// Allocate and store the new item in the tree model
	EntityNodeItem *pNewItem = new EntityNodeItem(m_ModelRef.GetItem(), sCodeName, pProjItem->GetType(), pProjItem->GetUuid());

	QVariant v;
	v.setValue<EntityNodeItem *>(pNewItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(iRow, iCol, parentIndex), v, Qt::UserRole) == false)
			HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);
	}

	return pNewItem;
}

bool EntityTreeModel::InsertChild(EntityNodeItem *pItem, int iRow)
{
	TreeModelItem *pParentTreeItem = GetItem(index(0, 0, QModelIndex()));
	QModelIndex parentIndex = FindIndex<EntityNodeItem *>(pParentTreeItem->data(0).value<EntityNodeItem *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);
	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityTreeModel::InsertChild() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(pItem->GetCodeName());

	QVariant v;
	v.setValue<EntityNodeItem *>(pItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(iRow, iCol, parentIndex), v, Qt::UserRole) == false)
			HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);
	}

	return true;
}

int32 EntityTreeModel::PopChild(EntityNodeItem *pItem)
{
	TreeModelItem *pTreeItem = GetItem(FindIndex<EntityNodeItem *>(pItem, 0));
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

	EntityNodeItem *pItem = pTreeItem->data(0).value<EntityNodeItem *>();
	ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(pItem->GetUuid());

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
		if(pProjItem && pProjItem->IsExistencePendingSave())
			return QVariant(pItem->GetIcon(SUBICON_New));
		else if(pProjItem && pProjItem->IsSaveClean() == false)
			return QVariant(pItem->GetIcon(SUBICON_Dirty));
		
		return QVariant(pItem->GetIcon(SUBICON_None));

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant(pItem->GetUuid().toString());

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
	QList<EntityNodeItem *> nodeList = GetChildrenNodes();
	uint uiConflictCount = 0;
	bool bIsUnique = false;
	do
	{
		QString sFullCodeName = sDesiredName;
		if(uiConflictCount > 0)
			sFullCodeName += QString::number(uiConflictCount);

		int i = 0;
		for(; i < nodeList.size(); ++i)
		{
			if(sFullCodeName.compare(nodeList[i]->GetCodeName()) == 0)
			{
				uiConflictCount++;
				break;
			}
		}
		if(i == nodeList.size())
		{
			sDesiredName = sFullCodeName;
			bIsUnique = true;
		}

	} while(!bIsUnique);

	return sDesiredName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	m_CodeNameMapper(this),
	m_EntityTypeMapper(this),
	m_TreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta["UUID"].toString(), this)
{
	InitStates<EntityStateData>(itemFileDataRef);
}

/*virtual*/ EntityModel::~EntityModel()
{
}

void EntityModel::RegisterWidgets(QLineEdit &txtCodeNameRef, QComboBox &cmbEntityTypeRef)
{
	m_CodeNameMapper.AddLineEditMapping(&txtCodeNameRef);
	m_EntityTypeMapper.AddComboBoxMapping(&cmbEntityTypeRef);
}

EntityTreeModel &EntityModel::GetNodeTreeModel()
{
	return m_TreeModel;
}

QList<EntityNodeItem *> EntityModel::Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow)
{
	QList<EntityNodeItem *> treeNodeList;
	for(auto *pItem : projItemList)
	{
		EntityNodeItem *pAddedItem = m_TreeModel.InsertNewChild(pItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_AddNewChildren could not insert a child: " % pItem->GetName(true), LOGTYPE_Error);
	}

	m_ItemRef.GetProject().RegisterItems(&m_ItemRef, projItemList);
	
	return treeNodeList;
}

bool EntityModel::Cmd_AddChild(EntityNodeItem *pNodeItem, int iRow)
{
	if(m_TreeModel.InsertChild(pNodeItem, iRow) == false)
		return false;

	ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(pNodeItem->GetUuid());
	if(pProjItem)
		m_ItemRef.GetProject().RegisterItems(&m_ItemRef, QList<ProjectItemData *>() << pProjItem);

	return true;
}

int32 EntityModel::Cmd_RemoveChild(EntityNodeItem *pItem)
{
	if(pItem == nullptr)
		return -1;

	int32 iRow = m_TreeModel.PopChild(pItem);
	if(iRow < 0)
		return iRow;

	ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(pItem->GetUuid());
	if(pProjItem)
		m_ItemRef.GetProject().RelinquishItems(&m_ItemRef, QList<ProjectItemData *>() << pProjItem);

	return iRow;
}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	return true;
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	itemSpecificFileDataOut.m_Meta.insert("codeName", m_CodeNameMapper.GetString());
	itemSpecificFileDataOut.m_Meta.insert("entityType", m_CodeNameMapper.GetString());
	
	QJsonArray childrenNodesArray;
	QList<EntityNodeItem *> childList = m_TreeModel.GetChildrenNodes();
	for(auto *pChild : childList)
	{
		QJsonObject childObj;
		pChild->InsertJsonInfo(childObj);
		childrenNodesArray.append(childObj);
	}

	itemSpecificFileDataOut.m_Meta.insert("childrenNodes", childrenNodesArray);
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
}

/*virtual*/ QList<AssetItemData *> EntityModel::GetAssets(AssetType eAssetType) const /*override*/
{
	// TODO: EntityModel::GetAssets
	return QList<AssetItemData *>();
}
