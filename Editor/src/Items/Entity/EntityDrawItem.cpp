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

EntityDrawItem::EntityDrawItem(Project &projectRef, ItemType eGuiType, QUuid uuid, QUuid itemUuid, HyEntity2d *pParent) :
	m_eGuiType(eGuiType),
	m_Uuid(uuid),
	m_ProjItemUuid(itemUuid),
	m_pChild(nullptr),
	m_Transform(pParent),
	m_ShapeCtrl(pParent)
{
	switch(m_eGuiType)
	{
	case ITEM_BoundingVolume:
	case ITEM_Primitive:
		m_pChild = nullptr;		// When either shape or primitive 'm_pChild' is provided via the m_ShapeCtrl
		break;

	case ITEM_Audio:			m_pChild = new HyAudio2d("", HY_GUI_DATAOVERRIDE, pParent); break;
	case ITEM_Text:				m_pChild = new HyText2d("", HY_GUI_DATAOVERRIDE, pParent); break;
	case ITEM_Spine:			m_pChild = new HySpine2d("", HY_GUI_DATAOVERRIDE, pParent); break;
	case ITEM_Sprite:			m_pChild = new HySprite2d("", HY_GUI_DATAOVERRIDE, pParent); break;

	case ITEM_Entity:			m_pChild = new HyEntity2d(pParent); break;

	case ITEM_AtlasFrame:		//m_pChild = new HyTexturedQuad2d(); break;
	default:
		HyGuiLog("EntityDrawItem ctor - unhandled child node type", LOGTYPE_Error);
		break;
	}
	if(m_pChild)
	{
		RefreshOverrideData(projectRef);
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

	//case ITEM_AtlasImage:
		//m_PropertiesTreeModel.AppendCategory("Textured Quad");
	//	break;

	case ITEM_Text: {
		QJsonObject textObj = propObj["Text"].toObject();
		static_cast<HyText2d *>(pHyNode)->SetState(textObj["State"].toInt());
		static_cast<HyText2d *>(pHyNode)->SetText(textObj["Text"].toString().toStdString());
		break; }

	case ITEM_Sprite: {
		QJsonObject spriteObj = propObj["Sprite"].toObject();
		static_cast<HySprite2d *>(pHyNode)->SetState(spriteObj["State"].toInt());
		static_cast<HySprite2d *>(pHyNode)->SetFrame(spriteObj["Frame"].toInt());
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
	GetShapeCtrl().RefreshOutline(pCamera);
}

void EntityDrawItem::RefreshOverrideData(Project &projectRef)
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
		SubEntityRefreshOverrideData(fileDataPair.m_Meta);
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
			static_cast<HyText2d *>(m_pChild)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject(), false); // The 'false' here has it so HyTextData loads the atlas as it would normally
			break;

		case ITEM_Spine:
			static_cast<HySpine2d *>(m_pChild)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
			break;

		case ITEM_Sprite:
			static_cast<HySprite2d *>(m_pChild)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
			break;

		case ITEM_Primitive:
		case ITEM_Audio:
		case ITEM_AtlasFrame:
		case ITEM_SoundClip:
		default:
			HyLogError("EntityDraw::ItemWidget::RefreshOverrideData - unhandled gui item type");
			break;
		}
	}
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

void EntityDrawItem::SubEntityRefreshOverrideData(QJsonObject metaObj)
{
	HyEntity2d *pEntity = static_cast<HyEntity2d *>(m_pChild);
	HyLogError("EntityItemDraw::SubEntityRefreshOverrideData - not implemented");
}
