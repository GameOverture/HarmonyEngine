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

/*virtual*/ void EntityDraw::OnApplyJsonData(HyJsonDoc &itemDataDocRef) /*override*/
{
}

/*virtual*/ void EntityDraw::OnShow() /*override*/
{
}

/*virtual*/ void EntityDraw::OnHide() /*override*/
{
	SetVisible(false, true);
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
}

/*virtual*/ void EntityDraw::OnZoom(HyZoomLevel eZoomLevel) /*override*/
{
	
}
