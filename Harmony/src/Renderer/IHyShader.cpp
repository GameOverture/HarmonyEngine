/**************************************************************************
*	IHyShader.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/IHyShader.h"
#include "Utilities/Crc32.h"
#include "Utilities/HyStrManip.h"

#include "Afx/HyInteropAfx.h"

HyShaderUniforms::HyShaderUniforms() :	m_bDirty(true),
										m_uiCrc32(0)
{
}

HyShaderUniforms::~HyShaderUniforms()
{
}

bool HyShaderUniforms::IsDirty()
{
	return m_bDirty;
}

int32 HyShaderUniforms::FindIndex(const char *szName)
{
	for(uint32 i = 0; i < m_vUniforms.size(); ++i)
	{
		if(0 == strcmp(m_vUniforms[i].GetName(), szName))
			return static_cast<int32>(i);
	}

	return -1;
}

void HyShaderUniforms::Set(const char *szName, float x, float y, float z)
{
	Set(szName, glm::vec3(x, y, z));
}

void HyShaderUniforms::Set(const char *szName, const glm::vec3 &v)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_vec3);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::vec3(v);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_vec3, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::vec3 *>(m_vUniforms[iIndex].GetData()) != v)
		{
			*reinterpret_cast<glm::vec3 *>(m_vUniforms[iIndex].GetData()) = v;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, const glm::vec4 &v)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_vec4);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::vec4(v);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_vec4, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::vec4 *>(m_vUniforms[iIndex].GetData()) != v)
		{
			*reinterpret_cast<glm::vec4 *>(m_vUniforms[iIndex].GetData()) = v;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, const glm::mat4 &m)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_mat4);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::mat4(m);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_mat4, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::mat4 *>(m_vUniforms[iIndex].GetData()) != m)
		{
			*reinterpret_cast<glm::mat4 *>(m_vUniforms[iIndex].GetData()) = m;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, const glm::mat3 &m)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_mat3);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::mat3(m);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_mat3, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::mat3 *>(m_vUniforms[iIndex].GetData()) != m)
		{
			*reinterpret_cast<glm::mat3 *>(m_vUniforms[iIndex].GetData()) = m;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, float fVal)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_float);
		newUniform.SetName(szName);
		new (newUniform.GetData()) float(fVal);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_float, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<float *>(m_vUniforms[iIndex].GetData()) != fVal)
		{
			*reinterpret_cast<float *>(m_vUniforms[iIndex].GetData()) = fVal;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, int32 iVal)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_int);
		newUniform.SetName(szName);
		new (newUniform.GetData()) int32(iVal);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_int, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<int32 *>(m_vUniforms[iIndex].GetData()) != iVal)
		{
			*reinterpret_cast<int32 *>(m_vUniforms[iIndex].GetData()) = iVal;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, uint32 uiVal)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_uint);
		newUniform.SetName(szName);
		new (newUniform.GetData()) uint32(uiVal);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_uint, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<uint32 *>(m_vUniforms[iIndex].GetData()) != uiVal)
		{
			*reinterpret_cast<uint32 *>(m_vUniforms[iIndex].GetData()) = uiVal;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, bool bVal)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HYSHADERVAR_bool);
		newUniform.SetName(szName);
		new (newUniform.GetData()) bool(bVal);

		m_vUniforms.push_back(newUniform);
	}
	else
	{
		HyAssert(m_vUniforms[iIndex].GetVariableType() == HYSHADERVAR_bool, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<bool *>(m_vUniforms[iIndex].GetData()) != bVal)
		{
			*reinterpret_cast<bool *>(m_vUniforms[iIndex].GetData()) = bVal;
			m_bDirty = true;
		}
	}
}

// This function is responsible for incrementing the passed in reference pointer the size of the data written
void HyShaderUniforms::WriteUniformsBufferData(char *&pRefDataWritePos)
{
	*reinterpret_cast<uint32 *>(pRefDataWritePos) = static_cast<uint32>(m_vUniforms.size());
	pRefDataWritePos += sizeof(uint32);

	for(uint32 i = 0; i < static_cast<uint32>(m_vUniforms.size()); ++i)
	{
		uint32 uiStrLen = static_cast<uint32>(strlen(m_vUniforms[i].GetName()) + 1);
		strncpy_s(pRefDataWritePos, uiStrLen, m_vUniforms[i].GetName(), HY_SHADER_UNIFORM_NAME_LENGTH);
		pRefDataWritePos += uiStrLen;

		*reinterpret_cast<int32 *>(pRefDataWritePos) = static_cast<int32>(m_vUniforms[i].GetVariableType());
		pRefDataWritePos += sizeof(int32);

		uint32 uiDataSize = 0;
		switch(m_vUniforms[i].GetVariableType())
		{
		case HYSHADERVAR_bool:		uiDataSize = sizeof(bool);			break;
		case HYSHADERVAR_int:		uiDataSize = sizeof(int32);			break;
		case HYSHADERVAR_uint:		uiDataSize = sizeof(uint32);		break;
		case HYSHADERVAR_float:		uiDataSize = sizeof(float);			break;
		case HYSHADERVAR_double:	uiDataSize = sizeof(double);		break;
		case HYSHADERVAR_bvec2:		uiDataSize = sizeof(glm::bvec2);	break;
		case HYSHADERVAR_bvec3:		uiDataSize = sizeof(glm::bvec3);	break;
		case HYSHADERVAR_bvec4:		uiDataSize = sizeof(glm::bvec4);	break;
		case HYSHADERVAR_ivec2:		uiDataSize = sizeof(glm::ivec2);	break;
		case HYSHADERVAR_ivec3:		uiDataSize = sizeof(glm::ivec3);	break;
		case HYSHADERVAR_ivec4:		uiDataSize = sizeof(glm::ivec4);	break;
		case HYSHADERVAR_vec2:		uiDataSize = sizeof(glm::vec2);		break;
		case HYSHADERVAR_vec3:		uiDataSize = sizeof(glm::vec3);		break;
		case HYSHADERVAR_vec4:		uiDataSize = sizeof(glm::vec4);		break;
		case HYSHADERVAR_dvec2:		uiDataSize = sizeof(glm::dvec2);	break;
		case HYSHADERVAR_dvec3:		uiDataSize = sizeof(glm::dvec3);	break;
		case HYSHADERVAR_dvec4:		uiDataSize = sizeof(glm::dvec4);	break;
		case HYSHADERVAR_mat3:		uiDataSize = sizeof(glm::mat3);		break;
		case HYSHADERVAR_mat4:		uiDataSize = sizeof(glm::mat4);		break;
		}
		memcpy(pRefDataWritePos, m_vUniforms[i].GetData(), uiDataSize);
		pRefDataWritePos += uiDataSize;
	}
	
	if(m_vUniforms.empty())
		m_uiCrc32 = 0;
	else
		m_uiCrc32 = crc32_fast(&m_vUniforms[0], m_vUniforms.size() * sizeof(UniformBuffer), m_uiCrc32);

	m_bDirty = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IHyShader::IHyShader(int32 iId) :	m_iID(iId),
									m_sOPTIONAL_LOAD_PATH(""),
									m_eLoadState(HYLOADSTATE_Inactive)
{
	for(int i = 0; i < HYNUMSHADERTYPES; ++i)
		m_sSourceCode[i].clear();
}
IHyShader::IHyShader(int32 iId, std::string sPrefix, std::string sName) :	m_iID(iId),
																			m_sOPTIONAL_LOAD_PATH(MakeStringProperPath(std::string(sPrefix + "/" + sName).c_str(), ".hyglsl", false)),
																			m_eLoadState(HYLOADSTATE_Inactive)
{
	for(int i = 0; i < HYNUMSHADERTYPES; ++i)
		m_sSourceCode[i].clear();
}

IHyShader::~IHyShader()
{
}

int32 IHyShader::GetId()
{
	return m_iID;
}

bool IHyShader::IsFinalized()
{
	return m_eLoadState != HYLOADSTATE_Inactive;
}

HyShaderUniforms *IHyShader::GetUniforms()
{
	return &m_Uniforms;
}

void IHyShader::SetSourceCode(std::string sSource, HyShaderType eType)
{
	if(sSource.empty())
		return;

	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "HyShader::SetSourceCode() was invoked on a locked shader");
	m_sSourceCode[eType] = sSource;
}

void IHyShader::SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize /*= false*/, uint32 uiInstanceDivisor /*= 0*/)
{
	if(szName == NULL)
		return;

	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "HyShader::SetVertexAttribute() was invoked on a locked shader");

	VertexAttribute vertAttrib;
	vertAttrib.sName = szName;
	vertAttrib.eVarType = eVarType;
	vertAttrib.bNormalized = bNormalize;
	vertAttrib.uiInstanceDivisor = uiInstanceDivisor;

	m_vVertexAttributes.push_back(vertAttrib);
}

