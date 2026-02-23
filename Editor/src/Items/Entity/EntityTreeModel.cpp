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
#include "EntityUndoCmds.h"
#include "GfxPrimitiveModel.h"
#include "GfxShapeModel.h"
#include "GfxChainModel.h"
#include "ContainerEditModel.h"

#include <QVariant>
#include <QStack>

EntityPropertiesTreeModel::EntityPropertiesTreeModel(ProjectItemData &projItemRef, int iStateIndex, QVariant subState, QObject *pParent /*= nullptr*/) :
	PropertiesTreeModel(&projItemRef, iStateIndex, subState, pParent)
{
}

/*virtual*/ EntityPropertiesTreeModel::~EntityPropertiesTreeModel()
{ }

/*virtual*/ PropertiesUndoCmd *EntityPropertiesTreeModel::AllocateUndoCmd(const QModelIndex &index, const QVariant &newData) /*override*/
{
	return new EntityUndoCmd_PropertyModified(this, index, newData);
}

EntityPropertiesTreeMultiModel::EntityPropertiesTreeMultiModel(ProjectItemData &projItemRef, int iStateIndex, QVariant subState, QList<PropertiesTreeModel *> multiModelList, QObject *pParent /*= nullptr*/) :
	PropertiesTreeMultiModel(projItemRef, iStateIndex, subState, multiModelList, pParent)
{
}

/*virtual*/ EntityPropertiesTreeMultiModel::~EntityPropertiesTreeMultiModel()
{ }

