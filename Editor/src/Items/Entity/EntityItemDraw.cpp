/**************************************************************************
*	EntityItemDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityItemDraw.h"
#include "MainWindow.h"

EntityItemDraw::EntityItemDraw(HyGuiItemType eGuiType, QUuid uuid, HyEntity2d *pParent) :
	m_eGuiType(eGuiType),
	m_ItemUuid(uuid),
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
}

/*virtual*/ EntityItemDraw::~EntityItemDraw()
{
	delete m_pChild;
	delete m_pShape;
}

void EntityItemDraw::RefreshJson(HyCamera2d *pCamera, QJsonObject childObj)
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

	//RefreshTransformCtrl(pCamera);

	m_bStale = false;
}

void EntityItemDraw::RefreshOverrideData()
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
