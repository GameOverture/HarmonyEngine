/**************************************************************************
 *	GrabPoint.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GrabPoint.h"

GrabPoint::GrabPoint(HyColor outlineColor, HyColor fillColor, HyColor selectedOutlineColor, HyColor selectedFillColor, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_GrabOutline(this),
	m_GrabFill(this),
	m_OutlineColor(outlineColor),
	m_FillColor(fillColor),
	m_SelectedOutlineColor(selectedOutlineColor),
	m_SelectedFillColor(selectedFillColor),
	m_bIsSelected(false)
{
	const float fRADIUS = 5.0f;

	m_GrabOutline.SetAsCircle(fRADIUS);
	m_GrabOutline.SetTint(m_OutlineColor);

	m_GrabFill.SetAsCircle(fRADIUS - 1.0f);
	m_GrabFill.SetTint(m_FillColor);

	UseWindowCoordinates(0);
}

/*virtual*/ GrabPoint::~GrabPoint()
{
}

void GrabPoint::GetLocalBoundingShape(HyShape2d &shapeRefOut)
{
	m_GrabOutline.CalcLocalBoundingShape(shapeRefOut);
}

bool GrabPoint::IsSelected() const
{
	return m_bIsSelected;
}

void GrabPoint::SetSelected(bool bSelected)
{
	m_bIsSelected = bSelected;
	if(m_bIsSelected)
	{
		m_GrabOutline.SetTint(m_SelectedOutlineColor);
		m_GrabFill.SetTint(m_SelectedFillColor);
	}
	else
	{
		m_GrabOutline.SetTint(m_OutlineColor);
		m_GrabFill.SetTint(m_FillColor);
	}
}

HyColor GrabPoint::GetOutlineColor()
{
	return HyColor(m_GrabOutline.topColor.X(), m_GrabOutline.topColor.Y(), m_GrabOutline.topColor.Z());
}

HyColor GrabPoint::GetFillColor()
{
	return HyColor(m_GrabFill.topColor.X(), m_GrabFill.topColor.Y(), m_GrabFill.topColor.Z());
}
