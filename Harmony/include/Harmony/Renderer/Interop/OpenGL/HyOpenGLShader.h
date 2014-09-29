/**************************************************************************
 *	HyGlfwShader.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyGlfwShader_h__
#define __HyGlfwShader_h__

#include "Afx/HyStdAfx.h"

#include <vector>
#include <map>
#include <algorithm>

class HyGlfwShader
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

	HyGlfwShader();
	virtual HyGlfwShader::~HyGlfwShader();

	bool   CompileFromFile(const char *szFileName, eGLSLShaderType eType);
	bool   CompileFromString(const char *szSource, eGLSLShaderType eType);
	bool   Link();
	void   Use();

	std::string Log();

	int GetHandle();
	bool IsLinked();

	void BindAttribLocation( GLuint location, const char * name);
	void BindFragDataLocation( GLuint location, const char * name );

	void SetUniform(const char *name, float x, float y, float z);
	void SetUniform(const char *name, const vec3 &v);
	void SetUniform(const char *name, const vec4 &v);
	void SetUniform(const char *name, const mat4 &m);
	void SetUniform(const char *name, const mat3 &m);
	void SetUniform(const char *name, float val);
	void SetUniform(const char *name, int32 val);
	void SetUniform(const char *name, uint32 val);
	void SetUniform(const char *name, bool val);

	void PrintActiveUniforms();
	void PrintActiveAttribs();

private:
	bool FileExists(const HyString & fileName);
};

#endif /* __HyGlfwShader_h__ */
