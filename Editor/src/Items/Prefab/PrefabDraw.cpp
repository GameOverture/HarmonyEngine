/**************************************************************************
*	PrefabDraw.cpp
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2018 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#include "Global.h"
#include "PrefabDraw.h"

PrefabDraw::PrefabDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
}

/*virtual*/ void PrefabDraw::OnResizeRenderer() /*override*/
{
}
