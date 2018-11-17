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
	const glm::vec2			m_vDIMENSIONS;
	float					m_fElapsedTime;

	enum PanelState
	{
		PANELSTATE_Hidden = 0,
		PANELSTATE_Showing,
		PANELSTATE_Shown,
		PANELSTATE_Hiding
	};
	PanelState				m_ePanelState;

protected:
	HyPrimitive2d			m_PanelFill;
	HyPrimitive2d			m_PanelFrameOutline;
	HyPrimitive2d			m_PanelFrame;

public:
	IHy9Slice(glm::vec2 vDimensions, HyEntity2d *pParent);
	virtual ~IHy9Slice();

	float GetWidth();
	float GetHeight();

	void Show();
	void Hide();

	bool IsTransition();
	bool IsShown();

	virtual float OnShow() = 0;	// Returns the duration (in seconds) of the shown transition
	virtual void OnShown() = 0;
	virtual float OnHide() = 0;	// Returns the duration (in seconds) of the hide transition
	virtual void OnHidden() = 0;

protected:
	virtual void OnUpdate() override final;
};

#endif /* IHy9Slice_h__ */
