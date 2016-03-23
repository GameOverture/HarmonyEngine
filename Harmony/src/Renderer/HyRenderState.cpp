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

#include "Renderer/IHyShader.h"

HyRenderState::HyRenderState() :	m_uiAttributeFlags(0),
									m_uiNumInstances(0),
									m_uiDataOffset(0),
									m_uiTextureBindHandle(0),
									m_iShaderIndex(-1),
									m_pShaderUniformsRef(NULL)
{
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

// This function is responsible for incrementing the passed in reference pointer the size of the data written
void HyRenderState::WriteRenderStateInfoBufferData(char *&pRefDataWritePos)
{
	// TODO: Write texture bind ID's here, then append below

	m_pShaderUniformsRef->WriteUniformsBufferData(pRefDataWritePos);
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

int32 HyRenderState::GetShaderIndex()
{
	return m_iShaderIndex;
}

void HyRenderState::SetShaderIndex(uint32 uiIndex)
{
	m_iShaderIndex = static_cast<int32>(uiIndex);
	PrimeShaderUniforms();
}

HyShaderUniforms *HyRenderState::PrimeShaderUniforms()
{
	IHyShader *pShader = IHyRenderer::GetShader(m_iShaderIndex);
	if(pShader)
		m_pShaderUniformsRef = pShader->GetUniforms();
	else
		m_pShaderUniformsRef = NULL;

	return m_pShaderUniformsRef;
}

uint32 HyRenderState::GetTextureHandle()
{
	return m_uiTextureBindHandle;
}

void HyRenderState::SetTextureHandle(uint32 uiHandleId)
{
	m_uiTextureBindHandle = uiHandleId;
}

bool HyRenderState::operator==(const HyRenderState &right) const
{
	if((this->m_uiAttributeFlags == right.m_uiAttributeFlags) && (m_uiTextureBindHandle == right.m_uiTextureBindHandle) && (m_iShaderIndex == right.m_iShaderIndex))
	{
		if(m_pShaderUniformsRef->IsDirty())
			return false;
		else
			return true;
	}
	return false;
}

bool HyRenderState::operator!=(const HyRenderState &right) const
{
	return !(*this == right);
}

bool HyRenderState::operator< (const HyRenderState &right) const
{
	if(m_uiAttributeFlags == right.m_uiAttributeFlags)
	{
		if(this->m_uiTextureBindHandle == right.m_uiTextureBindHandle)
			return this->m_iShaderIndex < right.m_iShaderIndex;
		else
			return (this->m_uiTextureBindHandle < right.m_uiTextureBindHandle);
	}

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
