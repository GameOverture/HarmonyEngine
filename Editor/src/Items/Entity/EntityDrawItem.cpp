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
#include "MainWindow.h"
#include "EntityDopeSheetScene.h"

EntityDrawItem::EntityDrawItem(Project &projectRef, EntityTreeItemData *pEntityTreeItemData, HyEntity2d *pParent) :
	m_pEntityTreeItemData(pEntityTreeItemData),
	m_pChild(nullptr),
	m_Transform(pParent),
	m_ShapeCtrl(pParent)
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
			m_pChild = new SubEntity(projectRef, fileDataPair.m_Meta["descChildList"].toArray(), fileDataPair.m_Meta["stateArray"].toArray(), pParent);
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

EntityTreeItemData *EntityDrawItem::GetEntityTreeItemData() const
{
	return m_pEntityTreeItemData;
}

IHyLoadable2d *EntityDrawItem::GetHyNode()
{
	if(m_pEntityTreeItemData->GetType() == ITEM_Primitive || m_pEntityTreeItemData->GetType() == ITEM_BoundingVolume)
		return &m_ShapeCtrl.GetPrimitive();

	return m_pChild;
}

// NOTE: The listed 3 functions below share logic that process all item properties. Any updates should reflect to all of them
//             - EntityTreeItemData::InitalizePropertyModel
//             - EntityModel::GenerateSrc_SetStateImpl
//             - EntityDrawItem::SetHyNode
void EntityDrawItem::SetHyNode(const EntityDopeSheetScene &entityDopeSheetSceneRef, HyCamera2d *pCamera)
{
	ItemType eItemType = m_pEntityTreeItemData->GetType();
	if(eItemType == ITEM_Prefix) // aka Shapes folder
		return;

	IHyLoadable2d *pThisHyNode = GetHyNode();

	const float fFRAME_DURATION = 1.0f / static_cast<EntityModel &>(entityDopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond();
	const int iCURRENT_FRAME = entityDopeSheetSceneRef.GetCurrentFrame();
	const QMap<int, QJsonObject> &keyFrameMapRef = entityDopeSheetSceneRef.GetKeyFramesMap()[m_pEntityTreeItemData];

	ExtrapolateProperties(pThisHyNode, &m_ShapeCtrl, m_pEntityTreeItemData->IsSelected(), eItemType, fFRAME_DURATION, iCURRENT_FRAME, keyFrameMapRef, pCamera);
}

ShapeCtrl &EntityDrawItem::GetShapeCtrl()
{
	return m_ShapeCtrl;
}

TransformCtrl &EntityDrawItem::GetTransformCtrl()
{
	return m_Transform;
}

bool EntityDrawItem::IsMouseInBounds()
{
	HyShape2d boundingShape;
	glm::mat4 transformMtx;
	ExtractTransform(boundingShape, transformMtx);
	
	glm::vec2 ptWorldMousePos;
	return HyEngine::Input().GetWorldMousePos(ptWorldMousePos) && boundingShape.TestPoint(transformMtx, ptWorldMousePos);
}

void EntityDrawItem::RefreshTransform(HyCamera2d *pCamera)
{
	HyShape2d boundingShape;
	glm::mat4 mtxShapeTransform;
	ExtractTransform(boundingShape, mtxShapeTransform);

	m_Transform.WrapTo(boundingShape, mtxShapeTransform, pCamera);
	GetShapeCtrl().DeserializeOutline(pCamera);
}

void EntityDrawItem::ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut)
{
	transformMtxOut = glm::identity<glm::mat4>();
	switch(m_pEntityTreeItemData->GetType())
	{
	//case ITEM_Sprite: {

	//	TreeModelItemData *pReferencedItemData = GetEntityTreeItemData()->GetEntityModel().GetItem().GetProject().FindItemData(m_pEntityTreeItemData->GetReferencedItemUuid());
	//	pReferencedItemData = pReferencedItemData;
	//	const QUuid &itemUuidRef = ;
	//	AtlasFrame *pAtlasFrame = GetEntityModel()->GetItem().GetProject().GetAtlasModel().FindFrame(itemUuidRef, );

	//	//HySprite2d *pSprite = static_cast<HySprite2d *>(GetHyNode());
	//	//const HySpriteData *pData = static_cast<const HySpriteData *>(pSprite->AcquireData());
	//	//if(pData)
	//	//{
	//	//	 // GetEntityModel()->GetItem().GetProject().GetAtlasModel().FindFrame(

	//	//	AtlasFrame *pAtlasFrame = pData->GetAtlasFrame();
	//	//}
	//	//break;
	//}
	case ITEM_Sprite:
	case ITEM_BoundingVolume:
	case ITEM_AtlasFrame:
	case ITEM_Primitive:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Entity: {
		IHyBody2d *pHyBody = static_cast<IHyBody2d *>(GetHyNode());
		pHyBody->CalcLocalBoundingShape(boundingShapeOut);
		transformMtxOut = GetHyNode()->GetSceneTransform(0.0f);
		break; }

	case ITEM_Audio:
	default:
		HyGuiLog("EntityItemDraw::ExtractTransform - unhandled child node type: " % HyGlobal::ItemName(m_pEntityTreeItemData->GetType(), false), LOGTYPE_Error);
		break;
	}
}

