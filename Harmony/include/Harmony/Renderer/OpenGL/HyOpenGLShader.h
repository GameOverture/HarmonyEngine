/**************************************************************************
 *	HyOpenGLShader.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyOpenGLShader_h__
#define __HyOpenGLShader_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/IHyShader.h"

#include <vector>
#include <map>
#include <algorithm>

class HyOpenGLShader : public IHyShader
{
	GLuint								m_hVAO;

	GLsizei								m_uiStride;

	// Shader creation
	GLuint								m_hProgHandle;
	bool								m_bLinked;

public:
	HyOpenGLShader(uint32 uiId);
	virtual HyOpenGLShader::~HyOpenGLShader();

	void CompileFromString(HyShaderType eType);
	void Link();
	void Use(uint32 uiDataOffset);

	int GetHandle();
	bool IsLinked();

	void BindAttribLocation(GLuint location, const char *szName);
	uint32 GetAttribLocation(const char *szName);

	void BindFragDataLocation(GLuint location, const char *szName);

	void SetUniformGLSL(const char *szName, float x, float y, float z);
	void SetUniformGLSL(const char *szName, const glm::vec3 &v);
	void SetUniformGLSL(const char *szName, const glm::vec4 &v);
	void SetUniformGLSL(const char *szName, const glm::mat4 &m);
	void SetUniformGLSL(const char *szName, const glm::mat3 &m);
	void SetUniformGLSL(const char *szName, float val);
	void SetUniformGLSL(const char *szName, int32 val);
	void SetUniformGLSL(const char *szName, uint32 val);
	void SetUniformGLSL(const char *szName, bool val);

	void PrintActiveUniforms();
	void PrintActiveAttribs();

	virtual void OnRenderThread(IHyRenderer &rendererRef);
};

#endif /* __HyOpenGLShader_h__ */
