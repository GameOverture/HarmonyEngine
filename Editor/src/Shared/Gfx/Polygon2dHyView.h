/**************************************************************************
 *	Polygon2dHyView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef POLYGON2DHYVIEW_H
#define POLYGON2DHYVIEW_H

#include "Global.h"
#include "IPolygon2dView.h"

class GrabPoint;

class Polygon2dHyView : public IPolygon2dView, public HyEntity2d
{
	HyPrimitive2d				m_Outline;				// This shape represents the raw data in the form of an outline unaffected by camera zoom. Uses window coordinates
	QList<HyPrimitive2d *>		m_PrimList;				// This shape represents how the item's transformation is applied. Uses world/camera coordinates. Usually just one primitive, but could be multiple for complex polygons
	QList<GrabPoint *>			m_VertexGrabPointList;	// Uses window coordinates

public:
	Polygon2dHyView(HyEntity2d *pParent = nullptr);
	virtual ~Polygon2dHyView();

	HyPrimitive *GetPrimitive(int iIndex);

	virtual void RefreshColor() override;
	virtual void RefreshView(bool bTransformPreview) override;

	virtual void OnHoverClear() override;

protected:
	void RefreshOutline();
	void SetVertexGrabPointListSize(uint32 uiNumGrabPoints);

	void ClearPrimitives();
};

#endif // POLYGON2DHYVIEW_H
