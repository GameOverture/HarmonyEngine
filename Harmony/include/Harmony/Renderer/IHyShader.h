/**************************************************************************
*	IHyShader.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyShader_h__
#define __IHyShader_h__

#include "Afx/HyStdAfx.h"

#include <vector>

class IHyRenderer;

#define HY_SHADER_UNIFORM_NAME_LENGTH		32	// includes the NULL terminator
#define HY_SHADER_UNIFORM_BUFFER_LENGTH		(sizeof(uint32) + HY_SHADER_UNIFORM_NAME_LENGTH + sizeof(glm::mat4))

class HyShaderUniforms
{
	bool							m_bDirty;
	uint32							m_uiCrc32;

	struct UniformBuffer
	{
		char m_pData[HY_SHADER_UNIFORM_BUFFER_LENGTH];

		HyShaderVariable GetVariableType()				{ return *reinterpret_cast<HyShaderVariable *>(m_pData); }
		char *GetName()									{ return m_pData + sizeof(uint32); }
		char *GetData()									{ return m_pData + sizeof(uint32) + HY_SHADER_UNIFORM_NAME_LENGTH; }

		void SetVariableType(HyShaderVariable eType)	{ *reinterpret_cast<HyShaderVariable *>(m_pData) = eType; }
		void SetName(const char *szName)
		{ 
			HyAssert(strlen(szName) < HY_SHADER_UNIFORM_NAME_LENGTH, "UniformBuffer::SetName() took a name greater than 'HY_SHADER_UNIFORM_NAME_LENGTH'");
			strcpy_s(GetName(), HY_SHADER_UNIFORM_NAME_LENGTH, szName);
		}
	};
	std::vector<UniformBuffer>	m_vUniforms;

public:
	HyShaderUniforms();
	~HyShaderUniforms();

	bool IsDirty();

	int32 FindIndex(const char *szName);

	void Set(const char *szName, float x, float y, float z);
	void Set(const char *szName, const glm::vec3 &v);
	void Set(const char *szName, const glm::vec4 &v);
	void Set(const char *szName, const glm::mat4 &m);
	void Set(const char *szName, const glm::mat3 &m);
	void Set(const char *szName, float val);
	void Set(const char *szName, int32 val);
	void Set(const char *szName, uint32 val);
	void Set(const char *szName, bool val);

	// This function is responsible for incrementing the passed in reference pointer the size of the data written
	void WriteUniformsBufferData(char *&pRefDataWritePos);
};

class IHyShader
{
public:
	enum eShaderProgram
	{
		SHADER_QuadBatch = 0,
		SHADER_Primitive,

		SHADER_CustomStartIndex
	};
protected:

	struct VertexAttribute
	{
		std::string			sName;
		HyShaderVariable	eVarType;
		bool				bNormalized;
		uint32				uiInstanceDivisor;
	};

	const int32						m_iID;

	HyLoadState						m_eLoadState;
	std::string						m_sSourceCode[HYNUMSHADERTYPES];
	std::vector<VertexAttribute>	m_vVertexAttributes;

	HyShaderUniforms				m_Uniforms;

	IHyShader(int32 iId);
public:
	virtual ~IHyShader();

	int32 GetId();
	bool IsFinalized();
	HyShaderUniforms *GetUniforms();

	void SetSourceCode(std::string sSource, HyShaderType eType);
	void SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);

	void Finalize();

	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* __IHyShader_h__ */
