/**************************************************************************
*	EntityDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDraw.h"
#include "MainWindow.h"

EntityDraw::ChildWidget::ChildWidget(HyGuiItemType eGuiType, QUuid uuid, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_eGuiType(eGuiType),
	m_ItemUuid(uuid),
	m_pChild(nullptr),
	m_bStale(false)
{
	switch(m_eGuiType)
	{
	case ITEM_Primitive:		m_pChild = new HyPrimitive2d(this); break;
	case ITEM_Audio:			m_pChild = new HyAudio2d("", HY_GUI_DATAOVERRIDE, this); break;
	case ITEM_Text:				m_pChild = new HyText2d("", HY_GUI_DATAOVERRIDE, this); break;
	case ITEM_Spine:			m_pChild = new HySpine2d("", HY_GUI_DATAOVERRIDE, this); break;
	case ITEM_Sprite:			m_pChild = new HySprite2d("", HY_GUI_DATAOVERRIDE, this); break;
	case ITEM_AtlasImage:	//m_pChild = new HyTexturedQuad2d();
	case ITEM_Entity:
	default:
		HyLogError("EntityDraw::OnApplyJsonData - unhandled child node type");
		break;
	}

	RefreshOverrideData();
	m_Transform.SetDisplayOrder(9999999);
}

/*virtual*/ EntityDraw::ChildWidget::~ChildWidget()
{
	delete m_pChild;
}

void EntityDraw::ChildWidget::RefreshJson(HyCamera2d *pCamera, QJsonObject childObj)
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
	pos.Set(glm::vec2(posArray[0].toDouble(), posArray[1].toDouble()));
	rot.Set(transformObj["Rotation"].toDouble());
	QJsonArray scaleArray = transformObj["Scale"].toArray();
	scale.Set(glm::vec2(scaleArray[0].toDouble(), scaleArray[1].toDouble()));

	RefreshTransformCtrl(pCamera);

	m_bStale = false;
}

void EntityDraw::ChildWidget::RefreshTransformCtrl(HyCamera2d *pCamera)
{
	m_Transform.WrapTo(m_eGuiType, m_pChild, pCamera);
}

void EntityDraw::ChildWidget::RefreshOverrideData()
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
			HyGuiLog("EntityDraw::ChildWidget::RefreshOverrideData failed to parse its file data", LOGTYPE_Error);

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
			HyLogError("EntityDraw::ChildWidget::RefreshOverrideData - unhandled child node type");
			break;
		}
	}
}

EntityDraw::EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
}

/*virtual*/ EntityDraw::~EntityDraw()
{
	SetEverythingStale();
	DeleteStaleChildren();
}

/*virtual*/ void EntityDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyPressEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMousePressEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseReleaseEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
	IDraw::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseMoveEvent(pEvent);

	if(m_bPanCameraKeyDown)
		RefreshTransforms();
}

/*virtual*/ void EntityDraw::OnApplyJsonMeta(QJsonObject &itemMetaObj) /*override*/
{
	SetEverythingStale();

	QJsonArray childArray = itemMetaObj["childList"].toArray();
	for(uint32 i = 0; i < childArray.size(); ++i)
	{
		QJsonObject childObj = childArray[i].toObject();

		HyGuiItemType eType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());
		QUuid uuid(childObj["itemUUID"].toString());
		ChildWidget *pChildWidget = FindStaleChild(eType, uuid);
		if(pChildWidget == nullptr)
		{
			pChildWidget = new ChildWidget(eType, uuid, this);
			pChildWidget->Load();
			m_ChildWidgetList.push_back(pChildWidget);
		}

		pChildWidget->RefreshJson(m_pCamera, childObj);
	}
	DeleteStaleChildren();

	QJsonArray shapeArray = itemMetaObj["shapeList"].toArray();
	for(uint32 i = 0; i < shapeArray.size(); ++i)
	{
		QJsonObject shapeObj = shapeArray[i].toObject();
	}

	
}

/*virtual*/ void EntityDraw::OnShow() /*override*/
{
	SetVisible(true);
}

/*virtual*/ void EntityDraw::OnHide() /*override*/
{
	SetVisible(false);
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
	RefreshTransforms();
}

/*virtual*/ void EntityDraw::OnZoom(HyZoomLevel eZoomLevel) /*override*/
{
	RefreshTransforms();
}

void EntityDraw::SetEverythingStale()
{
	for(ChildWidget *pChild : m_ChildWidgetList)
		pChild->SetStale();
}

EntityDraw::ChildWidget *EntityDraw::FindStaleChild(HyGuiItemType eType, QUuid uuid)
{
	for(ChildWidget *pChild : m_ChildWidgetList)
	{
		if(pChild->GetGuiType() == eType && pChild->GetUuid() == uuid && pChild->IsStale())
			return pChild;
	}

	return nullptr;
}

void EntityDraw::DeleteStaleChildren()
{
	for(auto iter = m_ChildWidgetList.begin(); iter != m_ChildWidgetList.end(); )
	{
		if((*iter)->IsStale())
		{
			delete (*iter);
			iter = m_ChildWidgetList.erase(iter);
		}
		else
			++iter;
	}
}

void EntityDraw::RefreshTransforms()
{
	for(ChildWidget *pChild : m_ChildWidgetList)
		pChild->RefreshTransformCtrl(m_pCamera);
}

