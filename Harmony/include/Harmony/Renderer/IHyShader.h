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

#include "Threading/BasicSync.h"

#include <vector>
#include <map>

class IHyRenderer;

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

	struct Uniform
	{
		HyShaderVariable	eVarType;
		void *				pData;
	};

	const uint32												m_uiINDEX;

	HyLoadState													m_eLoadState;
	std::string													m_sSourceCode[HYNUMSHADERTYPES];
	std::vector<VertexAttribute>								m_vVertexAttributes;

	BasicSection												m_csUniforms;
	std::map<std::string, Uniform>								m_mapUniforms;

	IHyShader(uint32 iIndex);
public:
	virtual ~IHyShader();

	uint32 GetIndex();

	void SetSourceCode(const char *szSource, HyShaderType eType);
	void SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);

	void Finalize();

	void SetUniform(const char *szName, float x, float y, float z);
	void SetUniform(const char *szName, const glm::vec3 &v);
	void SetUniform(const char *szName, const glm::vec4 &v);
	void SetUniform(const char *szName, const glm::mat4 &m);
	void SetUniform(const char *szName, const glm::mat3 &m);
	void SetUniform(const char *szName, float val);
	void SetUniform(const char *szName, int32 val);
	void SetUniform(const char *szName, uint32 val);
	void SetUniform(const char *szName, bool val);

	bool operator==(const IHyShader &right) const;
	bool operator!=(const IHyShader &right) const;

	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* __IHyShader_h__ */
