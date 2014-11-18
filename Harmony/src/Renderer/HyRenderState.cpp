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

HyRenderState::HyRenderState() :	m_uiAttributeFlags(0),
									m_uiNumInstances(0),
									m_uiDataOffset(0)
{
	memset(m_pTextureBinds, 0, sizeof(m_pTextureBinds[0])*HY_MAX_TEXTURE_BINDS);
}

HyRenderState::~HyRenderState(void)
{
}

void HyRenderState::SetDataOffset(uint32 uiVertexDataOffset)
{
	m_uiDataOffset = uiVertexDataOffset;
}

uint32 HyRenderState::GetDataOffset()
{
	return m_uiDataOffset;
}

void HyRenderState::AppendInstances(uint32 uiNumInstsToAppend)
{
	m_uiNumInstances += uiNumInstsToAppend;
}

uint32 HyRenderState::GetNumInstances()
{
	return m_uiNumInstances;
}

void HyRenderState::Enable(uint32 uiAttributes)
{
	m_uiAttributeFlags |= uiAttributes;
}

void HyRenderState::Disable(uint32 uiAttributes)
{
	m_uiAttributeFlags &= ~uiAttributes;
}

bool HyRenderState::CompareAttribute(const HyRenderState &rs, uint32 uiMask)
{
	return (m_uiAttributeFlags & uiMask) == (rs.m_uiAttributeFlags & uiMask);
}

bool HyRenderState::IsEnabled(eAttributes eAttrib)
{
	return 0 != (m_uiAttributeFlags & eAttrib);
}

uint32 HyRenderState::GetTextureHandle(uint32 uiTextureIndex)
{
	return m_pTextureBinds[uiTextureIndex];
}

void HyRenderState::SetTextureHandle(uint32 uiIndex, uint32 uiHandleId)
{
	m_pTextureBinds[uiIndex] = uiHandleId;
}

bool HyRenderState::operator==(const HyRenderState &right) const
{
	return this->m_uiAttributeFlags == right.m_uiAttributeFlags &&
		   0 == memcmp(this->m_pTextureBinds, right.m_pTextureBinds, sizeof(m_pTextureBinds[0])*HY_MAX_TEXTURE_BINDS);
}
