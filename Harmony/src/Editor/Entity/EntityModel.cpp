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

PropertiesTreeModel *EntityStateData::GetPropertiesModel(ExplorerItem *pItem)
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

/*virtual*/ int EntityStateData::AddFrame(AtlasFrame *pFrame) /*override*/
{
	return 0;
}

/*virtual*/ void EntityStateData::RelinquishFrame(AtlasFrame *pFrame) /*override*/
{
}

PropertiesTreeModel *EntityStateData::AllocNewPropertiesModel(ProjectItem &entityItemRef, QVariant &subState, ExplorerItem *pItemToAdd)
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

		case ITEM_Font: {
			QVariant var;
			var.setValue<ExplorerItem *>(pItemToAdd);
			pNewPropertiesModel->AppendCategory("Font", var);
			pNewPropertiesModel->AppendProperty("Font", "State", PROPERTIESTYPE_StatesComboBox, 0, "The font state to be displayed", false, QVariant(), QVariant(), QVariant(), "", "", var);
			} break;

		case ITEM_Sprite: {
			QVariant var;
			var.setValue<ExplorerItem *>(pItemToAdd);
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

EntityModel::EntityModel(ProjectItem &itemRef, QJsonArray stateArray) :
	IModel(itemRef),
	m_TreeModel(this, this)
{
	// If item's init value is defined, parse and initialize with it, otherwise make default empty sprite
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

PropertiesTreeModel *EntityModel::GetPropertiesModel(int iStateIndex, ExplorerItem *pItem)
{
	if(iStateIndex < 0)
		return nullptr;

	PropertiesTreeModel *pPropertiesModel = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesModel(pItem);
	return pPropertiesModel;
}

bool EntityModel::AddNewChild(ExplorerItem *pItem)
{
	return m_TreeModel.AddChildItem(pItem);
}

bool EntityModel::RemoveChild(ExplorerItem *pItem)
{
	return m_TreeModel.RemoveChild(pItem);
}

const QList<ProjectItem *> &EntityModel::GetPrimitiveList()
{
	return m_PrimitiveList;
}

ProjectItem *EntityModel::CreateNewPrimitive()
{
	ProjectItem *pNewPrimitiveItem = new ProjectItem(m_ItemRef.GetProject(), ITEM_Primitive, "Primitive", QJsonValue(), false);
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
