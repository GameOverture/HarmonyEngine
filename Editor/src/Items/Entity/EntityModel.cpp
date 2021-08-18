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

EntityNodeTreeModel::EntityNodeTreeModel(EntityModel *pEntityModel, QObject *parent) :
	ITreeModel(1, QStringList(), parent),
	m_pEntityModel(pEntityModel)
{
	// Insert self as root node
	QModelIndex parentIndex = FindIndex<ExplorerItemData *>(m_pRootItem->data(0).value<ExplorerItemData *>(), 0);
	int iRow = m_pRootItem->GetNumChildren();
	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityNodeTreeModel::EntityNodeTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}
	QVariant v;
	v.setValue<TreeModelItemData *>(&pEntityModel->GetItem());
	if(setData(index(iRow, 0, parentIndex), v, Qt::UserRole) == false)
		HyGuiLog("EntityNodeTreeModel::EntityNodeTreeModel() - setData failed", LOGTYPE_Error);
}

/*virtual*/ EntityNodeTreeModel::~EntityNodeTreeModel()
{
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

bool EntityNodeTreeModel::InsertNewChild(TreeModelItemData *pNewItem, TreeModelItem *pParentTreeItem /*= nullptr*/, int iRow /*= -1*/)
{
	if(pParentTreeItem == nullptr)
		pParentTreeItem = GetItem(FindIndex<TreeModelItemData *>(&m_pEntityModel->GetItem(), 0));

	QModelIndex parentIndex = FindIndex<TreeModelItemData *>(pParentTreeItem->data(0).value<TreeModelItemData *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);

	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("EntityNodeTreeModel::InsertNewChild() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	QVariant v;
	v.setValue<TreeModelItemData *>(pNewItem);
	if(setData(index(iRow, 0, parentIndex), v, Qt::UserRole) == false)
		HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);

	return true;
}

bool EntityNodeTreeModel::RemoveChild(TreeModelItemData *pItem)
{
	TreeModelItem *pTreeItem = GetItem(FindIndex<TreeModelItemData *>(pItem, 0));
	TreeModelItem *pParentTreeItem = pTreeItem->GetParent();
	return removeRow(pTreeItem->GetIndex(), createIndex(pParentTreeItem->GetIndex(), 0, pParentTreeItem));
}

QVariant EntityNodeTreeModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const
{
	if(indexRef.row() == 0 && indexRef.parent().isValid() == false)
	{
	}

	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	ExplorerItemData *pItem = pTreeItem->data(0).value<ExplorerItemData *>();
	switch(iRole)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
		return QVariant(pItem->GetName(false));

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(pItem->IsProjectItem())
		{
			ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pItem);
			if(pProjItem->IsExistencePendingSave())
				return QVariant(pItem->GetIcon(SUBICON_New));
			else if(pProjItem->IsSaveClean() == false)
				return QVariant(pItem->GetIcon(SUBICON_Dirty));
		}
		return QVariant(pItem->GetIcon(SUBICON_None));

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant(pItem->GetName(true));

	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		return QVariant(pItem->GetName(true));

	default:
		return QVariant();
	}
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
	for(auto iter = m_PropertiesMap.begin(); iter != m_PropertiesMap.end(); ++iter)
	{
		PropertiesTreeModel *pPropertiesModel = iter.value();
		delete pPropertiesModel;
	}

	m_PropertiesMap.clear();
}

PropertiesTreeModel *EntityStateData::GetPropertiesModel(ExplorerItemData *pItem)
{
	if(pItem == nullptr)
	{
		HyGuiLog("EntityStateData::GetPropertiesModel was given a nullptr", LOGTYPE_Error);
		return nullptr;
	}

	if(m_PropertiesMap.contains(pItem) == false)
	{
		QVariant var(reinterpret_cast<qulonglong>(pItem));
		m_PropertiesMap[pItem] = AllocNewPropertiesModel(m_ModelRef.GetItem(), var, pItem);
	}

	return m_PropertiesMap[pItem];
}

/*virtual*/ QVariant EntityStateData::OnLinkAsset(AssetItemData *pAsset) /*override*/
{
	return 0;
}

/*virtual*/ void EntityStateData::OnUnlinkAsset(AssetItemData *pAsset) /*override*/
{
}

PropertiesTreeModel *EntityStateData::AllocNewPropertiesModel(ProjectItemData &entityItemRef, QVariant &subState, ExplorerItemData *pItemToAdd)
{
	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	PropertiesTreeModel *pNewPropertiesModel = new PropertiesTreeModel(entityItemRef, GetIndex(), subState);

	pNewPropertiesModel->AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
	pNewPropertiesModel->AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", false, QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 1.0, "[", "]");
	pNewPropertiesModel->AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", false, QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 0.01, "[", "]");
	pNewPropertiesModel->AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", false, 0.0, 360.0, 0.1, "", "°");

	pNewPropertiesModel->AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
	pNewPropertiesModel->AppendProperty("Common", "Visible", PROPERTIESTYPE_bool, Qt::Checked,"Enabled dictates whether this gets drawn and updated");
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
			var.setValue<ExplorerItemData *>(pItemToAdd);
			pNewPropertiesModel->AppendCategory("Text", var);
			pNewPropertiesModel->AppendProperty("Text", "State", PROPERTIESTYPE_StatesComboBox, 0, "The text state to be displayed", false, QVariant(), QVariant(), QVariant(), "", "", var);
			} break;

		case ITEM_Sprite: {
			QVariant var;
			var.setValue<ExplorerItemData *>(pItemToAdd);
			pNewPropertiesModel->AppendCategory("Sprite", var);
			pNewPropertiesModel->AppendProperty("Sprite", "State", PROPERTIESTYPE_StatesComboBox, 0, "The sprite state to be displayed", false, QVariant(), QVariant(), QVariant(), "", "", var);
			pNewPropertiesModel->AppendProperty("Sprite", "Frame", PROPERTIESTYPE_SpriteFrames, 0, "The sprite frame index to start on");
			} break;

		default:
			HyGuiLog("EntityTreeItem::EntityTreeItem - unsupported type: " % QString::number(pItemToAdd->GetType()), LOGTYPE_Error);
	}

	return pNewPropertiesModel;
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
}

EntityNodeTreeModel &EntityModel::GetNodeTreeModel()
{
	return m_TreeModel;
}

PropertiesTreeModel *EntityModel::GetPropertiesModel(int iStateIndex, ExplorerItemData *pItem)
{
	if(iStateIndex < 0)
		return nullptr;

	PropertiesTreeModel *pPropertiesModel = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesModel(pItem);
	return pPropertiesModel;
}

void EntityModel::AddNewChildren(QList<TreeModelItemData *> itemList)
{
	for(auto item : itemList)
	{
		if(item->IsProjectItem())
		{
			QList<ProjectItemData *> list;
			list << static_cast<ProjectItemData *>(item);
			m_ItemRef.GetProject().RegisterItems(&m_ItemRef, list);
		}

		m_TreeModel.InsertNewChild(item);
	}
}

bool EntityModel::RemoveChild(TreeModelItemData *pItem)
{
	if(pItem->IsProjectItem())
	{
		QList<ProjectItemData *> list;
		list << static_cast<ProjectItemData *>(pItem);
		m_ItemRef.GetProject().RelinquishItems(&m_ItemRef, list);
	}

	return m_TreeModel.RemoveChild(pItem);
}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	return true;
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
}

/*virtual*/ QList<AssetItemData *> EntityModel::GetAssets(HyGuiItemType eType) const /*override*/
{
	return QList<AssetItemData *>();
}

/*virtual*/ QStringList EntityModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