void EntityDrawItem::ShowTransformCtrl(bool bShowGrabPoints)
{
	m_Transform.Show(bShowGrabPoints);
}

void EntityDrawItem::HideTransformCtrl()
{
	m_Transform.Hide();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SubEntity::SubEntity(Project &projectRef, const QJsonArray &descArray, const QJsonArray &stateArray, HyEntity2d *pParent) :
	HyEntity2d(pParent)
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
		QJsonObject stateObj = stateArray[i].toObject();
		
		m_StateInfoList.push_back(StateInfo());
		m_StateInfoList.back().m_iFramesPerSecond = stateObj["framesPerSecond"].toInt();

		QJsonObject keyFramesObj = stateObj["keyFrames"].toObject();
		for(auto iter = keyFramesObj.begin(); iter != keyFramesObj.end(); ++iter)
		{
			QUuid uuid(iter.key());
			m_StateInfoList.back().m_PropertiesMap.insert(uuidChildMap[uuid], QMap<int, QJsonObject>());
			QMap<int, QJsonObject> &propMapRef = m_StateInfoList.back().m_PropertiesMap[uuidChildMap[uuid]];

			QJsonArray itemKeyFrameArray = iter.value().toArray();
			for(int iKeyFrameArrayIndex = 0; iKeyFrameArrayIndex < itemKeyFrameArray.size(); ++iKeyFrameArrayIndex)
			{
				QJsonObject itemKeyFrameObj = itemKeyFrameArray[iKeyFrameArrayIndex].toObject();
				int iFrame = itemKeyFrameObj["frame"].toInt();
				QJsonObject propsObj = itemKeyFrameObj["props"].toObject();

				propMapRef.insert(iFrame, propsObj);
			}
		}
	}

	SetState(0);
}
/*virtual*/ SubEntity::~SubEntity()
{
	for(QPair<IHyLoadable2d *, ItemType> &childTypePair : m_ChildTypeList)
		delete childTypePair.first;
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
		// TODO: how do we GuiOverrideData for audio?
		//static_cast<HyAudio2d *>(pNewChild)->GuiOverrideData<HyAudioData>(itemDataDoc.GetObject());
		break;

	case ITEM_Particles:
		HyGuiLog("SubEntity ctor - Particles not implemented", LOGTYPE_Error);
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
		pNewChild = new SubEntity(projectRef, fileDataPair.m_Meta["descChildList"].toArray(), fileDataPair.m_Meta["stateArray"].toArray(), this);
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
void SubEntity::ExtrapolateChildProperties(const int iCURRENT_FRAME, HyCamera2d *pCamera)
{
	const float fFRAME_DURATION = 1.0f / m_StateInfoList[GetState()].m_iFramesPerSecond;
	const QMap<IHyNode2d *, QMap<int, QJsonObject>>	&propMapRef = m_StateInfoList[GetState()].m_PropertiesMap;
	
	for(QPair<IHyLoadable2d *, ItemType> &childTypePair : m_ChildTypeList)
		ExtrapolateProperties(childTypePair.first, nullptr, false, childTypePair.second, fFRAME_DURATION, iCURRENT_FRAME, propMapRef[childTypePair.first], pCamera);
}
// SubEntity
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ExtrapolateProperties(IHyLoadable2d *pThisHyNode, ShapeCtrl *pShapeCtrl, bool bIsSelected, ItemType eItemType, const float fFRAME_DURATION, const int iCURRENT_FRAME, const QMap<int, QJsonObject> &keyFrameMapRef, HyCamera2d *pCamera)
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
	TweenInfo tweenInfo[NUM_TWEENPROPS];// = { PROPERTIESTYPE_vec2, PROPERTIESTYPE_double, PROPERTIESTYPE_vec2, PROPERTIESTYPE_double };

	if(eItemType == ITEM_Entity)
		static_cast<SubEntity *>(pThisHyNode)->ExtrapolateChildProperties(iCURRENT_FRAME, pCamera);

	for(int iFrame : keyFrameMapRef.keys())
	{
		if(iFrame > iCURRENT_FRAME)
			break;

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
					QJsonObject tweenObj = propsObj[sCategory].toObject();

					tweenInfo[iTweenProp].m_iStartFrame = iFrame;
					tweenInfo[iTweenProp].m_fDuration = tweenObj["Duration"].toDouble();
					tweenInfo[iTweenProp].m_eTweenType = HyGlobal::GetTweenFromString(tweenObj["Tween Type"].toString());
					switch(iTweenProp)
					{
					case TWEENPROP_Position: {
						tweenInfo[iTweenProp].m_Start.setValue(QPointF(pThisHyNode->pos.GetX(), pThisHyNode->pos.GetY()));
						QJsonArray destinationArray = tweenObj["Destination"].toArray();
						tweenInfo[iTweenProp].m_Destination.setValue(QPointF(destinationArray[0].toDouble(), destinationArray[1].toDouble()));
						break; }

					case TWEENPROP_Rotation:
						tweenInfo[iTweenProp].m_Start = pThisHyNode->rot.Get();
						tweenInfo[iTweenProp].m_Destination = tweenObj["Destination"].toDouble();
						break;

					case TWEENPROP_Scale: {
						tweenInfo[iTweenProp].m_Start.setValue(QPointF(pThisHyNode->scale.GetX(), pThisHyNode->scale.GetY()));
						QJsonArray destinationArray = tweenObj["Destination"].toArray();
						tweenInfo[iTweenProp].m_Destination.setValue(QPointF(destinationArray[0].toDouble(), destinationArray[1].toDouble()));
						break; }

					case TWEENPROP_Alpha:
						tweenInfo[iTweenProp].m_Start = static_cast<IHyBody2d *>(pThisHyNode)->alpha.Get();
						tweenInfo[iTweenProp].m_Destination = tweenObj["Destination"].toDouble();
						break;

					default:
						HyGuiLog("EntityDrawItem::SetHyNode() - Unhandled tween property", LOGTYPE_Error);
						break;
					}
				}
			}
		}

		switch(eItemType)
		{
		case ITEM_Entity:
		//	// Call ExtrapolateProperties recursively on all pThisHyNode's children
		//	static_cast<SubEntity *>(pThisHyNode)->ExtrapolateChildProperties(iCURRENT_FRAME, pCamera);

		//	// "Physics" category doesn't need to be set
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

					pShapeCtrl->Setup(eShape, ENTCOLOR_Shape, fBvAlpha, fOutlineAlpha);
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
					pTextNode->SetTextAlignment(HyGlobal::GetAlignmentFromString(textObj["Alignment"].toString()));
				if(textObj.contains("Monospaced Digits"))
					pTextNode->SetMonospacedDigits(textObj["Monospaced Digits"].toBool());
				if(textObj.contains("Text Indent"))
					pTextNode->SetTextIndent(textObj["Text Indent"].toInt());

				// Apply the style and call ShapeCtrl::SetAsText()
				if(textObj.contains("Style"))
				{
					TextStyle eTextStyle = HyGlobal::GetTextStyleFromString(textObj["Style"].toString());
					if(eTextStyle == TEXTSTYLE_Line)
					{
						if(pTextNode->IsLine() == false)
							pTextNode->SetAsLine();
					}
					else if(eTextStyle == TEXTSTYLE_Vertical)
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

							if(eTextStyle == TEXTSTYLE_Column)
							{
								if(pTextNode->IsColumn() == false || vStyleSize.x != pTextNode->GetTextBoxDimensions().x)
									pTextNode->SetAsColumn(vStyleSize.x, false);
							}
							else // TEXTSTYLE_ScaleBox or TEXTSTYLE_ScaleBoxTopAlign
							{
								if(pTextNode->IsScaleBox() == false ||
									vStyleSize.x != pTextNode->GetTextBoxDimensions().x ||
									vStyleSize.y != pTextNode->GetTextBoxDimensions().y ||
									(eTextStyle == TEXTSTYLE_ScaleBox) != pTextNode->IsScaleBoxCenterVertically())
								{
									pTextNode->SetAsScaleBox(vStyleSize.x, vStyleSize.y, eTextStyle == TEXTSTYLE_ScaleBox);
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
			// We always pause the animation because it is set manually by extrapolating what frame it should be, and don't want time passing to affect it.
			static_cast<HySprite2d *>(pThisHyNode)->SetAnimPause(true);

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
	}

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
			static_cast<HySprite2d *>(pThisHyNode)->AdvanceAnim((iCURRENT_FRAME - std::get<SPRITE_EntityFrame>(spriteLastKnownAnimInfo)) * fFRAME_DURATION);
	}

	// Apply any active tweens
	for(int iTweenProp = 0; iTweenProp < NUM_TWEENPROPS; ++iTweenProp)
	{
		if(tweenInfo[iTweenProp].m_iStartFrame != -1)
		{
			float fElapsedTime = (iCURRENT_FRAME - tweenInfo[iTweenProp].m_iStartFrame) * fFRAME_DURATION;
			fElapsedTime = HyMath::Clamp(fElapsedTime, 0.0f, tweenInfo[iTweenProp].m_fDuration);
			HyTweenFunc fpTweenFunc = HyGlobal::GetTweenFunc(tweenInfo[iTweenProp].m_eTweenType);
			float fRatio = (tweenInfo[iTweenProp].m_fDuration > 0.0f) ? fpTweenFunc(fElapsedTime / tweenInfo[iTweenProp].m_fDuration) : 1.0f;

			switch(iTweenProp)
			{
			case TWEENPROP_Position:
				pThisHyNode->pos.SetX(static_cast<float>(tweenInfo[iTweenProp].m_Start.toPointF().x() + (tweenInfo[iTweenProp].m_Destination.toPointF().x() - tweenInfo[iTweenProp].m_Start.toPointF().x()) * fRatio));
				pThisHyNode->pos.SetY(static_cast<float>(tweenInfo[iTweenProp].m_Start.toPointF().y() + (tweenInfo[iTweenProp].m_Destination.toPointF().y() - tweenInfo[iTweenProp].m_Start.toPointF().y()) * fRatio));
				break;

			case TWEENPROP_Rotation:
				pThisHyNode->rot.Set(tweenInfo[iTweenProp].m_Start.toDouble() + (tweenInfo[iTweenProp].m_Destination.toDouble() - tweenInfo[iTweenProp].m_Start.toDouble()) * fRatio);
				break;

			case TWEENPROP_Scale:
				pThisHyNode->scale.SetX(static_cast<float>(tweenInfo[iTweenProp].m_Start.toPointF().x() + (tweenInfo[iTweenProp].m_Destination.toPointF().x() - tweenInfo[iTweenProp].m_Start.toPointF().x()) * fRatio));
				pThisHyNode->scale.SetY(static_cast<float>(tweenInfo[iTweenProp].m_Start.toPointF().y() + (tweenInfo[iTweenProp].m_Destination.toPointF().y() - tweenInfo[iTweenProp].m_Start.toPointF().y()) * fRatio));
				break;

			case TWEENPROP_Alpha:
				static_cast<IHyBody2d *>(pThisHyNode)->alpha.Set(tweenInfo[iTweenProp].m_Start.toDouble() + (tweenInfo[iTweenProp].m_Destination.toDouble() - tweenInfo[iTweenProp].m_Start.toDouble()) * fRatio);
				break;

			default:
				HyGuiLog("EntityDrawItem::SetHyNode() - Unhandled tween property (applying active tween)", LOGTYPE_Error);
				break;
			}
		}
	}
}
