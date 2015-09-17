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

#include <vector>
#include <map>
#include <algorithm>

class HyOpenGLShader
{
	// Shader creation
	GLuint								m_hProgHandle;
	bool								m_bLinked;
	std::string							m_sCurSrcCode;

	// Diagnostics
	std::string							m_sLogStr;

public:
	enum eGLSLShaderType
	{
		VERTEX, FRAGMENT, GEOMETRY,
		TESS_CONTROL, TESS_EVALUATION
	};

	HyOpenGLShader();
	virtual HyOpenGLShader::~HyOpenGLShader();

	bool   CompileFromFile(const char *szFileName, eGLSLShaderType eType);
	bool   CompileFromString(const char *szSource, eGLSLShaderType eType);
	bool   Link();
	void   Use();

	std::string Log();

	int GetHandle();
	bool IsLinked();

	void BindAttribLocation(GLuint location, const char *szName);
	uint32 GetAttribLocation(const char *szName);

	void BindFragDataLocation(GLuint location, const char *szName);

	void SetUniform(const char *szName, float x, float y, float z);
	void SetUniform(const char *szName, const vec3 &v);
	void SetUniform(const char *szName, const vec4 &v);
	void SetUniform(const char *szName, const mat4 &m);
	void SetUniform(const char *szName, const mat3 &m);
	void SetUniform(const char *szName, float val);
	void SetUniform(const char *szName, int32 val);
	void SetUniform(const char *szName, uint32 val);
	void SetUniform(const char *szName, bool val);

	void PrintActiveUniforms();
	void PrintActiveAttribs();

private:
	bool FileExists(const HyString & fileName);
};

#endif /* __HyOpenGLShader_h__ */
