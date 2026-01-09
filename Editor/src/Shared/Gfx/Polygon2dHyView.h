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

class Polygon2dHyView : public IPolygon2dView
{
	HyPrimitive2d				m_Fill;					// This shape represents how the item's transformation is applied. Uses world/camera coordinates
	HyPrimitive2d				m_Outline;				// This shape represents the raw data in the form of an outline unaffected by camera zoom. Uses window coordinates
	QList<GrabPoint *>			m_VertexGrabPointList;	// Uses window coordinates

public:
	Polygon2dHyView();
	virtual ~Polygon2dHyView();

	HyPrimitive &GetFillPrimitive();

	virtual void RefreshColor() override;
	virtual void RefreshView() override;

protected:
	void SetVertexGrabPointListSize(uint32 uiNumGrabPoints);
};

#endif // POLYGON2DHYVIEW_H
