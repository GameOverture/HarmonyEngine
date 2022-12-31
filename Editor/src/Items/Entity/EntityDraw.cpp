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

EntityDraw::ChildWidget::ChildWidget(HyGuiItemType eType, QUuid uuid, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_eGuiType(eType),
	m_ItemUuid(uuid),
	m_Transform(this),
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
}

/*virtual*/ EntityDraw::ChildWidget::~ChildWidget()
{
	delete m_pChild;
}

void EntityDraw::ChildWidget::RefreshJson(QJsonObject childObj)
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

	m_bStale = false;
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
		case ITEM_Primitive:
			break;

		case ITEM_Audio:
			break;

		case ITEM_Text:
			static_cast<HyText2d *>(m_pChild)->GuiOverrideData<HyTextData>(itemDataDoc.GetObject());
			break;

		case ITEM_Spine:
			static_cast<HySpine2d *>(m_pChild)->GuiOverrideData<HySpineData>(itemDataDoc.GetObject());
			break;

		case ITEM_Sprite:
			static_cast<HySprite2d *>(m_pChild)->GuiOverrideData<HySpriteData>(itemDataDoc.GetObject());
			break;

		case ITEM_AtlasImage:	//m_pChild = new HyTexturedQuad2d();
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
}

//void EntityDraw::Sync()
//{
//	const QList<ProjectItemData *> &primitiveListRef = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetPrimitiveList();
//	for(uint32 i = 0; i < static_cast<uint32>(primitiveListRef.size()); ++i)
//	{
//
//	}
//}

/*virtual*/ void EntityDraw::OnApplyJsonMeta(QJsonObject &itemMetaObj) /*override*/
{
	for(ChildWidget *pChild : m_ChildWidgetList)
		pChild->SetStale();

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

		pChildWidget->RefreshJson(childObj);
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
}

/*virtual*/ void EntityDraw::OnZoom(HyZoomLevel eZoomLevel) /*override*/
{
	
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
			iter = m_ChildWidgetList.erase(iter);
		else
			++iter;
	}

	//m_ChildWidgetList.erase(std::remove_if(m_ChildWidgetList.begin(), m_ChildWidgetList.end(),
	//	[](const ChildWidget *pChild)
	//	{
	//		return pChild->IsStale();
	//	}), m_ChildWidgetList.end());
}
