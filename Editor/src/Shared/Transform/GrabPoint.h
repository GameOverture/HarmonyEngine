/**************************************************************************
 *	GrabPoint.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GRABPOINT_H
#define GRABPOINT_H

#include "Global.h"

class GrabPoint : public HyEntity2d
{
	HyPrimitive2d				m_GrabOutline;
	HyPrimitive2d				m_GrabFill;

	HyColor						m_OutlineColor;
	HyColor						m_FillColor;
	HyColor						m_SelectedOutlineColor;
	HyColor						m_SelectedFillColor;

	bool						m_bIsSelected;

public:
	GrabPoint(HyColor outlineColor, HyColor fillColor, HyColor selectedOutlineColor, HyColor selectedFillColor, HyEntity2d *pParent);
	virtual ~GrabPoint();

	void GetLocalBoundingShape(HyShape2d &shapeRefOut);

	bool IsSelected() const;
	void SetSelected(bool bSelected);
	HyColor GetOutlineColor();
	HyColor GetFillColor();
};

#endif // GRABPOINT_H
