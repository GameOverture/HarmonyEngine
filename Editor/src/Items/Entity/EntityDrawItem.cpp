/**************************************************************************
*	EntityDrawItem.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDrawItem.h"
#include "EntityModel.h"
#include "EntityDraw.h"
#include "EntityDopeSheetScene.h"
#include "MainWindow.h"

EntityDrawItem::EntityDrawItem(Project &projectRef, EntityTreeItemData *pEntityTreeItemData, EntityDraw *pEntityDraw, HyEntity2d *pParent) :
	IDrawExItem(pEntityDraw),
	m_pEntityTreeItemData(pEntityTreeItemData),
	m_pChild(nullptr),
	m_ShapeCtrl(pEntityDraw)
{
	QUuid referencedItemUuid = m_pEntityTreeItemData->GetReferencedItemUuid();
	TreeModelItemData *pReferencedItemData = projectRef.FindItemData(referencedItemUuid);

	if(m_pEntityTreeItemData->IsAssetItem())
	{
		if(m_pEntityTreeItemData->GetType() == ITEM_AtlasFrame)
			m_pChild = new HyTexturedQuad2d(static_cast<IAssetItemData *>(pReferencedItemData)->GetChecksum(), static_cast<IAssetItemData *>(pReferencedItemData)->GetBankId(), pParent);
		else
			HyGuiLog("EntityDrawItem ctor - asset item not handled: " % HyGlobal::ItemName(m_pEntityTreeItemData->GetType(), false), LOGTYPE_Error);
	}
	else if(HyGlobal::IsItemType_Project(m_pEntityTreeItemData->GetType()))
	{
		if(pReferencedItemData == nullptr || pReferencedItemData->IsProjectItem() == false)
		{
			HyGuiLog("EntityDrawItem ctor - could not find referenced item data UUID: " % referencedItemUuid.toString(), LOGTYPE_Error);
			return;
		}

		ProjectItemData *pReferencedProjItemData = static_cast<ProjectItemData *>(pReferencedItemData);

		FileDataPair fileDataPair;
		pReferencedProjItemData->GetSavedFileData(fileDataPair);

		if(m_pEntityTreeItemData->GetType() == ITEM_Entity)
		{
			m_pChild = new SubEntity(projectRef,
									 //fileDataPair.m_Meta["framesPerSecond"].toInt(),
									 QUuid(fileDataPair.m_Meta["UUID"].toString()),
									 fileDataPair.m_Meta["descChildList"].toArray(),
									 fileDataPair.m_Meta["stateArray"].toArray(),
									 pParent);
		}
		else
		{
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("EntityDrawItem ctor - failed to parse its file data", LOGTYPE_Error);

#undef GetObject
			switch(m_pEntityTreeItemData->GetType())
			{
			case ITEM_Text:
				m_pChild = new HyText2d("", HY_GUI_DATAOVERRIDE, pParent);
				static_cast<HyText2d *>(m_pChild)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject(), false); // The 'false' here has it so HyTextData loads the atlas as it would normally
				break;

			case ITEM_Spine:
				m_pChild = new HySpine2d("", HY_GUI_DATAOVERRIDE, pParent);
				static_cast<HySpine2d *>(m_pChild)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
				break;

			case ITEM_Sprite:
				m_pChild = new HySprite2d("", HY_GUI_DATAOVERRIDE, pParent);
				static_cast<HySprite2d *>(m_pChild)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
				static_cast<HySprite2d *>(m_pChild)->SetAllBoundsIncludeAlphaCrop(true);
				break;

			case ITEM_Primitive:
			case ITEM_Audio:
			case ITEM_SoundClip:
			default:
				HyGuiLog("EntityDrawItem ctor - unhandled gui item type: " % HyGlobal::ItemName(m_pEntityTreeItemData->GetType(), false), LOGTYPE_Error);
				break;
			}
		}
	}

	if(m_pChild)
		m_pChild->Load();

	HideTransformCtrl();
}

/*virtual*/ EntityDrawItem::~EntityDrawItem()
{
	delete m_pChild;
}

EntityDraw &EntityDrawItem::GetEntityDraw()
{
	return static_cast<EntityDraw &>(*m_pEntityTreeItemData->GetEntityModel().GetItem().GetDraw());
}

/*virtual*/ bool EntityDrawItem::IsSelectable() const /*override*/
{
	return m_pEntityTreeItemData->IsSelectable();
}

/*virtual*/ IHyBody2d *EntityDrawItem::GetHyNode() /*override*/
{
	if(m_pEntityTreeItemData->GetType() == ITEM_Primitive || m_pEntityTreeItemData->GetType() == ITEM_FixtureShape || m_pEntityTreeItemData->GetType() == ITEM_FixtureChain)
		return &m_ShapeCtrl.GetPrimitive(true);

	return m_pChild;
}

/*virtual*/ bool EntityDrawItem::IsSelected() /*override*/
{
	return m_pEntityTreeItemData->IsSelected();
}

/*virtual*/ void EntityDrawItem::RefreshTransform(HyCamera2d *pCamera) /*override*/
{
	IDrawExItem::RefreshTransform(pCamera);
	GetShapeCtrl().DeserializeOutline(pCamera);
}

EntityTreeItemData *EntityDrawItem::GetEntityTreeItemData() const
{
	return m_pEntityTreeItemData;
}