/*virtual*/ PropertiesUndoCmd *EntityPropertiesTreeMultiModel::AllocateUndoCmd(const QModelIndex &index, const QVariant &newData) /*override*/
{
	return new EntityUndoCmd_PropertyModified(this, index, newData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityTreeItemData::EntityTreeItemData(EntityModel &entityModelRef, EntityItemDeclarationType eDeclarationType, QString sCodeName, ItemType eItemType, EntityItemType eEntType, QUuid uuidOfReferencedItem, QUuid uuidOfThis) :
	TreeModelItemData(eItemType, uuidOfThis, sCodeName),
	m_EntityModelRef(entityModelRef),
	m_eEntType(eEntType),
	m_pPropertiesModel(nullptr),
	m_pEditModel(nullptr),
	m_eDeclarationType(eDeclarationType),
	m_ReferencedItemUuid(uuidOfReferencedItem),
	m_bIsLocked(false),
	m_bIsSelected(false),
	m_bIsDopeExpanded(true),
	m_PreviewComponent()
{
	QVariant ptrVariant;
	ptrVariant.setValue<TreeModelItemData *>(this);
	m_pPropertiesModel = new EntityPropertiesTreeModel(entityModelRef.GetItem(), -1, ptrVariant, this);

	if(m_eEntType == ENTTYPE_Root || m_eEntType == ENTTYPE_FusedItem || m_eEntType == ENTTYPE_Item || m_eEntType == ENTTYPE_ArrayItem)
		InitalizePropertyModel();
}

EntityTreeItemData::EntityTreeItemData(EntityModel &entityModelRef, QJsonObject descObj, bool bIsArrayItem, bool bIsFusedItem) :
	TreeModelItemData(HyGlobal::GetTypeFromString(descObj["itemType"].toString()), QUuid(descObj["UUID"].toString()), descObj["codeName"].toString()),
	m_EntityModelRef(entityModelRef),
	m_eEntType(bIsArrayItem ? ENTTYPE_ArrayItem : (bIsFusedItem ? ENTTYPE_FusedItem : ENTTYPE_Item)),
	m_pPropertiesModel(nullptr),
	m_pEditModel(nullptr),
	m_sPromotedEntityType(descObj["promotedEntityType"].toString()),
	m_eDeclarationType(HyGlobal::GetEntityDeclType(descObj["declarationType"].toString())),
	m_ReferencedItemUuid(descObj["itemUUID"].toString()),
	m_bIsLocked(descObj["isLocked"].toBool(false)),
	m_bIsSelected(descObj["isSelected"].toBool(false)),
	m_bIsDopeExpanded(descObj["isDopeExpanded"].toBool(true)),
	m_PreviewComponent()
{
	QVariant ptrVariant;
	ptrVariant.setValue<TreeModelItemData *>(this);
	m_pPropertiesModel = new EntityPropertiesTreeModel(entityModelRef.GetItem(), -1, ptrVariant, this);

	InitalizePropertyModel();
}

/*virtual*/ EntityTreeItemData::~EntityTreeItemData()
{
	delete m_pEditModel;
	delete m_pPropertiesModel;
}

bool EntityTreeItemData::IsSelectable() const
{
	if(m_eEntType == ENTTYPE_FixtureFolder)
		return false;

	return m_bIsLocked == false;
}

bool EntityTreeItemData::IsEditable() const
{
	return m_pEditModel != nullptr && m_bIsLocked == false;
}

void EntityTreeItemData::SetLocked(bool bIsLocked)
{
	m_bIsLocked = bIsLocked;
	m_bIsSelected = IsSelectable() ? m_bIsSelected : false;
}

EntityItemType EntityTreeItemData::GetEntType() const
{
	return m_eEntType;
}

QString EntityTreeItemData::GetHyNodeTypeName(bool bIncludeNamespace) const
{
	switch(m_eTYPE)
	{
	case ITEM_Primitive:		return "HyPrimitive2d";
	case ITEM_Audio:			return "HyAudio2d";
	case ITEM_Text:				return "HyText2d";
	case ITEM_Spine:			return "HySpine2d";
	case ITEM_Sprite:			return "HySprite2d";
	case ITEM_AtlasFrame:		return "HyTexturedQuad2d";
	case ITEM_ShapeFixture: 	return "HyShape2d";
	case ITEM_ChainFixture: 	return "HyChain2d";
	case ITEM_Entity: {
		if(m_sPromotedEntityType.isEmpty() == false)
			return m_sPromotedEntityType;

		QUuid referencedItemUuid = GetReferencedItemUuid();
		ProjectItemData *pReferencedItemData = static_cast<ProjectItemData *>(m_EntityModelRef.GetItem().GetProject().FindItemData(referencedItemUuid));
		if(pReferencedItemData == nullptr)
			HyGuiLog("Could not find referenced item data from Sub-Entity's UUID: " + referencedItemUuid.toString(), LOGTYPE_Error);

		if(bIncludeNamespace)
			return "hy::" + pReferencedItemData->GetName(false);
		else
			return pReferencedItemData->GetName(false);
	}

	case ITEM_UiLabel:			return "HyLabel";
	case ITEM_UiRichLabel:		return "HyRichLabel";
	case ITEM_UiButton:			return "HyButton";
	case ITEM_UiRackMeter:		return "HyRackMeter";
	case ITEM_UiBarMeter:		return "HyBarMeter";
	case ITEM_UiCheckBox:		return "HyCheckBox";
	case ITEM_UiRadioButton:	return "HyRadioButton";
	case ITEM_UiTextField:		return "HyTextField";
	case ITEM_UiComboBox:		return "HyComboBox";
	case ITEM_UiSlider:			return "HySlider";

	case ITEM_UiLayout:
	case ITEM_UiSpacer:
	case ITEM_SoundClip:
	case ITEM_Prefab:
	case ITEM_Particles:
	default:
		HyGuiLog("EntityTreeItemData::GetHyNodeTypeName() - Unhandled item type: " % HyGlobal::ItemName(m_eTYPE, false), LOGTYPE_Error);
		break;
	}

	return QString();
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

void EntityTreeItemData::SetReferencedItemUuid(QUuid uuidOfReferencedItem)
{
	m_ReferencedItemUuid = uuidOfReferencedItem;
}

bool EntityTreeItemData::IsPromotedEntity() const
{
	return m_sPromotedEntityType.isEmpty() == false;
}

EntityItemDeclarationType EntityTreeItemData::GetDeclarationType() const
{
	return m_eDeclarationType;
}

EntityModel &EntityTreeItemData::GetEntityModel() const
{
	return m_EntityModelRef;
}

EntityPropertiesTreeModel &EntityTreeItemData::GetPropertiesModel() const
{
	return *m_pPropertiesModel;
}

IGfxEditModel *EntityTreeItemData::GetEditModel()
{
	return m_pEditModel;
}

bool EntityTreeItemData::IsSelected() const
{
	return m_bIsSelected;
}

void EntityTreeItemData::SetSelected(bool bIsSelected)
{
	m_bIsSelected = IsSelectable() ? bIsSelected : false;
}

bool EntityTreeItemData::IsDopeExpanded() const
{
	return m_bIsDopeExpanded;
}

void EntityTreeItemData::SetDopeExpanded(bool bIsDopeExpanded)
{
	m_bIsDopeExpanded = bIsDopeExpanded;
}

bool EntityTreeItemData::IsReallocateDrawItem() const
{
	return m_PreviewComponent.m_bReallocateDrawItem;
}

void EntityTreeItemData::SetReallocateDrawItem(bool bReallocateDrawItem)
{
	m_PreviewComponent.m_bReallocateDrawItem = bReallocateDrawItem;
}

EntityPreviewComponent &EntityTreeItemData::GetPreviewComponent()
{
	return m_PreviewComponent;
}

int EntityTreeItemData::GetArrayIndex() const
{
	if(m_eEntType == ENTTYPE_ArrayItem)
	{
		QModelIndex thisIndex = m_EntityModelRef.GetTreeModel().FindIndex<EntityTreeItemData *>(const_cast<EntityTreeItemData *>(this), 0);
		if(thisIndex.isValid())
			return thisIndex.row();
		else
			HyGuiLog("EntityTreeItemData::GetArrayIndex() - Failed to find index of this array item", LOGTYPE_Error);
	}
	
	return -1;
}

int EntityTreeItemData::GetNumArrayItems() const
{
	if(m_eEntType == ENTTYPE_ArrayItem)
	{
		QModelIndex thisIndex = m_EntityModelRef.GetTreeModel().FindIndex<EntityTreeItemData *>(const_cast<EntityTreeItemData *>(this), 0);
		QModelIndex arrayFolderIndex = m_EntityModelRef.GetTreeModel().parent(thisIndex);
		if(thisIndex.isValid() && arrayFolderIndex.isValid())
			return m_EntityModelRef.GetTreeModel().rowCount(arrayFolderIndex);
		else
			HyGuiLog("EntityTreeItemData::GetNumArrayItems() - Failed to find parent index of this array item", LOGTYPE_Error);
	}

	return -1;
}

void EntityTreeItemData::InsertJsonInfo_Desc(QJsonObject &childObjRef)
{
	// Common stuff
	childObjRef.insert("codeName", GetCodeName());
	childObjRef.insert("itemType", HyGlobal::ItemName(m_eTYPE, false));
	childObjRef.insert("UUID", GetUuid().toString(QUuid::WithoutBraces));
	childObjRef.insert("promotedEntityType", m_sPromotedEntityType);
	childObjRef.insert("declarationType", ENTITYITEMDECLARATIONTYPE_STRINGS[m_eDeclarationType]);
	childObjRef.insert("itemUUID", m_ReferencedItemUuid.toString(QUuid::WithoutBraces));
	childObjRef.insert("isLocked", m_bIsLocked);
	childObjRef.insert("isSelected", m_bIsSelected);
	childObjRef.insert("isDopeExpanded", m_bIsDopeExpanded);
	if(m_EntityModelRef.GetCtorKeyFramesMap().contains(this))
		childObjRef.insert("ctor", m_EntityModelRef.GetCtorKeyFramesMap()[this]);
	else
		childObjRef.insert("ctor", QJsonObject());
}

void EntityTreeItemData::InitializeRootBaseClass(EntityBaseClassType eBaseClass)
{
	if(GetEntType() != ENTTYPE_Root)
	{
		HyGuiLog("EntityTreeItemData::InitializeRootBaseClass() can only be called on Root entities", LOGTYPE_Error);
		return;
	}

	switch(eBaseClass)
	{
	case ENTBASECLASS_HyEntity2d:
		break;

	case ENTBASECLASS_HyActor2d:
		m_pPropertiesModel->InsertCategory(0, "Actor");
		m_pPropertiesModel->AppendProperty("Actor", "Jump", PROPERTIESTYPE_bool, Qt::Checked, "Actor attempts to perform a jump", PROPERTIESACCESS_ToggleUnchecked);
		break;
	case ENTBASECLASS_HyUiContainer:
		m_pPropertiesModel->InsertCategory(0, "Panel", QVariant(), false, "The main visual background portion of this container");
		m_pPropertiesModel->AppendProperty("Panel", "Setup", PROPERTIESTYPE_UiPanel, QVariant(), "Initializes and setup the main panel of this container", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Panel", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Panel", "Alpha", PROPERTIESTYPE_double, 1.0, "A value from 0.0 to 1.0 that indicates how opaque/transparent this item is", PROPERTIESACCESS_ToggleUnchecked, 0.0, 1.0, 0.05);
		
		m_pPropertiesModel->InsertCategory(0, "UI Container");
		m_pPropertiesModel->AppendProperty("UI Container", "Use Scroll Bars", PROPERTIESTYPE_bool, Qt::Checked, "", PROPERTIESACCESS_ToggleUnchecked);

		m_pEditModel = new ContainerEditModel();
		break;
	}
}

// NOTE: The listed 4 functions below share logic that process all item properties. Any updates should reflect to all of them
//             - EntityTreeItemData::InitalizePropertyModel
//             - EntityModel::GenerateSrc_SetStateImpl
//             - EntityDrawItem::ExtractPropertyData
//             - ExtrapolateProperties
void EntityTreeItemData::InitalizePropertyModel()
{
	// Default ranges
	const int iRANGE = MAX_INT_RANGE;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
	const double fRANGE = 16777215.0f;
	const double dRANGE = 16777215.0;

	const bool bIsBody = GetType() != ITEM_Audio;
	if(IsFixtureItem() == false && IsLayoutItem() == false)
	{
		if(GetEntType() == ENTTYPE_Root || GetType() == ITEM_Entity)
		{
			m_pPropertiesModel->InsertCategory(-1, "Timeline");
			m_pPropertiesModel->AppendProperty("Timeline", "State", PROPERTIESTYPE_StatesComboBox, 0, "Jump to a new state after processing this frame", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), GetReferencedItemUuid());
			m_pPropertiesModel->AppendProperty("Timeline", "Pause", PROPERTIESTYPE_bool, Qt::Unchecked, "Pausing the timeline will stop processing key frames, after this frame", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Timeline", "Frame", PROPERTIESTYPE_int, 0, "Jump to a different frame on the timeline, after processing this frame", PROPERTIESACCESS_ToggleUnchecked, 0, iRANGE, 1);
		}
		else if(IsAssetItem() == false)
		{
			m_pPropertiesModel->InsertCategory(-1, "Common");
			m_pPropertiesModel->AppendProperty("Common", "State", PROPERTIESTYPE_StatesComboBox, 0, "The " % HyGlobal::ItemName(GetType(), false) % "'s state to be displayed", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), GetReferencedItemUuid());
		}

		m_pPropertiesModel->InsertCategory(-1, "Common"); // Will just return 'false' if "Common" category already exists
		m_pPropertiesModel->AppendProperty("Common", "Update During Paused", PROPERTIESTYPE_bool, Qt::Unchecked, "Only items with this checked will receive updates when the game/application is paused", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Common", "User Tag", PROPERTIESTYPE_int, 0, "Not used by Harmony. You can set it to anything you like", PROPERTIESACCESS_ToggleUnchecked, -iRANGE, iRANGE, 1);

		m_pPropertiesModel->InsertCategory(-1, "Transformation");
		m_pPropertiesModel->AppendProperty("Transformation", "Position", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Position is relative to parent node", PROPERTIESACCESS_ToggleUnchecked, -fRANGE, fRANGE, 1.0, "[", "]");
		m_pPropertiesModel->AppendProperty("Transformation", "Scale", PROPERTIESTYPE_vec2, QPointF(1.0f, 1.0f), "Scale is relative to parent node", PROPERTIESACCESS_ToggleUnchecked, -fRANGE, fRANGE, 0.01, "[", "]");
		m_pPropertiesModel->AppendProperty("Transformation", "Rotation", PROPERTIESTYPE_double, 0.0, "Rotation is relative to parent node", PROPERTIESACCESS_ToggleUnchecked, -360.0, 360.0, 0.1);

		if(bIsBody)
		{
			m_pPropertiesModel->InsertCategory(-1, "Body");
			m_pPropertiesModel->AppendProperty("Body", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets rendered", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Body", "Color Tint", PROPERTIESTYPE_Color, QRect(255, 255, 255, 0), "A color to alpha blend this item with", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Body", "Alpha", PROPERTIESTYPE_double, 1.0, "A value from 0.0 to 1.0 that indicates how opaque/transparent this item is", PROPERTIESACCESS_ToggleUnchecked, 0.0, 1.0, 0.05);
			m_pPropertiesModel->AppendProperty("Body", "Override Display Order", PROPERTIESTYPE_int, 0, "Higher display orders get drawn above other items with less. Undefined ordering when equal", PROPERTIESACCESS_ToggleUnchecked, -iRANGE, iRANGE, 1);
		}
		
		if(GetEntType() == ENTTYPE_Root)
		{
			m_pPropertiesModel->InsertCategory(-1, "Physics", QVariant(), true, "Optionally create a physics component that can affect the transformation of this entity");
			m_pPropertiesModel->AppendProperty("Physics", "Activate/Deactivate", PROPERTIESTYPE_bool, Qt::Checked, "This entity will begin its physics simulation", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Physics", "Type", PROPERTIESTYPE_ComboBoxInt, 0, "A static body does not move. A kinematic body moves only by forces. A dynamic body moves by forces and collision (fully simulated)", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", QStringList() << "Static" << "Kinematic" << "Dynamic");
			m_pPropertiesModel->AppendProperty("Physics", "Fixed Rotation", PROPERTIESTYPE_bool, Qt::Unchecked, "Prevents this body from rotating if checked. Useful for characters", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Physics", "Initially Awake", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to make body initially awake. Start sleeping otherwise", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Physics", "Allow Sleep", PROPERTIESTYPE_bool, Qt::Checked, "Uncheck this if this body should never fall asleep. This increases CPU usage", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Physics", "Gravity Scale", PROPERTIESTYPE_double, 1.0, "Adjusts the gravity on this single body. Negative values will reverse gravity. Increased gravity can decrease stability", PROPERTIESACCESS_ToggleUnchecked, -100.0, 100.0, 0.1);
			m_pPropertiesModel->AppendProperty("Physics", "Dynamic CCD", PROPERTIESTYPE_bool, Qt::Unchecked, "Continuous collision detection for other dynamic moving bodies. Note that all bodies are prevented from tunneling through kinematic and static bodies. This setting is only considered on dynamic bodies. You should use this flag sparingly since it increases processing time", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Physics", "Linear Damping", PROPERTIESTYPE_double, 0.0, "Reduces the world linear velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", PROPERTIESACCESS_ToggleUnchecked, 0.0, 100.0, 0.01);
			m_pPropertiesModel->AppendProperty("Physics", "Angular Damping", PROPERTIESTYPE_double, 0.01, "Reduces the world angular velocity over time. 0 means no damping. Normally you will use a damping value between 0 and 0.1", PROPERTIESACCESS_ToggleUnchecked, 0.0, 100.0, 0.01);
			m_pPropertiesModel->AppendProperty("Physics", "Linear Velocity", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "Starting Linear velocity of the body's origin in scene coordinates", PROPERTIESACCESS_ToggleUnchecked, -fRANGE, fRANGE, 1.0, "[", "]");
			m_pPropertiesModel->AppendProperty("Physics", "Angular Velocity", PROPERTIESTYPE_double, 0.0, "Starting Angular velocity of the body", PROPERTIESACCESS_ToggleUnchecked, 0.0, 100.0, 0.01);
		}
	}

	switch(GetType())
	{
	case ITEM_Primitive: {
		m_pEditModel = new GfxPrimitiveModel();

		m_pPropertiesModel->InsertCategory(0, "Primitive", QVariant(), false, "A visible shape that can be drawn to the screen");
		m_pPropertiesModel->AppendProperty("Primitive", "Wireframe", PROPERTIESTYPE_bool, Qt::Unchecked, "Check to render only the wireframe of the shape type", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Primitive", "Line Thickness", PROPERTIESTYPE_double, 1.0, "When applicable, how thick to render lines", PROPERTIESACCESS_ToggleUnchecked, 1.0, 100.0, 1.0);

		QStringList sComboBoxList = HyGlobal::GetShapeNameList();
		sComboBoxList.append("Line Chain");
		m_pPropertiesModel->AppendProperty("Primitive", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", sComboBoxList);
		m_pPropertiesModel->AppendProperty("Primitive", "Data", PROPERTIESTYPE_FloatArray, "", "An array of floats representing the primitive's data", PROPERTIESACCESS_ToggleUnchecked);
		break; }

	case ITEM_Audio:
		m_pPropertiesModel->InsertCategory(0, "Audio", GetReferencedItemUuid().toString(QUuid::WithoutBraces));
		// TODO: m_pPropertiesModel->AppendProperty("Audio", "Play List Mode", PROPERTIESTYPE_ComboBoxString, HyGlobal::GetAudioPlayListModeList()[HYPLAYLIST_Shuffle], "The method by which the next audio asset is chosen when played", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetAudioPlayListModeList());
		// TODO: m_pPropertiesModel->AppendProperty("Audio", "Play", 
		m_pPropertiesModel->AppendProperty("Audio", "Volume", PROPERTIESTYPE_double, 1.0, "The volume of the audio", PROPERTIESACCESS_ToggleUnchecked, 0.0, 1.0, 0.01);
		m_pPropertiesModel->AppendProperty("Audio", "Pitch", PROPERTIESTYPE_double, 1.0, "The pitch of the audio", PROPERTIESACCESS_ToggleUnchecked, 0.0, fRANGE, 0.01);
		break;

	case ITEM_Text:
		m_pPropertiesModel->InsertCategory(0, "Text", GetReferencedItemUuid().toString(QUuid::WithoutBraces));
		m_pPropertiesModel->AppendProperty("Text", "Text", PROPERTIESTYPE_LineEdit, "Text123", "What UTF-8 string to be displayed", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Text", "Style", PROPERTIESTYPE_ComboBoxString, HyGlobal::GetTextTypeNameList()[HYTEXT_Line], "The style of how the text is shown", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTextTypeNameList());
		// TODO: Custom Text Style widget
		m_pPropertiesModel->AppendProperty("Text", "Style Dimensions", PROPERTIESTYPE_vec2, QPointF(200.0f, 50.0f), "Text box size used when required by the style (like ScaleBox or Column)", PROPERTIESACCESS_ToggleUnchecked, 0.0f, fRANGE, 1.0f);
		m_pPropertiesModel->AppendProperty("Text", "Alignment", PROPERTIESTYPE_ComboBoxString, HyGlobal::AlignmentName(HYALIGN_Left), "The alignment of the text", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetAlignmentNameList());
		m_pPropertiesModel->AppendProperty("Text", "Monospaced Digits", PROPERTIESTYPE_bool, false, "Check to use monospaced digits, which ensures all digits use the same width", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Text", "Text Indent", PROPERTIESTYPE_int, 0, "The number of pixels to indent the text", PROPERTIESACCESS_ToggleUnchecked, 0, iRANGE, 1);
		break;

	case ITEM_Spine:
		break;

	case ITEM_Sprite:
		m_pPropertiesModel->InsertCategory(0, "Sprite", GetReferencedItemUuid().toString(QUuid::WithoutBraces));
		m_pPropertiesModel->AppendProperty("Sprite", "Frame", PROPERTIESTYPE_SpriteFrames, 0, "The sprite frame index to start on", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), GetReferencedItemUuid());
		m_pPropertiesModel->AppendProperty("Sprite", "Anim Pause", PROPERTIESTYPE_bool, false, "The current state's animation starts paused", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Sprite", "Anim Rate", PROPERTIESTYPE_double, 1.0, "The animation rate modifier", PROPERTIESACCESS_ToggleUnchecked, 0.0, fRANGE, 0.1);
		m_pPropertiesModel->AppendProperty("Sprite", "Anim Loop", PROPERTIESTYPE_bool, false, "Override whatever the sprite's loop flag is, and make the animation loop (check) or don't loop (uncheck)", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Sprite", "Anim Reverse", PROPERTIESTYPE_bool, false, "Override whatever the sprite's reverse flag is, and make the animation play in reverse (checked) or don't play in reverse (uncheck)", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Sprite", "Anim Bounce", PROPERTIESTYPE_bool, false, "Override whatever the sprite's bounce flag is, and make the animation bounce (check) or don't bounce (uncheck)", PROPERTIESACCESS_ToggleUnchecked);
		break;

	case ITEM_Entity:
		m_pPropertiesModel->InsertCategory(0, "Entity", QVariant(), false, "Entity is an object that controls multiple nodes and components");
		m_pPropertiesModel->AppendProperty("Entity", "Mouse Input", PROPERTIESTYPE_bool, Qt::Unchecked, "Mouse hover and button inputs over this bounding volume or specified shapes", PROPERTIESACCESS_ToggleUnchecked);
		break;

	case ITEM_AtlasFrame:
		// No HyTexturedQuad2d specific properties
		break;

	case ITEM_ShapeFixture:
		m_pEditModel = new GfxShapeModel(HyGlobal::GetEditorColor(EDITORCOLOR_Fixtures));

		// NOTE: This should be the first categories added for fixtures
		m_pPropertiesModel->InsertCategory(-1, "Shape", QVariant(), false, "Use shapes to establish collision, mouse input, hitbox, etc");
		m_pPropertiesModel->AppendProperty("Shape", "Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::ShapeName(SHAPE_None), "The type of shape this is", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetShapeNameList());
		m_pPropertiesModel->AppendProperty("Shape", "Data", PROPERTIESTYPE_FloatArray, "", "An array of floats representing the shape's data", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->InsertCategory(-1, "Fixture", QVariant(), false, "Become a fixture used in physics simulations and collision");
		m_pPropertiesModel->AppendProperty("Fixture", "Density", PROPERTIESTYPE_double, 0.0, "Usually in kg / m^2. A shape should have a non-zero density when the entity's physics is dynamic", PROPERTIESACCESS_ToggleUnchecked, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_pPropertiesModel->AppendProperty("Fixture", "Friction", PROPERTIESTYPE_double, 0.2, "The friction coefficient, usually in the range [0,1]", PROPERTIESACCESS_ToggleUnchecked, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_pPropertiesModel->AppendProperty("Fixture", "Restitution", PROPERTIESTYPE_double, 0.0, "The restitution (elasticity) usually in the range [0,1]", PROPERTIESACCESS_ToggleUnchecked, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_pPropertiesModel->AppendProperty("Fixture", "Sensor", PROPERTIESTYPE_bool, Qt::Unchecked, "A sensor shape collects contact information but never generates a collision response", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Fixture", "Filter: Category Mask", PROPERTIESTYPE_int, 0x0001, "The collision category bits for this shape. Normally you would just set one bit", PROPERTIESACCESS_ToggleUnchecked, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_pPropertiesModel->AppendProperty("Fixture", "Filter: Collision Mask", PROPERTIESTYPE_int, 0xFFFF, "The collision mask bits. This states the categories that this shape would accept for collision", PROPERTIESACCESS_ToggleUnchecked, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_pPropertiesModel->AppendProperty("Fixture", "Filter: Group Override", PROPERTIESTYPE_int, 0, "Collision overrides allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision override", PROPERTIESACCESS_ToggleUnchecked, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max(), 1, QString(), QString(), QVariant());
		break;

	case ITEM_ChainFixture:
		m_pEditModel = new GfxChainModel(HyGlobal::GetEditorColor(EDITORCOLOR_Fixtures));

		// NOTE: This should be the first categories added for fixtures
		m_pPropertiesModel->InsertCategory(-1, "Chain", QVariant(), false, "Use shapes to establish collision, mouse input, hitbox, etc");
		m_pPropertiesModel->AppendProperty("Chain", "Data", PROPERTIESTYPE_FloatArray, "", "An array of floats representing the chain's data", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->InsertCategory(-1, "Fixture", QVariant(), false, "Become a fixture used in physics simulations and collision");
		m_pPropertiesModel->AppendProperty("Fixture", "Friction", PROPERTIESTYPE_double, 0.2, "The friction coefficient, usually in the range [0,1]", PROPERTIESACCESS_ToggleUnchecked, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_pPropertiesModel->AppendProperty("Fixture", "Restitution", PROPERTIESTYPE_double, 0.0, "The restitution (elasticity) usually in the range [0,1]", PROPERTIESACCESS_ToggleUnchecked, 0.0, fRANGE, 0.001, QString(), QString(), 5);
		m_pPropertiesModel->AppendProperty("Fixture", "Filter: Category Mask", PROPERTIESTYPE_int, 0x0001, "The collision category bits for this shape. Normally you would just set one bit", PROPERTIESACCESS_ToggleUnchecked, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_pPropertiesModel->AppendProperty("Fixture", "Filter: Collision Mask", PROPERTIESTYPE_int, 0xFFFF, "The collision mask bits. This states the categories that this shape would accept for collision", PROPERTIESACCESS_ToggleUnchecked, 0, 0xFFFF, 1, QString(), QString(), QVariant());
		m_pPropertiesModel->AppendProperty("Fixture", "Filter: Group Override", PROPERTIESTYPE_int, 0, "Collision overrides allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision override", PROPERTIESACCESS_ToggleUnchecked, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max(), 1, QString(), QString(), QVariant());
		break;

	case ITEM_UiLayout:
		m_pPropertiesModel->InsertCategory(-1, "Layout", QVariant(), false, "Holds UI widget entities and arranges them programatically");
		m_pPropertiesModel->AppendProperty("Layout", "Orientation", PROPERTIESTYPE_ComboBoxString, HyGlobal::OrientationName(HYORIENT_Horizontal), "The orientation of this layout. This determines which direction UI widgets are arranged", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), HyGlobal::GetOrientationNameList());
		m_pPropertiesModel->AppendProperty("Layout", "Margins", PROPERTIESTYPE_vec4, QRectF(0.0f, 0.0f, 0.0f, 0.0f), "The layout's margins to keep its contained widgets within (in pixels)", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Layout", "Widget Spacing", PROPERTIESTYPE_int, 0, "Sets the spacing in pixels between widgets inside the layout", PROPERTIESACCESS_ToggleUnchecked);
		break;

	case ITEM_UiSpacer:
		m_pPropertiesModel->InsertCategory(-1, "Spacer", QVariant(), false, "Pads space between widgets within a layout");
		m_pPropertiesModel->AppendProperty("Spacer", "Size Policy", PROPERTIESTYPE_ComboBoxString, HyGlobal::SizePolicyName(HYSIZEPOLICY_Expanding), "Hints to the spacer on how it should size itself", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), HyGlobal::GetSizePolicyNameList());
		m_pPropertiesModel->AppendProperty("Spacer", "Size", PROPERTIESTYPE_int, 0, "Sets the spacer's size hint in pixels", PROPERTIESACCESS_ToggleUnchecked);
		break;

	case ITEM_UiLabel:
	case ITEM_UiRichLabel:
	case ITEM_UiButton:
	case ITEM_UiRackMeter:
	case ITEM_UiBarMeter:
	case ITEM_UiCheckBox:
	case ITEM_UiRadioButton:
	case ITEM_UiTextField:
	case ITEM_UiComboBox: // TODO: Implement custom properties to populate combobox
	case ITEM_UiSlider:
		m_pPropertiesModel->InsertCategory(-1, "Widget", QVariant(), false, "Widgets are a 'user interface' type of entity");
		m_pPropertiesModel->AppendProperty("Widget", "Enabled", PROPERTIESTYPE_bool, Qt::Checked, "Use to disable or reenable a widget", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "KB Focus Allowed", PROPERTIESTYPE_bool, Qt::Checked, "Allow this widget to be the target of keyboard input", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "Highlighted", PROPERTIESTYPE_bool, Qt::Unchecked, "Whether to specify to the widget that it is highlighted", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "Hide Disabled", PROPERTIESTYPE_bool, Qt::Checked, "Whether to not show and visually indicate if disabled", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "Hide Hover State", PROPERTIESTYPE_bool, Qt::Checked, "Whether to not show and visually indicate a mouse 'hover state' when available", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "Hide Down State", PROPERTIESTYPE_bool, Qt::Checked, "Whether to not show and visually indicate a mouse 'down state' when available", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "Hide Highlighted", PROPERTIESTYPE_bool, Qt::Checked, "Whether to not show and visually indicate when the widget is considered 'highlighted'", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "Horizontal Policy", PROPERTIESTYPE_ComboBoxString, HyGlobal::SizePolicyName(HYSIZEPOLICY_Fixed), "Hints to the widget on how it should size its horizontal dimension", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), HyGlobal::GetSizePolicyNameList());
		m_pPropertiesModel->AppendProperty("Widget", "Vertical Policy", PROPERTIESTYPE_ComboBoxString, HyGlobal::SizePolicyName(HYSIZEPOLICY_Fixed), "Hints to the widget on how it should size its vertical dimension", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), HyGlobal::GetSizePolicyNameList());
		m_pPropertiesModel->AppendProperty("Widget", "Lock Proportions", PROPERTIESTYPE_bool, Qt::Unchecked, "Keeps the widget's aspect ratio when resizing", PROPERTIESACCESS_ToggleUnchecked);
		m_pPropertiesModel->AppendProperty("Widget", "Min Size", PROPERTIESTYPE_ivec2, QPoint(0, 0), "The widget's minimum size it'll use when resizing", PROPERTIESACCESS_ToggleUnchecked, 0, MAX_INT_RANGE, 1, "[", "]");
		m_pPropertiesModel->AppendProperty("Widget", "Max Size", PROPERTIESTYPE_ivec2, QPoint(MAX_INT_RANGE, MAX_INT_RANGE), "The widget's maximum size it'll use when resizing", PROPERTIESACCESS_ToggleUnchecked, 0, MAX_INT_RANGE, 1, "[", "]");

		if(GetType() != ITEM_UiSlider) // AKA all widgets derived from Label
		{
			m_pPropertiesModel->InsertCategory(0, "Label", QVariant(), false, "The main text used in this widget");
			m_pPropertiesModel->AppendProperty("Label", "Text Item", PROPERTIESTYPE_ComboBoxItems, QVariant(), "The specified project Text item used on this widget", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), ITEM_Text);
			if(GetType() == ITEM_UiRackMeter)
			{
				m_pPropertiesModel->AppendProperty("Label", "Set Value", PROPERTIESTYPE_int64, 0, "The numerical value represented in the rack meter", PROPERTIESACCESS_ToggleUnchecked);
				// TODO: m_pPropertiesModel->AppendProperty("Label", "Rack Value", PROPERTIESTYPE_int64, 0, "The numerical value represented in the rack meter", PROPERTIESACCESS_ToggleUnchecked);
			}
			else
			{
				m_pPropertiesModel->AppendProperty("Label", "Text", PROPERTIESTYPE_LineEdit, "Text123", "What UTF-8 string to be displayed on the label", PROPERTIESACCESS_ToggleUnchecked);

				if(GetType() == ITEM_UiTextField)
					m_pPropertiesModel->AppendProperty("Label", "Input Validator", PROPERTIESTYPE_LineEdit, "", "A case-sensitive regex that checks and permits only valid keyboard input", PROPERTIESACCESS_ToggleUnchecked);
			}
			m_pPropertiesModel->AppendProperty("Label", "Margins", PROPERTIESTYPE_vec4, QRectF(0.0f, 0.0f, 0.0f, 0.0f), "The text's margins within the main panel of this widget", PROPERTIESACCESS_ToggleUnchecked);
			//m_pPropertiesModel->AppendProperty("Label", "Style", PROPERTIESTYPE_ComboBoxString, HyGlobal::GetTextTypeNameList()[HYTEXT_Line], "The style of how the text is shown", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTextTypeNameList());
			// TODO: m_pPropertiesModel->AppendProperty("Label", "Text State", PROPERTIESTYPE_StatesComboBox, 0, "The text's state to be displayed", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), QString(), QString(), GetReferencedItemUuid());
			// TODO: virtual void SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor);
			// TODO: void SetAsSideBySide(bool bPanelBeforeText = true, int32 iPadding = 5, HyOrientation eOrientation = HYORIENT_Horizontal);	// Show the panel and text side by side specified accordingly to the arguments passed
			m_pPropertiesModel->AppendProperty("Label", "Text Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this widget's main text gets rendered", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Label", "Alignment", PROPERTIESTYPE_ComboBoxString, HyGlobal::AlignmentName(HYALIGN_Left), "The alignment of the text", PROPERTIESACCESS_ToggleUnchecked, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetAlignmentNameList());
			m_pPropertiesModel->AppendProperty("Label", "Monospaced Digits", PROPERTIESTYPE_bool, false, "Check to use monospaced digits, which ensures all digits use the same width", PROPERTIESACCESS_ToggleUnchecked);
			if(GetType() == ITEM_UiRackMeter)
			{
				m_pPropertiesModel->AppendProperty("Label", "Show As Cash", PROPERTIESTYPE_bool, false, "Check to format the text as currency/money", PROPERTIESACCESS_ToggleUnchecked);
				m_pPropertiesModel->AppendProperty("Label", "Spinning Digits", PROPERTIESTYPE_bool, false, "Check to have digits spin when racking to new values", PROPERTIESACCESS_ToggleUnchecked);
				// TODO: SetNumFormat(HyNumberFormat format);
				// TODO: SetDenomination(uint32 uiDenom);
			}
			else if(GetType() == ITEM_UiButton || GetType() == ITEM_UiCheckBox || GetType() == ITEM_UiRadioButton)
			{
				m_pPropertiesModel->InsertCategory(0, "Button", QVariant(), false, "A button is a label that can be depressed");
				m_pPropertiesModel->AppendProperty("Button", "Checked", PROPERTIESTYPE_bool, Qt::Unchecked, "Sets this button as 'checked'", PROPERTIESACCESS_ToggleUnchecked);
			}
			else if(GetType() == ITEM_UiBarMeter)
			{
				m_pPropertiesModel->InsertCategory(0, "Bar Meter", QVariant(), false, "Bar meter useful for things like a health bar or progress bar");
				m_pPropertiesModel->AppendProperty("Bar Meter", "Min Value", PROPERTIESTYPE_int, 0, "The minimum, clamped value that indicates the bar is empty", PROPERTIESACCESS_ToggleUnchecked, -iRANGE, iRANGE, 1);
				m_pPropertiesModel->AppendProperty("Bar Meter", "Max Value", PROPERTIESTYPE_int, 0, "The maximum, clamped value that indicates the bar is full", PROPERTIESACCESS_ToggleUnchecked, -iRANGE, iRANGE, 1);
				m_pPropertiesModel->AppendProperty("Bar Meter", "Value", PROPERTIESTYPE_int, 0, "The current bar meter's value, clamped to the Min and Max values", PROPERTIESACCESS_ToggleUnchecked, -iRANGE, iRANGE, 1);
				// TODO: SetNumFormat(HyNumberFormat format);
			}

			m_pPropertiesModel->InsertCategory(1, "Panel", QVariant(), false, "The main visual background portion of this widget");
			m_pPropertiesModel->AppendProperty("Panel", "Setup", PROPERTIESTYPE_UiPanel, QVariant(), "Initializes and setup the main panel of this widget", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Panel", "Visible", PROPERTIESTYPE_bool, Qt::Checked, "Enabled dictates whether this gets drawn and updated", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Panel", "Alpha", PROPERTIESTYPE_double, 1.0, "A value from 0.0 to 1.0 that indicates how opaque/transparent this item is", PROPERTIESACCESS_ToggleUnchecked, 0.0, 1.0, 0.05);

			if(GetType() == ITEM_UiBarMeter) // Extra panel properties for Bar Meter
			{
				m_pPropertiesModel->AppendProperty("Panel", "Bar Setup", PROPERTIESTYPE_UiPanel, QVariant(), "Initializes and setup the inner bar of this Bar Meter", PROPERTIESACCESS_ToggleUnchecked);
				m_pPropertiesModel->AppendProperty("Panel", "Bar Offset", PROPERTIESTYPE_ivec2, QPoint(0, 0), "The inner bar's positional offset from the main panel", PROPERTIESACCESS_ToggleUnchecked);
				// TODO: bool SetBarState(uint32 uiStateIndex);
				m_pPropertiesModel->AppendProperty("Panel", "Bar Vertical", PROPERTIESTYPE_bool, Qt::Unchecked, "When set the bar will grow vertically instead of rightward, horizontally", PROPERTIESACCESS_ToggleUnchecked);
				m_pPropertiesModel->AppendProperty("Panel", "Bar Inverted", PROPERTIESTYPE_bool, Qt::Unchecked, "When set the bar will grow from right to left or bottom to top", PROPERTIESACCESS_ToggleUnchecked);
				m_pPropertiesModel->AppendProperty("Panel", "Bar Stretched", PROPERTIESTYPE_bool, Qt::Unchecked, "When set and the 'Bar Setup' is a Node item, the bar node will be scaled to fit the range of the progress bar. Otherwise, the bar is stenciled/cropped to fit the range (default)", PROPERTIESACCESS_ToggleUnchecked);
				m_pPropertiesModel->AppendProperty("Panel", "Bar Under Panel", PROPERTIESTYPE_bool, Qt::Unchecked, "When set the bar will be drawn under the main panel instead of over it. Only useful if main panel is a node item with transparent center", PROPERTIESACCESS_ToggleUnchecked);
			}
		}
		else // Is ITEM_UiSlider
		{
			m_pPropertiesModel->InsertCategory(0, "Slider", QVariant(), false, "Bar meter useful for things like a health bar or progress bar");
			// TODO: m_pPropertiesModel->AppendProperty("Slider", "Set Range", PROPERTIESTYPE_UiSliderRange, 0, "Set the value range, or specify each value step along the slider", PROPERTIESACCESS_ToggleUnchecked);
			m_pPropertiesModel->AppendProperty("Slider", "Value", PROPERTIESTYPE_int, 0, "The current slider value, clamped or corrected to a value appropriate from 'Set Range'", PROPERTIESACCESS_ToggleUnchecked, -iRANGE, iRANGE, 1);
			m_pPropertiesModel->AppendProperty("Slider", "Vertical", PROPERTIESTYPE_bool, Qt::Unchecked, "When set the slider will be vertical instead of horizontal", PROPERTIESACCESS_ToggleUnchecked);
			// TODO: SetBarColors(HyColor posColor, HyColor negColor, HyColor strokeColor);
		}

		break;

	default:
		HyGuiLog(QString("EntityTreeItem::InitalizePropertiesTree - unsupported type: ") % QString::number(GetType()), LOGTYPE_Error);
		break;
	}

	// TWEENS - Make sure these Category names match HyGlobal's sm_TweenPropNames
	if(IsFixtureItem() == false && IsLayoutItem() == false)
	{
		m_pPropertiesModel->InsertCategory(-1, "Tween Position", QVariant(), true, "Start a positional tween from the currently selected frame");
		m_pPropertiesModel->AppendProperty("Tween Position", "Destination", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "The target destination for the tween to reach", PROPERTIESACCESS_Mutable, -fRANGE, fRANGE, 1.0, "[", "]");
		m_pPropertiesModel->AppendProperty("Tween Position", "Duration", PROPERTIESTYPE_double, 0.0, "How long it will take to reach the target destination for the tween", PROPERTIESACCESS_Mutable, 0.0, QVariant(), 0.01, QString(), "sec");
		m_pPropertiesModel->AppendProperty("Tween Position", "Tween Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::TweenFuncName(TWEENFUNC_Linear), "The type of tween to use", PROPERTIESACCESS_Mutable, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTweenFuncNameList());

		m_pPropertiesModel->InsertCategory(-1, "Tween Rotation", QVariant(), true, "Start a rotational tween from the currently selected frame");
		m_pPropertiesModel->AppendProperty("Tween Rotation", "Destination", PROPERTIESTYPE_double, 0.0, "The target rotation (in degrees) for the tween to reach", PROPERTIESACCESS_Mutable, -360, 360, 1.0);
		m_pPropertiesModel->AppendProperty("Tween Rotation", "Duration", PROPERTIESTYPE_double, 0.0, "How long it will take to reach the target rotation for the tween", PROPERTIESACCESS_Mutable, 0.0, QVariant(), 0.01, QString(), "sec");
		m_pPropertiesModel->AppendProperty("Tween Rotation", "Tween Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::TweenFuncName(TWEENFUNC_Linear), "The type of tween to use", PROPERTIESACCESS_Mutable, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTweenFuncNameList());

		m_pPropertiesModel->InsertCategory(-1, "Tween Scale", QVariant(), true, "Start a scaling tween from the currently selected frame");
		m_pPropertiesModel->AppendProperty("Tween Scale", "Destination", PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), "The target scale for the tween to reach", PROPERTIESACCESS_Mutable, -fRANGE, fRANGE, 0.01, "[", "]");
		m_pPropertiesModel->AppendProperty("Tween Scale", "Duration", PROPERTIESTYPE_double, 0.0, "How long it will take to reach the target scale for the tween", PROPERTIESACCESS_Mutable, 0.0, QVariant(), 0.01, QString(), "sec");
		m_pPropertiesModel->AppendProperty("Tween Scale", "Tween Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::TweenFuncName(TWEENFUNC_Linear), "The type of tween to use", PROPERTIESACCESS_Mutable, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTweenFuncNameList());

		if(bIsBody)
		{
			m_pPropertiesModel->InsertCategory(-1, "Tween Alpha", QVariant(), true, "Start an alpha/transparency tween from the currently selected frame");
			m_pPropertiesModel->AppendProperty("Tween Alpha", "Destination", PROPERTIESTYPE_double, 1.0, "The target alpha for the tween to reach", PROPERTIESACCESS_Mutable, 0.0, 1.0, 0.01);
			m_pPropertiesModel->AppendProperty("Tween Alpha", "Duration", PROPERTIESTYPE_double, 0.0, "How long it will take to reach the target rotation for the tween", PROPERTIESACCESS_Mutable, 0.0, QVariant(), 0.01, QString(), "sec");
			m_pPropertiesModel->AppendProperty("Tween Alpha", "Tween Type", PROPERTIESTYPE_ComboBoxString, HyGlobal::TweenFuncName(TWEENFUNC_Linear), "The type of tween to use", PROPERTIESACCESS_Mutable, QVariant(), QVariant(), QVariant(), "", "", HyGlobal::GetTweenFuncNameList());
		}
	}
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

	// Create root tree item data for each base class type.
	// Then use "fusedItemList" to init each base class's fused tree item data, or create a new fused item if it doesn't exist below
	QJsonArray fusedItemArray = fileMetaObj["fusedItemList"].toArray();
	for(int i = 0; i < NUM_ENTBASECLASSTYPES; ++i)
	{
		m_BaseClassInfoList[i].m_pRootTreeItemData = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sEntityCodeName, ITEM_Entity, ENTTYPE_Root, QUuid(fileMetaObj["UUID"].toString()), QUuid(fileMetaObj["UUID"].toString()));
		m_BaseClassInfoList[i].m_pRootTreeItemData->InitializeRootBaseClass(static_cast<EntityBaseClassType>(i));
		
		switch(i)
		{
		case ENTBASECLASS_HyEntity2d:
			m_BaseClassInfoList[i].m_pFusedTreeItemData = nullptr;
			break;
	
		case ENTBASECLASS_HyActor2d:
			if(fusedItemArray.empty() || i <= fusedItemArray.size())
				m_BaseClassInfoList[i].m_pFusedTreeItemData = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sEntityCodeName, ITEM_ShapeFixture, ENTTYPE_FusedItem, QUuid(), QUuid::createUuid());
			else
				m_BaseClassInfoList[i].m_pFusedTreeItemData = new EntityTreeItemData(m_ModelRef, fusedItemArray[i].toObject(), false, true);
			break;

		case ENTBASECLASS_HyUiContainer:
			if(fusedItemArray.empty() || i <= fusedItemArray.size())
				m_BaseClassInfoList[i].m_pFusedTreeItemData = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sEntityCodeName, ITEM_UiLayout, ENTTYPE_FusedItem, QUuid(), QUuid::createUuid());
			else
				m_BaseClassInfoList[i].m_pFusedTreeItemData = new EntityTreeItemData(m_ModelRef, fusedItemArray[i].toObject(), false, true);
			break;
		}
	}
	Cmd_ApplyRootBaseClass();

	// Insert 'folder' to hold bounding volumes (shapes)
	if(insertRow(1, QModelIndex()) == false)
	{
		HyGuiLog("EntityTreeModel::EntityTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pFixtureFolderItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, "Fixtures", ITEM_None, ENTTYPE_FixtureFolder, QUuid(), QUuid());
	QVariant fixtureData;
	fixtureData.setValue(pFixtureFolderItem);
	if(setData(index(1, 0, QModelIndex()), fixtureData, Qt::UserRole) == false)
		HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);


	// Insert all the 'children' and 'shape' items
	std::function<void(const QJsonArray &)> fpPopulateNodeTreeItems = [&](const QJsonArray &itemListArray)
	{
		for(int i = 0; i < itemListArray.size(); ++i)
		{
			if(itemListArray[i].isObject())
				m_ModelRef.Cmd_AddExistingItem(itemListArray[i].toObject(), false, false, i);
			else if(itemListArray[i].isArray())
			{
				QJsonArray subItemArray = itemListArray[i].toArray();
				for(int j = 0; j < subItemArray.size(); ++j)
					m_ModelRef.Cmd_AddExistingItem(subItemArray[j].toObject(), true, false, j == 0 ? i : j);
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
	{
		HyGuiLog("EntityTreeModel::GetRootTreeItemData - root data not created", LOGTYPE_Error);
		return nullptr;
	}

	if(m_BaseClassInfoList[m_ModelRef.GetBaseClassType()].m_pRootTreeItemData != m_pRootItem->GetChild(0)->data(0).value<EntityTreeItemData *>())
		HyGuiLog("EntityTreeModel::GetRootTreeItemData root item data does not match expected base class root item data", LOGTYPE_Error);

	return m_BaseClassInfoList[m_ModelRef.GetBaseClassType()].m_pRootTreeItemData;
	//return m_pRootItem->GetChild(0)->data(0).value<EntityTreeItemData *>();
}

TreeModelItem *EntityTreeModel::GetFixtureFolderTreeItem() const
{
	if(m_pRootItem->GetNumChildren() < 2)
		return nullptr;

	return m_pRootItem->GetChild(1);
}

EntityTreeItemData *EntityTreeModel::GetFixtureFolderTreeItemData() const
{
	if(m_pRootItem->GetNumChildren() < 2)
		return nullptr;

	return m_pRootItem->GetChild(1)->data(0).value<EntityTreeItemData *>();
}

QList<EntityTreeItemData *> EntityTreeModel::GetFusedItemData() const
{
	QList<EntityTreeItemData *> fusedItemList;
	for(int i = 0; i < NUM_ENTBASECLASSTYPES; ++i)
		fusedItemList.push_back(m_BaseClassInfoList[i].m_pFusedTreeItemData);
	
	return fusedItemList;
}

TreeModelItem *EntityTreeModel::GetArrayFolderTreeItem(EntityTreeItemData *pArrayItem) const
{
	if(pArrayItem->GetEntType() != ENTTYPE_ArrayItem)
	{
		HyGuiLog("EntityTreeModel::GetArrayFolderTreeItem was passed a non-array item", LOGTYPE_Error);
		return nullptr;
	}

	TreeModelItem *pParentFolderItem = pArrayItem->IsFixtureItem() ? GetFixtureFolderTreeItem() : GetRootTreeItem();
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

	TreeModelItem *pParentFolderItem = pArrayItem->IsFixtureItem() ? GetFixtureFolderTreeItem() : GetRootTreeItem();
	for(int i = 0; i < pParentFolderItem->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pSubItem = pParentFolderItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pArrayItem->GetCodeName() == pSubItem->GetCodeName() && pSubItem->GetEntType() == ENTTYPE_ArrayFolder)
			return pSubItem;
	}

	HyGuiLog("EntityTreeModel::GetArrayFolderTreeItemData array folder was not found", LOGTYPE_Error);
	return nullptr;
}

void EntityTreeModel::GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &fixtureListOut) const
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

	TreeModelItem *pThisFixturesFolder = GetFixtureFolderTreeItem();
	for(int i = 0; i < pThisFixturesFolder->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurFixture = pThisFixturesFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurFixture == nullptr)
			continue;

		if(pCurFixture->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisFixturesFolder->GetChild(i);
			for(int j = 0; j < pArrayFolder->GetNumChildren(); ++j)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(j)->data(0).value<EntityTreeItemData *>();
				fixtureListOut.push_back(pArrayItem);
			}
		}
		else
			fixtureListOut.push_back(pCurFixture);
	}
}

void EntityTreeModel::GetSelectedTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &fixtureListOut) const
{
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> fixtureList;
	GetTreeItemData(childList, fixtureList);
	for(EntityTreeItemData *pChildItem : childList)
	{
		if(pChildItem->IsSelected())
			childListOut.push_back(pChildItem);
	}
	for(EntityTreeItemData *pFixtureItem : fixtureList)
	{
		if(pFixtureItem->IsSelected())
			fixtureListOut.push_back(pFixtureItem);
	}
}

EntityTreeItemData *EntityTreeModel::FindTreeItemData(QUuid uuid) const
{
	EntityTreeItemData *pRootItemData = GetRootTreeItemData();
	if(pRootItemData && pRootItemData->GetUuid() == uuid)
		return pRootItemData;

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

	TreeModelItem *pThisFixtureFolder = GetFixtureFolderTreeItem();
	for(int i = 0; i < pThisFixtureFolder->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurShape = pThisFixtureFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurShape == nullptr)
			continue;

		if(pCurShape->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisFixtureFolder->GetChild(i);
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

	if(HyGlobal::IsItemType_Project(pItem->GetType()) == false && HyGlobal::IsItemType_Asset(pItem->GetType()) == false)
	{
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is not a valid child type: " % QString::number(pItem->GetType()), LOGTYPE_Warning);
		return false;
	}

	// Make sure the Project of 'pItem' is the same as this project
	if(m_ModelRef.GetItem().GetProject().FindItemData(pItem->GetUuid()) == nullptr)
	{
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is not a apart of this project", LOGTYPE_Warning);
		return false;
	}

	// Make sure pItem isn't or does not contain itself
	if(pItem->GetType() == ITEM_Entity)
	{
		ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pItem);

		FileDataPair entFileDataPair;
		pProjItem->GetLatestFileData(entFileDataPair);

		const QUuid &thisUuid = m_ModelRef.GetItem().GetUuid();
		if(thisUuid == QUuid(entFileDataPair.m_Meta["UUID"].toString()))
		{
			if(bShowDialogsOnFail)
				HyGuiLog(pItem->GetText() % " cannot insert a child node of itself", LOGTYPE_Warning);
			return false;
		}

		// Exhaustively look through all children (and children's children) to make sure this entity isn't a child of itself
		QJsonArray descChildList = entFileDataPair.m_Meta["descChildList"].toArray();
		QStack<QJsonObject> childObjStack;
		for(int i = 0; i < descChildList.size(); ++i)
		{
			if(descChildList[i].isObject())
				childObjStack.push_back(descChildList[i].toObject());
			else // QJsonArray
			{
				QJsonArray childArray = descChildList[i].toArray();
				for(int j = 0; j < childArray.size(); ++j)
					childObjStack.push_back(childArray[j].toObject());
			}

			while(childObjStack.empty() == false)
			{
				QJsonObject childObj = childObjStack.pop();
				QUuid childUuid(childObj["itemUUID"].toString());
				if(thisUuid == childUuid)
				{
					if(bShowDialogsOnFail)
						HyGuiLog(pItem->GetText() % " cannot insert an entity that cointains a child node of this", LOGTYPE_Warning);
					return false;
				}

				if(childObj["itemType"].toString() == HyGlobal::ItemName(ITEM_Entity, false))
				{
					ProjectItemData *pEntTreeItem = static_cast<ProjectItemData *>(m_ModelRef.GetItem().GetProject().FindItemData(childUuid));
					pEntTreeItem->GetLatestFileData(entFileDataPair);
					QJsonArray nestedChildList = entFileDataPair.m_Meta["descChildList"].toArray();
					for(int j = 0; j < nestedChildList.size(); ++j)
					{
						childObjStack.push_back(nestedChildList[j].toObject());
					}
				}
			}
		}
	}

	return true;
}

void EntityTreeModel::Cmd_ApplyRootBaseClass()
{
	QVariant v;
	v.setValue(m_BaseClassInfoList[m_ModelRef.GetBaseClassType()].m_pRootTreeItemData);
	if(setData(index(0, 0, QModelIndex()), v, Qt::UserRole) == false)
		HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);

	// Remove/replace any fused items
	for(int i = 0; i < NUM_ENTBASECLASSTYPES; ++i)
		RemoveTreeItem(m_BaseClassInfoList[i].m_pFusedTreeItemData);
	if(m_BaseClassInfoList[m_ModelRef.GetBaseClassType()].m_pFusedTreeItemData)
		InsertTreeItem(m_ModelRef.GetItem().GetProject(), m_BaseClassInfoList[m_ModelRef.GetBaseClassType()].m_pFusedTreeItemData, GetRootTreeItem(), 0);
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocChildTreeItem(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pProjItem->GetName(false));

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, pProjItem->GetType(), ENTTYPE_Item, pProjItem->GetUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	//// If pProjItem is a sub-entity, also create tree items for all of its children
	//if(pProjItem->GetType() == ITEM_Entity)
	//{
	//	TreeModelItem *pCurParent = GetItem(FindIndex<EntityTreeItemData *>(pNewItem, 0));

	//	FileDataPair entFileDataPair;
	//	pProjItem->GetLatestFileData(entFileDataPair);
	//	QJsonArray descChildList = entFileDataPair.m_Meta["descChildList"].toArray();
	//	for(int i = 0; i < descChildList.size(); ++i)
	//	{
	//		if(descChildList[i].isObject())
	//		{
	//			QJsonObject childObj = descChildList[i].toObject();
	//		}
	//		else
	//		{
	//			QJsonArray childArray = descChildList[i].toArray();
	//			for(int j = 0; j < childArray.size(); ++j)
	//			{
	//				QJsonObject childObj = childArray[j].toObject();
	//			}
	//		}

	//		EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef,
	//															  HyGlobal::GetEntityDeclType(childObj["declarationType"].toString()),
	//															  childObj["codeName"].toString(),
	//															  HyGlobal::GetTypeFromString(childObj["itemType"].toString()), ENTTYPE_Item, pProjItem->GetUuid(), QUuid::createUuid());
	//		InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pCurParent, iRow);
	//	}

	//	QStack<ProjectItemData *> projItemStack;
	//	for(ProjectItemData *pProjItem : projItemList)
	//		projItemStack.push_back(pProjItem);

	//	while(projItemStack.empty() == false)
	//	{
	//		ProjectItemData *pProjItem = projItemStack.pop();

	//		if(pProjItem->GetType() == ITEM_Entity)
	//		{
	//			// Exhaustively add all sub-entity children (and children's children)
	//			pProjItem->GetLatestFileData(entFileDataPair);

	//			QJsonArray descChildList = entFileDataPair.m_Meta["descChildList"].toArray();
	//			for(int i = 0; i < descChildList.size(); ++i)
	//			{
	//				QUuid nestedChildUuid(descChildList[i].toObject()["itemUUID"].toString());
	//				projItemStack.push_back(static_cast<ProjectItemData *>(m_ItemRef.GetProject().FindItemData(nestedChildUuid)));
	//			}
	//		}
	//	}
	//}





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
	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, eItemType, ENTTYPE_Item, assetUuid, QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocExistingTreeItem(QJsonObject descObj, bool bIsArrayItem, bool bIsFusedItem, int iRow)
{
	ItemType eGuiType = HyGlobal::GetTypeFromString(descObj["itemType"].toString());
	QString sCodeName = descObj["codeName"].toString();
	if(bIsArrayItem == false)
		sCodeName = GenerateCodeName(sCodeName);

	TreeModelItem *pParentTreeItem = nullptr;
	if(HyGlobal::IsItemType_Fixture(eGuiType) == false)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetFixtureFolderTreeItem();

	bool bFoundArrayFolder = false;
	if(bIsArrayItem)
		bFoundArrayFolder = FindOrCreateArrayFolder(pParentTreeItem, sCodeName, eGuiType, iRow);

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, descObj, bIsArrayItem, bIsFusedItem);
	iRow = (iRow < 0 || (bIsArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocWidgetTreeItem(ItemType eWidgetType, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new widget
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + HyGlobal::ItemName(eWidgetType, false));

	TreeModelItem *pParentTreeItem = GetRootTreeItem();
	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, eWidgetType, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocPrimitiveTreeItem(QString sCodeNamePrefix, int iRow /*= -1*/)
{
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + "Primitive");

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, ITEM_Primitive, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocFixtureTreeItem(bool bIsShape, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName;
	if(bIsShape)
		sCodeName = GenerateCodeName(sCodeNamePrefix + "Shape");
	else
		sCodeName = GenerateCodeName(sCodeNamePrefix + "Chain");

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, bIsShape ? ITEM_ShapeFixture : ITEM_ChainFixture, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetFixtureFolderTreeItem(), iRow);

	return pNewItem;
}

bool EntityTreeModel::Cmd_ReaddChild(EntityTreeItemData *pItem, int iRow)
{
	// Generate a unique code name for this new item
	//QString sCodeName = GenerateCodeName(pItem->GetCodeName());

	TreeModelItem *pParentTreeItem = nullptr;
	if(pItem->IsFixtureItem() == false)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetFixtureFolderTreeItem();

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
				return QVariant(pItem->GetCodeName() % "[" % QString::number(pTreeItem->GetNumChildren()) % "]");
			else if(pItem->GetEntType() == ENTTYPE_ArrayItem)
				return QVariant("[" % QString::number(pTreeItem->GetIndex()) % "] - " % pItem->GetThisUuid().toString(QUuid::StringFormat::WithoutBraces).split('-')[0]);
			else
				return QVariant(pItem->GetCodeName());
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
		if(indexRef.column() != COLUMN_CodeName)
			return QVariant();

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

		if(pItem->GetEntType() == ENTTYPE_FixtureFolder)
			return QIcon(":/icons16x16/fixture-folder.png");
		else if(pItem->GetEntType() == ENTTYPE_ArrayFolder)
			return HyGlobal::ItemIcon(pItem->GetType(), SUBICON_Open);

		if(pItem->GetType() == ITEM_Primitive || pItem->GetType() == ITEM_ShapeFixture)
		{
			int iStateIndex = 0;
			if(m_ModelRef.GetItem().GetWidget())
				iStateIndex = m_ModelRef.GetItem().GetWidget()->GetCurStateIndex();
			const EntityDopeSheetScene &dopeSheetSceneRef = static_cast<EntityStateData *>(m_ModelRef.GetStateData(iStateIndex))->GetDopeSheetScene();

			QIcon icon;
			QString sIconUrl = ":/icons16x16/shapes/" % QString(pItem->GetType() == ITEM_Primitive ? "primitive_" : "shapes_");

			QString sShapeType = dopeSheetSceneRef.BasicExtrapolateKeyFrameProperty(pItem, dopeSheetSceneRef.GetCurrentFrame(), pItem->GetType() == ITEM_Primitive ? "Primitive" : "Shape", "Type").toString();
			switch(HyGlobal::GetShapeFromString(sShapeType))
			{
			case SHAPE_None:		sIconUrl += "icon.png"; break;
			case SHAPE_Box:			sIconUrl += "box.png"; break;
			case SHAPE_Circle:		sIconUrl += "circle.png"; break;
			case SHAPE_LineSegment:	sIconUrl += "lineSeg.png"; break;
			case SHAPE_Polygon:		sIconUrl += "polygon.png"; break;
			case SHAPE_Capsule:		sIconUrl += "capsule.png"; break;
			default: // Special case for primitives
				sIconUrl += "lineChain.png";
				break;
			}

			icon.addFile(sIconUrl);
			return QVariant(icon);
		}
			
		return QVariant(pItem->GetIcon(SUBICON_None));

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant();// QVariant(pItem->GetThisUuid().toString());

	default:
		return QVariant();
	}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags EntityTreeModel::flags(const QModelIndex &indexRef) const /*override*/
{
	EntityTreeItemData *pItem = GetItem(indexRef)->data(0).value<EntityTreeItemData *>();

	Qt::ItemFlags returnFlags = Qt::NoItemFlags;
	if(pItem->IsSelectable())
		returnFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	
	if(pItem->GetEntType() == ENTTYPE_FixtureFolder)
		returnFlags |= Qt::ItemIsEnabled;
	
	return returnFlags;
}

/*virtual*/ void EntityTreeModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*override*/
{
}

QString EntityTreeModel::GenerateCodeName(QString sDesiredName) const
{
	// Use 'HyGlobal::CodeNameValidator()' (QRegExpValidator) to make sure 'sDesiredName' is valid
	int pos = 0;
	if(HyGlobal::CodeNameValidator()->validate(sDesiredName, pos) != QValidator::Acceptable)
		sDesiredName.remove(QRegularExpression("[^a-zA-Z0-9_]"));

	if(sDesiredName.isEmpty())
		sDesiredName = "unnamed";

	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> fixtureList;
	GetTreeItemData(childList, fixtureList);
	childList += fixtureList; // Just combine the two since they all need to be unique

	uint uiConflictCount = 0;
	bool bIsUnique = false;
	do
	{
		QString sFullCodeName = sDesiredName;
		if(uiConflictCount > 0)
			sFullCodeName += QString::number(uiConflictCount + 1);

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

		//if(pParentTreeItemOut->GetNumChildren() > 0)
		//	iArrayFolderRow = HyMath::Min(iArrayFolderRow, pParentTreeItemOut->GetNumChildren() - 1);

		if(insertRow(iArrayFolderRow, parentIndex) == false)
		{
			HyGuiLog("EntityTreeModel::FindOrCreateArrayFolder - ArrayFolder insertRow failed", LOGTYPE_Error);
			return false;
		}
		// Allocate and store the new array folder item in the tree model
		EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, eItemType, ENTTYPE_ArrayFolder, QUuid(), QUuid());
		QVariant v;
		v.setValue(pNewItem);
		for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
		{
			if(setData(index(iArrayFolderRow, iCol, parentIndex), v, Qt::UserRole) == false)
				HyGuiLog("ExplorerModel::FindOrCreateArrayFolder() - setData failed", LOGTYPE_Error);
		}

		pParentTreeItemOut = pParentTreeItemOut->GetChild(iArrayFolderRow);
	}

	return bFoundArrayFolder;
}
