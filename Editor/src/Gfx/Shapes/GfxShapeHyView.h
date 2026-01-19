/**************************************************************************
 *	GfxShapeHyView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GfxShapeHyView_H
#define GfxShapeHyView_H

#include "Global.h"
#include "IGfxShapeView.h"

class GfxGrabPointView;

class GfxShapeHyView : public IGfxShapeView, public HyEntity2d
{
	HyPrimitive2d				m_PrimOutline;			// This primitive is set as line chain. It forms an outline unaffected by camera zoom, using window coordinates
	QList<HyPrimitive2d *>		m_PrimList;				// This shape represents how the item's transformation is applied. Uses world/camera coordinates. Usually just one primitive, but could be multiple for complex polygons
	QList<GfxGrabPointView *>	m_GrabPointViewList;	// Project to window coordinates

	HyPrimitive2d				m_AppendSegmentLine;	// A red line to the crosshair cursor from the last appended vertex

public:
	GfxShapeHyView(HyEntity2d *pParent = nullptr);
	virtual ~GfxShapeHyView();

	HyPrimitive *GetPrimitive(int iIndex);

	virtual void RefreshColor() override;
	virtual void RefreshView(bool bTransformPreview) override;

	virtual void OnMouseMoveIdle(ShapeMouseMoveResult eResult) override;

protected:
	void ClearPrimitives();
	void ClearGrabPoints();
};

#endif // GfxShapeHyView_H
