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
	m_bIsForwardDeclared(bIsForwardDeclared),
	m_ReferencedItemUuid(uuidOfReferencedItem),
	m_bIsSelected(false)
{
	m_bIsAssetItem = HyGlobal::IsItemType_Asset(m_eTYPE);
	m_bIsProjectItem = HyGlobal::IsItemType_Project(m_eTYPE);

	if(m_eEntType == ENTTYPE_Root || m_eEntType == ENTTYPE_Item || m_eEntType == ENTTYPE_ArrayItem)
	{
		for(int i = 0; i < m_EntityModelRef.GetNumStates(); ++i)
		{
			EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityModelRef.GetStateData(i));
			pStateData->InsertNewPropertiesModel(this, QJsonObject());
		}
	}
}

EntityTreeItemData::EntityTreeItemData(EntityModel &entityModelRef, bool bIsForwardDeclared, QJsonObject descObj, QJsonArray propArray, bool bIsArrayItem) :
	TreeModelItemData(HyGlobal::GetTypeFromString(descObj["itemType"].toString()), descObj["UUID"].toString(), descObj["codeName"].toString()),
	m_EntityModelRef(entityModelRef),
	m_eEntType(bIsArrayItem ? ENTTYPE_ArrayItem : ENTTYPE_Item),
	m_sPromotedEntityType(descObj["promotedEntityType"].toString()),
	m_bIsForwardDeclared(bIsForwardDeclared),
	m_ReferencedItemUuid(descObj["itemUUID"].toString()),
	m_bIsSelected(descObj["isSelected"].toBool())
{
	m_bIsAssetItem = HyGlobal::IsItemType_Asset(m_eTYPE);
	m_bIsProjectItem = HyGlobal::IsItemType_Project(m_eTYPE);

	if(propArray.size() != m_EntityModelRef.GetNumStates())
	{
		HyGuiLog("EntityTreeItemData::EntityTreeItemData() - propArray size doesn't equal number of this entity states", LOGTYPE_Error);
		return;
	}

	for(int i = 0; i < m_EntityModelRef.GetNumStates(); ++i)
	{
		EntityStateData *pStateData = static_cast<EntityStateData *>(m_EntityModelRef.GetStateData(i));
		pStateData->InsertNewPropertiesModel(this, propArray[i].toObject());
	}
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

PropertiesTreeModel *EntityTreeItemData::GetPropertiesModel(int iStateIndex)
{
	if(m_eEntType == ENTTYPE_BvFolder || m_eEntType == ENTTYPE_ArrayFolder)
		return nullptr;

	return static_cast<EntityStateData *>(m_EntityModelRef.GetStateData(iStateIndex))->GetPropertiesTreeModel(this);
}

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
QString EntityTreeItemData::GenerateStateSrc(uint32 uiStateIndex, QString sNewLine)
{
	PropertiesTreeModel *pPropertiesModel = GetPropertiesModel(uiStateIndex);
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
	bool bActivatePhysics = false;
	
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
				sSrc += sCodeName + "pos.Set(" + QString::number(valueRef.toPointF().x()) + "f, " + QString::number(valueRef.toPointF().y()) + "f);" + sNewLine;
			else if(sPropertyName == "Scale")
				sSrc += sCodeName + "scale.Set(" + QString::number(valueRef.toPointF().x()) + "f, " + QString::number(valueRef.toPointF().y()) + "f);" + sNewLine;
			else if(sPropertyName == "Rotation")
				sSrc += sCodeName + "rot.Set(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
		}
		else if(sCategoryName == "Body")
		{
			if(sPropertyName == "Visible")
				sSrc += sCodeName + "SetVisible(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Color Tint")
				sSrc += sCodeName + "SetTint(HyColor(" + QString::number(valueRef.toRect().left()) + ", " + QString::number(valueRef.toRect().top()) + ", " + QString::number(valueRef.toRect().width()) + ", " + QString::number(valueRef.toRect().height()) + "));" + sNewLine;
			else if(sPropertyName == "Alpha")
				sSrc += sCodeName + "alpha.Set(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			//else if(sPropertyName == "Display Order")
		}
		else if(sCategoryName == "Physics") // NOTE: This is only from the 'root' node (aka *this entity)
		{
			if(sPropertyName == "Physics_checked") // NOTE: We can rely on *_checked to be the first property in the category
			{
				if(valueRef.toBool() == false)
					sSrc += "physics.Deactivate();" + sNewLine;
			}
			else if(sPropertyName == "Start Activated")
				bActivatePhysics = valueRef.toBool();
			else if(sPropertyName == "Type")
				sSrc += "physics.SetType(static_cast<HyBodyType>(" + QString::number(valueRef.toInt()) + "));" + sNewLine;
			else if(sPropertyName == "Fixed Rotation")
				sSrc += "physics.SetFixedRotation(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Initially Awake")
				sSrc += "physics.SetAwake(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Allow Sleep")
				sSrc += "physics.SetSleepingAllowed(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Gravity Scale")
				sSrc += "physics.SetGravityScale(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Dynamic CCD")
				sSrc += "physics.SetCcd(" + (valueRef.toBool() ? QString("true);") : QString("false);")) + sNewLine;
			else if(sPropertyName == "Linear Damping")
				sSrc += "physics.SetLinearDamping(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Angular Damping")
				sSrc += "physics.SetAngularDamping(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Linear Velocity")
				sSrc += "physics.SetVel(" + QString::number(valueRef.toPointF().x()) + "f, " + QString::number(valueRef.toPointF().y()) + "f);" + sNewLine;
			else if(sPropertyName == "Angular Velocity")
				sSrc += "physics.SetAngVel(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
		}
		else if(sCategoryName == "Primitive")
		{
			if(sPropertyName == "Wireframe")
				sSrc += sCodeName + "SetWireframe(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Line Thickness")
				sSrc += sCodeName + "SetLineThickness(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
		}
		else if(sCategoryName == "Shape")
		{
			if(sPropertyName == "Data")
			{
				EditorShape eShapeType = HyGlobal::GetShapeFromString(pPropertiesModel->FindPropertyValue("Shape", "Type").toString());
				sSrc += ShapeCtrl::DeserializeAsRuntimeCode(sCodeName, eShapeType, valueRef.toString(), sNewLine);
			}
		}
		else if(sCategoryName == "Fixture")
		{
			if(sPropertyName == "Fixture_checked") // NOTE: We can rely on *_checked to be the first property in the category
				sSrc += sCodeName + "SetFixtureAllowed(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Density")
				sSrc += sCodeName + "SetDensity(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Friction")
				sSrc += sCodeName + "SetFriction(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Restitution")
				sSrc += sCodeName + "SetRestitution(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Restitution Threshold")
				sSrc += sCodeName + "SetRestitutionThreshold(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Sensor")
				sSrc += sCodeName + "SetSensor(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
			else if(sPropertyName == "Filter: Category Mask")
			{
				uint16 iCategory = static_cast<uint16>(valueRef.toInt());
				uint16 iMask = static_cast<uint16>(pPropertiesModel->FindPropertyValue("Fixture", "Filter: Collision Mask").toInt());
				uint16 iGroup = static_cast<uint16>(pPropertiesModel->FindPropertyValue("Fixture", "Filter: Group Override").toInt());
				sSrc += sCodeName + "SetFilter([](){ b2Filter filter = { " + QString::number(iCategory) + ", " + QString::number(iMask) + ", " + QString::number(iGroup) + " }; return filter; }());" + sNewLine;
			}
		}
		else if(sCategoryName == "Sprite")
		{
			if(sPropertyName == "Frame")
				sSrc += sCodeName + "SetFrame(" + QString::number(valueRef.toInt()) + ");" + sNewLine;
			else if(sPropertyName == "Anim Rate")
				sSrc += sCodeName + "SetAnimRate(" + QString::number(valueRef.toDouble()) + "f);" + sNewLine;
			else if(sPropertyName == "Anim Paused")
				sSrc += sCodeName + "SetAnimPaused(" + (valueRef.toBool() ? "true" : "false") + ");" + sNewLine;
		}
		else if(sCategoryName == "Text")
		{
			if(sPropertyName == "Text")
				sSrc += sCodeName + "SetText(" + valueRef.toString() + ");" + sNewLine;
			else if(sPropertyName == "Style")
			{
				QPointF vStyleDimensions = pPropertiesModel->FindPropertyValue("Text", "Style Dimensions").toPointF();
				switch(HyGlobal::GetTextStyleFromString(valueRef.toString()))
				{
				case TEXTSTYLE_Line:				sSrc += sCodeName + "SetAsLine();" + sNewLine; break;
				case TEXTSTYLE_Column:				sSrc += sCodeName + "SetAsColumn(" + QString::number(vStyleDimensions.x()) + ");" + sNewLine; break;
				case TEXTSTYLE_ScaleBox:			sSrc += sCodeName + "SetAsScaleBox(" + QString::number(vStyleDimensions.x()) + ", " + QString::number(vStyleDimensions.y()) + ", true);" + sNewLine; break;
				case TEXTSTYLE_ScaleBoxTopAlign:	sSrc += sCodeName + "SetAsScaleBox(" + QString::number(vStyleDimensions.x()) + ", " + QString::number(vStyleDimensions.y()) + ", false);" + sNewLine; break;
				case TEXTSTYLE_Vertical:			sSrc += sCodeName + "SetAsVertical();" + sNewLine; break;
				}
			}
		}
	};
	pPropertiesModel->ForEachProperty(fpForEach, true);

	if(bActivatePhysics)
		sSrc += "physics.Activate();" + sNewLine;

	return sSrc;
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
	EntityTreeItemData *pThisEntityItem = new EntityTreeItemData(m_ModelRef, false, sEntityCodeName, ITEM_Entity, ENTTYPE_Root, uuidOfEntity, uuidOfEntity);
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

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pProjItem->GetName(false));

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ShouldForwardDeclare(pProjItem), sCodeName, pProjItem->GetType(), ENTTYPE_Item, pProjItem->GetUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewAsset(IAssetItemData *pAssetItem, QString sCodeNamePrefix, int iRow /*= -1*/)
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

