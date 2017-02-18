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
class HyDataDraw;

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
	std::vector<UniformBuffer>	m_UniformList;

public:
	HyShaderUniforms();
	~HyShaderUniforms();

	uint32 GetCrc32();

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

	// This function is responsible for incrementing the passed in reference pointer the size of the data written
	void WriteUniformsBufferData(char *&pRefDataWritePos);

	void Clear();
};

class IHyShader
{
protected:
	const int32						m_iID;
	const std::string				m_sOPTIONAL_LOAD_PATH;

	struct VertexAttribute
	{
		std::string			sName;
		HyShaderVariable	eVarType;
		bool				bNormalized;
		uint32				uiInstanceDivisor;
	};

	HyLoadState						m_eLoadState;
	std::string						m_sSourceCode[HYNUMSHADERTYPES];
	std::vector<VertexAttribute>	m_VertexAttributeList;

	uint32							m_uiRefCount;

	IHyShader(int32 iId);
	IHyShader(int32 iId, std::string sPrefix, std::string sName);
public:
	virtual ~IHyShader();

	int32 GetId();
	bool IsFinalized();

	void SetSourceCode(std::string sSource, HyShaderType eType);
	void SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);

	void Finalize(HyShaderProgram eDefaultsFrom);

	void OnLoadThread();
	void OnRenderThread(IHyRenderer &rendererRef, HyDataDraw *pData);

	virtual void OnUpload(IHyRenderer &rendererRef) = 0;
	virtual void OnDelete(IHyRenderer &rendererRef) = 0;
};

#endif /* __IHyShader_h__ */
