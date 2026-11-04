/**************************************************************************
 *	GfxGrabPointView.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2026 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef GfxGrabPointView_H
#define GfxGrabPointView_H

#include "Global.h"
#include "HyEngine.h"

class GfxGrabPointModel;

class GfxGrabPointView : public HyEntity2d
{
	HyPrimitive2d				m_GrabPt;

public:
	GfxGrabPointView(HyEntity2d *pParent);
	virtual ~GfxGrabPointView();

	void GetLocalBoundingShape(HyShape2d &shapeRefOut);

	void Sync(const GfxGrabPointModel *pModel);
};

#endif // GfxGrabPointView_H
