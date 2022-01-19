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
	m_eORIEN_TYPE(eOrienType)
{
	m_eLoadState = HYLOADSTATE_Loaded;
}

/*virtual*/ HySpacer::~HySpacer()
{
}

void HySpacer::Setup(HySizePolicy eSizePolicy, uint32 uiSize)
{
	HyAssert(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0, "HySpacer::Setup() invoked and not attached to a layout");

	if(m_eORIEN_TYPE == HYORIEN_Horizontal)
	{
		SetMinSize(uiSize, 0);
		SetSizePolicy(eSizePolicy, eSizePolicy);
	}
	else
	{
		SetMinSize(0, uiSize);
		SetSizePolicy(eSizePolicy, eSizePolicy);
	}
}

/*virtual*/ glm::vec2 HySpacer::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ void HySpacer::OnSetSizeHint() /*override*/
{
	m_vSizeHint = m_vMinSize;
}

/*virtual*/ glm::ivec2 HySpacer::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	// NOTE: Values incoming to this OnResize() are already clamped to Min/Max sizes and respect size policies
	return glm::ivec2(uiNewWidth, uiNewHeight);
}
