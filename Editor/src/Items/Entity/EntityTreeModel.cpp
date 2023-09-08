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

EntityTreeItemData::EntityTreeItemData(EntityModel &entityModelRef, bool bIsForwardDeclared, QString sCodeName, ItemType eItemType, EntityItemType eEntType, QUuid uuidOfReferencedItem, QUuid uuidOfThis) :
	TreeModelItemData(eItemType, uuidOfThis, sCodeName),
	m_EntityModelRef(entityModelRef),
	m_eEntType(eEntType),
	m_PropertiesModel(entityModelRef.GetItem(), -1, reinterpret_cast<qulonglong>(this), this),
	m_bIsForwardDeclared(bIsForwardDeclared),
	m_ReferencedItemUuid(uuidOfReferencedItem),
	m_bIsSelected(false)
{
	if(m_eEntType == ENTTYPE_Root || m_eEntType == ENTTYPE_Item || m_eEntType == ENTTYPE_ArrayItem)
	{
		// TODO: Make InitalizePropertyModel apart of this class
		InitalizePropertyModel(this, m_PropertiesModel);

		//// TODO: Remove below
		//for(int i = 0; i < m_EntityModelRef.GetNumStates(); ++i)
		//{
		//	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityModelRef.GetStateData(i));
		//	pStateData->InsertNewPropertiesModel(this, QJsonObject());
		//}
	}
}

