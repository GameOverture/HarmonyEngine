/**************************************************************************
*	HyShaderUniforms.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Renderer/Components/HyShaderUniforms.h"
#include "Utilities/Crc32.h"
#include "Afx/HyInteropAfx.h"

HyShaderUniforms::HyShaderUniforms() :	m_bDirty(true),
										m_uiCrc32(0)
{
}

HyShaderUniforms::HyShaderUniforms(const HyShaderUniforms &copyRef) :	m_bDirty(copyRef.m_bDirty),
																		m_uiCrc32(copyRef.m_uiCrc32),
																		m_UniformList(copyRef.m_UniformList)
{
}

HyShaderUniforms::~HyShaderUniforms()
{
}

HyShaderUniforms &HyShaderUniforms::operator=(const HyShaderUniforms &rhs)
{
	m_bDirty = rhs.m_bDirty;
	m_uiCrc32 = rhs.m_uiCrc32;
	m_UniformList = rhs.m_UniformList;

	return *this;
}

bool HyShaderUniforms::operator==(HyShaderUniforms &rhs)
{
	return GetCrc32() == rhs.GetCrc32();
}

uint32 HyShaderUniforms::GetCrc32()
{
	if(m_bDirty == false)
		return m_uiCrc32;

	if(m_UniformList.empty())
		m_uiCrc32 = 0;
	else
		m_uiCrc32 = crc32_fast(&m_UniformList[0], m_UniformList.size() * sizeof(UniformBuffer), m_uiCrc32);

	m_bDirty = false;

	return m_uiCrc32;
}

uint32 HyShaderUniforms::GetNumUniforms() const
{
	return static_cast<uint32>(m_UniformList.size());
}

HyShaderVariable HyShaderUniforms::GetVariableType(uint32 uiIndex) const
{
	return m_UniformList[uiIndex].GetVariableType();
}

const char *HyShaderUniforms::GetName(uint32 uiIndex) const
{
	return m_UniformList[uiIndex].GetName();
}

const uint8 *HyShaderUniforms::GetData(uint32 uiIndex) const
{
	return m_UniformList[uiIndex].GetData();
}

int32 HyShaderUniforms::FindIndex(const char *szName)
{
	for(uint32 i = 0; i < m_UniformList.size(); ++i)
	{
		if(0 == strcmp(m_UniformList[i].GetName(), szName))
			return static_cast<int32>(i);
	}

	return -1;
}

void HyShaderUniforms::Set(const char *szName, const glm::vec2 &v)
{
	int32 iIndex = FindIndex(szName);
	if(iIndex == -1)
	{
		UniformBuffer newUniform;
		newUniform.SetVariableType(HyShaderVariable::vec2);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::vec2(v);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::vec2, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::vec2 *>(m_UniformList[iIndex].GetData()) != v)
		{
			*reinterpret_cast<glm::vec2 *>(m_UniformList[iIndex].GetData()) = v;
			m_bDirty = true;
		}
	}
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
		newUniform.SetVariableType(HyShaderVariable::vec3);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::vec3(v);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::vec3, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::vec3 *>(m_UniformList[iIndex].GetData()) != v)
		{
			*reinterpret_cast<glm::vec3 *>(m_UniformList[iIndex].GetData()) = v;
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
		newUniform.SetVariableType(HyShaderVariable::vec4);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::vec4(v);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::vec4, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::vec4 *>(m_UniformList[iIndex].GetData()) != v)
		{
			*reinterpret_cast<glm::vec4 *>(m_UniformList[iIndex].GetData()) = v;
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
		newUniform.SetVariableType(HyShaderVariable::mat4);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::mat4(m);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::mat4, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::mat4 *>(m_UniformList[iIndex].GetData()) != m)
		{
			*reinterpret_cast<glm::mat4 *>(m_UniformList[iIndex].GetData()) = m;
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
		newUniform.SetVariableType(HyShaderVariable::mat3);
		newUniform.SetName(szName);
		new (newUniform.GetData()) glm::mat3(m);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::mat3, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<glm::mat3 *>(m_UniformList[iIndex].GetData()) != m)
		{
			*reinterpret_cast<glm::mat3 *>(m_UniformList[iIndex].GetData()) = m;
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
		newUniform.SetVariableType(HyShaderVariable::float32);
		newUniform.SetName(szName);
		new (newUniform.GetData()) float(fVal);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::float32, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<float *>(m_UniformList[iIndex].GetData()) != fVal)
		{
			*reinterpret_cast<float *>(m_UniformList[iIndex].GetData()) = fVal;
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
		newUniform.SetVariableType(HyShaderVariable::int32);
		newUniform.SetName(szName);
		new (newUniform.GetData()) int32(iVal);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::int32, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<int32 *>(m_UniformList[iIndex].GetData()) != iVal)
		{
			*reinterpret_cast<int32 *>(m_UniformList[iIndex].GetData()) = iVal;
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
		newUniform.SetVariableType(HyShaderVariable::uint32);
		newUniform.SetName(szName);
		new (newUniform.GetData()) uint32(uiVal);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::uint32, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<uint32 *>(m_UniformList[iIndex].GetData()) != uiVal)
		{
			*reinterpret_cast<uint32 *>(m_UniformList[iIndex].GetData()) = uiVal;
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
		newUniform.SetVariableType(HyShaderVariable::boolean);
		newUniform.SetName(szName);
		new (newUniform.GetData()) bool(bVal);

		m_UniformList.push_back(newUniform);
		m_bDirty = true;
	}
	else
	{
		HyAssert(m_UniformList[iIndex].GetVariableType() == HyShaderVariable::boolean, "IHyShader::SetUniform() has changed the data type of '" << szName << "'");

		if(*reinterpret_cast<bool *>(m_UniformList[iIndex].GetData()) != bVal)
		{
			*reinterpret_cast<bool *>(m_UniformList[iIndex].GetData()) = bVal;
			m_bDirty = true;
		}
	}
}

//void HyShaderUniforms::WriteUniformsBufferData(HyRenderBuffer &renderBufferRef)
//{
//	*reinterpret_cast<uint32 *>(pWritePositionRef) = static_cast<uint32>(m_UniformList.size());
//	pWritePositionRef += sizeof(uint32);
//
//	for(uint32 i = 0; i < static_cast<uint32>(m_UniformList.size()); ++i)
//	{
//		uint32 uiStrLen = static_cast<uint32>(strlen(m_UniformList[i].GetName()) + 1);
//		strncpy_s(reinterpret_cast<char *>(pWritePositionRef), uiStrLen, m_UniformList[i].GetName(), HY_SHADER_UNIFORM_NAME_LENGTH);
//		pWritePositionRef += uiStrLen;
//
//		*reinterpret_cast<int32 *>(pWritePositionRef) = static_cast<int32>(m_UniformList[i].GetVariableType());
//		pWritePositionRef += sizeof(int32);
//
//		uint32 uiDataSize = 0;
//		switch(m_UniformList[i].GetVariableType())
//		{
//		case boolean:		uiDataSize = sizeof(bool);			break;
//		case int32:		uiDataSize = sizeof(int32);			break;
//		case uint32:		uiDataSize = sizeof(uint32);		break;
//		case float32:		uiDataSize = sizeof(float);			break;
//		case double64:	uiDataSize = sizeof(double);		break;
//		case bvec2:		uiDataSize = sizeof(glm::bvec2);	break;
//		case bvec3:		uiDataSize = sizeof(glm::bvec3);	break;
//		case bvec4:		uiDataSize = sizeof(glm::bvec4);	break;
//		case ivec2:		uiDataSize = sizeof(glm::ivec2);	break;
//		case ivec3:		uiDataSize = sizeof(glm::ivec3);	break;
//		case ivec4:		uiDataSize = sizeof(glm::ivec4);	break;
//		case vec2:		uiDataSize = sizeof(glm::vec2);		break;
//		case vec3:		uiDataSize = sizeof(glm::vec3);		break;
//		case vec4:		uiDataSize = sizeof(glm::vec4);		break;
//		case dvec2:		uiDataSize = sizeof(glm::dvec2);	break;
//		case dvec3:		uiDataSize = sizeof(glm::dvec3);	break;
//		case dvec4:		uiDataSize = sizeof(glm::dvec4);	break;
//		case mat3:		uiDataSize = sizeof(glm::mat3);		break;
//		case mat4:		uiDataSize = sizeof(glm::mat4);		break;
//		}
//		memcpy(pWritePositionRef, m_UniformList[i].GetData(), uiDataSize);
//		pWritePositionRef += uiDataSize;
//	}
//}

void HyShaderUniforms::Clear()
{
	m_UniformList.clear();
}
