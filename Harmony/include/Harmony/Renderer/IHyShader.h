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
#include <map>

class IHyRenderer;

class HyShaderUniforms
{
	bool							m_bDirty;

	struct Uniform
	{
		HyShaderVariable	eVarType;
		void *				pData;
	};
	std::map<std::string, Uniform>	m_mapUniforms;

public:
	HyShaderUniforms();
	~HyShaderUniforms();

	bool IsDirty();

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

	const uint32												m_uiINDEX;

	HyLoadState													m_eLoadState;
	std::string													m_sSourceCode[HYNUMSHADERTYPES];
	std::vector<VertexAttribute>								m_vVertexAttributes;

	HyShaderUniforms											m_Uniforms;

	IHyShader(uint32 iIndex);
public:
	virtual ~IHyShader();

	uint32 GetIndex();
	HyShaderUniforms *GetUniforms();

	void SetSourceCode(const char *szSource, HyShaderType eType);
	void SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);

	void Finalize();

	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* __IHyShader_h__ */
