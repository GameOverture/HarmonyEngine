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
#include "Scene/Instances/IHyInst2d.h"

HyRenderState::HyRenderState() :	m_uiAttributeFlags(0),
									m_uiNumInstances(0),
									m_uiDataOffset(0)
{
	memset(m_pTextureBinds, 0, sizeof(m_pTextureBinds[0])*HY_MAX_TEXTURE_BINDS);
}

HyRenderState::~HyRenderState(void)
{
}

void HyRenderState::SetDataOffset(size_t uiVertexDataOffset)
{
	m_uiDataOffset = uiVertexDataOffset;
}

size_t HyRenderState::GetDataOffset() const
{
	return m_uiDataOffset;
}

void HyRenderState::AppendInstances(uint32 uiNumInstsToAppend)
{
	m_uiNumInstances += uiNumInstsToAppend;
}

uint32 HyRenderState::GetNumInstances() const
{
	return m_uiNumInstances;
}

void HyRenderState::SetNumInstances(uint32 uiNumInsts)
{
	m_uiNumInstances = uiNumInsts;
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

uint32 HyRenderState::GetAttributeBitFlags() const
{
	return m_uiAttributeFlags;
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

bool HyRenderState::operator!=(const HyRenderState &right) const
{
	return !(*this == right);
}

bool HyRenderState::operator< (const HyRenderState &right) const
{
	if(m_uiAttributeFlags == right.m_uiAttributeFlags)
		return (memcmp(this->m_pTextureBinds, right.m_pTextureBinds, sizeof(m_pTextureBinds[0])*HY_MAX_TEXTURE_BINDS) < 0);

	return m_uiAttributeFlags < right.m_uiAttributeFlags;
}

bool HyRenderState::operator> (const HyRenderState &right) const
{
	return *this < right;
}

bool HyRenderState::operator<=(const HyRenderState &right) const
{
	return !(*this > right);
}

bool HyRenderState::operator>=(const HyRenderState &right) const
{
	return !(*this < right);
}
