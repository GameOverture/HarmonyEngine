/**************************************************************************
*	IHy9Slice.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHy9Slice_h__
#define IHy9Slice_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyPrimitive2d.h"

class IHy9Slice : public HyEntity2d
{
	glm::vec2				m_vFillDimensions;
	float					m_fBorderThickness;
	float					m_fElapsedTime;

	enum SliceState
	{
		PANELSTATE_Hidden = 0,
		PANELSTATE_Showing,
		PANELSTATE_Shown,
		PANELSTATE_Hiding
	};
	SliceState				m_ePanelState;

protected:
	class Border : public HyEntity2d
	{
		enum SliceCorner
		{	
			LowerLeft = 0,
			UpperLeft,
			UpperRight,
			LowerRight,
			NumCorners
		};

		enum SliceHorz
		{
			Upper = 0,
			Lower,
			NumHorz
		};

		enum SliceVert
		{
			Left = 0,
			Right,
			NumVert
		};

		HyPrimitive2d			m_Corners[NumCorners];
		HyPrimitive2d			m_Horz[NumHorz];
		HyPrimitive2d			m_Vert[NumVert];

		HyStencil				m_StencilForCorner;
		HyStencil				m_StencilForEdges;

	public:
		Border(glm::vec2 vFillDimensions, float fBorderThickness, HyPrimitive2d &fillRef, HyEntity2d *pParent);
	};
	HyPrimitive2d			m_Fill;
	Border					m_Border;

public:
	IHy9Slice(glm::vec2 vFillDimensions, float fBorderThickness, HyEntity2d *pParent);
	virtual ~IHy9Slice();

	float GetWidth(bool bIncludeBorders);
	float GetHeight(bool bIncludeBorders);
	float GetBorderThickness();

	HyEntity2d &GetBorder();
	HyPrimitive2d &GetFill();

	bool Show();
	bool Hide();

	bool IsTransition();
	bool IsShown();

	virtual float OnShow() = 0;	// Returns the duration (in seconds) of the shown transition
	virtual void OnShown() = 0;
	virtual float OnHide() = 0;	// Returns the duration (in seconds) of the hide transition
	virtual void OnHidden() = 0;

protected:
	virtual void OnUpdate() override;
};

#endif /* IHy9Slice_h__ */
