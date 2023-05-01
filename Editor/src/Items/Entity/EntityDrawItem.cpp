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

EntityDrawItem::EntityDrawItem(Project &projectRef, ItemType eGuiType, quint32 uiAssetChecksum, QUuid uuid, QUuid itemUuid, HyEntity2d *pParent) :
	m_eGuiType(eGuiType),
	m_Uuid(uuid),
	m_ProjItemUuid(itemUuid),
	m_pChild(nullptr),
	m_Transform(pParent),
	m_ShapeCtrl(pParent)
{
	if(m_eGuiType != ITEM_BoundingVolume && m_eGuiType != ITEM_Primitive)
	{
		ProjectItemData *pProjItemData = static_cast<ProjectItemData *>(projectRef.FindItemData(m_ProjItemUuid));
		if(pProjItemData == nullptr)
		{
			HyGuiLog("EntityDrawItem::RefreshOverrideData - could not find item data for: " % m_ProjItemUuid.toString(), LOGTYPE_Error);
			return;
		}

		FileDataPair fileDataPair;
		pProjItemData->GetSavedFileData(fileDataPair);

		if(m_eGuiType == ITEM_Entity)
		{
			m_pChild = new SubEntity(pParent);
			SubEntity *pSubEntity = static_cast<SubEntity *>(m_pChild);
			pSubEntity->Assemble(projectRef, fileDataPair.m_Meta["childList"].toArray(), QJsonArray());
		}
		else
		{
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("EntityDraw::ItemWidget::RefreshOverrideData failed to parse its file data", LOGTYPE_Error);

#undef GetObject
			switch(m_eGuiType)
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

			case ITEM_AtlasFrame:
				m_pChild = new HyTexturedQuad2d(uiAssetChecksum, pParent);
				break;

			case ITEM_Primitive:
			case ITEM_Audio:
			case ITEM_SoundClip:
			default:
				HyLogError("EntityDrawItem ctor - unhandled gui item type");
				break;
			}
		}

		m_pChild->Load();
	}

	HideTransformCtrl();
}

/*virtual*/ EntityDrawItem::~EntityDrawItem()
{
	delete m_pChild;
}

ItemType EntityDrawItem::GetGuiType() const
{
	return m_eGuiType;
}

const QUuid &EntityDrawItem::GetThisUuid() const
{
	return m_Uuid;
}

const QUuid &EntityDrawItem::GetProjItemUuid() const
{
	return m_ProjItemUuid;
}