ShapeCtrl &EntityDrawItem::GetShapeCtrl()
{
	return m_ShapeCtrl;
}

// NOTE: The listed 4 functions below share logic that process all item properties. Any updates should reflect to all of them
//             - EntityTreeItemData::InitalizePropertyModel
//             - EntityModel::GenerateSrc_SetStateImpl
//             - EntityDrawItem::ExtractPropertyData
//             - ExtrapolateProperties
QJsonValue EntityDrawItem::ExtractPropertyData(QString sCategory, QString sPropertyName)
{
	IHyBody2d *pThisHyNode = GetHyNode();
	if(pThisHyNode == nullptr)
		return QJsonValue();

	if(sCategory == "Timeline")
	{
		if(sPropertyName == "State")
			return QJsonValue(static_cast<qint64>(pThisHyNode->GetState()));
		if(sPropertyName == "Pause")
			return QJsonValue(static_cast<SubEntity *>(pThisHyNode)->IsTimelinePaused());
		if(sPropertyName == "Frame")
			return QJsonValue(static_cast<SubEntity *>(pThisHyNode)->GetTimelineFrame());
	}
	else if(sCategory == "Common")
	{
		if(sPropertyName == "State")
			return QJsonValue(static_cast<qint64>(pThisHyNode->GetState()));
		if(sPropertyName == "Update During Paused")
			return QJsonValue(pThisHyNode->IsPauseUpdate());
		if(sPropertyName == "User Tag")
			return QJsonValue(pThisHyNode->GetTag());
	}
	else if(sCategory == "Transformation")
	{
		if(sPropertyName == "Position")
			return QJsonValue(QJsonArray({ QJsonValue(static_cast<double>(pThisHyNode->pos.GetX())), QJsonValue(static_cast<double>(pThisHyNode->pos.GetY())) }));
		if(sPropertyName == "Scale")
			return QJsonValue(QJsonArray({ QJsonValue(static_cast<double>(pThisHyNode->scale.GetX())), QJsonValue(static_cast<double>(pThisHyNode->scale.GetY())) }));
		if(sPropertyName == "Rotation")
			return QJsonValue(static_cast<double>(pThisHyNode->rot.Get()));
	}
	else if(sCategory == "Body")
	{
		if(sPropertyName == "Visible")
			return QJsonValue(pThisHyNode->IsVisible());
		if(sPropertyName == "Color Tint")
			QJsonValue(QJsonArray({ QJsonValue(static_cast<IHyBody2d *>(pThisHyNode)->topColor.GetX()), QJsonValue(static_cast<IHyBody2d *>(pThisHyNode)->topColor.GetY()), QJsonValue(static_cast<IHyBody2d *>(pThisHyNode)->topColor.GetZ()) }));
		if(sPropertyName == "Alpha")
			return QJsonValue(static_cast<double>(static_cast<IHyBody2d *>(pThisHyNode)->alpha.Get()));
		if(sPropertyName == "Override Display Order")
			return QJsonValue(static_cast<IHyBody2d *>(pThisHyNode)->GetDisplayOrder());
	}
	//else if(sCategory == "Physics")
	//{
	//}
	else if(sCategory == "Entity")
	{
		if(sPropertyName == "Mouse Input")
			return QJsonValue(static_cast<HyEntity2d *>(pThisHyNode)->IsMouseInputEnabled());
	}
	//else if(sCategory == "Primitive")
	//{
	//}
	//else if(sCategory == "Shape")
	//{
	//}
	//else if(sCategory == "Fixture")
	//{
	//}
	else if(sCategory == "Sprite")
	{
		if(sPropertyName == "Frame")
			return QJsonValue(static_cast<int>(static_cast<HySprite2d *>(pThisHyNode)->GetFrame()));
		if(sPropertyName == "Anim Pause")
			return QJsonValue(static_cast<HySprite2d *>(pThisHyNode)->IsAnimPaused());
		if(sPropertyName == "Anim Rate")
			return QJsonValue(static_cast<double>(static_cast<HySprite2d *>(pThisHyNode)->GetAnimRate()));
		if(sPropertyName == "Anim Loop")
			return QJsonValue(static_cast<HySprite2d *>(pThisHyNode)->IsAnimLoop());
		if(sPropertyName == "Anim Reverse")
			return QJsonValue(static_cast<HySprite2d *>(pThisHyNode)->IsAnimReverse());
		if(sPropertyName == "Anim Bounce")
			return QJsonValue(static_cast<HySprite2d *>(pThisHyNode)->IsAnimBounce());
	}
	else if(sCategory == "Text")
	{
		if(sPropertyName == "Text")
			return QJsonValue(QString::fromUtf8(static_cast<HyText2d *>(pThisHyNode)->GetUtf8String().c_str()));
		if(sPropertyName == "Style")
			return QJsonValue(HyGlobal::GetTextTypeNameList()[static_cast<HyText2d *>(pThisHyNode)->GetTextType()]);
		if(sPropertyName == "Style Dimensions")
			return QJsonValue(QJsonArray({ QJsonValue(static_cast<double>(static_cast<HyText2d *>(pThisHyNode)->GetTextBoxDimensions().x)), QJsonValue(static_cast<double>(static_cast<HyText2d *>(pThisHyNode)->GetTextBoxDimensions().y)) }));
		if(sPropertyName == "Alignment")
			return QJsonValue(HyGlobal::GetAlignmentNameList()[static_cast<HyText2d *>(pThisHyNode)->GetAlignment()]);
		if(sPropertyName == "Monospaced Digits")
			return QJsonValue(static_cast<HyText2d *>(pThisHyNode)->IsMonospacedDigits());
		if(sPropertyName == "Text Indent")
			return QJsonValue(static_cast<int>(static_cast<HyText2d *>(pThisHyNode)->GetTextIndent()));
	}

	return QJsonValue();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SubEntity::SubEntity(Project &projectRef, QUuid subEntityUuid, const QJsonArray &descArray, const QJsonArray &stateArray, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_bSubTimelinePaused(false),
	m_iSubTimelineStartFrame(0),
	m_iSubTimelineRemainingFrames(0),
	m_bSubTimelineDirty(false),
	m_iElapsedTimelineFrames(0),
	m_iRemainingTimelineFrames(0)
{
	QMap<QUuid, IHyLoadable2d *> uuidChildMap; // Temporary map to hold the QUuid's of the children so we can link them up with their key frame properties
	
	for(int i = 0; i < descArray.size(); ++i)
	{
		if(descArray[i].isObject())
		{
			QJsonObject childObj = descArray[i].toObject();
			CtorInitJsonObj(projectRef, uuidChildMap, childObj);
		}
		else // isArray()
		{
			QJsonArray childArray = descArray[i].toArray();
			for(int i = 0; i < childArray.size(); ++i)
			{
				QJsonObject childObj = childArray[i].toObject();
				CtorInitJsonObj(projectRef, uuidChildMap, childObj);
			}
		}
	}

	for(int i = 0; i < stateArray.size(); ++i)
	{
		m_StateInfoList.push_back(StateInfo());

		// Store the child properties
		QJsonObject stateObj = stateArray[i].toObject();
		QJsonObject keyFramesObj = stateObj["keyFrames"].toObject();
		for(auto iter = keyFramesObj.begin(); iter != keyFramesObj.end(); ++iter)
		{
			QUuid uuid(iter.key());
			QJsonArray itemKeyFrameArray = iter.value().toArray();

			if(subEntityUuid == uuid)
			{
				for(int iKeyFrameArrayIndex = 0; iKeyFrameArrayIndex < itemKeyFrameArray.size(); ++iKeyFrameArrayIndex)
				{
					QJsonObject itemKeyFrameObj = itemKeyFrameArray[iKeyFrameArrayIndex].toObject();
					int iFrame = itemKeyFrameObj["frame"].toInt();
					QJsonObject propsObj = itemKeyFrameObj["props"].toObject();

					m_StateInfoList.back().m_RootPropertiesMap.insert(iFrame, propsObj);
				}
			}
			else
			{
				m_StateInfoList.back().m_ChildPropertiesMap.insert(uuidChildMap[uuid], QMap<int, QJsonObject>());
				QMap<int, QJsonObject> &childPropMapRef = m_StateInfoList.back().m_ChildPropertiesMap[uuidChildMap[uuid]];

				for(int iKeyFrameArrayIndex = 0; iKeyFrameArrayIndex < itemKeyFrameArray.size(); ++iKeyFrameArrayIndex)
				{
					QJsonObject itemKeyFrameObj = itemKeyFrameArray[iKeyFrameArrayIndex].toObject();
					int iFrame = itemKeyFrameObj["frame"].toInt();
					QJsonObject propsObj = itemKeyFrameObj["props"].toObject();

					childPropMapRef.insert(iFrame, propsObj);
				}
			}
		}
	}
}
/*virtual*/ SubEntity::~SubEntity()
{
	for(ChildInfo &childInfo : m_ChildInfoList)
	{
		if(childInfo.m_pChild == this)
			continue;

		delete childInfo.m_pChild;
		delete childInfo.m_pPreviewComponent;
	}
}
void SubEntity::CtorInitJsonObj(Project &projectRef, QMap<QUuid, IHyLoadable2d *> &uuidChildMapRef, const QJsonObject &childObj)
{
	ItemType eItemType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());
	IHyLoadable2d *pNewChild = nullptr;
	switch(eItemType)
	{
	case ITEM_Primitive:
		pNewChild = new HyPrimitive2d(this);
		break;

	case ITEM_Audio:
		pNewChild = new HyAudio2d("", HY_GUI_DATAOVERRIDE, this);
		HyError("SubEntity ctor - Audio not implemented");
		// TODO: how do we GuiOverrideData for audio?
		//static_cast<HyAudio2d *>(pNewChild)->GuiOverrideData<HyAudioData>(itemDataDoc.GetObject());
		break;

	case ITEM_Particles:
		HyError("SubEntity ctor - Particles not implemented");
		break;

	case ITEM_Text: {
		pNewChild = new HyText2d("", HY_GUI_DATAOVERRIDE, this);

		TreeModelItemData *pReferencedItemData = projectRef.FindItemData(QUuid(childObj["itemUUID"].toString()));
		FileDataPair fileDataPair;
		static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
		QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
		HyJsonDoc itemDataDoc;
		if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
			HyGuiLog("SubEntity ctor failed to parse audio: " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

		static_cast<HyText2d *>(pNewChild)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject());
		break; }

	case ITEM_Spine: {
		pNewChild = new HySpine2d("", HY_GUI_DATAOVERRIDE, this);

		TreeModelItemData *pReferencedItemData = projectRef.FindItemData(QUuid(childObj["itemUUID"].toString()));
		FileDataPair fileDataPair;
		static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
		QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
		HyJsonDoc itemDataDoc;
		if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
			HyGuiLog("SubEntity ctor failed to parse audio: " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

		static_cast<HySpine2d *>(pNewChild)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
		break; }

	case ITEM_Sprite: {
		pNewChild = new HySprite2d("", HY_GUI_DATAOVERRIDE, this);

		TreeModelItemData *pReferencedItemData = projectRef.FindItemData(QUuid(childObj["itemUUID"].toString()));
		FileDataPair fileDataPair;
		static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
		QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
		HyJsonDoc itemDataDoc;
		if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
			HyGuiLog("SubEntity ctor failed to parse audio: " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

		static_cast<HySprite2d *>(pNewChild)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
		break; }

	case ITEM_Prefab:
		HyGuiLog("SubEntity ctor - Prefab not implemented", LOGTYPE_Error);
		break;

	case ITEM_Entity: {
		TreeModelItemData *pReferencedItemData = projectRef.FindItemData(QUuid(childObj["itemUUID"].toString()));
		FileDataPair fileDataPair;
		static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
		pNewChild = new SubEntity(projectRef,
			//fileDataPair.m_Meta["framesPerSecond"].toInt(),
			QUuid(fileDataPair.m_Meta["UUID"].toString()),
			fileDataPair.m_Meta["descChildList"].toArray(),
			fileDataPair.m_Meta["stateArray"].toArray(),
			this);
		break; }

	case ITEM_AtlasFrame: {
		TreeModelItemData *pReferencedItemData = projectRef.FindItemData(QUuid(childObj["itemUUID"].toString()));
		pNewChild = new HyTexturedQuad2d(static_cast<IAssetItemData *>(pReferencedItemData)->GetChecksum(),
			static_cast<IAssetItemData *>(pReferencedItemData)->GetBankId(),
			this);
		break; }

	default:
		HyGuiLog("SubEntity ctor - unhandled child node type: " % HyGlobal::ItemName(eItemType, false), LOGTYPE_Error);
		break;
	}

	m_ChildInfoList.append(ChildInfo(pNewChild, eItemType, new EntityPreviewComponent()));
	uuidChildMapRef.insert(QUuid(childObj["UUID"].toString()), pNewChild);

	pNewChild->Load();
}

void SubEntity::Extrapolate(const QMap<int, QJsonObject> &propMapRef, EntityPreviewComponent &previewComponentRef, bool bIsSelected, float fFrameDuration, int iMainDestinationFrame, HyCamera2d *pCamera)
{
	m_bSubTimelinePaused = false;
	m_iElapsedTimelineFrames = 0;
	m_iRemainingTimelineFrames = iMainDestinationFrame;
	m_iSubTimelineStartFrame = 0;
	m_iSubTimelineRemainingFrames = iMainDestinationFrame;
	do
	{
		m_bSubTimelineDirty = false;

		QMap<int, QJsonObject> mergedMap = propMapRef;
		MergeRootProperties(mergedMap);

		// NOTE: ExtrapolateProperties may invoke `this::TimelineEvent()` and exit early.
		//       If so, it will set `m_bSubTimelineDirty` to true in order to re-merge the properties and then continue the extrapolation
		ExtrapolateProperties(this,
							  nullptr,
							  bIsSelected,
							  ITEM_Entity,
							  fFrameDuration,
							  m_iElapsedTimelineFrames, // Starting frame to extrapolate from
							  m_iElapsedTimelineFrames + m_iRemainingTimelineFrames,
							  mergedMap,
							  previewComponentRef,
							  pCamera);

	} while(m_bSubTimelineDirty);

	if(m_iSubTimelineRemainingFrames > 0 && m_bSubTimelinePaused == false)
		ExtrapolateChildProperties(m_iSubTimelineRemainingFrames, GetState(), pCamera);
}

void SubEntity::MergeRootProperties(QMap<int, QJsonObject> &mergeMapOut)
{
	m_ConflictingPropsList.clear(); // TODO: Fix this because of re-merging timelines

	QMap<int, QJsonObject> &rootPropMapRef = m_StateInfoList[GetState()].m_RootPropertiesMap;
	for(auto iter = rootPropMapRef.begin(); iter != rootPropMapRef.end(); ++iter)
	{
		int iFrameIndex = iter.key() + m_iElapsedTimelineFrames; // NOTE: Ensure to offset the frames by 'm_iElapsedTimelineFrames' to align with the main timeline
		QJsonObject &subEntPropsObjRef = iter.value();

		if(mergeMapOut.contains(iFrameIndex) == false)
			mergeMapOut.insert(iFrameIndex, subEntPropsObjRef);
		else
		{
			QJsonObject &mergePropsObjRef = mergeMapOut[iFrameIndex];

			for(auto subEntIter = subEntPropsObjRef.begin(); subEntIter != subEntPropsObjRef.end(); ++subEntIter)
			{
				QString sSubEntCategoryName = subEntIter.key();

				if(mergePropsObjRef.contains(sSubEntCategoryName) == false) // Doesn't contain the category
					mergePropsObjRef.insert(sSubEntCategoryName, subEntIter.value());
				else // Contains the same category, check if the properties are unique to merge. If not, add to `m_ConflictingPropsList`
				{
					QJsonObject mergePropsCatObj = mergePropsObjRef[sSubEntCategoryName].toObject();
					QJsonObject subEntCatObj = subEntIter.value().toObject();

					for(QString sSubEntPropertyName : subEntCatObj.keys())
					{
						if(mergePropsCatObj.contains(sSubEntPropertyName) == false)
							mergePropsCatObj.insert(sSubEntPropertyName, subEntCatObj[sSubEntPropertyName]);
						else // Conflict detected - don't overwrite main ent's property and store in conflict list
							m_ConflictingPropsList.push_back(QPair<int, QString>(iFrameIndex, sSubEntCategoryName + '/' + sSubEntPropertyName));
					}

					mergePropsObjRef[sSubEntCategoryName] = mergePropsCatObj;
				}
			}
		}
	}
}
bool SubEntity::IsTimelinePaused() const
{
	return m_bSubTimelinePaused;
}
int SubEntity::GetTimelineFrame() const
{
	return m_iSubTimelineStartFrame;
}
bool SubEntity::TimelineEvent(int iMainTimelineFrame, QJsonObject timelineObj, HyCamera2d *pCamera)
{
	int iPrevState = GetState(); // If the state is changing, we need to extrapolate the children on the current state's timeline up to this point

	// Extrapolate the children on the current state timeline, up until this event was invoked on `iMainTimelineFrame`
	bool bStateUpdated = timelineObj.contains("State") && SetState(timelineObj["State"].toInt());
	bool bPausedUpdated = timelineObj.contains("Paused");
	bool bFrameUpdated = timelineObj.contains("Frame");
	if(bStateUpdated || bPausedUpdated || bFrameUpdated)
	{
		int iNumFramesPassed = iMainTimelineFrame - m_iElapsedTimelineFrames;

		if(m_bSubTimelinePaused == false) // Checking if we were paused prior, NOT if we just got paused
			ExtrapolateChildProperties(iNumFramesPassed, iPrevState, pCamera);

		iNumFramesPassed += 1; // This frame has been processed, so add 1 when offsetting frame counters below

		m_iElapsedTimelineFrames += iNumFramesPassed;
		m_iRemainingTimelineFrames -= iNumFramesPassed;
		m_iSubTimelineRemainingFrames -= iNumFramesPassed;
	}

	// After extrapolating children up until this point, now update the timeline properties, which will be re-merged
	// NOTE: If the state was set, it has already been done above when initializing `bStateUpdated`
	if(bPausedUpdated)
		m_bSubTimelinePaused = timelineObj["Paused"].toBool();

	if(bFrameUpdated) // If both state and frame are updated, the specified frame will take precedence (runtime invokes setframe after setstate)
		m_iSubTimelineStartFrame = timelineObj["Frame"].toInt();
	else if(bStateUpdated)
		m_iSubTimelineStartFrame = 0;

	m_bSubTimelineDirty = bStateUpdated || bPausedUpdated || bFrameUpdated;
	return m_bSubTimelineDirty;
}

void SubEntity::ExtrapolateChildProperties(int iNumFramesDuration, uint32 uiStateIndex, HyCamera2d *pCamera)
{
	const QMap<IHyNode2d *, QMap<int, QJsonObject>> &childPropMapRef = m_StateInfoList[uiStateIndex].m_ChildPropertiesMap;


	for(ChildInfo &childInfoRef : m_ChildInfoList)
		ExtrapolateProperties(childInfoRef.m_pChild, nullptr, false, childInfoRef.m_eItemType, 1.0f / 60, m_iSubTimelineStartFrame, m_iSubTimelineStartFrame + iNumFramesDuration, childPropMapRef[childInfoRef.m_pChild], *childInfoRef.m_pPreviewComponent, pCamera);
}
// SubEntity
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOTE: The listed 4 functions below share logic that process all item properties. Any updates should reflect to all of them
//             - EntityTreeItemData::InitalizePropertyModel
//             - EntityModel::GenerateSrc_SetStateImpl
//             - EntityDrawItem::ExtractPropertyData
//             - ExtrapolateProperties
void ExtrapolateProperties(IHyLoadable2d *pThisHyNode,
						   ShapeCtrl *pShapeCtrl,
						   bool bIsSelected,
						   ItemType eItemType,
						   const float fFRAME_DURATION,
						   const int iSTART_FRAME,
						   const int iDESTINATION_FRAME,
						   const QMap<int, QJsonObject> &keyFrameMapRef,
						   EntityPreviewComponent &previewComponentRef,
						   HyCamera2d *pCamera)
{
	if(eItemType == ITEM_Sprite)
		static_cast<HySprite2d *>(pThisHyNode)->SetAnimPause(true); // We always pause the animation because it is set manually by extrapolating what frame it should be, and don't want time passing to affect it.

	for(int iFrame : keyFrameMapRef.keys())
	{
		if(iFrame > iDESTINATION_FRAME)
			break;

		if(iFrame < iSTART_FRAME)
			continue;

		// Process all properties that occurred on frame 'iFrame'
		const QJsonObject &propsObj = keyFrameMapRef[iFrame];

		// Parse all and only the potential categories of the 'eItemType' type, and set the values to 'pHyNode'
		if(eItemType != ITEM_FixtureShape && eItemType != ITEM_FixtureChain)
		{
			if(propsObj.contains("Common"))
			{
				QJsonObject commonObj = propsObj["Common"].toObject();

				if(HyGlobal::IsItemType_Asset(eItemType) == false && commonObj.contains("State"))
				{
					if(pThisHyNode->SetState(commonObj["State"].toInt()) && eItemType == ITEM_Sprite)
						previewComponentRef.m_SpriteInfo = EntityPreviewComponent::LastKnownSpriteInfo(iFrame, -1, false, previewComponentRef.m_SpriteInfo.m_bPaused);
				}
				if(commonObj.contains("Update During Paused"))
					pThisHyNode->SetPauseUpdate(commonObj["Update During Paused"].toBool());
				if(commonObj.contains("User Tag"))
					pThisHyNode->SetTag(commonObj["User Tag"].toVariant().toLongLong());
			}

			if(propsObj.contains("Transformation"))
			{
				QJsonObject transformObj = propsObj["Transformation"].toObject();
				if(transformObj.contains("Position"))
				{
					QJsonArray posArray = transformObj["Position"].toArray();
					pThisHyNode->pos.Set(glm::vec2(posArray[0].toDouble(), posArray[1].toDouble()));
					previewComponentRef.m_TweenInfo[TWEENPROP_Position].Clear();
				}
				if(transformObj.contains("Rotation"))
				{
					pThisHyNode->rot.Set(transformObj["Rotation"].toDouble());
					previewComponentRef.m_TweenInfo[TWEENPROP_Rotation].Clear();
				}
				if(transformObj.contains("Scale"))
				{
					QJsonArray scaleArray = transformObj["Scale"].toArray();
					pThisHyNode->scale.Set(glm::vec2(scaleArray[0].toDouble(), scaleArray[1].toDouble()));
					previewComponentRef.m_TweenInfo[TWEENPROP_Scale].Clear();
				}
			}

			if(eItemType != ITEM_Audio && (pThisHyNode->GetInternalFlags() & IHyNode::NODETYPE_IsBody) != 0)
			{
				if(propsObj.contains("Body"))
				{
					QJsonObject bodyObj = propsObj["Body"].toObject();
					if(bodyObj.contains("Visible"))
						pThisHyNode->SetVisible(bodyObj["Visible"].toBool());
					if(bodyObj.contains("Color Tint"))
					{
						QJsonArray colorArray = bodyObj["Color Tint"].toArray();
						static_cast<IHyBody2d *>(pThisHyNode)->SetTint(HyColor(colorArray[0].toInt(), colorArray[1].toInt(), colorArray[2].toInt()));
					}
					if(bodyObj.contains("Alpha"))
					{
						static_cast<IHyBody2d *>(pThisHyNode)->alpha.Set(bodyObj["Alpha"].toDouble());
						previewComponentRef.m_TweenInfo[TWEENPROP_Alpha].Clear();
					}
					if(bodyObj.contains("Override Display Order"))
						static_cast<IHyBody2d *>(pThisHyNode)->SetDisplayOrder(bodyObj["Override Display Order"].toInt());
				}
			}

			// Ensure all tweens' respective properties have been parsed above
			// Then parse and store the start of any tween this frame
			for(int iTweenProp = 0; iTweenProp < NUM_TWEENPROPS; ++iTweenProp)
			{
				TweenProperty eTweenProp = static_cast<TweenProperty>(iTweenProp);
				QString sCategory = "Tween " % HyGlobal::TweenPropName(eTweenProp);
				if(propsObj.contains(sCategory))
				{
					// If a tween is already active, then we need to extrapolate the tween's value to the current frame before replacing it
					if(previewComponentRef.m_TweenInfo[iTweenProp].IsActive())
						previewComponentRef.m_TweenInfo[iTweenProp].ExtrapolateIntoNode(pThisHyNode, iFrame, fFRAME_DURATION);


					// TODO: THIS IS WRONG! You cannot use pThisHyNode's current values. Need to store last good known value for each tween property, and it needs to work between states!
					QVariant startValue;

					

					switch(iTweenProp)
					{
					case TWEENPROP_Position:
						startValue = QPointF(pThisHyNode->pos.GetX(), pThisHyNode->pos.GetY());
						break;
					case TWEENPROP_Rotation:
						startValue = pThisHyNode->rot.Get();
						break;
					case TWEENPROP_Scale:
						startValue = QPointF(pThisHyNode->scale.GetX(), pThisHyNode->scale.GetY());
						break;
					case TWEENPROP_Alpha:
						startValue = static_cast<IHyBody2d *>(pThisHyNode)->alpha.Get();
						break;

					default:
						HyGuiLog("HyEntity2d::ExtrapolateProperties - Unknown TweenProperty", LOGTYPE_Error);
						break;
					}

					QJsonObject tweenObj = propsObj[sCategory].toObject();
					previewComponentRef.m_TweenInfo[iTweenProp].Set(iFrame, tweenObj, startValue);
				}
			}
		}

		
		switch(eItemType)
		{
		case ITEM_None:		// 'ITEM_None' is passed for the main entity root node
		case ITEM_Entity:	// 'ITEM_Entity' is passed when this is a sub-entity (NOTE: sub-entity timeline events are handled above)
			break;

		case ITEM_Primitive: {
			if(propsObj.contains("Primitive"))
			{
				QJsonObject primitiveObj = propsObj["Primitive"].toObject();
				if(primitiveObj.contains("Wireframe"))
					static_cast<HyPrimitive2d *>(pThisHyNode)->SetWireframe(primitiveObj["Wireframe"].toBool());
				if(primitiveObj.contains("Line Thickness"))
					static_cast<HyPrimitive2d *>(pThisHyNode)->SetLineThickness(primitiveObj["Line Thickness"].toDouble());
			}
		}
		[[fallthrough]];
		case ITEM_FixtureShape:
		case ITEM_FixtureChain: {
			if(pShapeCtrl && propsObj.contains("Shape"))
			{
				QJsonObject shapeObj = propsObj["Shape"].toObject();
				if(shapeObj.contains("Type") && shapeObj.contains("Data"))
				{
					EditorShape eShape = HyGlobal::GetShapeFromString(shapeObj["Type"].toString());
					float fBvAlpha = (eItemType == ITEM_FixtureShape || eItemType == ITEM_FixtureChain) ? 0.0f : 1.0f;
					float fOutlineAlpha = ((eItemType == ITEM_FixtureShape || eItemType == ITEM_FixtureChain) || bIsSelected) ? 1.0f : 0.0f;

					HyColor color = HyGlobal::GetEditorColor(EDITORCOLOR_Shape);
					if(eItemType == ITEM_Primitive)
					{
						color = HyColor::White;
						if(propsObj.contains("Body") && propsObj["Body"].toObject().contains("Color Tint"))
						{
							QJsonArray colorArray = propsObj["Body"].toObject()["Color Tint"].toArray();
							color = HyColor(colorArray[0].toInt(), colorArray[1].toInt(), colorArray[2].toInt());
						}
					}

					pShapeCtrl->Setup(eShape, color, fBvAlpha, fOutlineAlpha);
					pShapeCtrl->Deserialize(shapeObj["Data"].toString(), pCamera);
				}
			}

			// "Fixture" category doesn't need to be set
			break; }

		case ITEM_AtlasFrame:
			break;

		case ITEM_Text: {
			HyText2d *pTextNode = static_cast<HyText2d *>(pThisHyNode);

			if(propsObj.contains("Text"))
			{
				QJsonObject textObj = propsObj["Text"].toObject();

				// Apply all text properties before the style, so the ShapeCtrl can properly calculate itself within ShapeCtrl::SetAsText()
				if(textObj.contains("Text"))
					pTextNode->SetText(textObj["Text"].toString().toStdString());
				if(textObj.contains("Alignment"))
					pTextNode->SetAlignment(HyGlobal::GetAlignmentFromString(textObj["Alignment"].toString()));
				if(textObj.contains("Monospaced Digits"))
					pTextNode->SetMonospacedDigits(textObj["Monospaced Digits"].toBool());
				if(textObj.contains("Text Indent"))
					pTextNode->SetTextIndent(textObj["Text Indent"].toInt());

				// Apply the style and call ShapeCtrl::SetAsText()
				if(textObj.contains("Style"))
				{
					HyTextType eTextStyle = HyGlobal::GetTextTypeFromString(textObj["Style"].toString());
					if(eTextStyle == HYTEXT_Line)
					{
						if(pTextNode->IsLine() == false)
							pTextNode->SetAsLine();
					}
					else if(eTextStyle == HYTEXT_Vertical)
					{
						if(pTextNode->IsVertical() == false)
							pTextNode->SetAsVertical();
					}
					else
					{
						if(textObj.contains("Style Dimensions"))
						{
							glm::vec2 vStyleSize;
							QJsonArray styleDimensionsArray = textObj["Style Dimensions"].toArray();
							if(styleDimensionsArray.size() == 2)
								HySetVec(vStyleSize, styleDimensionsArray[0].toDouble(), styleDimensionsArray[1].toDouble());
							else
								HyGuiLog("Invalid 'Style Dimensions' array size", LOGTYPE_Error);

							if(eTextStyle == HYTEXT_Column)
							{
								if(pTextNode->IsColumn() == false || vStyleSize.x != pTextNode->GetTextBoxDimensions().x)
									pTextNode->SetAsColumn(vStyleSize.x);
							}
							else if(eTextStyle == HYTEXT_Box)
							{
								if(pTextNode->IsBox() == false ||
									vStyleSize.x != pTextNode->GetTextBoxDimensions().x ||
									vStyleSize.y != pTextNode->GetTextBoxDimensions().y)
								{
									pTextNode->SetAsBox(vStyleSize.x, vStyleSize.y, false); // TODO: Add ability to set vertical alignment
								}
							}
							else if(eTextStyle == HYTEXT_ScaleBox)
							{
								if(pTextNode->IsScaleBox() == false ||
									vStyleSize.x != pTextNode->GetTextBoxDimensions().x ||
									vStyleSize.y != pTextNode->GetTextBoxDimensions().y)
								{
									pTextNode->SetAsScaleBox(vStyleSize.x, vStyleSize.y, true); // TODO: Add ability to set vertical alignment
								}
							}
						}
					}
				}
			}
			if(pShapeCtrl)
				pShapeCtrl->SetAsText(pTextNode, bIsSelected, pCamera);
			break; }

		case ITEM_Sprite:
			// If the state was changed on this frame 'iFrame', it was already applied to pThisNode above in "Common", "State"
			if(propsObj.contains("Sprite"))
			{
				// Set the sprite to the last known anim info, and let it "naturally" AdvanceAnim() to frame 'iFrame'
				if(previewComponentRef.m_SpriteInfo.m_iSpriteFrame == -1)
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimCtrl(HYANIMCTRL_Reset);
				else
				{
					static_cast<HySprite2d *>(pThisHyNode)->SetFrame(previewComponentRef.m_SpriteInfo.m_iSpriteFrame);
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimInBouncePhase(previewComponentRef.m_SpriteInfo.m_bBouncePhase);
				}

				if(previewComponentRef.m_SpriteInfo.m_bPaused == false)
					static_cast<HySprite2d *>(pThisHyNode)->AdvanceAnim((iFrame - previewComponentRef.m_SpriteInfo.m_iEntityFrame) * fFRAME_DURATION);

				// Update the last known anim info after AdvanceAnim()
				previewComponentRef.m_SpriteInfo.m_iSpriteFrame = static_cast<HySprite2d *>(pThisHyNode)->GetFrame();
				previewComponentRef.m_SpriteInfo.m_iEntityFrame = iFrame;
				previewComponentRef.m_SpriteInfo.m_bBouncePhase = static_cast<HySprite2d *>(pThisHyNode)->IsAnimInBouncePhase();

				QJsonObject spriteObj = propsObj["Sprite"].toObject();
				if(spriteObj.contains("Frame"))
					static_cast<HySprite2d *>(pThisHyNode)->SetFrame(spriteObj["Frame"].toInt());
				if(spriteObj.contains("Anim Rate"))
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimRate(spriteObj["Anim Rate"].toDouble());
				if(spriteObj.contains("Anim Loop"))
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimCtrl(spriteObj["Anim Loop"].toBool() ? HYANIMCTRL_Loop : HYANIMCTRL_DontLoop);
				if(spriteObj.contains("Anim Reverse"))
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimCtrl(spriteObj["Anim Reverse"].toBool() ? HYANIMCTRL_Reverse : HYANIMCTRL_DontReverse);
				if(spriteObj.contains("Anim Bounce"))
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimCtrl(spriteObj["Anim Bounce"].toBool() ? HYANIMCTRL_Bounce : HYANIMCTRL_DontBounce);

				// Store whether the animation is paused, so we don't AdvanceAnim()
				if(spriteObj.contains("Anim Pause"))
					previewComponentRef.m_SpriteInfo.m_bPaused = spriteObj["Anim Pause"].toBool();

				// Update again to get the above properties of this frame
				previewComponentRef.m_SpriteInfo.m_iSpriteFrame = static_cast<HySprite2d *>(pThisHyNode)->GetFrame();
				previewComponentRef.m_SpriteInfo.m_bBouncePhase = static_cast<HySprite2d *>(pThisHyNode)->IsAnimInBouncePhase();
			}
			break;

		default:
			HyGuiLog(QString("EntityDrawItem::RefreshJson - unsupported type: ") % HyGlobal::ItemName(eItemType, false), LOGTYPE_Error);
			break;
		}

		// Lastly, if this is a sub-entity, determine if the timeline is changing based on properties this frame
		if(eItemType == ITEM_Entity &&
		   propsObj.contains("Timeline") &&
		   static_cast<SubEntity *>(pThisHyNode)->TimelineEvent(iFrame, propsObj["Timeline"].toObject(), pCamera))
		{
			return; // This indicates we need to re-extrapolate, from this point on but with a newly merged sub-entity's timeline
		}
	} // For Loop - keyFrameMapRef.keys()

	// Extrapolate any remaining time to iDESTINATION_FRAME
	// SPRITE ANIMS
	if(eItemType == ITEM_Sprite)
	{
		if(previewComponentRef.m_SpriteInfo.m_iSpriteFrame == -1)
			static_cast<HySprite2d *>(pThisHyNode)->SetAnimCtrl(HYANIMCTRL_Reset);
		else
		{
			static_cast<HySprite2d *>(pThisHyNode)->SetFrame(previewComponentRef.m_SpriteInfo.m_iSpriteFrame);
			static_cast<HySprite2d *>(pThisHyNode)->SetAnimInBouncePhase(previewComponentRef.m_SpriteInfo.m_bBouncePhase);
		}

		if(previewComponentRef.m_SpriteInfo.m_bPaused == false)
			static_cast<HySprite2d *>(pThisHyNode)->AdvanceAnim((iDESTINATION_FRAME - previewComponentRef.m_SpriteInfo.m_iEntityFrame) * fFRAME_DURATION);
	}

	// TWEENS
	for(int iTweenProp = 0; iTweenProp < NUM_TWEENPROPS; ++iTweenProp)
	{
		if(previewComponentRef.m_TweenInfo[iTweenProp].IsActive())
			previewComponentRef.m_TweenInfo[iTweenProp].ExtrapolateIntoNode(pThisHyNode, iDESTINATION_FRAME, fFRAME_DURATION);
	}
}
