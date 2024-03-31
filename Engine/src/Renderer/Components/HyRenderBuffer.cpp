/**************************************************************************
 *	HyRenderBuffer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Renderer/Components/HyRenderBuffer.h"
#include "Renderer/Components/HyShaderUniforms.h"
#include "Renderer/IHyRenderer.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "HyEngine.h"

HyRenderBuffer::HyRenderBuffer() :
	m_pBUFFER(HY_NEW uint8[HY_RENDERSTATE_BUFFER_SIZE]),
	m_pCurWritePosition(m_pBUFFER),
	m_pRenderStatesUserStartPos(nullptr),
	m_uiPrevUniformCrc(0),
	m_pPrevRenderState(nullptr)
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
	m_uiPrevUniformCrc = 0;
	m_pPrevRenderState = nullptr;
}

void HyRenderBuffer::AppendRenderState(uint32 uiId, IHyDrawable2d &instanceRef, HyCameraMask uiCameraMask, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent)
{
	HyRenderMode eRenderMode = HYRENDERMODE_Unknown;
	uint32 uiNumInstances = 0, uiNumVerticesPerInstance = 0;
	bool bIsBatchable = false;
	uint32 uiStageIndex = 0;
	do
	{
		instanceRef.PrepRenderStage(uiStageIndex, eRenderMode, uiNumInstances, uiNumVerticesPerInstance, bIsBatchable);

		State *pRenderState = new (m_pCurWritePosition)State(uiId,
															 uiCameraMask,
															 vertexBufferRef.GetNumUsedBytes2d(), // Gets current offset into vertex buffer
															 eRenderMode,
															 instanceRef.GetShaderHandle(),
															 (instanceRef.GetScissorStencil() != nullptr) ? instanceRef.GetScissorStencil()->GetHandle() : HY_UNUSED_HANDLE,
															 (instanceRef.GetStencil() != nullptr && instanceRef.GetStencil()->IsMaskReady()) ? instanceRef.GetStencil()->GetHandle() : HY_UNUSED_HANDLE,
															 instanceRef.GetCoordinateSystem(),
															 uiNumInstances,
															 uiNumVerticesPerInstance);
		m_pCurWritePosition += sizeof(State);
		uint8 *pStartOfExData = m_pCurWritePosition;

		// Determine if we can combine this render state with the previous one, to batch less render calls
		if(bIsBatchable &&
		   m_uiPrevUniformCrc == instanceRef.GetShaderUniforms().GetCrc64() &&
		   m_pPrevRenderState && *pRenderState == *m_pPrevRenderState)
		{
			m_pPrevRenderState->m_uiNumInstances += pRenderState->m_uiNumInstances;
			m_pCurWritePosition -= sizeof(State);
		}
		else
		{
			AppendExData(instanceRef.GetShaderUniforms()); // This advances the 'm_pCurWritePosition'
			pRenderState->m_uiExDataSize = (static_cast<uint32>(m_pCurWritePosition - pStartOfExData));
			HyAssert(static_cast<uint32>(m_pCurWritePosition - m_pBUFFER) < HY_RENDERSTATE_BUFFER_SIZE, "IHyRenderer::AppendDrawable2d() has written passed its render state bounds! Embiggen 'HY_RENDERSTATE_BUFFER_SIZE'");

			if(m_pRenderStatesUserStartPos)
				reinterpret_cast<Header *>(m_pRenderStatesUserStartPos)->m_uiNum2dRenderStates++;

			m_uiPrevUniformCrc = instanceRef.GetShaderUniforms().GetCrc64();
			m_pPrevRenderState = pRenderState;
		}

		uiStageIndex++;
	} while(instanceRef.WriteVertexData(uiNumInstances, vertexBufferRef, fExtrapolatePercent) == false);
}

void HyRenderBuffer::CreateRenderHeader()
{
	m_pRenderStatesUserStartPos = m_pCurWritePosition;
	Header *pHeader = reinterpret_cast<Header *>(m_pCurWritePosition);
	memset(pHeader, 0, sizeof(Header));

	m_pCurWritePosition += sizeof(Header);
}

void HyRenderBuffer::AppendExData(HyShaderUniforms &shaderUniformRef)
{
	uint32 uiNumSamplers = shaderUniformRef.GetNumTexUnits();
	*reinterpret_cast<uint32 *>(m_pCurWritePosition) = uiNumSamplers;
	m_pCurWritePosition += sizeof(uint32);

	for(uint32 i = 0; i < uiNumSamplers; ++i)
	{
		*reinterpret_cast<HyTextureHandle *>(m_pCurWritePosition) = static_cast<HyTextureHandle>(shaderUniformRef.GetTexHandle(i));
		m_pCurWritePosition += sizeof(HyTextureHandle);
	}


	uint32 uiNumUniforms = shaderUniformRef.GetNumUniforms();
	*reinterpret_cast<uint32 *>(m_pCurWritePosition) = uiNumUniforms;
	m_pCurWritePosition += sizeof(uint32);

	for(uint32 i = 0; i < uiNumUniforms; ++i)
	{
		const char *szUniformName = shaderUniformRef.GetName(i);
		strncpy(reinterpret_cast<char *>(m_pCurWritePosition), szUniformName, HY_SHADER_UNIFORM_NAME_LENGTH);
		m_pCurWritePosition += HY_SHADER_UNIFORM_NAME_LENGTH; // += strlen may cause bad alignment fault

		HyShaderVariable eVariableType = shaderUniformRef.GetVariableType(i);
		*reinterpret_cast<int32 *>(m_pCurWritePosition) = static_cast<int32>(eVariableType);
		m_pCurWritePosition += sizeof(int32);

		uint32 uiDataSize = 0;
		switch(eVariableType)
		{
		case HyShaderVariable::boolean:		uiDataSize = sizeof(bool);			break;
		case HyShaderVariable::int32:		uiDataSize = sizeof(int32);			break;
		case HyShaderVariable::uint32:		uiDataSize = sizeof(uint32);		break;
		case HyShaderVariable::float32:		uiDataSize = sizeof(float);			break;
		case HyShaderVariable::double64:	uiDataSize = sizeof(double);		break;
		case HyShaderVariable::bvec2:		uiDataSize = sizeof(glm::bvec2);	break;
		case HyShaderVariable::bvec3:		uiDataSize = sizeof(glm::bvec3);	break;
		case HyShaderVariable::bvec4:		uiDataSize = sizeof(glm::bvec4);	break;
		case HyShaderVariable::ivec2:		uiDataSize = sizeof(glm::ivec2);	break;
		case HyShaderVariable::ivec3:		uiDataSize = sizeof(glm::ivec3);	break;
		case HyShaderVariable::ivec4:		uiDataSize = sizeof(glm::ivec4);	break;
		case HyShaderVariable::vec2:		uiDataSize = sizeof(glm::vec2);		break;
		case HyShaderVariable::vec3:		uiDataSize = sizeof(glm::vec3);		break;
		case HyShaderVariable::vec4:		uiDataSize = sizeof(glm::vec4);		break;
		case HyShaderVariable::dvec2:		uiDataSize = sizeof(glm::dvec2);	break;
		case HyShaderVariable::dvec3:		uiDataSize = sizeof(glm::dvec3);	break;
		case HyShaderVariable::dvec4:		uiDataSize = sizeof(glm::dvec4);	break;
		case HyShaderVariable::mat3:		uiDataSize = sizeof(glm::mat3);		break;
		case HyShaderVariable::mat4:		uiDataSize = sizeof(glm::mat4);		break;
		}
		memcpy(m_pCurWritePosition, shaderUniformRef.GetData(i), uiDataSize);
		m_pCurWritePosition += uiDataSize;
	}

	m_uiPrevUniformCrc = shaderUniformRef.GetCrc64();
}
