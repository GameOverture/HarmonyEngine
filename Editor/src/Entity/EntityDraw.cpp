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

EntityDraw::EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef),
	m_primOriginHorz(this),
	m_primOriginVert(this)
{
	std::vector<glm::vec2> lineList(2, glm::vec2());

	lineList[0].x = -5000.0f;
	lineList[0].y = 0.0f;
	lineList[1].x = 5000.0f;
	lineList[1].y = 0.0f;
	m_primOriginHorz.SetLineThickness(2.0f);
	m_primOriginHorz.SetTint(1.0f, 1.0f, 1.0f);
	m_primOriginHorz.SetVisible(false);
	m_primOriginHorz.SetAsLineChain(&lineList[0], static_cast<uint32>(lineList.size()));

	lineList[0].x = 0.0f;
	lineList[0].y = -5000.0f;
	lineList[1].x = 0.0f;
	lineList[1].y = 5000.0f;
	m_primOriginVert.SetLineThickness(2.0f);
	m_primOriginVert.SetTint(1.0f, 1.0f, 1.0f);
	m_primOriginVert.SetVisible(false);
	m_primOriginVert.SetAsLineChain(&lineList[0], static_cast<uint32>(lineList.size()));
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

/*virtual*/ void EntityDraw::OnApplyJsonData(HyJsonObj itemDataObj) /*override*/
{
}

/*virtual*/ void EntityDraw::OnShow() /*override*/
{
	m_primOriginHorz.SetVisible(true);
	m_primOriginVert.SetVisible(true);
}

/*virtual*/ void EntityDraw::OnHide() /*override*/
{
	SetVisible(false, true);
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
}
