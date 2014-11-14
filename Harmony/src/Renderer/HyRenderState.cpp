/**************************************************************************
 *	HyRenderState.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/HyRenderState.h"
#include "Creator/Instances/IObjInst2d.h"

HyRenderState::HyRenderState(IObjInst2d *pInst, uint32 uiVertexDataOffset) : m_uiAttributeFlags(0)
{
	memset(m_pTextureBinds, 0, sizeof(m_pTextureBinds[0])*HY_MAX_TEXTURE_BINDS);
}

HyRenderState::~HyRenderState(void)
{
}

void Set(IObjInst2d *pInst, uint32 uiVertexDataOffset)
{
}

void HyRenderState::Enable(uint32 uiAttributes)
{
	m_uiAttributeFlags |= uiAttributes;
}

void HyRenderState::Disable(uint32 uiAttributes)
{
	m_uiAttributeFlags &= ~uiAttributes;
}

bool HyRenderState::operator==(const HyRenderState &right) const
{
	return this->m_uiAttributeFlags == right.m_uiAttributeFlags &&
		   0 == memcmp(this->m_pTextureBinds, right.m_pTextureBinds, sizeof(m_pTextureBinds[0])*HY_MAX_TEXTURE_BINDS);
}