EntityTreeItemData *EntityTreeModel::Cmd_InsertNewItem(QJsonObject descObj, QJsonArray propArray, bool bIsArrayItem, int iRow /*= -1*/)
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

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ShouldForwardDeclare(descObj), descObj, propArray, bIsArrayItem);
	iRow = (iRow < 0 || (bIsArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);

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

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, false, sCodeName, bIsPrimitive ? ITEM_Primitive : ITEM_BoundingVolume, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	for(int iStateIndex = 0; iStateIndex < m_ModelRef.GetNumStates(); ++iStateIndex)
	{
		PropertiesTreeModel *pPropTreeModel = pNewItem->GetPropertiesModel(iStateIndex);
		if(pPropTreeModel == nullptr)
		{
			HyGuiLog("EntityTreeModel::Cmd_InsertNewShape - Failed to get properties model for state: " % QString::number(iStateIndex), LOGTYPE_Error);
			continue;
		}

		pPropTreeModel->SetPropertyValue("Shape", "Type", HyGlobal::ShapeName(eShape));
		pPropTreeModel->SetPropertyValue("Shape", "Data", sData);
	}

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
				if(m_ModelRef.GetItem().GetWidget())
					iStateIndex = m_ModelRef.GetItem().GetWidget()->GetCurStateIndex();
				
				PropertiesTreeModel *pPropTreeModel = pItem->GetPropertiesModel(iStateIndex);
				if(pPropTreeModel == nullptr)
				{
					HyGuiLog("EntityTreeModel::data() - Shape item PropertiesTreeModel is nullptr", LOGTYPE_Error);
					return QVariant();
				}

				switch(HyGlobal::GetShapeFromString(pPropTreeModel->FindPropertyValue("Shape", "Type").toString()))
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

		iArrayFolderRow = HyMath::Min(pParentTreeItemOut->GetNumChildren() - 1, iArrayFolderRow);
		if(insertRow(iArrayFolderRow, parentIndex) == false)
		{
			HyGuiLog("EntityTreeModel::Cmd_InsertNewChild() - ArrayFolder insertRow failed", LOGTYPE_Error);
			return nullptr;
		}
		// Allocate and store the new array folder item in the tree model
		EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, false, sCodeName, eItemType, ENTTYPE_ArrayFolder, QUuid(), QUuid());
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