EntityTreeItemData::EntityTreeItemData(EntityModel &entityModelRef, bool bIsForwardDeclared, QJsonObject descObj, bool bIsArrayItem) :
	TreeModelItemData(HyGlobal::GetTypeFromString(descObj["itemType"].toString()), descObj["UUID"].toString(), descObj["codeName"].toString()),
	m_EntityModelRef(entityModelRef),
	m_eEntType(bIsArrayItem ? ENTTYPE_ArrayItem : ENTTYPE_Item),
	m_PropertiesModel(entityModelRef.GetItem(), -1, QVariant(), this),
	m_sPromotedEntityType(descObj["promotedEntityType"].toString()),
	m_bIsForwardDeclared(bIsForwardDeclared),
	m_ReferencedItemUuid(descObj["itemUUID"].toString()),
	m_bIsSelected(descObj["isSelected"].toBool())
{
	// TODO: Make InitalizePropertyModel apart of this class
	InitalizePropertyModel(this, m_PropertiesModel);

	//// TODO: Remove below
	//if(propArray.size() != m_EntityModelRef.GetNumStates())
	//{
	//	HyGuiLog("EntityTreeItemData::EntityTreeItemData() - propArray size doesn't equal number of this entity states", LOGTYPE_Error);
	//	return;
	//}

	//for(int i = 0; i < m_EntityModelRef.GetNumStates(); ++i)
	//{
	//	EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityModelRef.GetStateData(i));
	//	pStateData->InsertNewPropertiesModel(this, propArray[i].toObject());
	//}
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

const QUuid &EntityTreeItemData::GetReferencedItemUuid() const
{
	return m_ReferencedItemUuid;
}

QString EntityTreeItemData::GetHyNodeTypeName() const
{
	switch(m_eTYPE)
	{
	case ITEM_Primitive:		return "HyPrimitive2d";
	case ITEM_Audio:			return "HyAudio2d";
	case ITEM_Text:				return "HyText2d";
	case ITEM_Spine:			return "HySpine2d";
	case ITEM_Sprite:			return "HySprite2d";
	case ITEM_AtlasFrame:		return "HyTexturedQuad2d";
	case ITEM_BoundingVolume:	return "HyShape2d";
	
	case ITEM_Entity:
		if(m_sPromotedEntityType.isEmpty() == false)
			return m_sPromotedEntityType;
		else
			return "HyEntity2d";

	case ITEM_SoundClip:
	case ITEM_Prefab:
	case ITEM_Particles:
	default:
		HyGuiLog("EntityTreeItemData::GetHyNodeTypeName() - Unhandled item type: " % HyGlobal::ItemName(m_eTYPE, false), LOGTYPE_Error);
		break;
	}

	return QString();
}

bool EntityTreeItemData::IsForwardDeclared() const
{
	return m_bIsForwardDeclared;
}

//PropertiesTreeModel &EntityTreeItemData::GetPropertiesModel()
//{
//	return m_PropertiesModel;
//}

bool EntityTreeItemData::IsSelected() const
{
	return m_bIsSelected;
}

void EntityTreeItemData::SetSelected(bool bIsSelected)
{
	m_bIsSelected = bIsSelected;
}

void EntityTreeItemData::InsertJsonInfo_Desc(QJsonObject &childObjRef)
{
	// Common stuff
	childObjRef.insert("codeName", GetCodeName());
	childObjRef.insert("itemType", HyGlobal::ItemName(m_eTYPE, false));
	childObjRef.insert("UUID", GetUuid().toString(QUuid::WithoutBraces));
	childObjRef.insert("promotedEntityType", m_sPromotedEntityType);
	childObjRef.insert("itemUUID", m_ReferencedItemUuid.toString(QUuid::WithoutBraces));
	childObjRef.insert("isSelected", m_bIsSelected);
}

// NOTE: The following functions share logic that handle all the item specific properties: EntityStateData::InitalizePropertyModel, EntityTreeItemData::GenerateStateSrc, EntityDrawItem.cpp - ApplyProperties
//		 Updates here should reflect to the functions above
QString EntityTreeItemData::GenerateStateSrc(uint32 uiStateIndex, QString sNewLine, bool &bActivatePhysicsOut, uint32 &uiMaxVertListSizeOut)
{
	PropertiesTreeModel *pPropertiesModel = &GetPropertiesModel(/*uiStateIndex*/);
	if(pPropertiesModel == nullptr)
	{
		HyGuiLog("EntityTreeItemData::GenerateStateSrc() - pPropertiesModel is nullptr", LOGTYPE_Error);
		return QString();
	}

	QString sCodeName;
	if(m_eEntType != ENTTYPE_Root)
	{
		if(IsForwardDeclared())
			sCodeName = GetCodeName() + "->";
		else
			sCodeName = GetCodeName() + ".";
	}
	
	QString sSrc;
	
	std::function<void(QString sCategoryName, QString sPropertyName, const QVariant &valueRef)> fpForEach = [&](QString sCategoryName, QString sPropertyName, const QVariant &valueRef)
	{
		if(sCategoryName == "Common")
		{
			if(sPropertyName == "State")
				sSrc += sCodeName + "SetState(" + QString::number(valueRef.toInt()) + ");" + sNewLine;
			else if(sPropertyName == "Update During Paused")
				sSrc += sCodeName + "SetPauseUpdate(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "User Tag")
			{
				sSrc += "#ifdef HY_ENABLE_USER_TAGS" + sNewLine;
				sSrc += sCodeName + "SetTag(" + QString::number(valueRef.toLongLong()) + ");" + sNewLine;
				sSrc += "#endif" + sNewLine;
			}
		}
		else if(sCategoryName == "Transformation")
		{
			if(sPropertyName == "Position")
				sSrc += sCodeName + "pos.Set(" + QString::number(valueRef.toPointF().x(), 'f') + "f, " + QString::number(valueRef.toPointF().y(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Scale")
				sSrc += sCodeName + "scale.Set(" + QString::number(valueRef.toPointF().x(), 'f') + "f, " + QString::number(valueRef.toPointF().y(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Rotation")
				sSrc += sCodeName + "rot.Set(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
		}
		else if(sCategoryName == "Body")
		{
			if(sPropertyName == "Visible")
				sSrc += sCodeName + "SetVisible(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Color Tint")
				sSrc += sCodeName + "SetTint(HyColor(" + QString::number(valueRef.toRect().left()) + ", " + QString::number(valueRef.toRect().top()) + ", " + QString::number(valueRef.toRect().width()) + "));" + sNewLine;
			else if(sPropertyName == "Alpha")
				sSrc += sCodeName + "alpha.Set(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Override Display Order")
				sSrc += sCodeName + "SetDisplayOrder(" + QString::number(valueRef.toInt()) + ");" + sNewLine;
		}
		else if(sCategoryName == "Physics") // NOTE: This is only from the 'root' node (aka *this entity)
		{
			if(sPropertyName == "Physics_checked") // NOTE: We can rely on *_checked to be the first property in the category
			{
				if(valueRef.toBool() == false)
					sSrc += "physics.Deactivate();" + sNewLine;
			}
			else if(sPropertyName == "Start Activated")
				bActivatePhysicsOut = valueRef.toBool();
			else if(sPropertyName == "Type")
				sSrc += "physics.SetType(static_cast<HyBodyType>(" + QString::number(valueRef.toInt()) + "));" + sNewLine;
			else if(sPropertyName == "Fixed Rotation")
				sSrc += "physics.SetFixedRotation(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Initially Awake")
				sSrc += "physics.SetAwake(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Allow Sleep")
				sSrc += "physics.SetSleepingAllowed(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Gravity Scale")
				sSrc += "physics.SetGravityScale(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Dynamic CCD")
				sSrc += "physics.SetCcd(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Linear Damping")
				sSrc += "physics.SetLinearDamping(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Angular Damping")
				sSrc += "physics.SetAngularDamping(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Linear Velocity")
				sSrc += "physics.SetVel(" + QString::number(valueRef.toPointF().x(), 'f') + "f, " + QString::number(valueRef.toPointF().y(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Angular Velocity")
				sSrc += "physics.SetAngVel(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
		}
		else if(sCategoryName == "Primitive")
		{
			if(sPropertyName == "Wireframe")
				sSrc += sCodeName + "SetWireframe(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Line Thickness")
				sSrc += sCodeName + "SetLineThickness(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
		}
		else if(sCategoryName == "Shape")
		{
			if(sPropertyName == "Data")
			{
				EditorShape eShapeType = HyGlobal::GetShapeFromString(pPropertiesModel->FindPropertyValue("Shape", "Type").toString());
				sSrc += ShapeCtrl::DeserializeAsRuntimeCode(sCodeName, eShapeType, valueRef.toString(), sNewLine, uiMaxVertListSizeOut);
			}
		}
		else if(sCategoryName == "Fixture")
		{
			if(sPropertyName == "Fixture_checked") // NOTE: We can rely on *_checked to be the first property in the category
				sSrc += sCodeName + "SetFixtureAllowed(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Density")
				sSrc += sCodeName + "SetDensity(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Friction")
				sSrc += sCodeName + "SetFriction(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Restitution")
				sSrc += sCodeName + "SetRestitution(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Restitution Threshold")
				sSrc += sCodeName + "SetRestitutionThreshold(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Sensor")
				sSrc += sCodeName + "SetSensor(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Filter: Category Mask")
			{
				uint16 iCategory = static_cast<uint16>(valueRef.toInt());
				uint16 iMask = static_cast<uint16>(pPropertiesModel->FindPropertyValue("Fixture", "Filter: Collision Mask").toInt());
				uint16 iGroup = static_cast<uint16>(pPropertiesModel->FindPropertyValue("Fixture", "Filter: Group Override").toInt());
				sSrc += sCodeName + "SetFilter([](){ b2Filter filter; filter.categoryBits = " + QString::number(iCategory) + "; filter.maskBits = " + QString::number(iMask) + "; filter.groupIndex = " + QString::number(iGroup) + "; return filter; }());" + sNewLine;
			}
		}
		else if(sCategoryName == "Sprite")
		{
			if(sPropertyName == "Frame")
				sSrc += sCodeName + "SetFrame(" + QString::number(valueRef.toInt()) + ");" + sNewLine;
			else if(sPropertyName == "Anim Rate")
				sSrc += sCodeName + "SetAnimRate(" + QString::number(valueRef.toDouble(), 'f') + "f);" + sNewLine;
			else if(sPropertyName == "Anim Paused")
				sSrc += sCodeName + "SetAnimPause(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
		}
		else if(sCategoryName == "Text")
		{
			if(sPropertyName == "Text")
				sSrc += sCodeName + "SetText(\"" + valueRef.toString() + "\");" + sNewLine;
			else if(sPropertyName == "Style")
			{
				QPointF vStyleDimensions = pPropertiesModel->FindPropertyValue("Text", "Style Dimensions").toPointF();
				switch(HyGlobal::GetTextStyleFromString(valueRef.toString()))
				{
				case TEXTSTYLE_Line:				sSrc += sCodeName + "SetAsLine();" + sNewLine; break;
				case TEXTSTYLE_Column:				sSrc += sCodeName + "SetAsColumn(" + QString::number(vStyleDimensions.x(), 'f') + "f);" + sNewLine; break;
				case TEXTSTYLE_ScaleBox:			sSrc += sCodeName + "SetAsScaleBox(" + QString::number(vStyleDimensions.x(), 'f') + "f, " + QString::number(vStyleDimensions.y(), 'f') + "f, true);" + sNewLine; break;
				case TEXTSTYLE_ScaleBoxTopAlign:	sSrc += sCodeName + "SetAsScaleBox(" + QString::number(vStyleDimensions.x(), 'f') + "f, " + QString::number(vStyleDimensions.y(), 'f') + "f, false);" + sNewLine; break;
				case TEXTSTYLE_Vertical:			sSrc += sCodeName + "SetAsVertical();" + sNewLine; break;
				}
			}
		}
	};

	// 'Foreach' property
	for(int i = 0; i < pPropertiesModel->GetNumCategories(); ++i)
	{
		QString sCategoryName = pPropertiesModel->GetCategoryName(i);

		if(pPropertiesModel->IsCategoryCheckable(i))
		{
			// Logic elsewhere relies on _checked being the first property in the category
			bool bIsChecked = pPropertiesModel->IsCategoryEnabled(sCategoryName);
			fpForEach(sCategoryName, sCategoryName % "_checked", bIsChecked);

			if(bIsChecked == false)
				continue;
		}

		for(int j = 0; j < pPropertiesModel->GetNumProperties(i); ++j)
		{
			QString sPropertyName = pPropertiesModel->GetPropertyName(i, j);

			// TODO: This isn't needed if this function is passed delta keyframes

			//// Only invoke 'fpForEach' if the value is different from the default value in every state's properties
			//bool bIsDefault = true;
			//for(int iStateIndex = 0; iStateIndex < m_EntityModelRef.GetNumStates(); ++iStateIndex)
			//{
			//	PropertiesTreeModel *pTestPropertiesModel = GetPropertiesModel(iStateIndex);
			//	if(pTestPropertiesModel->IsPropertyDefaultValue(i, j) == false)
			//	{
			//		bIsDefault = false;
			//		break;
			//	}
			//}

			//if(bIsDefault == false)
				fpForEach(sCategoryName, sPropertyName, pPropertiesModel->GetPropertyValue(i, j));
		}
	}

	return sSrc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EntityTreeModel::EntityTreeModel(EntityModel &modelRef, QString sEntityCodeName, QJsonObject fileMetaObj, QObject *pParent /*= nullptr*/) :
	ITreeModel(NUMCOLUMNS, QStringList(), pParent),
	m_ModelRef(modelRef)
{
	// Insert self as root node
	if(insertRow(0, QModelIndex()) == false)
	{
		HyGuiLog("EntityTreeModel::EntityTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pThisEntityItem = new EntityTreeItemData(m_ModelRef, false, sEntityCodeName, ITEM_Entity, ENTTYPE_Root, QUuid(fileMetaObj["UUID"].toString()), QUuid(fileMetaObj["UUID"].toString()));
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
	EntityTreeItemData *pShapeFolderItem = new EntityTreeItemData(m_ModelRef, false, "Bounding Volumes", ITEM_Prefix, ENTTYPE_BvFolder, QUuid(), QUuid());
	QVariant shapeData;
	shapeData.setValue<EntityTreeItemData *>(pShapeFolderItem);
	for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
	{
		if(setData(index(1, iCol, QModelIndex()), shapeData, Qt::UserRole) == false)
			HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);
	}

	// Insert all the 'children' and 'shape' items
	std::function<void(const QJsonArray &)> fpPopulateNodeTreeItems = [&](const QJsonArray &itemListArray)
	{
		for(int i = 0; i < itemListArray.size(); ++i)
		{
			if(itemListArray[i].isObject())
				m_ModelRef.Cmd_AddExistingItem(itemListArray[i].toObject(), false, i);
			else if(itemListArray[i].isArray())
			{
				QJsonArray subItemArray = itemListArray[i].toArray();
				for(int j = 0; j < subItemArray.size(); ++j)
					m_ModelRef.Cmd_AddExistingItem(subItemArray[j].toObject(), true, j == 0 ? i : j);
			}
			else
				HyGuiLog("EntityTreeModel::EntityTreeModel invalid JSON type", LOGTYPE_Error);
		}
	};
	// Insert all the 'child' items into the nodeTree
	fpPopulateNodeTreeItems(fileMetaObj["descChildList"].toArray());
	fpPopulateNodeTreeItems(fileMetaObj["descShapeList"].toArray());
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

TreeModelItem *EntityTreeModel::GetArrayFolderTreeItem(EntityTreeItemData *pArrayItem) const
{
	if(pArrayItem->GetEntType() != ENTTYPE_ArrayItem)
	{
		HyGuiLog("EntityTreeModel::GetArrayFolderTreeItem was passed a non-array item", LOGTYPE_Error);
		return nullptr;
	}

	TreeModelItem *pParentFolderItem = (pArrayItem->GetType() == ITEM_BoundingVolume) ? GetBvFolderTreeItem() : GetRootTreeItem();
	for(int i = 0; i < pParentFolderItem->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pSubItem = pParentFolderItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pArrayItem->GetCodeName() == pSubItem->GetCodeName() && pSubItem->GetEntType() == ENTTYPE_ArrayFolder)
			return pParentFolderItem->GetChild(i);
	}

	HyGuiLog("EntityTreeModel::GetArrayFolderTreeItem array folder was not found", LOGTYPE_Error);
	return nullptr;
}

EntityTreeItemData *EntityTreeModel::GetArrayFolderTreeItemData(EntityTreeItemData *pArrayItem) const
{
	if(pArrayItem->GetEntType() != ENTTYPE_ArrayItem)
	{
		HyGuiLog("EntityTreeModel::GetArrayFolderTreeItemData was passed a non-array item", LOGTYPE_Error);
		return nullptr;
	}

	TreeModelItem *pParentFolderItem = (pArrayItem->GetType() == ITEM_BoundingVolume) ? GetBvFolderTreeItem() : GetRootTreeItem();
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

	if(HyGlobal::IsItemType_Project(pItem->GetType()) == false && pItem->GetType() != ITEM_AtlasFrame && pItem->GetType() != ITEM_SoundClip)
	{
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is not a valid child type: " % QString::number(pItem->GetType()), LOGTYPE_Error);
		return false;
	}

	return true;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocChildTreeItem(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pProjItem->GetName(false));

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ShouldForwardDeclare(pProjItem), sCodeName, pProjItem->GetType(), ENTTYPE_Item, pProjItem->GetUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocAssetTreeItem(IAssetItemData *pAssetItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pAssetItem->GetName());

	ItemType eItemType = ITEM_Unknown;
	switch(pAssetItem->GetAssetManagerType())
	{
	case ASSETMAN_Atlases: eItemType = ITEM_AtlasFrame; break;
	case ASSETMAN_Audio: eItemType = ITEM_SoundClip; break;
	default:
		HyGuiLog("EntityTreeModel::Cmd_InsertNewAsset - tried to add an unhandled asset manager type: " % QString::number(pAssetItem->GetAssetManagerType()), LOGTYPE_Error);
		break;
	}
	QUuid assetUuid = pAssetItem->GetUuid();
	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, false, sCodeName, eItemType, ENTTYPE_Item, assetUuid, QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocExistingTreeItem(QJsonObject descObj, bool bIsArrayItem, int iRow /*= -1*/)
{
	ItemType eGuiType = HyGlobal::GetTypeFromString(descObj["itemType"].toString());
	QString sCodeName = descObj["codeName"].toString();
	if(bIsArrayItem == false)
		sCodeName = GenerateCodeName(sCodeName);

	TreeModelItem *pParentTreeItem = nullptr;
	if(eGuiType != ITEM_BoundingVolume)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetBvFolderTreeItem();

	bool bFoundArrayFolder = false;
	if(bIsArrayItem)
		bFoundArrayFolder = FindOrCreateArrayFolder(pParentTreeItem, sCodeName, eGuiType, iRow);

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ShouldForwardDeclare(descObj), descObj, bIsArrayItem);
	iRow = (iRow < 0 || (bIsArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocShapeTreeItem(EditorShape eShape, QString sData, bool bIsPrimitive, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + (bIsPrimitive ? "Prim" : "") + HyGlobal::ShapeName(eShape).simplified().remove(' '));

	TreeModelItem *pParentTreeItem = nullptr;
	if(bIsPrimitive)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetBvFolderTreeItem();

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, false, sCodeName, bIsPrimitive ? ITEM_Primitive : ITEM_BoundingVolume, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);

	return pNewItem;
}

bool EntityTreeModel::Cmd_ReaddChild(EntityTreeItemData *pItem, int iRow)
{
	// Generate a unique code name for this new item
	//QString sCodeName = GenerateCodeName(pItem->GetCodeName());

	TreeModelItem *pParentTreeItem = nullptr;
	if(pItem->GetType() != ITEM_BoundingVolume)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetBvFolderTreeItem();

	bool bFoundArrayFolder = false;
	if(pItem->GetEntType() == ENTTYPE_ArrayItem)
		bFoundArrayFolder = FindOrCreateArrayFolder(pParentTreeItem, pItem->GetCodeName(), pItem->GetType(), iRow);

	iRow = (iRow < 0 || (pItem->GetEntType() == ENTTYPE_ArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	return InsertTreeItem(m_ModelRef.GetItem().GetProject(), pItem, pParentTreeItem, iRow);
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
	TreeModelItemData *pReferencedItemData = m_ModelRef.GetItem().GetProject().FindItemData(pItem->GetReferencedItemUuid());

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
			if(pReferencedItemData)
			{
				if(pReferencedItemData->IsProjectItem())
					return static_cast<ProjectItemData *>(pReferencedItemData)->GetName(true);
				else if(pReferencedItemData->IsAssetItem())
					return static_cast<IAssetItemData *>(pReferencedItemData)->GetPropertyInfo();
			}
			
			return QVariant();
		}

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(indexRef.column() == COLUMN_CodeName)
		{
			if(pReferencedItemData)
			{
				if(pReferencedItemData->IsProjectItem())
				{
					ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pReferencedItemData);
					if(pProjItem->IsExistencePendingSave())
						return QVariant(pItem->GetIcon(SUBICON_New));
					else if(pProjItem->IsSaveClean() == false)
						return QVariant(pItem->GetIcon(SUBICON_Dirty));
				}
				else
					return QVariant(pReferencedItemData->GetIcon(SUBICON_None));
			}

			if(pItem->GetEntType() == ENTTYPE_ArrayFolder)
				return HyGlobal::ItemIcon(pItem->GetType(), SUBICON_Open);

			if(pItem->GetType() == ITEM_Primitive || pItem->GetType() == ITEM_BoundingVolume)
			{
				QIcon icon;
				QString sIconUrl = ":/icons16x16/shapes/" % QString(pItem->GetType() == ITEM_Primitive ? "primitive_" : "shapes_");
				
				int iStateIndex = 0;
				int iFrameIndex = 0;
				if(m_ModelRef.GetItem().GetWidget())
				{
					iStateIndex = m_ModelRef.GetItem().GetWidget()->GetCurStateIndex();
					iFrameIndex = static_cast<EntityStateData *>(m_ModelRef.GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
				}

				QString sShapeType = static_cast<EntityStateData *>(m_ModelRef.GetStateData(iStateIndex))->GetDopeSheetScene().GetKeyFrameProperty(pItem, iFrameIndex, "Shape", "Type").toString();
				switch(HyGlobal::GetShapeFromString(sShapeType))
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

bool EntityTreeModel::FindOrCreateArrayFolder(TreeModelItem *&pParentTreeItemOut, QString sCodeName, ItemType eItemType, int iRowToCreateAt)
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

		if(pParentTreeItemOut->GetNumChildren() > 0)
			iArrayFolderRow = HyMath::Min(iArrayFolderRow, pParentTreeItemOut->GetNumChildren() - 1);

		if(insertRow(iArrayFolderRow, parentIndex) == false)
		{
			HyGuiLog("EntityTreeModel::FindOrCreateArrayFolder - ArrayFolder insertRow failed", LOGTYPE_Error);
			return nullptr;
		}
		// Allocate and store the new array folder item in the tree model
		EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, false, sCodeName, eItemType, ENTTYPE_ArrayFolder, QUuid(), QUuid());
		QVariant v;
		v.setValue<EntityTreeItemData *>(pNewItem);
		for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
		{
			if(setData(index(iArrayFolderRow, iCol, parentIndex), v, Qt::UserRole) == false)
				HyGuiLog("ExplorerModel::FindOrCreateArrayFolder() - setData failed", LOGTYPE_Error);
		}

		pParentTreeItemOut = pParentTreeItemOut->GetChild(iArrayFolderRow);
	}

	return bFoundArrayFolder;
}

// NOTE: The following functions share logic that handle all the item specific properties: InitalizePropertyModel, EntityTreeItemData::GenerateStateSrc, EntityDrawItem.cpp - ApplyProperties
//		 Updates here should reflect to the functions above
void InitalizePropertyModel(EntityTreeItemData *pItemData, PropertiesTreeModel &propertiesTreeModelRef)
{
	// Default ranges
	const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	if(pItemData->GetType() != ITEM_BoundingVolume)
	{
		if(pItemData->IsAssetItem() == false && pItemData->GetEntType() != ENTTYPE_Root)
		{
			propertiesTreeModelRef.AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix));
			propertiesTreeModelRef.AppendProperty("Common", "State", PROPERTIESTYPE_StatesComboBox, 0, "The " % HyGlobal::ItemName(pItemData->GetType(), false) % "'s state to be displayed", PROPERTIESACCESS_ToggleOff, QVariant(), QVariant(), QVariant(), QString(), QString(), pItemData->GetReferencedItemUuid());
		}

		if(pItemData->GetEntType() != ENTTYPE_Root)
		{
			propertiesTreeModelRef.AppendCategory("Common", HyGlobal::ItemColor(ITEM_Prefix)); // Will just return 'false' if "Common" category already exists
			propertiesTreeModelRef.AppendProperty("Common", "Update During Paused", PROPERTIESTYPE_bool, Qt::Unchecked, "Only items with this checked will receive updates when the game/application is paused", PROPERTIESACCESS_ToggleOff);
			propertiesTreeModelRef.AppendProperty("Common", "User Tag", PROPERTIESTYPE_int, 0, "Not used by Harmony. You can set it to anything you like", PROPERTIESACCESS_ToggleOff, -iRANGE, iRANGE, 1);

			propertiesTreeModelRef.AppendCategory("Transformation", HyGlobal::ItemColor(ITEM_Project));
			propertiesTreeModelRef.AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", PROPERTIESACCESS_ToggleOff, -fRANGE, fRANGE, 1.0, "[", "]");
			propertiesTreeModelRef.AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", PROPERTIESACCESS_ToggleOff, -fRANGE, fRANGE, 0.01, "[", "]");
			propertiesTreeModelRef.AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", PROPERTIESACCESS_ToggleOff, 0.0, 360.0, 0.1, "", "°");

			if(pItemData->GetType() != ITEM_Audio)
			{
				propertiesTreeModelRef.AppendCategory("Body", HyGlobal::ItemColor(ITEM_Prefix));
				propertiesTreeModelRef.AppendProperty("Body", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated", PROPERTIESACCESS_ToggleOff);
				propertiesTreeModelRef.AppendProperty("Body", "Color Tint", PROPERTIESTYPE_Color, QRect(255, 255, 255, 0), "A color to alpha blend this item with", PROPERTIESACCESS_ToggleOff);
				propertiesTreeModelRef.AppendProperty("Body", "Alpha", PROPERTIESTYPE_double, 1.0, "A value from 0.0 to 1.0 that indicates how opaque/transparent this item is", PROPERTIESACCESS_ToggleOff, 0.0, 1.0, 0.05);
				propertiesTreeModelRef.AppendProperty("Body", "Override Display Order", PROPERTIESTYPE_int, 0, "Higher display orders get drawn above other items with less. Undefined ordering when equal", PROPERTIESACCESS_ToggleOff, -iRANGE, iRANGE, 1);
			}
		}
		else // ENTTYPE_Root
		{
			propertiesTreeModelRef.AppendCategory("Physics", QVariant(), true, false, "Optionally create a physics component that can affect the transformation of this entity");
			propertiesTreeModelRef.AppendProperty("Physics", "Activate", PROPERTIESTYPE_bool, Qt::Checked, "This entity will begin its physics simulation", PROPERTIESACCESS_ToggleOff);
			propertiesTreeModelRef.AppendProperty("Physics", "Type", PROPERTIESTYPE_ComboBoxInt, 0, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)", PROPERTIESACCESS_ToggleOff, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Static" << "Kinematic" << "Dynamic");
			propertiesTreeModelRef.AppendProperty("Physics", "Fixed Rotation", PROPERTIESTYPE_bool, Qt::Unchecked, "Prevents this body from rotating if checked. Useful for characters", PROPERTIESACCESS_ToggleOff);
			propertiesTreeModelRef.AppendProperty("Physics", "Initially Awake", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to make body initially awake. Start sleeping otherwise", PROPERTIESACCESS_ToggleOff);
			propertiesTreeModelRef.AppendProperty("Physics", "Allow Sleep", PROPERTIESTYPE_bool, Qt::Checked, "Uncheck this if this body should never fall asleep. This increases CPU usage", PROPERTIESACCESS_ToggleOff);
			propertiesTreeModelRef.AppendProperty("Physics", "Gravity Scale", PROPERTIESTYPE_double, 1.0, "Adjusts the gravity on this single body. Negative values will reverse gravity. Increased gravity can decrease stability", PROPERTIESACCESS_ToggleOff, -100.0, 100.0, 0.1);
			propertiesTreeModelRef.AppendProperty("Physics", "Dynamic CCD", PROPERTIESTYPE_bool, Qt::Unchecked, "Continuous collision detection for other dynamic moving bodies. Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies. You should use this flag sparingly since it increases processing time", PROPERTIESACCESS_ToggleOff);
			propertiesTreeModelRef.AppendProperty("Physics", "Linear Damping", PROPERTIESTYPE_double, 0.0, "Reduces the world linear velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", PROPERTIESACCESS_ToggleOff, 0.0, 100.0, 0.01);
			propertiesTreeModelRef.AppendProperty("Physics", "Angular Damping", PROPERTIESTYPE_double, 0.01, "Reduces the world angular velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", PROPERTIESACCESS_ToggleOff, 0.0, 100.0, 0.01);
			propertiesTreeModelRef.AppendProperty("Physics", "Linear Velocity", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Starting Linear velocity of the body's origin in scene coordinates", PROPERTIESACCESS_ToggleOff, -fRANGE, fRANGE, 1.0, "[", "]");
			propertiesTreeModelRef.AppendProperty("Physics", "Angular Velocity", PROPERTIESTYPE_double, 0.0, "Starting Angular velocity of the body", PROPERTIESACCESS_ToggleOff, 0.0, 100.0, 0.01);
		}
	}

	switch(pItemData->GetType())
	{
	case ITEM_Entity:
		break;

	case ITEM_Primitive:
		propertiesTreeModelRef.AppendCategory("Primitive", QVariant(), false, false, "A visible shape that can be drawn to the screen");
		propertiesTreeModelRef.AppendProperty("Primitive", "Wireframe", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to render only the wireframe of the shape type", PROPERTIESACCESS_ToggleOff);
		propertiesTreeModelRef.AppendProperty("Primitive", "Line Thickness", PROPERTIESTYPE_double, 1.0, "When applicable, how thick to render lines", PROPERTIESACCESS_ToggleOff, 1.0, 100.0, 1.0);
		propertiesTreeModelRef.AppendCategory("Shape", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc");
		propertiesTreeModelRef.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", PROPERTIESACCESS_Mutable, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetShapeNameList());
		propertiesTreeModelRef.AppendProperty("Shape", "Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", PROPERTIESACCESS_ReadOnly);
		break;

	case ITEM_BoundingVolume:
		propertiesTreeModelRef.AppendCategory("Shape", QVariant(), false, false, "Use shapes to establish collision, mouse input, hitbox, etc");
		propertiesTreeModelRef.AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", PROPERTIESACCESS_Mutable, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetShapeNameList());
		propertiesTreeModelRef.AppendProperty("Shape", "Data", PROPERTIESTYPE_LineEdit, "", "A string representation of the shape's data", PROPERTIESACCESS_ReadOnly);
		propertiesTreeModelRef.AppendCategory("Fixture", QVariant(), true, true, "Become a fixture used in physics simulations and collision");
		propertiesTreeModelRef.AppendProperty("Fixture", "Density", PROPERTIESTYPE_double, 0.0, "Usually in kg / m^2. A shape should have a non-zero density when the entity's physics is dynamic", PROPERTIESACCESS_ToggleOff, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Friction", PROPERTIESTYPE_double, 0.2, "The friction coefficient, usually in the range [0,1]", PROPERTIESACCESS_ToggleOff, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Restitution", PROPERTIESTYPE_double, 0.0, "The restitution (elasticity) usually in the range [0,1]", PROPERTIESACCESS_ToggleOff, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Restitution Threshold", PROPERTIESTYPE_double, 1.0, "Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)", PROPERTIESACCESS_ToggleOff, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		propertiesTreeModelRef.AppendProperty("Fixture", "Sensor", PROPERTIESTYPE_bool, Qt::Unchecked, "A sensor shape collects contact information but never generates a collision response", PROPERTIESACCESS_ToggleOff);
		propertiesTreeModelRef.AppendProperty("Fixture", "Filter: Category Mask", PROPERTIESTYPE_int, 0x0001, "The collision category bits for this shape. Normally you would just set one bit", PROPERTIESACCESS_ToggleOff, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		propertiesTreeModelRef.AppendProperty("Fixture", "Filter: Collision Mask", PROPERTIESTYPE_int, 0xFFFF, "The collision mask bits. This states the categories that this shape would accept for collision", PROPERTIESACCESS_ToggleOff, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		propertiesTreeModelRef.AppendProperty("Fixture", "Filter: Group Override", PROPERTIESTYPE_int, 0, "Collision overrides allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision override", PROPERTIESACCESS_ToggleOff, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max(), 1, QString(), QString(), QVariant());
		break;

	case ITEM_AtlasFrame:
		// No texture quad specific properties
		break;

	case ITEM_Sprite:
		propertiesTreeModelRef.AppendCategory("Sprite", pItemData->GetReferencedItemUuid().toString(QUuid::WithoutBraces));
		propertiesTreeModelRef.AppendProperty("Sprite", "Frame", PROPERTIESTYPE_SpriteFrames, 0, "The sprite frame index to start on", PROPERTIESACCESS_ToggleOff, QVariant(), QVariant(), QVariant(), QString(), QString(), pItemData->GetReferencedItemUuid());
		propertiesTreeModelRef.AppendProperty("Sprite", "Anim Rate", PROPERTIESTYPE_double, 1.0, "The animation rate modifier", PROPERTIESACCESS_ToggleOff, 0.0, fRANGE, 0.1);
		propertiesTreeModelRef.AppendProperty("Sprite", "Anim Paused", PROPERTIESTYPE_bool, false, "The current state's animation starts paused", PROPERTIESACCESS_ToggleOff);
		break;

	case ITEM_Text:
		propertiesTreeModelRef.AppendCategory("Text", pItemData->GetReferencedItemUuid().toString(QUuid::WithoutBraces));
		propertiesTreeModelRef.AppendProperty("Text", "Text", PROPERTIESTYPE_LineEdit, "Text123", "What UTF-8 string to be displayed", PROPERTIESACCESS_ToggleOff);
		propertiesTreeModelRef.AppendProperty("Text", "Style", PROPERTIESTYPE_ComboBoxString, HyGlobal::GetTextStyleNameList()[TEXTSTYLE_Line], "The style of how the text is shown", PROPERTIESACCESS_ToggleOff, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTextStyleNameList());
		propertiesTreeModelRef.AppendProperty("Text", "Style Dimensions", PROPERTIESTYPE_vec2, QPointF(200.0f, 50.0f), "Text box size used when required by the style (like ScaleBox or Column)", PROPERTIESACCESS_ToggleOff, 0.0f, fRANGE, 1.0f);
		propertiesTreeModelRef.AppendProperty("Text", "Alignment", PROPERTIESTYPE_ComboBoxString, HyGlobal::GetAlignmentNameList()[HYALIGN_Left], "The alignment of the text", PROPERTIESACCESS_ToggleOff, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetAlignmentNameList());
		propertiesTreeModelRef.AppendProperty("Text", "Monospaced Digits", PROPERTIESTYPE_bool, false, "Check to use monospaced digits, which ensures all digits use the same width", PROPERTIESACCESS_ToggleOff);
		propertiesTreeModelRef.AppendProperty("Text", "Text Indent", PROPERTIESTYPE_int, 0, "The number of pixels to indent the text", PROPERTIESACCESS_ToggleOff, 0, iRANGE, 1);
		break;

	default:
		HyGuiLog(QString("EntityTreeItem::InitalizePropertiesTree - unsupported type: ") % QString::number(pItemData->GetType()), LOGTYPE_Error);
		break;
	}
}
