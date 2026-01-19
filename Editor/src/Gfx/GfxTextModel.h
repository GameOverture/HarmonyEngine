/**************************************************************************
*	GfxTextModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GfxTextModel_H
#define GfxTextModel_H

#include "Global.h"

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
