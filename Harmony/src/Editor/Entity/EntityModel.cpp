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

#include <QVariant>

EntityStateData::EntityStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj) :
	IStateData(iStateIndex, modelRef, stateObj["name"].toString())
{
	if(stateObj.empty() == false)
	{
	}
	else
	{

	}
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

PropertiesTreeModel *EntityStateData::GetPropertiesModel(EntityTreeItem *pTreeItem)
{
	if(pTreeItem == nullptr)
	{
		HyGuiLog("EntityStateData::GetPropertiesModel was given a nullptr", LOGTYPE_Error);
		return nullptr;
	}

	if(m_PropertiesMap.contains(pTreeItem) == false)
	{
		QVariant var(reinterpret_cast<qulonglong>(pTreeItem));
		m_PropertiesMap[pTreeItem] = AllocNewPropertiesModel(m_ModelRef.GetItem(), var, pTreeItem->GetProjItem());
	}

	return m_PropertiesMap[pTreeItem];
}

/*virtual*/ int EntityStateData::AddFrame(AtlasFrame *pFrame) /*override*/
{
	return 0;
}

/*virtual*/ void EntityStateData::RelinquishFrame(AtlasFrame *pFrame) /*override*/
{
}

PropertiesTreeModel *EntityStateData::AllocNewPropertiesModel(ProjectItem &entityItemRef, QVariant &subState, ProjectItem *pItemToAdd)
{
	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	PropertiesTreeModel *pNewPropertiesModel = new PropertiesTreeModel(entityItemRef, GetIndex(), subState);

	PropertiesDef defBool;
	defBool.eType = PROPERTIESTYPE_bool;
	PropertiesDef defInt(PROPERTIESTYPE_int, 0, -iRANGE, iRANGE, 1, "", "");
	PropertiesDef defVec2(PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 1.0, "[", "]");

	pNewPropertiesModel->AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
	pNewPropertiesModel->AppendProperty("Transformation", "Position", defVec2, "Position is relative to parent node");
	defVec2.defaultData = QPointF(1.0f, 1.0f);
	defVec2.stepAmt = 0.01;
	pNewPropertiesModel->AppendProperty("Transformation", "Scale", defVec2, "Scale is relative to parent node");
	pNewPropertiesModel->AppendProperty("Transformation", "Rotation", PropertiesDef(PROPERTIESTYPE_double, 0.0, 0.0, 360.0, 1.0, "", "°"), "Rotation is relative to parent node");

	pNewPropertiesModel->AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
	defBool.defaultData = Qt::Checked;
	pNewPropertiesModel->AppendProperty("Common", "Enabled", defBool, "Enabled dictates whether this gets drawn and updated");
	defBool.defaultData = Qt::Unchecked;
	pNewPropertiesModel->AppendProperty("Common", "Update while game paused", defBool, "Only items with this checked will recieve updates when the game/application is paused");
	pNewPropertiesModel->AppendProperty("Common", "User Tag", defInt, "Not used by Harmony. You can set it to anything you like");
	pNewPropertiesModel->AppendProperty("Common", "Display Order", defInt, "Higher display orders get drawn above other items with less. Undefined ordering when equal");

	switch(pItemToAdd->GetType())
	{
		case ITEM_Entity: {
			pNewPropertiesModel->AppendCategory("Physics", HyGlobal::ItemColor(ITEM_Physics), true, true, false, "Check whether this node has a physics body");

			PropertiesDef defComboBox;
			defComboBox.eType = PROPERTIESTYPE_ComboBox;
			defComboBox.defaultData = 0;
			QStringList sList;
			sList << "Static" << "Kinematic" << "Dynamic";
			defComboBox.delegateBuilder = sList;
			pNewPropertiesModel->AppendProperty("Physics", "Type", defComboBox, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)");
			pNewPropertiesModel->AppendProperty("Physics", "Gravity Scale", PropertiesDef(PROPERTIESTYPE_double, 1.0, -100.0, 100.0, 0.1, "", ""), "Adjusts the gravity on this single body. Negative values will reverse gravity. Increased gravity can decrease stability");
			pNewPropertiesModel->AppendProperty("Physics", "Linear Damping", PropertiesDef(PROPERTIESTYPE_double, 0.0, 0.0, 100.0, 0.01, "", ""), "Reduces the world linear velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1");
			pNewPropertiesModel->AppendProperty("Physics", "Angular Damping", PropertiesDef(PROPERTIESTYPE_double, 0.01, 0.0, 100.0, 0.01, "", ""), "Reduces the world angular velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1");
			defBool.defaultData = Qt::Unchecked;
			pNewPropertiesModel->AppendProperty("Physics", "Dynamic CCD", defBool, "Continous collision detection for other dynamic moving bodies. Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies. You should use this flag sparingly since it increases processing time");
			pNewPropertiesModel->AppendProperty("Physics", "Fixed Rotation", defBool, "Prevents this body from rotating if checked. Useful for characters");
			pNewPropertiesModel->AppendProperty("Physics", "Initially Awake", defBool, "Check to make body initially awake. Start sleeping otherwise");
			defBool.defaultData = Qt::Checked;
			pNewPropertiesModel->AppendProperty("Physics", "Allow Sleep", defBool, "Uncheck this if this body should never fall asleep. This increases CPU usage");

		} break;

		case ITEM_Primitive:
			pNewPropertiesModel->AppendCategory("Primitive", HyGlobal::ItemColor(ITEM_Primitive));
			break;
		case ITEM_AtlasImage:
			pNewPropertiesModel->AppendCategory("Textured Quad", HyGlobal::ItemColor(ITEM_AtlasImage));
			break;

		case ITEM_Font: {
			QVariant var;
			var.setValue<ProjectItem *>(pItemToAdd);
			pNewPropertiesModel->AppendCategory("Font", HyGlobal::ItemColor(ITEM_Font), var);

			PropertiesDef defStateComboBox;
			defStateComboBox.eType = PROPERTIESTYPE_StatesComboBox;
			defStateComboBox.defaultData = 0;
			defStateComboBox.delegateBuilder.setValue<ProjectItem *>(pItemToAdd);
			pNewPropertiesModel->AppendProperty("Font", "State", defStateComboBox, "The font state to be displayed");
			} break;

		case ITEM_Sprite: {
			QVariant var;
			var.setValue<ProjectItem *>(pItemToAdd);
			pNewPropertiesModel->AppendCategory("Sprite", HyGlobal::ItemColor(ITEM_Sprite), var);

			PropertiesDef defStateComboBox;
			defStateComboBox.eType = PROPERTIESTYPE_StatesComboBox;
			defStateComboBox.defaultData = 0;
			defStateComboBox.delegateBuilder.setValue<ProjectItem *>(pItemToAdd);
			pNewPropertiesModel->AppendProperty("Sprite", "State", defStateComboBox, "The sprite state to be displayed");
			pNewPropertiesModel->AppendProperty("Sprite", "Frame", PropertiesDef(PROPERTIESTYPE_SpriteFrames, 0), "The sprite frame index to start on");
			} break;

		default:
			HyGuiLog("EntityTreeItem::EntityTreeItem - unsupported type: " % QString::number(pItemToAdd->GetType()), LOGTYPE_Error);
	}

	return pNewPropertiesModel;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItem &itemRef, QJsonArray stateArray) :
	IModel(itemRef),
	m_TreeModel(this, itemRef)
{
	// If item's init value is defined, parse and initalize with it, otherwise make default empty sprite
	if(stateArray.empty() == false)
	{
		for(int i = 0; i < stateArray.size(); ++i)
			AppendState<EntityStateData>(stateArray[i].toObject());
	}
	else
		AppendState<EntityStateData>(QJsonObject());
}

/*virtual*/ EntityModel::~EntityModel()
{
}

//EntityTreeModel &EntityModel::GetTreeModel()
//{
//	return m_TreeModel;
//}

PropertiesTreeModel *EntityModel::GetPropertiesModel(int iStateIndex, EntityTreeItem *pTreeItem)
{
	if(iStateIndex < 0)
		return nullptr;

	PropertiesTreeModel *pPropertiesModel = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesModel(pTreeItem);
	return pPropertiesModel;
}

bool EntityModel::IsChildAddable(ProjectItem *pItem)
{
	if(pItem == nullptr || &m_ItemRef == pItem)
		return false;

	if(pItem->GetType() == ITEM_Entity)
	{
		// TODO: Ensure that this child entity doesn't contain this as child
	}

	return true;
}

bool EntityModel::AddNewChild(ProjectItem *pItem)
{
	if(IsChildAddable(pItem) == false)
		return false;

	m_TreeModel.AddItem(pItem);
//	for(int i = 0; i < m_StateList.size(); ++i)
//		m_StateList[i]->

	return true;
}

bool EntityModel::InsertTreeItem(int iRow, EntityTreeItem *pItem, EntityTreeItem *pParentItem)
{
	if(IsChildAddable(pItem->GetProjItem()) == false)
		return false;

	m_TreeModel.InsertItem(iRow, pItem, pParentItem);
    return true;
}

bool EntityModel::RemoveTreeItems(int iRow, int iCount, EntityTreeItem *pParentItem)
{
	return m_TreeModel.RemoveItems(iRow, iCount, pParentItem);
}

const QList<ProjectItem *> &EntityModel::GetPrimitiveList()
{
	return m_PrimitiveList;
}

ProjectItem *EntityModel::CreateNewPrimitive()
{
	ProjectItem *pNewPrimitiveItem = new ProjectItem(m_ItemRef.GetProject(), ITEM_Primitive, nullptr, "Primitive", QJsonValue(), false);
	m_PrimitiveList.push_back(pNewPrimitiveItem);

	return pNewPrimitiveItem;
}

void EntityModel::SetWidget(QTreeView *pTreeView)
{
	pTreeView->setModel(&m_TreeModel);
}

/*virtual*/ void EntityModel::OnSave() /*override*/
{
}

/*virtual*/ QJsonObject EntityModel::PopStateAt(uint32 uiIndex) /*override*/
{
	return QJsonObject();
}

/*virtual*/ QJsonValue EntityModel::GetJson() const /*override*/
{
	return QJsonValue();
}

/*virtual*/ QList<AtlasFrame *> EntityModel::GetAtlasFrames() const /*override*/
{
	return QList<AtlasFrame *>();
}

/*virtual*/ QStringList EntityModel::GetFontUrls() const /*override*/
{
	return QStringList();
}

/*virtual*/ void EntityModel::Refresh() /*override*/
{
}
