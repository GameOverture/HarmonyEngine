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

const HyScreenRect<int32> &HyScissor::Get()
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
}

void HyScissor::Clear()
{
	m_LocalScissorRect.iTag = 0;
	m_RenderState.ClearScissorRect();

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
			static_cast<IHyDraw2d *>(m_ChildList[i])->m_RenderState.ClearScissorRect();
	}
}
