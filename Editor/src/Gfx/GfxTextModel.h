/**************************************************************************
*	GfxTextModel.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2026 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#ifndef GfxTextModel_H
#define GfxTextModel_H

#include "Global.h"
#include "HyEngine.h"

class GfxTextModel
{
	HyColor								m_Color;
	HyShape2d							m_BoundingVolume;

public:
	GfxTextModel(HyColor color);
	virtual ~GfxTextModel();

	void Refresh(HyText2d *pTextNode, bool bShowOutline);
};

#endif // GfxTextModel_H
