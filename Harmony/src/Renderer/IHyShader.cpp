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

IHyShader::IHyShader(uint32 iIndex) :	m_uiINDEX(iIndex),
										m_eLoadState(HYLOADSTATE_Inactive)
{
}

IHyShader::~IHyShader()
{
	for(std::map<std::string, Uniform>::iterator iter = m_mapUniforms.begin(); iter != m_mapUniforms.end(); ++iter)
	{
		delete iter->second.pData;
	}
}

uint32 IHyShader::GetIndex()
{
	return m_uiINDEX;
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

void IHyShader::SetUniform(const char *szName, float x, float y, float z)
{
	m_csUniforms.Lock();

	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_vec3;
		newUniform.pData = new glm::vec3(x, y, z);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_vec3, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");
		reinterpret_cast<glm::vec3 *>(m_mapUniforms[szName].pData)->x = x;
		reinterpret_cast<glm::vec3 *>(m_mapUniforms[szName].pData)->y = y;
		reinterpret_cast<glm::vec3 *>(m_mapUniforms[szName].pData)->z = z;
	}

	m_csUniforms.Unlock();
}

void IHyShader::SetUniform(const char *szName, const glm::vec3 &v)
{
	SetUniform(szName, v.x, v.y, v.z);
}

void IHyShader::SetUniform(const char *szName, const glm::vec4 &v)
{
	m_csUniforms.Lock();

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
		*reinterpret_cast<glm::vec4 *>(m_mapUniforms[szName].pData) = v;
	}

	m_csUniforms.Unlock();
}

void IHyShader::SetUniform(const char *szName, const glm::mat4 &m)
{
	m_csUniforms.Lock();

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
		*reinterpret_cast<glm::mat4 *>(m_mapUniforms[szName].pData) = m;
	}

	m_csUniforms.Unlock();
}

void IHyShader::SetUniform(const char *szName, const glm::mat3 &m)
{
	m_csUniforms.Lock();

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
		*reinterpret_cast<glm::mat3 *>(m_mapUniforms[szName].pData) = m;
	}

	m_csUniforms.Unlock();
}

void IHyShader::SetUniform(const char *szName, float val)
{
	m_csUniforms.Lock();

	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_float;
		newUniform.pData = new float(val);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_float, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");
		*reinterpret_cast<float *>(m_mapUniforms[szName].pData) = val;
	}

	m_csUniforms.Unlock();
}

void IHyShader::SetUniform(const char *szName, int32 val)
{
	m_csUniforms.Lock();

	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_int;
		newUniform.pData = new int32(val);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_int, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");
		*reinterpret_cast<int32 *>(m_mapUniforms[szName].pData) = val;
	}

	m_csUniforms.Unlock();
}

void IHyShader::SetUniform(const char *szName, uint32 val)
{
	m_csUniforms.Lock();

	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_uint;
		newUniform.pData = new uint32(val);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_uint, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");
		*reinterpret_cast<uint32 *>(m_mapUniforms[szName].pData) = val;
	}

	m_csUniforms.Unlock();
}

void IHyShader::SetUniform(const char *szName, bool val)
{
	m_csUniforms.Lock();

	if(m_mapUniforms.find(szName) == m_mapUniforms.end())
	{
		Uniform newUniform;
		newUniform.eVarType = HYSHADERVAR_bool;
		newUniform.pData = new bool(val);

		m_mapUniforms[szName] = newUniform;
	}
	else
	{
		HyAssert(m_mapUniforms[szName].eVarType == HYSHADERVAR_bool, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");
		*reinterpret_cast<bool *>(m_mapUniforms[szName].pData) = val;
	}

	m_csUniforms.Unlock();
}
