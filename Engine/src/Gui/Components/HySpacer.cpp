/**************************************************************************
*	HySpacer.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Components/HySpacer.h"

HySpacer::HySpacer(HyOrientation eOrienType) :
	IHyWidget(nullptr),
	m_eORIEN_TYPE(eOrienType),
	m_uiSizeHint(0)
{
	m_eLoadState = HYLOADSTATE_Loaded;
}

/*virtual*/ HySpacer::~HySpacer()
{
}

int32 HySpacer::GetActualSize()
{
	return static_cast<int32>((m_eORIEN_TYPE == HYORIENT_Horizontal) ? GetWidth() : GetHeight());
}

void HySpacer::Setup(HySizePolicy eSizePolicy, uint32 uiSizeHint)
{
	HyAssert(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0, "HySpacer::Setup() invoked and not attached to a layout");

	m_uiSizeHint = uiSizeHint;
	if(m_eORIEN_TYPE == HYORIENT_Horizontal)
		SetSizePolicy(eSizePolicy, HYSIZEPOLICY_Fixed);
	else
		SetSizePolicy(HYSIZEPOLICY_Fixed, eSizePolicy);
}

void HySpacer::SetSize(uint32 uiSizeHint)
{
	m_uiSizeHint = uiSizeHint;
	if(m_eORIEN_TYPE == HYORIENT_Horizontal)
		Resize(m_uiSizeHint, 1);
	else
		Resize(1, m_uiSizeHint);
}

/*virtual*/ glm::ivec2 HySpacer::OnCalcPreferredSize() /*override*/
{
	if(m_eORIEN_TYPE == HYORIENT_Horizontal)
		return glm::ivec2(m_uiSizeHint, 1);
	else
		return glm::ivec2(1, m_uiSizeHint);
}

/*virtual*/ glm::ivec2 HySpacer::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	return glm::ivec2(uiNewWidth, uiNewHeight);
}
