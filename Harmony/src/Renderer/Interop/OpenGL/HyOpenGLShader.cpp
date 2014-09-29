/**************************************************************************
 *	HyGlfwShader.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Interop/OpenGL/HyOpenGLShader.h"
#include "FileIO/HyFileIO.h"
#include "Utilities/HyStrManip.h"

HyGlfwShader::HyGlfwShader() : m_hProgHandle(0), m_bLinked(false) 
{
}

HyGlfwShader::~HyGlfwShader()
{
	// Delete shader objects, shader program, and uniforms allocated in the constructor

	// ... Delete shader objects, etc 
    if(m_hProgHandle > 0)
		glDeleteProgram(m_hProgHandle);
}

bool HyGlfwShader::CompileFromFile(const char *szFileName, eGLSLShaderType eType)
{
	// check if the user passed us null for either filename
	if (szFileName == NULL)
	{
		m_sLogStr = "szFileName was empty.";
		return false;
	}

	// Combine full path and filename to shaders
	std::string sFullFilePath;
	sFullFilePath = HyFileIO::GetFilePath(HYINST_Shader, "", szFileName);

	switch(eType)
	{
	case VERTEX:
		if(0 != strcmp(&sFullFilePath[sFullFilePath.size() - 5], ".vert"))
			sFullFilePath += ".vert";
		break;
	case FRAGMENT:
		if(0 != strcmp(&sFullFilePath[sFullFilePath.size() - 5], ".frag"))
			sFullFilePath += ".frag";
		break;
	case GEOMETRY:
		if(0 != strcmp(&sFullFilePath[sFullFilePath.size() - 5], ".geom"))
			sFullFilePath += ".geom";
		break;
	case TESS_CONTROL:
	case TESS_EVALUATION:
		if(0 != strcmp(&sFullFilePath[sFullFilePath.size() - 5], ".tess"))
			sFullFilePath += ".tess";
		break;
	default:
		m_sLogStr = "Unknown shader type";
		return false;
	}

	/** Get Vertex And Fragment Shader Sources **/


	//  read the shader source code from constructed file path
	if(!HyFileIO::FileExists(sFullFilePath))
	{
		m_sLogStr = "File not found.";
		return false;
	}
	m_sCurSrcCode = HyFileIO::ReadTextFile(sFullFilePath.c_str());
	if(m_sCurSrcCode.empty())
	{
		m_sLogStr = "CreateShaderProgram - Vertex shader was not read correctly";
		return false;
	}

	// Create main program handle if one hasn't been created yet (first shader compile)
	if(m_hProgHandle <= 0)
	{
		m_hProgHandle = glCreateProgram();
		if(m_hProgHandle == 0)
		{
			m_sLogStr = "Unable to create shader program.";
			return false;
		}
	}

	return CompileFromString(m_sCurSrcCode.c_str(), eType);
}

bool HyGlfwShader::CompileFromString(const char *szSource, eGLSLShaderType type)
{
	// Create main program handle if one hasn't been created yet (first shader compile)
	if(m_hProgHandle <= 0)
	{
		m_hProgHandle = glCreateProgram();
		if( m_hProgHandle == 0) 
		{
			m_sLogStr = "Unable to create shader program.";
			return false;
		}
	}

	GLuint iShaderHandle = 0;

	switch( type )
	{
	case VERTEX:			iShaderHandle = glCreateShader(GL_VERTEX_SHADER);				break;
	case FRAGMENT:			iShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);				break;
	case GEOMETRY:			iShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);				break;
	case TESS_CONTROL:		iShaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);			break;
	case TESS_EVALUATION:	iShaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);		break;
	default:
		m_sLogStr = "Unknown shader type";
		return false;
	}

	// Compile the shader from the passed in source code
	glShaderSource(iShaderHandle, 1, &szSource, NULL);
	glCompileShader(iShaderHandle);

	// Check for errors
	GLint result;
	glGetShaderiv(iShaderHandle, GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result)
	{
		// Compile failed, store Log and return false
		GLint iLength = 0;
		m_sLogStr.clear();
		glGetShaderiv(iShaderHandle, GL_INFO_LOG_LENGTH, &iLength);
		if(iLength > 0)
		{
			char *szlog = new char[iLength];
			GLint written = 0;
			glGetShaderInfoLog(iShaderHandle, iLength, &written, szlog);
			HyError(szlog);

			m_sLogStr = szlog;
			delete [] szlog;
		}

		return false;
	}
	else
	{
		// Compile succeeded, attach shader and return true
		glAttachShader(m_hProgHandle, iShaderHandle);
		return true;
	}
}

bool HyGlfwShader::Link()
{
	if( m_bLinked )
		return true;

	if( m_hProgHandle <= 0 )
	{
		m_sLogStr = "Shader has not been created yet";
		return false;
	}

	glLinkProgram(m_hProgHandle);

	GLint status = 0;
	glGetProgramiv( m_hProgHandle, GL_LINK_STATUS, &status);
	if(GL_FALSE == status)
	{
		// Store Log and return false
		GLint length = 0;
		m_sLogStr.clear();

		glGetProgramiv(m_hProgHandle, GL_INFO_LOG_LENGTH, &length);

		if( length > 0 )
		{
			char *szlog = new char[length];
			GLint written = 0;
			glGetProgramInfoLog(m_hProgHandle, length, &written, szlog);

			m_sLogStr = szlog;
			delete [] szlog;
		}

		return false;
	}
	else
	{
		m_bLinked = true;
		return m_bLinked;
	}
}

void HyGlfwShader::Use()
{
	if(m_hProgHandle <= 0 || (! m_bLinked))
		return;

	glUseProgram(m_hProgHandle);
}

std::string HyGlfwShader::Log()
{
	return m_sLogStr;
}

GLint HyGlfwShader::GetHandle()
{
	return m_hProgHandle;
}

bool HyGlfwShader::IsLinked()
{
	return m_bLinked;
}

void HyGlfwShader::BindAttribLocation(GLuint location, const char *name)
{
	glBindAttribLocation(m_hProgHandle, location, name);
}

void HyGlfwShader::BindFragDataLocation(GLuint location, const char *name)
{
	glBindFragDataLocation(m_hProgHandle, location, name);
}

void HyGlfwShader::SetUniform(const char *name, float x, float y, float z)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if(loc >= 0)
		glUniform3f(loc,x,y,z);
}

void HyGlfwShader::SetUniform(const char *name, const vec3 &v)
{
	this->SetUniform(name, v.x, v.y, v.z);
}

void HyGlfwShader::SetUniform(const char *name, const vec4 &v)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if( loc >= 0 )
		glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void HyGlfwShader::SetUniform(const char *name, const mat4 &m)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if(loc >= 0)
		glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void HyGlfwShader::SetUniform(const char *name, const mat3 &m)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if(loc >= 0)
		glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void HyGlfwShader::SetUniform(const char *name, float val )
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if(loc >= 0)
		glUniform1f(loc, val);
}

void HyGlfwShader::SetUniform(const char *name, int32 val)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if(loc >= 0)
		glUniform1i(loc, val);
}

void HyGlfwShader::SetUniform(const char *name, uint32 val)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if(loc >= 0)
		glUniform1ui(loc, val);
}

void HyGlfwShader::SetUniform(const char *name, bool val)
{
	uint32 loc = glGetUniformLocation(m_hProgHandle, name);
	if(loc >= 0)
		glUniform1i(loc, val);
}

void HyGlfwShader::PrintActiveUniforms()
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

void HyGlfwShader::PrintActiveAttribs()
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
