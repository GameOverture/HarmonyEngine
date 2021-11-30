/**************************************************************************
*	HySpacer.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HySpacer.h"
#include "UI/Containers/Components/HyLayout.h"

HySpacer::HySpacer(HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent)
{
	SetSizePolicy(HYSIZEPOLICY_Expanding, HYSIZEPOLICY_Expanding);
}

/*virtual*/ HySpacer::~HySpacer()
{
}

/*virtual*/ void HySpacer::SetMinSize(uint32 uiMinSizeX, uint32 uiMinSizeY) /*override*/
{
	HySetVec(m_vMinSize, uiMinSizeX, uiMinSizeY);

	m_bSizeHintDirty = true;
	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
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
	return glm::ivec2(uiNewWidth, uiNewHeight);
}