IHyLoadable2d *EntityDrawItem::GetHyNode()
{
	if(m_eGuiType == ITEM_Primitive || m_eGuiType == ITEM_BoundingVolume)
		return &m_ShapeCtrl.GetPrimitive();

	return m_pChild;
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

// NOTE: This matches how EntityStateData::InitalizePropertyModel initializes the 'PropertiesTreeModel'
//		 Updates here should reflect to the function above
void EntityDrawItem::RefreshJson(QJsonObject descObj, QJsonObject propObj, HyCamera2d *pCamera)
{
	if(m_eGuiType == ITEM_Prefix) // aka Shapes folder
		return;

	IHyLoadable2d *pHyNode = GetHyNode();
	bool bIsSelected = descObj["isSelected"].toBool();

	// Parse all and only the potential categories of the 'm_eGuiType' type, and set the values to 'pHyNode'
	HyColor colorTint = ENTCOLOR_Shape;
	if(m_eGuiType != ITEM_BoundingVolume)
	{
		QJsonObject commonObj = propObj["Common"].toObject();

		pHyNode->SetState(commonObj["State"].toInt());

		pHyNode->SetPauseUpdate(commonObj["Update During Paused"].toBool());
		pHyNode->SetTag(commonObj["User Tag"].toVariant().toLongLong());

		QJsonObject transformObj = propObj["Transformation"].toObject();
		QJsonArray posArray = transformObj["Position"].toArray();
		pHyNode->pos.Set(glm::vec2(posArray[0].toDouble(), posArray[1].toDouble()));
		pHyNode->rot.Set(transformObj["Rotation"].toDouble());
		QJsonArray scaleArray = transformObj["Scale"].toArray();
		pHyNode->scale.Set(glm::vec2(scaleArray[0].toDouble(), scaleArray[1].toDouble()));

		if(m_eGuiType != ITEM_Audio && (pHyNode->GetInternalFlags() & IHyNode::NODETYPE_IsBody) != 0)
		{
			QJsonObject bodyObj = propObj["Body"].toObject();
			pHyNode->SetVisible(bodyObj["Visible"].toBool());

			QJsonArray colorArray = bodyObj["Color Tint"].toArray();
			colorTint = HyColor(colorArray[0].toInt(), colorArray[1].toInt(), colorArray[2].toInt());
			static_cast<IHyBody2d *>(pHyNode)->SetTint(colorTint);

			static_cast<IHyBody2d *>(pHyNode)->alpha.Set(bodyObj["Alpha"].toDouble());

			int iDisplayOrder = bodyObj["Display Order"].toInt();
			if(iDisplayOrder != 0)
				static_cast<IHyBody2d *>(pHyNode)->SetDisplayOrder(iDisplayOrder);
		}
	}

	switch(m_eGuiType)
	{
	case ITEM_Entity:
		// "Physics" category doesn't need to be set
		break;

	case ITEM_Primitive: {
		QJsonObject primitiveObj = propObj["Primitive"].toObject();
		static_cast<HyPrimitive2d *>(pHyNode)->SetWireframe(primitiveObj["Wireframe"].toBool());
		static_cast<HyPrimitive2d *>(pHyNode)->SetLineThickness(primitiveObj["Line Thickness"].toDouble());
		}
		[[fallthrough]];
	case ITEM_BoundingVolume: {
		QJsonObject shapeObj = propObj["Shape"].toObject();
		EditorShape eShape = HyGlobal::GetShapeFromString(shapeObj["Type"].toString());
		float fBvAlpha = (m_eGuiType == ITEM_BoundingVolume) ? 0.0f : 1.0f;
		float fOutlineAlpha = (m_eGuiType == ITEM_BoundingVolume || bIsSelected) ? 1.0f : 0.0f;

		GetShapeCtrl().Setup(eShape, colorTint, fBvAlpha, fOutlineAlpha);
		GetShapeCtrl().Deserialize(shapeObj["Data"].toString(), pCamera);
		// "Fixture" category doesn't need to be set
		break; }

	case ITEM_AtlasFrame:
		break;

	case ITEM_Text: {
		HyText2d *pTextNode = static_cast<HyText2d *>(pHyNode);

		QJsonObject textObj = propObj["Text"].toObject();

		// Apply all text properties before the style, so the ShapeCtrl can properly calculate itself within ShapeCtrl::SetAsText()
		pTextNode->SetText(textObj["Text"].toString().toStdString());
		pTextNode->SetTextAlignment(HyGlobal::GetAlignmentFromString(textObj["Alignment"].toString()));
		pTextNode->SetMonospacedDigits(textObj["Monospaced Digits"].toBool());
		pTextNode->SetTextIndent(textObj["Text Indent"].toInt());
		
		// Apply the style and call ShapeCtrl::SetAsText()
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
		GetShapeCtrl().SetAsText(pTextNode, bIsSelected, pCamera);
		break; }

	case ITEM_Sprite: {
		QJsonObject spriteObj = propObj["Sprite"].toObject();
		static_cast<HySprite2d *>(pHyNode)->SetFrame(spriteObj["Frame"].toInt());
		static_cast<HySprite2d *>(pHyNode)->SetAnimRate(spriteObj["Anim Rate"].toDouble());
		static_cast<HySprite2d *>(pHyNode)->SetAnimPause(spriteObj["Anim Paused"].toBool());
		break; }

	default:
		HyGuiLog(QString("EntityDrawItem::RefreshJson - unsupported type: ") % QString::number(m_eGuiType), LOGTYPE_Error);
		break;
	}
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
	switch(GetGuiType())
	{
	case ITEM_BoundingVolume:
	case ITEM_AtlasFrame:
	case ITEM_Primitive:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Sprite: {
		IHyDrawable2d *pDrawable = static_cast<IHyDrawable2d *>(GetHyNode());
		pDrawable->CalcLocalBoundingShape(boundingShapeOut);
		transformMtxOut = GetHyNode()->GetSceneTransform(0.0f);
		break; }

	case ITEM_Audio:
	case ITEM_Entity:
	default:
		HyLogError("EntityItemDraw::ExtractTransform - unhandled child node type");
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

SubEntity::SubEntity(HyEntity2d *pParent) :
	HyEntity2d(pParent)
{
}

/*virtual*/ SubEntity::~SubEntity()
{
	for(int i = 0; i < m_ChildPtrList.size(); ++i)
		delete m_ChildPtrList[i];
}

void SubEntity::Assemble(Project &projectRef, QJsonArray descListArray, QJsonArray propListArray)
{
	if(descListArray.size() != propListArray.size())
	{
		HyGuiLog("SubEntity::Assemble - descListArray and propListArray are not the same size", LOGTYPE_Error);
		return;
	}

	for(int i = 0; i < descListArray.size(); ++i)
	{
		QJsonObject childObj = descListArray[i].toObject();
		ItemType eItemType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());

		switch(eItemType)
		{
		case ITEM_Primitive:	m_ChildPtrList.append(new HyPrimitive2d(this)); break;
		case ITEM_Audio:		m_ChildPtrList.append(new HyAudio2d("", HY_GUI_DATAOVERRIDE, this)); break;
		case ITEM_Particles:	HyGuiLog("SubEntity::Assemble - Particles not implemented", LOGTYPE_Error); break;
		case ITEM_Text:			m_ChildPtrList.append(new HyText2d("", HY_GUI_DATAOVERRIDE, this)); break;
		case ITEM_Spine:		m_ChildPtrList.append(new HySpine2d("", HY_GUI_DATAOVERRIDE, this)); break;
		case ITEM_Sprite:		m_ChildPtrList.append(new HySprite2d("", HY_GUI_DATAOVERRIDE, this)); break;
		case ITEM_Prefab:		HyGuiLog("SubEntity::Assemble - Prefab not implemented", LOGTYPE_Error); break;
		case ITEM_Entity:		m_ChildPtrList.append(new SubEntity(this)); break;

		default:
			HyGuiLog("SubEntity::Assemble - unhandled child node type", LOGTYPE_Error);
			break;
		}

		QUuid uuid = QUuid(childObj["itemUUID"].toString());
		TreeModelItemData *pItemData = projectRef.FindItemData(uuid);
		if(pItemData->IsProjectItem() == false)
		{
			HyGuiLog("SubEntity::Assemble - child item '" % pItemData->GetText() % "' is not a project item", LOGTYPE_Error);
			continue;
		}
		ProjectItemData *pProjItemData = static_cast<ProjectItemData *>(pItemData);

		FileDataPair fileDataPair;
		pProjItemData->GetSavedFileData(fileDataPair);

		if(eItemType == ITEM_Entity)
			static_cast<SubEntity *>(m_ChildPtrList.back())->Assemble(projectRef, fileDataPair.m_Meta["childList"].toArray(), QJsonArray());
		else
		{
			QByteArray src = JsonValueToSrc(fileDataPair.m_Data);
			HyJsonDoc itemDataDoc;
			if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
				HyGuiLog("IDraw::ApplyJsonData failed to parse", LOGTYPE_Error);

			HyJsonObj itemDataObj = itemDataDoc.GetObject();
			static_cast<IHyDrawable2d *>(m_ChildPtrList.back())->GuiOverrideData<HySpriteData>(itemDataObj);
		}
	}
}
