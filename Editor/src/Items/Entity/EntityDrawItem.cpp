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
#include "MainWindow.h"

EntityDrawItem::EntityDrawItem(HyGuiItemType eGuiType, QUuid uuid, QUuid itemUuid, HyEntity2d *pParent) :
	m_eGuiType(eGuiType),
	m_Uuid(uuid),
	m_ItemUuid(itemUuid),
	m_pChild(nullptr),
	m_pShape(nullptr),
	m_bStale(false)
{
	switch(m_eGuiType)
	{
	case ITEM_Primitive:		m_pChild = new HyPrimitive2d(pParent); break;
	case ITEM_Audio:			m_pChild = new HyAudio2d("", HY_GUI_DATAOVERRIDE, pParent); break;
	case ITEM_Text:				m_pChild = new HyText2d("", HY_GUI_DATAOVERRIDE, pParent); break;
	case ITEM_Spine:			m_pChild = new HySpine2d("", HY_GUI_DATAOVERRIDE, pParent); break;
	case ITEM_Sprite:			m_pChild = new HySprite2d("", HY_GUI_DATAOVERRIDE, pParent); break;

	case ITEM_Shape:			m_pShape = new HyShape2d(pParent); break;

	case ITEM_AtlasImage:		//m_pChild = new HyTexturedQuad2d();
	case ITEM_Entity:
	default:
		HyLogError("EntityDraw::OnApplyJsonData - unhandled child node type");
		break;
	}

	if(m_pChild)
	{
		RefreshOverrideData();
		m_pChild->Load();
	}

	HideTransformCtrl();
}

/*virtual*/ EntityDrawItem::~EntityDrawItem()
{
	delete m_pChild;
	delete m_pShape;
}

HyGuiItemType EntityDrawItem::GetGuiType() const
{
	return m_eGuiType;
}

const QUuid &EntityDrawItem::GetThisUuid() const
{
	return m_Uuid;
}

const QUuid &EntityDrawItem::GetItemUuid() const
{
	return m_ItemUuid;
}

IHyLoadable2d *EntityDrawItem::GetNodeChild() const
{
	return m_pChild;
}

HyShape2d *EntityDrawItem::GetShape() const
{
	return m_pShape;
}

TransformCtrl &EntityDrawItem::GetTransformCtrl()
{
	return m_Transform;
}

bool EntityDrawItem::IsStale() const
{
	return m_bStale;
}

void EntityDrawItem::SetStale()
{
	m_bStale = true;
}

bool EntityDrawItem::IsMouseInBounds() const
{
	HyShape2d boundingShape;
	glm::mat4 transformMtx;
	ExtractTransform(boundingShape, transformMtx);
	
	glm::vec2 ptWorldMousePos;
	return HyEngine::Input().GetWorldMousePos(ptWorldMousePos) && boundingShape.TestPoint(transformMtx, ptWorldMousePos);
}

void EntityDrawItem::RefreshJson(HyCamera2d *pCamera, QJsonObject childObj)
{
	QJsonObject commonObj = childObj["Common"].toObject();
	if(commonObj.contains("Display Order"))
	{
		int iDisplayOrder = commonObj["Display Order"].toInt();
		if(iDisplayOrder != 0)
			static_cast<IHyBody2d *>(m_pChild)->SetDisplayOrder(iDisplayOrder);
	}
	m_pChild->SetPauseUpdate(commonObj["Update During Paused"].toBool());
	m_pChild->SetTag(commonObj["User Tag"].toVariant().toLongLong());
	m_pChild->SetVisible(commonObj["Visible"].toBool());

	QJsonObject transformObj = childObj["Transformation"].toObject();
	QJsonArray posArray = transformObj["Position"].toArray();
	m_pChild->pos.Set(glm::vec2(posArray[0].toDouble(), posArray[1].toDouble()));
	m_pChild->rot.Set(transformObj["Rotation"].toDouble());
	QJsonArray scaleArray = transformObj["Scale"].toArray();
	m_pChild->scale.Set(glm::vec2(scaleArray[0].toDouble(), scaleArray[1].toDouble()));

	//RefreshTransform(pCamera);

	m_bStale = false;
}

void EntityDrawItem::RefreshTransform(HyCamera2d *pCamera)
{
	HyShape2d boundingShape;
	glm::mat4 mtxShapeTransform;
	ExtractTransform(boundingShape, mtxShapeTransform);

	m_Transform.WrapTo(boundingShape, mtxShapeTransform, pCamera);
}

void EntityDrawItem::RefreshOverrideData()
{
	ProjectItemData *pItemData = MainWindow::GetExplorerModel().FindByUuid(m_ItemUuid);
	FileDataPair fileDataPair;
	pItemData->GetLatestFileData(fileDataPair);

	if(m_eGuiType == ITEM_Entity)
	{
		fileDataPair.m_Meta;
		HyGuiLog("Entity RefreshOverrideData() not implemented!", LOGTYPE_Error);
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
			static_cast<HyText2d *>(m_pChild)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject());
			break;

		case ITEM_Spine:
			static_cast<HySpine2d *>(m_pChild)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
			break;

		case ITEM_Sprite:
			static_cast<HySprite2d *>(m_pChild)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
			break;

		case ITEM_Primitive:
		case ITEM_Audio:
		case ITEM_AtlasImage:
		default:
			HyLogError("EntityDraw::ItemWidget::RefreshOverrideData - unhandled child node type");
			break;
		}
	}
}

void EntityDrawItem::ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut) const
{
	transformMtxOut = glm::identity<glm::mat4>();
	switch(GetGuiType())
	{
	case ITEM_Shape:
		boundingShapeOut = *GetShape();
		break;

	case ITEM_AtlasImage:
	case ITEM_Primitive:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Sprite: {
		IHyDrawable2d *pDrawable = static_cast<IHyDrawable2d *>(GetNodeChild());
		pDrawable->CalcLocalBoundingShape(boundingShapeOut);
		transformMtxOut = GetNodeChild()->GetSceneTransform(0.0f);
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
