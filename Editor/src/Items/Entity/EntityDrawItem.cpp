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
			m_pChild = new SubEntity(projectRef, fileDataPair.m_Meta["descChildList"].toArray(), pParent);
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
//             - EntityTreeItemData::GenerateStateSrc
//             - EntityDrawItem::SetHyNode
void EntityDrawItem::SetHyNode(const EntityDopeSheetScene &entityDopeSheetSceneRef, HyCamera2d *pCamera)
{
	ItemType eItemType = m_pEntityTreeItemData->GetType();
	if(eItemType == ITEM_Prefix) // aka Shapes folder
		return;

	IHyLoadable2d *pThisHyNode = GetHyNode();

	const float fFRAME_DURATION = 1.0f / entityDopeSheetSceneRef.GetFramesPerSecond();
	const int iCURRENT_FRAME = entityDopeSheetSceneRef.GetCurrentFrame();
	const QMap<int, QJsonObject> &keyFrameMapRef = entityDopeSheetSceneRef.GetKeyFramesMap()[m_pEntityTreeItemData];

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
	TweenInfo<glm::vec2> tweenPos;
	TweenInfo<float> tweenRot;
	TweenInfo<glm::vec2> tweenScale;
	TweenInfo<float> tweenAlpha;

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
					tweenPos.Clear();
				}
				if(transformObj.contains("Rotation"))
				{
					pThisHyNode->rot.Set(transformObj["Rotation"].toDouble());
					tweenRot.Clear();
				}
				if(transformObj.contains("Scale"))
				{
					QJsonArray scaleArray = transformObj["Scale"].toArray();
					pThisHyNode->scale.Set(glm::vec2(scaleArray[0].toDouble(), scaleArray[1].toDouble()));
					tweenScale.Clear();
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
						tweenAlpha.Clear();
					}
					if(bodyObj.contains("Override Display Order"))
						static_cast<IHyBody2d *>(pThisHyNode)->SetDisplayOrder(bodyObj["Override Display Order"].toInt());
				}
			}

			// Parse Position, Rotation, Scale, and Alpha above
			// 
			// Then parse their respective tweens
			if(propsObj.contains("Tween Position"))
			{
				QJsonObject tweenPosObj = propsObj["Tween Position"].toObject();

				tweenPos.m_iStartFrame = iFrame;
				tweenPos.m_Start = pThisHyNode->pos.Get();
				QJsonArray destinationArray = tweenPosObj["Destination"].toArray();
				tweenPos.m_Destination.x = destinationArray[0].toDouble();
				tweenPos.m_Destination.y = destinationArray[1].toDouble();
				tweenPos.m_fDuration = tweenPosObj["Duration"].toDouble();
				tweenPos.m_eTweenType = HyGlobal::GetTweenFromString(tweenPosObj["Tween Type"].toString());
			}
			if(propsObj.contains("Tween Rotation"))
			{
				QJsonObject tweenRotObj = propsObj["Tween Rotation"].toObject();

				tweenRot.m_iStartFrame = iFrame;
				tweenRot.m_Start = pThisHyNode->rot.Get();
				tweenRot.m_Destination = tweenRotObj["Destination"].toDouble();
				tweenRot.m_fDuration = tweenRotObj["Duration"].toDouble();
				tweenRot.m_eTweenType = HyGlobal::GetTweenFromString(tweenRotObj["Tween Type"].toString());
			}
			if(propsObj.contains("Tween Scale"))
			{
				QJsonObject tweenScaleObj = propsObj["Tween Scale"].toObject();

				tweenScale.m_iStartFrame = iFrame;
				tweenScale.m_Start = pThisHyNode->scale.Get();
				QJsonArray destinationArray = tweenScaleObj["Destination"].toArray();
				tweenScale.m_Destination.x = destinationArray[0].toDouble();
				tweenScale.m_Destination.y = destinationArray[1].toDouble();
				tweenScale.m_fDuration = tweenScaleObj["Duration"].toDouble();
				tweenScale.m_eTweenType = HyGlobal::GetTweenFromString(tweenScaleObj["Tween Type"].toString());
			}
			if(propsObj.contains("Tween Alpha"))
			{
				QJsonObject tweenAlphaObj = propsObj["Tween Alpha"].toObject();

				tweenAlpha.m_iStartFrame = iFrame;
				tweenAlpha.m_Start = static_cast<IHyBody2d *>(pThisHyNode)->alpha.Get();
				tweenAlpha.m_Destination = tweenAlphaObj["Destination"].toDouble();
				tweenAlpha.m_fDuration = tweenAlphaObj["Duration"].toDouble();
				tweenAlpha.m_eTweenType = HyGlobal::GetTweenFromString(tweenAlphaObj["Tween Type"].toString());
			}
			// Apply any active tweens
			if(tweenPos.m_iStartFrame != -1)
			{
				float fElapsedTime = (iFrame - tweenPos.m_iStartFrame) * fFRAME_DURATION;
				fElapsedTime = HyMath::Clamp(fElapsedTime, 0.0f, tweenPos.m_fDuration);
				HyTweenFunc fpTweenFunc = HyGlobal::GetTweenFunc(tweenPos.m_eTweenType);
				float fRatio = (tweenPos.m_fDuration > 0.0f) ? fpTweenFunc(fElapsedTime / tweenPos.m_fDuration) : 1.0f;
				pThisHyNode->pos.SetX(tweenPos.m_Start.x + (tweenPos.m_Destination.x - tweenPos.m_Start.x) * fRatio);
				pThisHyNode->pos.SetY(tweenPos.m_Start.y + (tweenPos.m_Destination.y - tweenPos.m_Start.y) * fRatio);
			}
			if(tweenRot.m_iStartFrame != -1)
			{
				float fElapsedTime = (iFrame - tweenRot.m_iStartFrame) * fFRAME_DURATION;
				fElapsedTime = HyMath::Clamp(fElapsedTime, 0.0f, tweenRot.m_fDuration);
				HyTweenFunc fpTweenFunc = HyGlobal::GetTweenFunc(tweenRot.m_eTweenType);
				float fRatio = (tweenPos.m_fDuration > 0.0f) ? fpTweenFunc(fElapsedTime / tweenPos.m_fDuration) : 1.0f;
				pThisHyNode->rot.Set(tweenRot.m_Start + (tweenRot.m_Destination - tweenRot.m_Start) * fRatio);
			}
			if(tweenScale.m_iStartFrame != -1)
			{
				float fElapsedTime = (iFrame - tweenScale.m_iStartFrame) * fFRAME_DURATION;
				fElapsedTime = HyMath::Clamp(fElapsedTime, 0.0f, tweenScale.m_fDuration);
				HyTweenFunc fpTweenFunc = HyGlobal::GetTweenFunc(tweenScale.m_eTweenType);
				float fRatio = (tweenPos.m_fDuration > 0.0f) ? fpTweenFunc(fElapsedTime / tweenPos.m_fDuration) : 1.0f;
				pThisHyNode->scale.SetX(tweenScale.m_Start.x + (tweenScale.m_Destination.x - tweenScale.m_Start.x) * fRatio);
				pThisHyNode->scale.SetY(tweenScale.m_Start.y + (tweenScale.m_Destination.y - tweenScale.m_Start.y) * fRatio);
			}
			if(tweenAlpha.m_iStartFrame != -1)
			{
				float fElapsedTime = (iFrame - tweenAlpha.m_iStartFrame) * fFRAME_DURATION;
				fElapsedTime = HyMath::Clamp(fElapsedTime, 0.0f, tweenAlpha.m_fDuration);
				HyTweenFunc fpTweenFunc = HyGlobal::GetTweenFunc(tweenAlpha.m_eTweenType);
				float fRatio = (tweenPos.m_fDuration > 0.0f) ? fpTweenFunc(fElapsedTime / tweenPos.m_fDuration) : 1.0f;
				static_cast<IHyBody2d *>(pThisHyNode)->alpha.Set(tweenAlpha.m_Start + (tweenAlpha.m_Destination - tweenAlpha.m_Start) * fRatio);
			}
		}

		switch(eItemType)
		{
		case ITEM_Entity:
			// "Physics" category doesn't need to be set
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
			if(propsObj.contains("Shape"))
			{
				QJsonObject shapeObj = propsObj["Shape"].toObject();
				if(shapeObj.contains("Type") && shapeObj.contains("Data"))
				{
					EditorShape eShape = HyGlobal::GetShapeFromString(shapeObj["Type"].toString());
					float fBvAlpha = (eItemType == ITEM_BoundingVolume) ? 0.0f : 1.0f;
					float fOutlineAlpha = (eItemType == ITEM_BoundingVolume || m_pEntityTreeItemData->IsSelected()) ? 1.0f : 0.0f;

					m_ShapeCtrl.Setup(eShape, ENTCOLOR_Shape, fBvAlpha, fOutlineAlpha);
					m_ShapeCtrl.Deserialize(shapeObj["Data"].toString(), pCamera);
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
			m_ShapeCtrl.SetAsText(pTextNode, m_pEntityTreeItemData->IsSelected(), pCamera);
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
				if(spriteObj.contains("Anim Paused"))
					std::get<SPRITE_Paused>(spriteLastKnownAnimInfo) = spriteObj["Anim Paused"].toBool();
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
	case ITEM_BoundingVolume:
	case ITEM_AtlasFrame:
	case ITEM_Primitive:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Sprite:
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
SubEntity::SubEntity(Project &projectRef, const QJsonArray &descArray, HyEntity2d *pParent) :
	HyEntity2d(pParent)
{
	for(int i = 0; i < descArray.size(); ++i)
	{
		QJsonObject childObj = descArray[i].toObject();

		ItemType eItemType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());
		TreeModelItemData *pReferencedItemData = projectRef.FindItemData(QUuid(childObj["itemUUID"].toString()));

		switch(eItemType)
		{
		case ITEM_Primitive:
			m_ChildPtrList.append(qMakePair(new HyPrimitive2d(this), eItemType));
			break;

		case ITEM_Audio: {
			m_ChildPtrList.append(qMakePair(new HyAudio2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HyAudioData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Particles:
			HyGuiLog("SubEntity ctor - Particles not implemented", LOGTYPE_Error);
			break;

		case ITEM_Text: {
			m_ChildPtrList.append(qMakePair(new HyText2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Spine: {
			m_ChildPtrList.append(qMakePair(new HySpine2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Sprite: {
			m_ChildPtrList.append(qMakePair(new HySprite2d("", HY_GUI_DATAOVERRIDE, this), eItemType));

			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("SubEntity ctor failed to parse " % HyGlobal::ItemName(eItemType, false) % " JSON data", LOGTYPE_Error);

			static_cast<IHyDrawable2d *>(m_ChildPtrList.back().first)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
			break; }

		case ITEM_Prefab:
			HyGuiLog("SubEntity ctor - Prefab not implemented", LOGTYPE_Error);
			break;

		case ITEM_Entity: {
			FileDataPair fileDataPair; static_cast<ProjectItemData *>(pReferencedItemData)->GetSavedFileData(fileDataPair);
			m_ChildPtrList.append(qMakePair(new SubEntity(projectRef, fileDataPair.m_Meta["descChildList"].toArray(), this), eItemType));
			break; }

		case ITEM_AtlasFrame:
			//uint32 uiAtlasFrameChecksum = static_cast<uint32>(childObj["assetChecksum"].toVariant().toLongLong());
			m_ChildPtrList.append(qMakePair(new HyTexturedQuad2d(static_cast<IAssetItemData *>(pReferencedItemData)->GetChecksum(), static_cast<IAssetItemData *>(pReferencedItemData)->GetBankId(), this), eItemType));
			break;

		default:
			HyGuiLog("SubEntity ctor - unhandled child node type: " % HyGlobal::ItemName(eItemType, false), LOGTYPE_Error);
			break;
		}
	}

	SetState(0);
}
/*virtual*/ SubEntity::~SubEntity()
{
	for(int i = 0; i < m_ChildPtrList.size(); ++i)
		delete m_ChildPtrList[i].first;
}
void SubEntity::RefreshProperties(const QList<QJsonObject> &propsObjList, float fElapsedTime)
{
	//for(int i = 0; i < m_ChildPtrList.size(); ++i)
	//	ApplyExtrapolatedProperties(m_ChildPtrList[i].first, nullptr, m_ChildPtrList[i].second, false, propsObjList[i], fElapsedTime, nullptr);
}
// SubEntity
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
