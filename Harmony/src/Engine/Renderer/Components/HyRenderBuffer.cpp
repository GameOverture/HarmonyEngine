/**************************************************************************
 *	HyRenderBuffer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyRenderBuffer.h"
#include "Renderer/Components/HyShaderUniforms.h"
#include "Renderer/IHyRenderer.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable3d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"

HyRenderBuffer::HyRenderBuffer() :	m_pBUFFER(HY_NEW uint8[HY_RENDERSTATE_BUFFER_SIZE]),
									m_pCurWritePosition(m_pBUFFER),
									m_pRenderStatesUserStartPos(nullptr)
{
#ifdef HY_DEBUG
	memset(m_pBUFFER, 0, HY_RENDERSTATE_BUFFER_SIZE);
#endif
}

HyRenderBuffer::~HyRenderBuffer()
{
	delete[] m_pBUFFER;
}

HyRenderBuffer::Header *HyRenderBuffer::GetHeaderPtr()
{
	return reinterpret_cast<Header *>(m_pRenderStatesUserStartPos);
}

HyRenderBuffer::State *HyRenderBuffer::GetCurWritePosPtr()
{
	return reinterpret_cast<State *>(m_pCurWritePosition);
}

void HyRenderBuffer::Reset()
{
	m_pCurWritePosition = m_pBUFFER;
	m_pRenderStatesUserStartPos = nullptr;
}

void HyRenderBuffer::AppendRenderState(uint32 uiId, IHyDrawable &instanceRef, HyCameraMask uiCameraMask, HyScreenRect<int32> &scissorRectRef, HyStencilHandle hStencil, int32 iCoordinateSystem, uint32 uiDataOffset, uint32 uiNumInstances, uint32 uiNumVerticesPerInstance)
{
	State *pRenderState = new (m_pCurWritePosition)State(uiId,
														 uiCameraMask,
														 uiDataOffset,
														 instanceRef.GetRenderMode(),
														 instanceRef.GetTextureHandle(),
														 instanceRef.GetShaderHandle(),
														 scissorRectRef,
														 hStencil,
														 iCoordinateSystem,
														 uiNumInstances,
														 uiNumVerticesPerInstance);
	m_pCurWritePosition += sizeof(State);

	uint8 *pStartOfExData = m_pCurWritePosition;
	AppendShaderUniforms(instanceRef.GetShaderUniforms());
	pRenderState->m_uiExDataSize = (static_cast<uint32>(m_pCurWritePosition - pStartOfExData));
	HyAssert(static_cast<uint32>(m_pCurWritePosition - m_pBUFFER) < HY_RENDERSTATE_BUFFER_SIZE, "IHyRenderer::AppendDrawable2d() has written passed its render state bounds! Embiggen 'HY_RENDERSTATE_BUFFER_SIZE'");

	if(m_pRenderStatesUserStartPos)
	{
		Header *pHeader = reinterpret_cast<Header *>(m_pRenderStatesUserStartPos);

		if(instanceRef._DrawableGetNodeRef().Is2D())
			pHeader->uiNum2dRenderStates++;
		else
			pHeader->uiNum3dRenderStates++;
	}
}

void HyRenderBuffer::CreateRenderHeader()
{
	m_pRenderStatesUserStartPos = m_pCurWritePosition;
	Header *pHeader = reinterpret_cast<Header *>(m_pCurWritePosition);
	memset(pHeader, 0, sizeof(Header));

	m_pCurWritePosition += sizeof(Header);
}

void HyRenderBuffer::AppendShaderUniforms(const HyShaderUniforms &shaderUniformRef)
{
	uint32 uiNumUniforms = shaderUniformRef.GetNumUniforms();
	*reinterpret_cast<uint32 *>(m_pCurWritePosition) = uiNumUniforms;
	m_pCurWritePosition += sizeof(uint32);

	for(uint32 i = 0; i < uiNumUniforms; ++i)
	{
		const char *szUniformName = shaderUniformRef.GetName(i);
		uint32 uiStrLen = static_cast<uint32>(strlen(szUniformName) + 1);
		strncpy_s(reinterpret_cast<char *>(m_pCurWritePosition), uiStrLen, szUniformName, HY_SHADER_UNIFORM_NAME_LENGTH);
		m_pCurWritePosition += uiStrLen;

		HyShaderVariable eVariableType = shaderUniformRef.GetVariableType(i);
		*reinterpret_cast<HyShaderVariable *>(m_pCurWritePosition) = eVariableType;
		m_pCurWritePosition += sizeof(HyShaderVariable);

		uint32 uiDataSize = 0;
		switch(eVariableType)
		{
		case HyShaderVariable::boolean:	uiDataSize = sizeof(bool);			break;
		case HyShaderVariable::int32:		uiDataSize = sizeof(int32);			break;
		case HyShaderVariable::uint32:	uiDataSize = sizeof(uint32);		break;
		case HyShaderVariable::float32:	uiDataSize = sizeof(float);			break;
		case HyShaderVariable::double64:	uiDataSize = sizeof(double);		break;
		case HyShaderVariable::bvec2:	uiDataSize = sizeof(glm::bvec2);	break;
		case HyShaderVariable::bvec3:	uiDataSize = sizeof(glm::bvec3);	break;
		case HyShaderVariable::bvec4:	uiDataSize = sizeof(glm::bvec4);	break;
		case HyShaderVariable::ivec2:	uiDataSize = sizeof(glm::ivec2);	break;
		case HyShaderVariable::ivec3:	uiDataSize = sizeof(glm::ivec3);	break;
		case HyShaderVariable::ivec4:	uiDataSize = sizeof(glm::ivec4);	break;
		case HyShaderVariable::vec2:	uiDataSize = sizeof(glm::vec2);		break;
		case HyShaderVariable::vec3:	uiDataSize = sizeof(glm::vec3);		break;
		case HyShaderVariable::vec4:	uiDataSize = sizeof(glm::vec4);		break;
		case HyShaderVariable::dvec2:	uiDataSize = sizeof(glm::dvec2);	break;
		case HyShaderVariable::dvec3:	uiDataSize = sizeof(glm::dvec3);	break;
		case HyShaderVariable::dvec4:	uiDataSize = sizeof(glm::dvec4);	break;
		case HyShaderVariable::mat3:	uiDataSize = sizeof(glm::mat3);		break;
		case HyShaderVariable::mat4:	uiDataSize = sizeof(glm::mat4);		break;
		}
		memcpy(m_pCurWritePosition, shaderUniformRef.GetData(i), uiDataSize);
		m_pCurWritePosition += uiDataSize;
	}
}
