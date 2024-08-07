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
	m_pEntityTreeItemData(pEntityTreeItemData)
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

/*virtual*/ IHyBody2d *EntityDrawItem::GetHyNode() /*override*/
{
	if(m_pEntityTreeItemData->GetType() == ITEM_Primitive || m_pEntityTreeItemData->GetType() == ITEM_BoundingVolume)
		return &m_ShapeCtrl.GetPrimitive();

	return m_pChild;
}

/*virtual*/ bool EntityDrawItem::IsSelected() /*override*/
{
	return m_pEntityTreeItemData->IsSelected();
}

EntityTreeItemData *EntityDrawItem::GetEntityTreeItemData() const
{
	return m_pEntityTreeItemData;
}

// NOTE: The listed 4 functions below share logic that process all item properties. Any updates should reflect to all of them
//             - EntityTreeItemData::InitalizePropertyModel
//             - EntityModel::GenerateSrc_SetStateImpl
//             - EntityDrawItem::ExtractPropertyData
//             - ExtrapolateProperties
QJsonValue EntityDrawItem::ExtractPropertyData(QString sCategory, QString sPropertyName)
{
	if(m_pChild == nullptr)
		return QJsonValue();

	if(sCategory == "Timeline")
	{
		if(sPropertyName == "State")
			return QJsonValue(static_cast<qint64>(m_pChild->GetState()));
		if(sPropertyName == "Pause")
			return QJsonValue(static_cast<SubEntity *>(m_pChild)->IsTimelinePaused());
		if(sPropertyName == "Frame")
			return QJsonValue(static_cast<SubEntity *>(m_pChild)->GetTimelineFrame());
	}
	else if(sCategory == "Common")
	{
		if(sPropertyName == "State")
			return QJsonValue(static_cast<qint64>(m_pChild->GetState()));
		if(sPropertyName == "Update During Paused")
			return QJsonValue(m_pChild->IsPauseUpdate());
		if(sPropertyName == "User Tag")
			return QJsonValue(m_pChild->GetTag());
	}
	else if(sCategory == "Transformation")
	{
		if(sPropertyName == "Position")
			return QJsonValue(QJsonArray({ QJsonValue(static_cast<double>(m_pChild->pos.GetX())), QJsonValue(static_cast<double>(m_pChild->pos.GetY())) }));
		if(sPropertyName == "Scale")
			return QJsonValue(QJsonArray({ QJsonValue(static_cast<double>(m_pChild->scale.GetX())), QJsonValue(static_cast<double>(m_pChild->scale.GetY())) }));
		if(sPropertyName == "Rotation")
			return QJsonValue(static_cast<double>(m_pChild->rot.Get()));
	}
	else if(sCategory == "Body")
	{
		if(sPropertyName == "Visible")
			return QJsonValue(m_pChild->IsVisible());
		if(sPropertyName == "Color Tint")
			QJsonValue(QJsonArray({ QJsonValue(static_cast<IHyBody2d *>(m_pChild)->topColor.GetX()), QJsonValue(static_cast<IHyBody2d *>(m_pChild)->topColor.GetY()), QJsonValue(static_cast<IHyBody2d *>(m_pChild)->topColor.GetZ()) }));
		if(sPropertyName == "Alpha")
			return QJsonValue(static_cast<double>(static_cast<IHyBody2d *>(m_pChild)->alpha.Get()));
		if(sPropertyName == "Override Display Order")
			return QJsonValue(static_cast<IHyBody2d *>(m_pChild)->GetDisplayOrder());
	}
	//else if(sCategory == "Physics")
	//{
	//}
	else if(sCategory == "Entity")
	{
		if(sPropertyName == "Mouse Input")
			return QJsonValue(static_cast<HyEntity2d *>(m_pChild)->IsMouseInputEnabled());
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
			return QJsonValue(static_cast<int>(static_cast<HySprite2d *>(m_pChild)->GetFrame()));
		if(sPropertyName == "Anim Pause")
			return QJsonValue(static_cast<HySprite2d *>(m_pChild)->IsAnimPaused());
		if(sPropertyName == "Anim Rate")
			return QJsonValue(static_cast<double>(static_cast<HySprite2d *>(m_pChild)->GetAnimRate()));
		if(sPropertyName == "Anim Loop")
			return QJsonValue(static_cast<HySprite2d *>(m_pChild)->IsAnimLoop());
		if(sPropertyName == "Anim Reverse")
			return QJsonValue(static_cast<HySprite2d *>(m_pChild)->IsAnimReverse());
		if(sPropertyName == "Anim Bounce")
			return QJsonValue(static_cast<HySprite2d *>(m_pChild)->IsAnimBounce());
	}
	else if(sCategory == "Text")
	{
		if(sPropertyName == "Text")
			return QJsonValue(QString::fromUtf8(static_cast<HyText2d *>(m_pChild)->GetUtf8String().c_str()));
		if(sPropertyName == "Style")
			return QJsonValue(HyGlobal::GetTextTypeNameList()[static_cast<HyText2d *>(m_pChild)->GetTextType()]);
		if(sPropertyName == "Style Dimensions")
			return QJsonValue(QJsonArray({ QJsonValue(static_cast<double>(static_cast<HyText2d *>(m_pChild)->GetTextBoxDimensions().x)), QJsonValue(static_cast<double>(static_cast<HyText2d *>(m_pChild)->GetTextBoxDimensions().y)) }));
		if(sPropertyName == "Alignment")
			return QJsonValue(HyGlobal::GetAlignmentNameList()[static_cast<HyText2d *>(m_pChild)->GetAlignment()]);
		if(sPropertyName == "Monospaced Digits")
			return QJsonValue(static_cast<HyText2d *>(m_pChild)->IsMonospacedDigits());
		if(sPropertyName == "Text Indent")
			return QJsonValue(static_cast<int>(static_cast<HyText2d *>(m_pChild)->GetTextIndent()));
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
	for(QPair<IHyLoadable2d *, ItemType> &childTypePair : m_ChildTypeList)
	{
		if(childTypePair.first == this)
			continue;

		delete childTypePair.first;
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
	m_ChildTypeList.append(QPair<IHyLoadable2d *, ItemType>(pNewChild, eItemType));
	uuidChildMapRef.insert(QUuid(childObj["UUID"].toString()), pNewChild);

	pNewChild->Load();
}

void SubEntity::Extrapolate(const QMap<int, QJsonObject> &propMapRef, bool bIsSelected, float fFrameDuration, int iMainDestinationFrame, HyCamera2d *pCamera)
{
	m_iElapsedTimelineFrames = 0;
	m_iRemainingTimelineFrames = iMainDestinationFrame;

	m_iSubTimelineStartFrame = 0;
	m_iSubTimelineRemainingFrames = iMainDestinationFrame;
	do
	{
		m_bSubTimelineDirty = false;

		QMap<int, QJsonObject> mergedMap = propMapRef;
		MergeRootProperties(mergedMap);

		// NOTE: ExtrapolateProperties may invoke `this::TimelineEvent()` which may set m_bSubTimelineDirty to true
		ExtrapolateProperties(this,
							  nullptr,
							  bIsSelected,
							  ITEM_Entity,
							  fFrameDuration,
							  m_iElapsedTimelineFrames, // Starting frame to extrapolate from
							  m_iElapsedTimelineFrames + m_iRemainingTimelineFrames,
							  mergedMap,
							  pCamera);

	} while(m_bSubTimelineDirty);

	if(m_iSubTimelineRemainingFrames > 0)
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
	for(QPair<IHyLoadable2d *, ItemType> &childTypePair : m_ChildTypeList)
		ExtrapolateProperties(childTypePair.first, nullptr, false, childTypePair.second, 1.0f / 60, m_iSubTimelineStartFrame, m_iSubTimelineStartFrame + iNumFramesDuration, childPropMapRef[childTypePair.first], pCamera);
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
						   HyCamera2d *pCamera)
{
	// Sprite Special Case:
	// To determine the sprite's animation frame that should be presented, whenever a property that might affect
	// what frame the sprite's animation could be on, calculate 'spriteLastKnownAnimInfo' up to that point.
	// Once all properties have been processed, extrapolate the remaining time up to the Entity's 'iCURRENT_FRAME'
	enum {
		SPRITE_SpriteFrame = 0,	// Sprite's frame (-1 indicates it hasn't been set, and should be HYANIMCTRL_Reset)
		SPRITE_EntityFrame,
		SPRITE_BouncePhase,		// A boolean whether animation is in the "bounce phase"
		SPRITE_Paused			// A boolean whether animation is paused
	};
	std::tuple<int, int, bool, bool> spriteLastKnownAnimInfo(-1, 0, false, false);

	// Tween Special Case:
	// To determine the tween's current value, store the info that kicked it off, and extrapolate the based on Entity's 'iCURRENT_FRAME'
	TweenInfo tweenInfo[NUM_TWEENPROPS] = { TWEENPROP_Position, TWEENPROP_Rotation, TWEENPROP_Scale, TWEENPROP_Alpha };
	std::function<void(int, int)> fpApplyTween = [&](int iTweenProp, int iFrameIndex)
		{
			QVariant extrapolatedValue = tweenInfo[iTweenProp].Extrapolate(iFrameIndex, fFRAME_DURATION);
			switch(iTweenProp)
			{
			case TWEENPROP_Position:
				pThisHyNode->pos.SetX(static_cast<float>(extrapolatedValue.toPointF().x()));
				pThisHyNode->pos.SetY(static_cast<float>(extrapolatedValue.toPointF().y()));
				break;

			case TWEENPROP_Rotation:
				pThisHyNode->rot.Set(extrapolatedValue.toDouble());
				break;

			case TWEENPROP_Scale:
				pThisHyNode->scale.SetX(static_cast<float>(extrapolatedValue.toPointF().x()));
				pThisHyNode->scale.SetY(static_cast<float>(extrapolatedValue.toPointF().y()));
				break;

			case TWEENPROP_Alpha:
				static_cast<IHyBody2d *>(pThisHyNode)->alpha.Set(extrapolatedValue.toDouble());
				break;

			default:
				HyGuiLog("ExtrapolateProperties() - Unhandled tween property (fpApplyTween)", LOGTYPE_Error);
				break;
			}
		};

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
		if(eItemType != ITEM_BoundingVolume)
		{
			if(propsObj.contains("Common"))
			{
				QJsonObject commonObj = propsObj["Common"].toObject();

				if(HyGlobal::IsItemType_Asset(eItemType) == false && commonObj.contains("State"))
				{
					if(pThisHyNode->SetState(commonObj["State"].toInt()) && eItemType == ITEM_Sprite)
						spriteLastKnownAnimInfo = std::make_tuple(-1, iFrame, false, std::get<SPRITE_Paused>(spriteLastKnownAnimInfo));
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
					tweenInfo[TWEENPROP_Position].Clear();
				}
				if(transformObj.contains("Rotation"))
				{
					pThisHyNode->rot.Set(transformObj["Rotation"].toDouble());
					tweenInfo[TWEENPROP_Rotation].Clear();
				}
				if(transformObj.contains("Scale"))
				{
					QJsonArray scaleArray = transformObj["Scale"].toArray();
					pThisHyNode->scale.Set(glm::vec2(scaleArray[0].toDouble(), scaleArray[1].toDouble()));
					tweenInfo[TWEENPROP_Scale].Clear();
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
						tweenInfo[TWEENPROP_Alpha].Clear();
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
					if(tweenInfo[iTweenProp].IsActive())
						fpApplyTween(iTweenProp, iFrame);

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
					tweenInfo[iTweenProp].Set(iFrame, tweenObj, startValue);
				}
			}
		}

		
		switch(eItemType)
		{
		case ITEM_Unknown:	// 'ITEM_Unknown' is passed for the main entity root node
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
		case ITEM_BoundingVolume: {
			if(pShapeCtrl && propsObj.contains("Shape"))
			{
				QJsonObject shapeObj = propsObj["Shape"].toObject();
				if(shapeObj.contains("Type") && shapeObj.contains("Data"))
				{
					EditorShape eShape = HyGlobal::GetShapeFromString(shapeObj["Type"].toString());
					float fBvAlpha = (eItemType == ITEM_BoundingVolume) ? 0.0f : 1.0f;
					float fOutlineAlpha = (eItemType == ITEM_BoundingVolume || bIsSelected) ? 1.0f : 0.0f;

					HyColor color = HyGlobal::GetEditorColor(EDITORCOLOR_Shape);
					if(eItemType == ITEM_Primitive)
					{
						color = HyColor::White;
						if(propsObj.contains("Body") && propsObj["Body"].toObject().contains("Color Tint"))
						{
							QJsonArray colorArray = propsObj["Color Tint"].toArray();
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
				if(std::get<SPRITE_SpriteFrame>(spriteLastKnownAnimInfo) == -1)
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimCtrl(HYANIMCTRL_Reset);
				else
				{
					static_cast<HySprite2d *>(pThisHyNode)->SetFrame(std::get<SPRITE_SpriteFrame>(spriteLastKnownAnimInfo));
					static_cast<HySprite2d *>(pThisHyNode)->SetAnimInBouncePhase(std::get<SPRITE_BouncePhase>(spriteLastKnownAnimInfo));
				}

				if(std::get<SPRITE_Paused>(spriteLastKnownAnimInfo) == false)
					static_cast<HySprite2d *>(pThisHyNode)->AdvanceAnim((iFrame - std::get<SPRITE_EntityFrame>(spriteLastKnownAnimInfo)) * fFRAME_DURATION);

				// Update the last known anim info after AdvanceAnim()
				std::get<SPRITE_SpriteFrame>(spriteLastKnownAnimInfo) = static_cast<HySprite2d *>(pThisHyNode)->GetFrame();
				std::get<SPRITE_EntityFrame>(spriteLastKnownAnimInfo) = iFrame;
				std::get<SPRITE_BouncePhase>(spriteLastKnownAnimInfo) = static_cast<HySprite2d *>(pThisHyNode)->IsAnimInBouncePhase();

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
					std::get<SPRITE_Paused>(spriteLastKnownAnimInfo) = spriteObj["Anim Pause"].toBool();
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
		if(std::get<SPRITE_SpriteFrame>(spriteLastKnownAnimInfo) == -1)
			static_cast<HySprite2d *>(pThisHyNode)->SetAnimCtrl(HYANIMCTRL_Reset);
		else
		{
			static_cast<HySprite2d *>(pThisHyNode)->SetFrame(std::get<SPRITE_SpriteFrame>(spriteLastKnownAnimInfo));
			static_cast<HySprite2d *>(pThisHyNode)->SetAnimInBouncePhase(std::get<SPRITE_BouncePhase>(spriteLastKnownAnimInfo));
		}

		if(std::get<SPRITE_Paused>(spriteLastKnownAnimInfo) == false)
			static_cast<HySprite2d *>(pThisHyNode)->AdvanceAnim((iDESTINATION_FRAME - std::get<SPRITE_EntityFrame>(spriteLastKnownAnimInfo)) * fFRAME_DURATION);
	}

	// TWEENS
	for(int iTweenProp = 0; iTweenProp < NUM_TWEENPROPS; ++iTweenProp)
	{
		if(tweenInfo[iTweenProp].IsActive())
			fpApplyTween(iTweenProp, iDESTINATION_FRAME);
	}
}
