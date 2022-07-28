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

#include <QVariant>

EntityNodeItemData::EntityNodeItemData(HyGuiItemType eType, QString sName, QUuid uuid) :
	TreeModelItemData(eType, sName),
	m_ItemGuid(uuid)
{
}

/*virtual*/ EntityNodeItemData::~EntityNodeItemData()
{
}

QString EntityNodeItemData::GetCodeName() const
{
	return m_sName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EntityNodeTreeModel::EntityNodeTreeModel(EntityModel *pEntityModel, QObject *parent) :
	ITreeModel(NUMCOLUMNS, QStringList(), parent),
	m_pEntityModel(pEntityModel)
{
	// Insert self as root node
	if(insertRow(0, QModelIndex()) == false)
	{
		HyGuiLog("EntityNodeTreeModel::EntityNodeTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}

	EntityNodeItemData *pNewItem = new EntityNodeItemData(ITEM_Entity, "", pEntityModel->GetItem().GetUuid());
	m_NodeList.push_back(pNewItem); // Now always [0] in m_NodeList

	QVariant v;
	v.setValue<EntityNodeItemData *>(pNewItem);
	if(setData(index(0, 0, QModelIndex()), v, Qt::UserRole) == false)
		HyGuiLog("EntityNodeTreeModel::EntityNodeTreeModel() - setData failed", LOGTYPE_Error);
}

/*virtual*/ EntityNodeTreeModel::~EntityNodeTreeModel()
{
}

EntityNodeItemData *EntityNodeTreeModel::FindEntityNodeItem(ProjectItemData *pItem)
{
	//for(int32 i = 0; i < m_NodeList.size(); ++i)
	//{
	//	if(m_NodeList[i]->GetProjItem() == pItem)
	//		return m_NodeList[i];
	//}
	return nullptr;
}

bool EntityNodeTreeModel::IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const
{
	if(pItem == nullptr)
	{
		if(bShowDialogsOnFail)
			HyGuiLog("Entity tried to add a null item", LOGTYPE_Error);
		return false;
	}
	if(&m_pEntityModel->GetItem() == pItem)
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

bool EntityNodeTreeModel::InsertNewChild(ProjectItemData *pProjItem, TreeModelItem *pParentTreeItem /*= nullptr*/, int iRow /*= -1*/)
{
	if(pParentTreeItem == nullptr)
		pParentTreeItem = GetItem(index(0, 0, QModelIndex()));// FindIndex<EntityNodeItemData *>(&m_pEntityModel->GetItem(), 0));

	QModelIndex parentIndex = FindIndex<EntityNodeItemData *>(pParentTreeItem->data(0).value<EntityNodeItemData *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);

	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityNodeTreeModel::InsertNewChild() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	EntityNodeItemData *pNewItem = new EntityNodeItemData(pProjItem->GetType(), pProjItem->GetName(true), pProjItem->GetUuid());
	m_NodeList.push_back(pNewItem);

	QVariant v;
	v.setValue<EntityNodeItemData *>(pNewItem);
	if(setData(index(iRow, 0, parentIndex), v, Qt::UserRole) == false)
		HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);

	return true;
}

bool EntityNodeTreeModel::RemoveChild(EntityNodeItemData *pItem)
{
	TreeModelItem *pTreeItem = GetItem(FindIndex<EntityNodeItemData *>(pItem, 0));
	TreeModelItem *pParentTreeItem = pTreeItem->GetParent();
	if(m_NodeList.removeOne(pItem) == false)
	{
		HyGuiLog("ExplorerModel::RemoveChild() - m_NodeList.removeOne() failed", LOGTYPE_Error);
		return false;
	}

	if(removeRow(pTreeItem->GetIndex(), createIndex(pParentTreeItem->GetIndex(), 0, pParentTreeItem)) == false)
	{
		HyGuiLog("ExplorerModel::RemoveChild() - removeRow failed", LOGTYPE_Error);
		return false;
	}

	delete pItem;
	return true;
}

QVariant EntityNodeTreeModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	//EntityNodeItemData *pItem = pTreeItem->data(0).value<EntityNodeItemData *>();
	//switch(iRole)
	//{
	//case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	//case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
	//	return QVariant(pItem->GetProjItem()->GetName(false));

	//case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
	//	if(pItem->IsProjectItem())
	//	{
	//		if(pItem->GetProjItem()->IsExistencePendingSave())
	//			return QVariant(pItem->GetIcon(SUBICON_New));
	//		else if(pItem->GetProjItem()->IsSaveClean() == false)
	//			return QVariant(pItem->GetIcon(SUBICON_Dirty));
	//	}
	//	return QVariant(pItem->GetIcon(SUBICON_None));

	//case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
	//	return QVariant(pItem->GetProjItem()->GetName(true));

	//case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
	//	return QVariant(pItem->GetProjItem()->GetName(true));

	//default:
	//	return QVariant();
	//}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags EntityNodeTreeModel::flags(const QModelIndex &indexRef) const /*override*/
{
	return QAbstractItemModel::flags(indexRef);
}

/*virtual*/ void EntityNodeTreeModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*override*/
{
}

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

EntityModel::EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_TreeModel(this, this)
{
	InitStates<EntityStateData>(itemFileDataRef);
}

/*virtual*/ EntityModel::~EntityModel()
{
	for(auto iter = m_PropertiesMap.begin(); iter != m_PropertiesMap.end(); ++iter)
	{
		PropertiesTreeModel *pPropertiesModel = iter.value();
		delete pPropertiesModel;
	}

	m_PropertiesMap.clear();
}

EntityNodeTreeModel &EntityModel::GetNodeTreeModel()
{
	return m_TreeModel;
}

PropertiesTreeModel *EntityModel::GetPropertiesModel(EntityNodeItemData *pItem)
{
	if(pItem == nullptr)
	{
		HyGuiLog("EntityModel::GetPropertiesModel was given a nullptr", LOGTYPE_Error);
		return nullptr;
	}

	if(m_PropertiesMap.contains(pItem) == false)
	{
		QVariant var(reinterpret_cast<qulonglong>(pItem));
		m_PropertiesMap[pItem] = AllocNewPropertiesModel(var, pItem);
	}

	return m_PropertiesMap[pItem];
}

void EntityModel::Cmd_AddNewChildren(QList<ProjectItemData *> projItemList)
{
	QList<ProjectItemData *> list;
	for(auto *pItem : projItemList)
	{
		list << static_cast<ProjectItemData *>(pItem);
		m_TreeModel.InsertNewChild(static_cast<ProjectItemData *>(pItem));
	}

	m_ItemRef.GetProject().RegisterItems(&m_ItemRef, list);
}

bool EntityModel::Cmd_RemoveChild(ProjectItemData *pItem)
{
	EntityNodeItemData *pChildNode = m_TreeModel.FindEntityNodeItem(pItem);
	if(pChildNode == nullptr)
		return false;

	if(m_TreeModel.RemoveChild(pChildNode))
	{
		QList<ProjectItemData *> list;
		list << static_cast<ProjectItemData *>(pItem);
		m_ItemRef.GetProject().RelinquishItems(&m_ItemRef, list);

		return true;
	}

	return false;
}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	return true;
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	QJsonArray childrenNodesArray;


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

PropertiesTreeModel *EntityModel::AllocNewPropertiesModel(QVariant &subState, EntityNodeItemData *pItemToAdd)
{
	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	PropertiesTreeModel *pNewPropertiesModel = new PropertiesTreeModel(GetItem(), 0, subState);

	pNewPropertiesModel->AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
	pNewPropertiesModel->AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", false, QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 1.0, "[", "]");
	pNewPropertiesModel->AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", false, QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 0.01, "[", "]");
	pNewPropertiesModel->AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", false, 0.0, 360.0, 0.1, "", "°");

	pNewPropertiesModel->AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
	pNewPropertiesModel->AppendProperty("Common", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated");
	pNewPropertiesModel->AppendProperty("Common", "Update while game paused", PROPERTIESTYPE_bool, Qt::Unchecked, "Only items with this checked will receive updates when the game/application is paused");
	pNewPropertiesModel->AppendProperty("Common", "User Tag", PROPERTIESTYPE_int, 0, "Not used by Harmony. You can set it to anything you like", false, -iRANGE, iRANGE, 1);
	pNewPropertiesModel->AppendProperty("Common", "Display Order", PROPERTIESTYPE_int, 0, "Higher display orders get drawn above other items with less. Undefined ordering when equal", false, -iRANGE, iRANGE, 1);

	switch(pItemToAdd->GetType())
	{
	case ITEM_Entity: {
		pNewPropertiesModel->AppendCategory("Physics", QVariant(), true, false, "Check whether this node has a physics body");

		pNewPropertiesModel->AppendProperty("Physics", "Type", PROPERTIESTYPE_ComboBox, 0, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)", false, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Static" << "Kinematic" << "Dynamic");
		pNewPropertiesModel->AppendProperty("Physics", "Gravity Scale", PROPERTIESTYPE_double, 1.0, "Adjusts the gravity on this single body. Negative values will reverse gravity. Increased gravity can decrease stability", false, -100.0, 100.0, 0.1);
		pNewPropertiesModel->AppendProperty("Physics", "Linear Damping", PROPERTIESTYPE_double, 0.0, "Reduces the world linear velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		pNewPropertiesModel->AppendProperty("Physics", "Angular Damping", PROPERTIESTYPE_double, 0.01, "Reduces the world angular velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", false, 0.0, 100.0, 0.01);
		pNewPropertiesModel->AppendProperty("Physics", "Dynamic CCD", PROPERTIESTYPE_bool, Qt::Unchecked, "Continuous collision detection for other dynamic moving bodies. Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies. You should use this flag sparingly since it increases processing time");
		pNewPropertiesModel->AppendProperty("Physics", "Fixed Rotation", PROPERTIESTYPE_bool, Qt::Unchecked, "Prevents this body from rotating if checked. Useful for characters");
		pNewPropertiesModel->AppendProperty("Physics", "Initially Awake", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to make body initially awake. Start sleeping otherwise");
		pNewPropertiesModel->AppendProperty("Physics", "Allow Sleep", PROPERTIESTYPE_bool, Qt::Checked, "Uncheck this if this body should never fall asleep. This increases CPU usage");

	} break;

	case ITEM_Primitive:
		pNewPropertiesModel->AppendCategory("Primitive");
		break;
	case ITEM_AtlasImage:
		pNewPropertiesModel->AppendCategory("Textured Quad");
		break;

	case ITEM_Text: {
		QVariant var;
		var.setValue<EntityNodeItemData *>(pItemToAdd);
		pNewPropertiesModel->AppendCategory("Text", var);
		pNewPropertiesModel->AppendProperty("Text", "State", PROPERTIESTYPE_StatesComboBox, 0, "The text state to be displayed", false, QVariant(), QVariant(), QVariant(), "", "", var);
	} break;

	case ITEM_Sprite: {
		QVariant var;
		var.setValue<EntityNodeItemData *>(pItemToAdd);
		pNewPropertiesModel->AppendCategory("Sprite", var);
		pNewPropertiesModel->AppendProperty("Sprite", "State", PROPERTIESTYPE_StatesComboBox, 0, "The sprite state to be displayed", false, QVariant(), QVariant(), QVariant(), "", "", var);
		pNewPropertiesModel->AppendProperty("Sprite", "Frame", PROPERTIESTYPE_SpriteFrames, 0, "The sprite frame index to start on");
	} break;

	default:
		HyGuiLog("EntityTreeItem::EntityTreeItem - unsupported type: " % QString::number(pItemToAdd->GetType()), LOGTYPE_Error);
	}

	return pNewPropertiesModel;
}
