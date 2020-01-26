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

EntityDraw::EntityDraw(ProjectItem *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
}

/*virtual*/ EntityDraw::~EntityDraw()
{
}

void EntityDraw::Sync()
{
	const QList<ProjectItem *> &primitiveListRef = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetPrimitiveList();
	for(uint32 i = 0; i < static_cast<uint32>(primitiveListRef.size()); ++i)
	{

	}
}

/*virtual*/ void EntityDraw::OnApplyJsonData(jsonxx::Object &itemDataObjRef) /*override*/
{
}

/*virtual*/ void EntityDraw::OnShow() /*override*/
{
}

/*virtual*/ void EntityDraw::OnHide() /*override*/
{
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
}
