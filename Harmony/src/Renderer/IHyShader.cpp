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

HyShaderUniforms::HyShaderUniforms() :	m_bDirty(true),
										m_uiCrc32(0)
{ }

HyShaderUniforms::~HyShaderUniforms()
{
	for(std::map<std::string, Uniform>::iterator iter = m_mapUniforms.begin(); iter != m_mapUniforms.end(); ++iter)
		delete iter->second.pData;
}

bool HyShaderUniforms::IsDirty()
{
	return m_bDirty;
}

void HyShaderUniforms::Set(const char *szName, float x, float y, float z)
{
	Set(szName, glm::vec3(x, y, z));
}

void HyShaderUniforms::Set(const char *szName, const glm::vec3 &v)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_vec3;
		newUniform.pData = new glm::vec3(v);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_vec3, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::vec3 *>(m_mapUniforms[szName].pData) != v)
		{
			*reinterpret_cast<glm::vec3 *>(m_mapUniforms[szName].pData) = v;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, const glm::vec4 &v)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_vec4;
		newUniform.pData = new glm::vec4(v);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_vec4, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::vec4 *>(m_mapUniforms[szName].pData) != v)
		{
			*reinterpret_cast<glm::vec4 *>(m_mapUniforms[szName].pData) = v;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, const glm::mat4 &m)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_mat4;
		newUniform.pData = new glm::mat4(m);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_mat4, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::mat4 *>(m_mapUniforms[szName].pData) != m)
		{
			*reinterpret_cast<glm::mat4 *>(m_mapUniforms[szName].pData) = m;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, const glm::mat3 &m)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_mat3;
		newUniform.pData = new glm::mat3(m);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_mat3, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::mat3 *>(m_mapUniforms[szName].pData) != m)
		{
			*reinterpret_cast<glm::mat3 *>(m_mapUniforms[szName].pData) = m;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, float fVal)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_float;
		newUniform.pData = new float(fVal);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_float, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<float *>(m_mapUniforms[szName].pData) != fVal)
		{
			*reinterpret_cast<float *>(m_mapUniforms[szName].pData) = fVal;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, int32 iVal)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_int;
		newUniform.pData = new int32(iVal);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_int, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<int32 *>(m_mapUniforms[szName].pData) != iVal)
		{
			*reinterpret_cast<int32 *>(m_mapUniforms[szName].pData) = iVal;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, uint32 uiVal)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_uint;
		newUniform.pData = new uint32(uiVal);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_uint, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<uint32 *>(m_mapUniforms[szName].pData) != uiVal)
		{
			*reinterpret_cast<uint32 *>(m_mapUniforms[szName].pData) = uiVal;
			m_bDirty = true;
		}
	}
}

void HyShaderUniforms::Set(const char *szName, bool bVal)
{
	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_bool;
		newUniform.pData = new bool(bVal);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_bool, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<bool *>(m_mapUniforms[szName].pData) != bVal)
		{
			*reinterpret_cast<bool *>(m_mapUniforms[szName].pData) = bVal;
			m_bDirty = true;
		}
	}
}

// This function is responsible for incrementing the passed in reference pointer the size of the data written
void HyShaderUniforms::WriteUniformsBufferData(char *&pRefDataWritePos)
{
	*reinterpret_cast<uint32 *>(pRefDataWritePos) = static_cast<uint32>(m_mapUniforms.size());
	pRefDataWritePos += sizeof(uint32);

	for(std::map<std::string, Uniform>::iterator iter = m_mapUniforms.begin(); iter != m_mapUniforms.end(); ++iter)
	{
		// TODO: should I ensure that I start all writes on a 4byte boundary? ARM systems may be an issue

		size_t uiStrLen = iter->first.length() + 1;	// +1 for NULL terminator
		strncpy_s(pRefDataWritePos, uiStrLen, iter->first.c_str(), uiStrLen);
		pRefDataWritePos += uiStrLen;

		*reinterpret_cast<int32 *>(pRefDataWritePos) = static_cast<int32>(iter->second.eVarType);
		pRefDataWritePos += sizeof(int32);

		uint32 uiDataSize = 0;
		switch(iter->second.eVarType)
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
		memcpy(pRefDataWritePos, iter->second.pData, uiDataSize);
		pRefDataWritePos += uiDataSize;
	}

	m_uiCrc32 = crc32_fast(
	m_bDirty = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IHyShader::IHyShader(uint32 iIndex) :	m_uiINDEX(iIndex),
										m_eLoadState(HYLOADSTATE_Inactive)
{
}

IHyShader::~IHyShader()
{
}

uint32 IHyShader::GetIndex()
{
	return m_uiINDEX;
}

HyShaderUniforms *IHyShader::GetUniforms()
{
	return &m_Uniforms;
}

void IHyShader::SetSourceCode(const char *szSource, HyShaderType eType)
{
	if(szSource == NULL)
		return;

	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "HyShader::SetSourceCode() was invoked on a locked shader");
	m_sSourceCode[eType] = szSource;
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

void IHyShader::Finalize()
{
	m_eLoadState = HYLOADSTATE_Queued;
}