void IHyShader::Finalize(HyShaderProgram eDefaultsFrom)
{
	// If unassigned vertex shader, fill in defaults
	if(m_sSourceCode[HYSHADER_Vertex].empty())
	{
		m_vVertexAttributes.clear();

		switch(eDefaultsFrom)
		{
		case HYSHADERPROG_QuadBatch:
			SetSourceCode(szHYQUADBATCH_VERTEXSHADER, HYSHADER_Vertex);
			SetVertexAttribute("size", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("offset", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("tint", HYSHADERVAR_vec4, false, 1);
			SetVertexAttribute("textureIndex", HYSHADERVAR_float, false, 1);
			SetVertexAttribute("UVcoord0", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("UVcoord1", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("UVcoord2", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("UVcoord3", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("mtxLocalToWorld", HYSHADERVAR_mat4, false, 1);
			break;

		case HYSHADERPROG_Primitive:
			SetSourceCode(szHYPRIMATIVE_VERTEXSHADER, HYSHADER_Vertex);
			SetVertexAttribute("position", HYSHADERVAR_vec4);
			break;
		}
	}

	// If unassigned fragment shader, fill in defaults
	if(m_sSourceCode[HYSHADER_Fragment].empty())
	{
		switch(eDefaultsFrom)
		{
		case HYSHADERPROG_QuadBatch:
			SetSourceCode(szHYQUADBATCH_FRAGMENTSHADER, HYSHADER_Fragment);
			break;

		case HYSHADERPROG_Primitive:
			SetSourceCode(szHYPRIMATIVE_FRAGMENTSHADER, HYSHADER_Fragment);
			break;
		}
	}

	m_eLoadState = HYLOADSTATE_Queued;
}

void IHyShader::OnLoadThread()
{
	if(m_sOPTIONAL_LOAD_PATH.empty())
		return;

	HyError("IHyShader::OnLoadThread not implemented. Need to parse hy shader file and get setup shader source and vertex attribs");
}
