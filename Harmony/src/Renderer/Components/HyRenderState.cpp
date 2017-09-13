/**************************************************************************
 *	HyRenderState.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Components/HyShaderUniforms.h"

HyRenderState::HyRenderState() :	m_iRenderMode(HYRENDERMODE_Unknown),
									m_uiAttributeFlags(0),
									m_uiTextureBindHandle(0),
									m_iShaderId(-1),
									m_uiUniformsCrc32(0),
									m_uiNumInstances(0),
									m_uiNumVerticesPerInstance(0),
									m_uiDataOffset(0),
									m_ScissorRect(0, 0, 0, 0),
									m_iWindowIndex(-1)
{
}


HyRenderState::~HyRenderState(void)
{
}

void HyRenderState::SetRenderMode(HyRenderMode eRenderMode)
{
	m_iRenderMode = eRenderMode;
}

HyRenderMode HyRenderState::GetRenderMode()
{
	return static_cast<HyRenderMode>(m_iRenderMode);
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

uint32 HyRenderState::GetNumVerticesPerInstance()
{
	return m_uiNumVerticesPerInstance;
}

void HyRenderState::SetNumVerticesPerInstance(uint32 uiNumVerts)
{
	m_uiNumVerticesPerInstance = uiNumVerts;
}

bool HyRenderState::IsScissorRect()
{
	return (m_uiAttributeFlags & SCISSORTEST) != 0;
}

const HyScreenRect<int32> &HyRenderState::GetScissorRect()
{
	return m_ScissorRect;
}

void HyRenderState::SetScissorRect(const HyScreenRect<int32> &rect)
{
	m_ScissorRect = rect;

	m_uiAttributeFlags |= SCISSORTEST;
}

void HyRenderState::SetScissorRect(int32 uiX, int32 uiY, uint32 uiWidth, uint32 uiHeight)
{
	m_ScissorRect.x = uiX;
	m_ScissorRect.y = uiY;
	m_ScissorRect.width = uiWidth;
	m_ScissorRect.height = uiHeight;

	m_uiAttributeFlags |= SCISSORTEST;
}

void HyRenderState::ClearScissorRect()
{
	memset(&m_ScissorRect, 0, sizeof(HyScreenRect<uint32>));
	m_uiAttributeFlags &= ~SCISSORTEST;
}

bool HyRenderState::IsUsingCameraCoordinates()
{
	return m_iWindowIndex == -1;
}

void HyRenderState::SetCoordinateSystem(int32 iWindowIndex)
{
	m_iWindowIndex = iWindowIndex;
}

int32 HyRenderState::GetAssignedWindow()
{
	return m_iWindowIndex;
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

bool HyRenderState::IsEnabled(Attributes eAttrib) const
{
	return 0 != (m_uiAttributeFlags & eAttrib);
}

uint32 HyRenderState::GetAttributeBitFlags() const
{
	return m_uiAttributeFlags;
}

int32 HyRenderState::GetShaderId() const
{
	return m_iShaderId;
}

void HyRenderState::SetShaderId(int32 iId)
{
	// Don't overwrite the shader Id if a custom one is already set, and this new 'iId' is a built in shader
	if(m_iShaderId < HYSHADERPROG_CustomStartIndex || iId >= HYSHADERPROG_CustomStartIndex)
		m_iShaderId = iId;
}

void HyRenderState::SetUniformCrc32(uint32 uiCrc32)
{
	m_uiUniformsCrc32 = uiCrc32;
}

uint32 HyRenderState::GetTextureHandle() const
{
	return m_uiTextureBindHandle;
}

void HyRenderState::SetTextureHandle(uint32 uiHandleId)
{
	m_uiTextureBindHandle = uiHandleId;
}

bool HyRenderState::operator==(const HyRenderState &right) const
{
	if(m_iRenderMode == right.m_iRenderMode &&
	   m_uiAttributeFlags == right.m_uiAttributeFlags &&
	   m_uiTextureBindHandle == right.m_uiTextureBindHandle &&
	   m_iShaderId == right.m_iShaderId &&
	   m_uiUniformsCrc32 == right.m_uiUniformsCrc32 &&
	   m_ScissorRect == right.m_ScissorRect)
	{
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
	if(this->m_iRenderMode == right.m_iRenderMode)
	{
		if(m_uiAttributeFlags == right.m_uiAttributeFlags)
		{
			if(this->m_uiTextureBindHandle == right.m_uiTextureBindHandle)
				return this->m_iShaderId < right.m_iShaderId;
			else
				return (this->m_uiTextureBindHandle < right.m_uiTextureBindHandle);
		}
		else
			return m_uiAttributeFlags < right.m_uiAttributeFlags;
	}
	else
		return this->m_iRenderMode < right.m_iRenderMode;
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
