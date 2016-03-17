/**************************************************************************
 *	HyOpenGLShader.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/OpenGL/HyOpenGLShader.h"

HyOpenGLShader::HyOpenGLShader() : m_hProgHandle(0), m_bLinked(false) 
{
}

HyOpenGLShader::~HyOpenGLShader()
{
	// Delete shader objects, shader program, and uniforms allocated in the constructor

	// ... Delete shader objects, etc 
	if(m_hProgHandle > 0)
		glDeleteProgram(m_hProgHandle);
}

void HyOpenGLShader::CompileFromString(HyShaderType eType)
{
	// Create main program handle if one hasn't been created yet (first shader compile)
	if(m_hProgHandle <= 0)
	{
		m_hProgHandle = glCreateProgram();
		HyAssert(m_hProgHandle != 0, "Unable to create shader program");
	}

	GLuint iShaderHandle = 0;

	switch(eType)
	{
	case HYSHADER_Vertex:			iShaderHandle = glCreateShader(GL_VERTEX_SHADER);				break;
	case HYSHADER_Fragment:			iShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);				break;
	case HYSHADER_Geometry:			iShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);				break;
	case HYSHADER_TessControl:		iShaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);			break;
	case HYSHADER_TessEvaluation:	iShaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);		break;
	default:
		HyError("Unknown shader type");
	}

	// Compile the shader from the passed in source code
	const char *szSrc = m_sSourceCode[eType].c_str();
	glShaderSource(iShaderHandle, 1, &szSrc, NULL);
	glCompileShader(iShaderHandle);

#ifdef HY_DEBUG
	// Check for errors
	GLint result;
	glGetShaderiv(iShaderHandle, GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result)
	{
		// Compile failed
		GLint iLength = 0;
		glGetShaderiv(iShaderHandle, GL_INFO_LOG_LENGTH, &iLength);
		if(iLength > 0)
		{
			char *szlog = new char[iLength];
			GLint written = 0;
			glGetShaderInfoLog(iShaderHandle, iLength, &written, szlog);
			
			HyError(szlog);
			delete [] szlog;	// Not that this matters
		}
	}
#endif
	
	// Compile succeeded, attach shader
	glAttachShader(m_hProgHandle, iShaderHandle);
}

void HyOpenGLShader::Link()
{
	if(m_bLinked)
		return;

	HyAssert(m_hProgHandle > 0, "Shader has not been created yet");

	glLinkProgram(m_hProgHandle);

#ifdef HY_DEBUG
	GLint status = 0;
	glGetProgramiv( m_hProgHandle, GL_LINK_STATUS, &status);
	if(GL_FALSE == status)
	{
		GLint length = 0;

		glGetProgramiv(m_hProgHandle, GL_INFO_LOG_LENGTH, &length);

		if( length > 0)
		{
			char *szlog = new char[length];
			GLint written = 0;
			glGetProgramInfoLog(m_hProgHandle, length, &written, szlog);

			HyError("Shader program failed to link!\n" << szlog);
			delete [] szlog;	// Not that this matters
		}
	}
#endif

	// TODO: After linking (whether successfully or not), it is a good idea to detach all shader objects from the program. Call glDetachShader and glDeleteShader (If not intended to use shader object to link another program)
	m_bLinked = true;
}

void HyOpenGLShader::Use()
{
	if(m_hProgHandle <= 0 || (! m_bLinked))
		return;

	glUseProgram(m_hProgHandle);
}

GLint HyOpenGLShader::GetHandle()
{
	return m_hProgHandle;
}

bool HyOpenGLShader::IsLinked()
{
	return m_bLinked;
}

void HyOpenGLShader::BindAttribLocation(GLuint location, const char *szName)
{
	glBindAttribLocation(m_hProgHandle, location, szName);
}

uint32 HyOpenGLShader::GetAttribLocation(const char *szName)
{
	return glGetAttribLocation(m_hProgHandle, szName);
}

void HyOpenGLShader::BindFragDataLocation(GLuint location, const char *szName)
{
	glBindFragDataLocation(m_hProgHandle, location, szName);
}

void HyOpenGLShader::SetUniform(const char *szName, float x, float y, float z)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if(loc >= 0)
		glUniform3f(loc,x,y,z);
}

void HyOpenGLShader::SetUniform(const char *szName, const vec3 &v)
{
	this->SetUniform(szName, v.x, v.y, v.z);
}

void HyOpenGLShader::SetUniform(const char *szName, const vec4 &v)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if( loc >= 0 )
		glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void HyOpenGLShader::SetUniform(const char *szName, const mat4 &m)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if(loc >= 0)
		glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void HyOpenGLShader::SetUniform(const char *szName, const mat3 &m)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if(loc >= 0)
		glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void HyOpenGLShader::SetUniform(const char *szName, float val )
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if(loc >= 0)
		glUniform1f(loc, val);
}

void HyOpenGLShader::SetUniform(const char *szName, int32 val)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if(loc >= 0)
		glUniform1i(loc, val);
}

void HyOpenGLShader::SetUniform(const char *szName, uint32 val)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if(loc >= 0)
		glUniform1ui(loc, val);
}

void HyOpenGLShader::SetUniform(const char *szName, bool val)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, szName);
	if(loc >= 0)
		glUniform1i(loc, val);
}

void HyOpenGLShader::PrintActiveUniforms()
{
	GLint nUniforms, size, location, maxLen;
	GLchar * name;
	GLsizei written;
	GLenum type;

	glGetProgramiv( m_hProgHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv( m_hProgHandle, GL_ACTIVE_UNIFORMS, &nUniforms);

	name = (GLchar *) malloc( maxLen );

	printf(" Location | Name\n");
	printf("------------------------------------------------\n");
	for(GLint i = 0; i < nUniforms; ++i)
	{
		glGetActiveUniform( m_hProgHandle, i, maxLen, &written, &size, &type, name );
		location = glGetUniformLocation(m_hProgHandle, name);
		printf(" %-8d | %s\n",location, name);
	}

	free(name);
}

void HyOpenGLShader::PrintActiveAttribs()
{

	GLint written, size, location, maxLength, nAttribs;
	GLenum type;
	GLchar * name;

	glGetProgramiv(m_hProgHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	glGetProgramiv(m_hProgHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);

	name = (GLchar *) malloc( maxLength );

	printf(" Index | Name\n");
	printf("------------------------------------------------\n");
	for(GLint i = 0; i < nAttribs; i++)
	{
		glGetActiveAttrib( m_hProgHandle, i, maxLength, &written, &size, &type, name );
		location = glGetAttribLocation(m_hProgHandle, name);
		printf(" %-5d | %s\n",location, name);
	}

	free(name);
}
