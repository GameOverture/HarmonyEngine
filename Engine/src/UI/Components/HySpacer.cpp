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
#include "UI/Components/HySpacer.h"

HySpacer::HySpacer(HyOrientation eOrienType) :
	IHyWidget(nullptr),
	m_eORIEN_TYPE(eOrienType),
	m_iSizeHint(0),
	m_iActualSize(0)
{
	m_eLoadState = HYLOADSTATE_Loaded;
}

/*virtual*/ HySpacer::~HySpacer()
{
}

/*virtual*/ float HySpacer::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	return (m_eORIEN_TYPE == HYORIENT_Horizontal) ? m_iActualSize * fPercent : 0;
}

/*virtual*/ float HySpacer::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return (m_eORIEN_TYPE == HYORIENT_Vertical) ? m_iActualSize * fPercent : 0;
}

int32 HySpacer::GetActualSize() const
{
	return m_iActualSize;
}

void HySpacer::Setup(HySizePolicy eSizePolicy, uint32 uiSizeHint)
{
	HyAssert(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0, "HySpacer::Setup() invoked and not attached to a layout");

	m_iSizeHint = uiSizeHint;
	if(m_eORIEN_TYPE == HYORIENT_Horizontal)
		SetSizePolicy(eSizePolicy, HYSIZEPOLICY_Fixed);
	else
		SetSizePolicy(HYSIZEPOLICY_Fixed, eSizePolicy);
}

void HySpacer::SetSize(uint32 uiSizeHint)
{
	m_iSizeHint = uiSizeHint;
	SetSizeAndLayoutDirty();
}

/*virtual*/ glm::vec2 HySpacer::GetBotLeftOffset() /*override*/
{
	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ glm::ivec2 HySpacer::OnCalcPreferredSize() /*override*/
{
	glm::ivec2 vSizeHint;
	if(m_eORIEN_TYPE == HYORIENT_Horizontal)
		HySetVec(m_vSizeHint, m_iSizeHint, 0);
	else
		HySetVec(m_vSizeHint, 0, m_iSizeHint);
}

/*virtual*/ glm::ivec2 HySpacer::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	// NOTE: Values incoming to this OnResize() are already clamped to Min/Max sizes and respect size policies
	m_iActualSize = (m_eORIEN_TYPE == HYORIENT_Horizontal) ? uiNewWidth : uiNewHeight;
	return glm::ivec2(uiNewWidth, uiNewHeight);
}
