/**************************************************************************
*	HyProfiler.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyProfiler_h__
#define HyProfiler_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyPrimitive2d.h"

#include <stack>

enum HyProfilerSection
{
	HYPROFILERSECTION_None = -1,
	HYPROFILERSECTION_Physics = 0,
	HYPROFILERSECTION_Nodes,
	HYPROFILERSECTION_Update,
	HYPROFILERSECTION_PrepRender,
	HYPROFILERSECTION_Render,
	HYPROFILERSECTION_User1,
	HYPROFILERSECTION_User2,
	HYPROFILERSECTION_User3,

	HYNUM_PROFILERSECTION
};

class HyProfiler : public HyEntity2d
{
	uint64							m_uiSectionTicks[HYNUM_PROFILERSECTION];
	HyProfilerSection				m_eCurrentSection;

	std::stack<HyProfilerSection>	m_SectionStack;
	uint64							m_uiTotalTicks;

public:
	HyProfiler();
	virtual ~HyProfiler();

	void NewFrame();

	void BeginSection(HyProfilerSection eSection);
	void EndSection();
};

#endif /* HyProfiler_h__ */
