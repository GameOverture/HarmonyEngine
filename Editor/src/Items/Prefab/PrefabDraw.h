/**************************************************************************
*	PrefabDraw.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2018 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#ifndef PREFABDRAW_H
#define PREFABDRAW_H

#include "IDraw.h"
#include "PrefabModel.h"

class PrefabDraw : public IDraw
{
public:
	PrefabDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);

	virtual void OnResizeRenderer() override;
};

#endif // PREFABDRAW_H
