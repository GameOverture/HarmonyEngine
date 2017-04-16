/**************************************************************************
*	HyScissor.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Components/HyScissor.h"

HyScissor::HyScissor(IHyNode2d &ownerRef) :	m_OwnerRef(ownerRef)
{
}

HyScissor::~HyScissor()
{
}

HyScissor &HyScissor::operator =(HyScissor &rightSideRef)
{
	m_LocalScissorRect = rightSideRef.m_LocalScissorRect;
}

bool HyScissor::IsEnabled()
{
	return m_LocalScissorRect.iTag == 1;
}

const HyScreenRect<int32> &HyScissor::GetLocal()
{
	return m_LocalScissorRect;
}

void HyScissor::Set(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight)
{
	m_LocalScissorRect.x = uiLocalX;
	m_LocalScissorRect.y = uiLocalY;
	m_LocalScissorRect.width = uiWidth;
	m_LocalScissorRect.height = uiHeight;
	
	m_LocalScissorRect.iTag = 1;
	m_OwnerRef.SetDirty(HYNODEDIRTY_Scissor);
}

void HyScissor::Clear()
{
	m_LocalScissorRect.iTag = 0;
	m_OwnerRef.SetDirty(HYNODEDIRTY_Scissor);
}

const HyScreenRect<int32> &HyScissor::GetWorld()
{
	if(m_OwnerRef.IsDirty(HYNODEDIRTY_Color))
	{
		if(m_LocalScissorRect.iTag == 0)
		{
		if(m_OwnerRef.m_pParent)
		{
			m_fCachedAlpha *= m_OwnerRef.m_pParent->color.CalculateAlpha();
			m_CachedTopColor *= m_OwnerRef.m_pParent->color.CalculateTopTint();
			m_CachedBotColor *= m_OwnerRef.m_pParent->color.CalculateTopTint();
		}

		m_OwnerRef.ClearDirty(HYNODEDIRTY_Color);
	}
}
