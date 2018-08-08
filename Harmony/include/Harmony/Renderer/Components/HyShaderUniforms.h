/**************************************************************************
*	HyShaderUniforms.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyShaderUniforms_h__
#define HyShaderUniforms_h__

#include "Afx/HyStdAfx.h"
#include "Assets/HyAssets.h"
#include "Renderer/Components/HyVertexBuffer.h"

#define HY_SHADER_UNIFORM_NAME_LENGTH		32	// includes the NULL terminator
#define HY_SHADER_UNIFORM_BUFFER_LENGTH		(sizeof(uint32) + HY_SHADER_UNIFORM_NAME_LENGTH + sizeof(glm::mat4))

class HyShaderUniforms
{
	bool							m_bDirty;
	uint32							m_uiCrc32;

	struct UniformBuffer
	{
		uint8 m_pData[HY_SHADER_UNIFORM_BUFFER_LENGTH];

		HyShaderVariable GetVariableType() const		{ return *reinterpret_cast<const HyShaderVariable *>(m_pData); }
		const char *GetName() const						{ return reinterpret_cast<const char *>(m_pData + sizeof(uint32)); }
		uint8 *GetData()								{ return m_pData + sizeof(uint32) + HY_SHADER_UNIFORM_NAME_LENGTH; }
		const uint8 *GetData() const					{ return m_pData + sizeof(uint32) + HY_SHADER_UNIFORM_NAME_LENGTH; }

		void SetVariableType(HyShaderVariable eType)	{ *reinterpret_cast<HyShaderVariable *>(m_pData) = eType; }
		void SetName(const char *szName)
		{ 
			HyAssert(strlen(szName) < HY_SHADER_UNIFORM_NAME_LENGTH, "UniformBuffer::SetName() took a name greater than 'HY_SHADER_UNIFORM_NAME_LENGTH'");
			strcpy_s(reinterpret_cast<char *>(m_pData + sizeof(uint32)), HY_SHADER_UNIFORM_NAME_LENGTH, szName);
		}
	};
	std::vector<UniformBuffer>	m_UniformList;

public:
	HyShaderUniforms();
	HyShaderUniforms(const HyShaderUniforms &copyRef);
	~HyShaderUniforms();

	HyShaderUniforms &operator=(const HyShaderUniforms &rhs);
	bool operator==(HyShaderUniforms &rhs);

	uint32 GetCrc32();
	uint32 GetNumUniforms() const;
	HyShaderVariable GetVariableType(uint32 uiIndex) const;
	const char *GetName(uint32 uiIndex) const;
	const uint8 *GetData(uint32 uiIndex) const;

	int32 FindIndex(const char *szName);

	void Set(const char *szName, const glm::vec2 &v);
	void Set(const char *szName, float x, float y, float z);
	void Set(const char *szName, const glm::vec3 &v);
	void Set(const char *szName, const glm::vec4 &v);
	void Set(const char *szName, const glm::mat4 &m);
	void Set(const char *szName, const glm::mat3 &m);
	void Set(const char *szName, float val);
	void Set(const char *szName, int32 val);
	void Set(const char *szName, uint32 val);
	void Set(const char *szName, bool val);

	void Clear();
};

#endif /* HyShaderUniforms_h__ */
