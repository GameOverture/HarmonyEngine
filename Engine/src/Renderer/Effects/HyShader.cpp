/**************************************************************************
*	HyShader.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
**************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/IHyRenderer.h"

/*static*/ HyShaderHandle HyShader::sm_hHandleCount = 0;

HyShader::HyShader(HyShaderProgramDefaults eDefaultsFrom) :
	IHyFile(HYFILE_Shader, "", 0, 0),
	m_hHANDLE(++sm_hHandleCount),
	m_eDEFAULTS_FROM(eDefaultsFrom),
	m_bIsFinalized(false),
	m_uiStride(0)
{
	for(int i = 0; i < HYNUM_SHADERTYPES; ++i)
		m_sSourceCode[i].clear();
}

HyShader::~HyShader()
{
}

void HyShader::Destroy()
{
	IHyRenderer::RemoveShader(this);
	delete this;
}

HyShaderHandle HyShader::GetHandle()
{
	return m_hHANDLE;
}

bool HyShader::IsFinalized()
{
	return m_bIsFinalized;
}

int32 HyShader::GetStride()
{
	return static_cast<int32>(m_uiStride);
}

HyShaderProgramDefaults HyShader::GetDefaults() const
{
	return m_eDEFAULTS_FROM;
}

const std::string &HyShader::GetSourceCode(HyShaderType eType)
{
	return m_sSourceCode[eType];
}

void HyShader::SetSourceCode(std::string sSource, HyShaderType eType)
{
	if(sSource.empty())
		return;

	m_sSourceCode[eType] = sSource;
}

/*virtual*/ void HyShader::AddVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize /*= false*/, uint32 uiInstanceDivisor /*= 0*/)
{
	if(szName == nullptr)
		return;

	HyShaderVertexAttribute vertAttrib;
	vertAttrib.sName = szName;
	vertAttrib.eVarType = eVarType;
	vertAttrib.bNormalized = bNormalize;
	vertAttrib.uiInstanceDivisor = uiInstanceDivisor;

	m_VertexAttributeList.push_back(vertAttrib);
}

std::vector<HyShaderVertexAttribute> &HyShader::GetVertextAttributes()
{
	return m_VertexAttributeList;
}

void HyShader::Finalize()
{
	IHyRenderer::AddShader(this);
	m_bIsFinalized = true;
}

/*virtual*/ std::string HyShader::AssetTypeName() /*override*/
{
	return "Shader";
}

/*virtual*/ void HyShader::OnLoadThread() /*override*/
{
}

/*virtual*/ void HyShader::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
	rendererRef.UploadShader(this);

	// Calculate the stride based on the specified vertex attributes
	m_uiStride = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_VertexAttributeList.size()); ++i)
	{
		switch(m_VertexAttributeList[i].eVarType)
		{
		case HyShaderVariable::boolean:		m_uiStride += sizeof(bool);			break;
		case HyShaderVariable::int32:		m_uiStride += sizeof(int32);		break;
		case HyShaderVariable::uint32:		m_uiStride += sizeof(uint32);		break;
		case HyShaderVariable::float32:		m_uiStride += sizeof(float);		break;
		case HyShaderVariable::double64:	m_uiStride += sizeof(double);		break;
		case HyShaderVariable::bvec2:		m_uiStride += sizeof(glm::bvec2);	break;
		case HyShaderVariable::bvec3:		m_uiStride += sizeof(glm::bvec3);	break;
		case HyShaderVariable::bvec4:		m_uiStride += sizeof(glm::bvec4);	break;
		case HyShaderVariable::ivec2:		m_uiStride += sizeof(glm::ivec2);	break;
		case HyShaderVariable::ivec3:		m_uiStride += sizeof(glm::ivec3);	break;
		case HyShaderVariable::ivec4:		m_uiStride += sizeof(glm::ivec4);	break;
		case HyShaderVariable::vec2:		m_uiStride += sizeof(glm::vec2);	break;
		case HyShaderVariable::vec3:		m_uiStride += sizeof(glm::vec3);	break;
		case HyShaderVariable::vec4:		m_uiStride += sizeof(glm::vec4);	break;
		case HyShaderVariable::dvec2:		m_uiStride += sizeof(glm::dvec2);	break;
		case HyShaderVariable::dvec3:		m_uiStride += sizeof(glm::dvec3);	break;
		case HyShaderVariable::dvec4:		m_uiStride += sizeof(glm::dvec4);	break;
		case HyShaderVariable::mat3:		m_uiStride += sizeof(glm::mat3);	break;
		case HyShaderVariable::mat4:		m_uiStride += sizeof(glm::mat4);	break;
		case HyShaderVariable::color:		m_uiStride += (sizeof(uint8_t) * 4);break; // 4 uint8 RGBA - becomes a vec4 in shader
		}
	}

	m_bIsFinalized = true;
}

/*virtual*/ std::string HyShader::GetAssetInfo() /*override*/
{
	std::stringstream ss;
	ss << m_sFILE_NAME << ", Defaults From: " << m_eDEFAULTS_FROM;
	return ss.str();
}
