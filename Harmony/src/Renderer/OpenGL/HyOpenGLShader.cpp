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
#include "Renderer/OpenGL/HyOpenGL.h"

#include <vector>
#include <map>
#include <algorithm>

HyOpenGLShader::HyOpenGLShader(int32 iId) :	HyShader(iId),
											m_uiStride(0),
											m_uiGlHandle(0),
											m_bLinked(false)
{
}

HyOpenGLShader::HyOpenGLShader(int32 iId, std::string sPrefix, std::string sName) :	HyShader(iId, sPrefix, sName),
																					m_uiStride(0),
																					m_uiGlHandle(0),
																					m_bLinked(false)
{
}

HyOpenGLShader::~HyOpenGLShader()
{
}

void HyOpenGLShader::Link()
{
	if(m_bLinked)
		return;

	HyAssert(m_uiGlHandle > 0, "Shader has not been created yet");

	glLinkProgram(m_uiGlHandle);
	HyErrorCheck_OpenGL("HyOpenGLShader::Link", "glLinkProgram");

#ifdef HY_DEBUG
	GLint status = 0;
	glGetProgramiv(m_uiGlHandle, GL_LINK_STATUS, &status);
	if(GL_FALSE == status)
	{
		GLint length = 0;

		glGetProgramiv(m_uiGlHandle, GL_INFO_LOG_LENGTH, &length);

		if( length > 0)
		{
			char *szlog = HY_NEW char[length];
			GLint written = 0;
			glGetProgramInfoLog(m_uiGlHandle, length, &written, szlog);

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
	if(m_uiGlHandle <= 0 || (! m_bLinked))
		return;

	glUseProgram(m_uiGlHandle);
	HyErrorCheck_OpenGL("HyOpenGLShader::Use", "glUseProgram");
}

void HyOpenGLShader::SetVertexAttributePtrs(size_t uiStartOffset)
{

#if 0 // TODO: if OpenGL 4.3 is available
	glBindVertexBuffer(QUADBATCH, m_hVBO2d, uiDataOffset, 132);
#else

	size_t uiOffset = 0;
	for(size_t i = 0; i < m_VertexAttributeList.size(); ++i)
	{
		GLuint uiLocation = GetAttribLocation(m_VertexAttributeList[i].sName.c_str());

		switch(m_VertexAttributeList[i].eVarType)
		{
		case HYSHADERVAR_bool:
			glVertexAttribPointer(uiLocation, 1, GL_BYTE, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLboolean);
			break;
		case HYSHADERVAR_int:
			glVertexAttribPointer(uiLocation, 1, GL_INT, m_VertexAttributeList[i].bNormalized, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLint);
			break;
		case HYSHADERVAR_uint:
			glVertexAttribPointer(uiLocation, 1, GL_UNSIGNED_INT, m_VertexAttributeList[i].bNormalized, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLuint);
			break;
		case HYSHADERVAR_float:
			glVertexAttribPointer(uiLocation, 1, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLfloat);
			break;
		case HYSHADERVAR_double:
			glVertexAttribLPointer(uiLocation, 1, GL_DOUBLE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLdouble);
			break;
		case HYSHADERVAR_bvec2:
			glVertexAttribPointer(uiLocation, 2, GL_BYTE, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::bvec2);
			break;
		case HYSHADERVAR_bvec3:
			glVertexAttribPointer(uiLocation, 3, GL_BYTE, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::bvec3);
			break;
		case HYSHADERVAR_bvec4:
			glVertexAttribPointer(uiLocation, 4, GL_BYTE, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::bvec4);
			break;
		case HYSHADERVAR_ivec2:
			glVertexAttribPointer(uiLocation, 2, GL_INT, m_VertexAttributeList[i].bNormalized ? GL_TRUE : GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::ivec2);
			break;
		case HYSHADERVAR_ivec3:
			glVertexAttribPointer(uiLocation, 3, GL_INT, m_VertexAttributeList[i].bNormalized ? GL_TRUE : GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::ivec3);
			break;
		case HYSHADERVAR_ivec4:
			glVertexAttribPointer(uiLocation, 4, GL_INT, m_VertexAttributeList[i].bNormalized ? GL_TRUE : GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::ivec4);
			break;
		case HYSHADERVAR_vec2:
			glVertexAttribPointer(uiLocation, 2, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec2);
			break;
		case HYSHADERVAR_vec3:
			glVertexAttribPointer(uiLocation, 3, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			break;
		case HYSHADERVAR_vec4:
			glVertexAttribPointer(uiLocation, 4, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			break;
		case HYSHADERVAR_dvec2:
			glVertexAttribLPointer(uiLocation, 2, GL_DOUBLE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::dvec2);
			break;
		case HYSHADERVAR_dvec3:
			glVertexAttribLPointer(uiLocation, 3, GL_DOUBLE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::dvec3);
			break;
		case HYSHADERVAR_dvec4:
			glVertexAttribLPointer(uiLocation, 4, GL_DOUBLE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::dvec4);
			break;
		case HYSHADERVAR_mat3:
			glVertexAttribPointer(uiLocation, 3, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			glVertexAttribPointer(uiLocation + 1, 3, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			glVertexAttribPointer(uiLocation + 2, 3, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			break;
		case HYSHADERVAR_mat4:
			glVertexAttribPointer(uiLocation, 4, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			glVertexAttribPointer(uiLocation + 1, 4, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			glVertexAttribPointer(uiLocation + 2, 4, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			glVertexAttribPointer(uiLocation + 3, 4, GL_FLOAT, GL_FALSE, m_uiStride, reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			break;
		}

		HyErrorCheck_OpenGL("HyOpenGLShader::SetVertexAttributePtrs", "glVertexAttribPointer[" << m_VertexAttributeList[i].eVarType << "]");
	}
#endif
}

GLuint HyOpenGLShader::GetGlHandle()
{
	return m_uiGlHandle;
}

bool HyOpenGLShader::IsLinked()
{
	return m_bLinked;
}

void HyOpenGLShader::BindAttribLocation(GLuint location, const char *szName)
{
	glEnableVertexAttribArray(location);
	HyErrorCheck_OpenGL("HyOpenGLShader::BindAttribLocation", "glEnableVertexAttribArray");

	glBindAttribLocation(m_uiGlHandle, location, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::BindAttribLocation", "glBindAttribLocation");
}

GLint HyOpenGLShader::GetAttribLocation(const char *szName)
{
	GLint iRetVal = glGetAttribLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::GetAttribLocation", "glGetAttribLocation");

	return iRetVal;
}

void HyOpenGLShader::BindFragDataLocation(GLuint location, const char *szName)
{
	glBindFragDataLocation(m_uiGlHandle, location, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::BindFragDataLocation", "glBindFragDataLocation");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::bvec2 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform2i(loc, v.x, v.y);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::bvec3 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform3i(loc, v.x, v.y, v.z);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::bvec4 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");
	
	glUniform4i(loc, v.x, v.y, v.z, v.w);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::ivec2 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform2i(loc, v.x, v.y);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::ivec3 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform3i(loc, v.x, v.y, v.z);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::ivec4 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform4i(loc, v.x, v.y, v.z, v.w);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::vec2 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform2f(loc, v.x, v.y);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::vec3 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform3f(loc, v.x, v.y, v.z);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::vec4 &v)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform4f(loc, v.x, v.y, v.z, v.w);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::mat4 &m)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix4fv");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::mat3 &m)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix3fv");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, float val)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform1f(loc, val);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, int32 val)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform1i(loc, val);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, uint32 val)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform1ui(loc, val);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1ui");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, bool val)
{
	GLint loc = glGetUniformLocation(m_uiGlHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform1i(loc, val);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1i");
}

void HyOpenGLShader::PrintActiveUniforms()
{
	GLint nUniforms, size, location, maxLen;
	GLchar * name;
	GLsizei written;
	GLenum type;

	glGetProgramiv( m_uiGlHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv( m_uiGlHandle, GL_ACTIVE_UNIFORMS, &nUniforms);

	name = (GLchar *) malloc( maxLen );

	printf(" Location | Name\n");
	printf("------------------------------------------------\n");
	for(GLint i = 0; i < nUniforms; ++i)
	{
		glGetActiveUniform( m_uiGlHandle, i, maxLen, &written, &size, &type, name );
		location = glGetUniformLocation(m_uiGlHandle, name);
		printf(" %-8d | %s\n",location, name);
	}

	free(name);
}

void HyOpenGLShader::PrintActiveAttribs()
{
	GLint written, size, location, maxLength, nAttribs;
	GLenum type;
	GLchar * name;

	glGetProgramiv(m_uiGlHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	glGetProgramiv(m_uiGlHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);

	name = (GLchar *) malloc( maxLength );

	printf(" Index | Name\n");
	printf("------------------------------------------------\n");
	for(GLint i = 0; i < nAttribs; i++)
	{
		glGetActiveAttrib( m_uiGlHandle, i, maxLength, &written, &size, &type, name );
		location = glGetAttribLocation(m_uiGlHandle, name);
		printf(" %-5d | %s\n",location, name);
	}

	free(name);
}

/*virtual*/ void HyOpenGLShader::OnUpload(IHyRenderer &rendererRef)
{
	HyAssert(GetLoadableState() != HYLOADSTATE_Discarded, "HyOpenGLShader::OnRenderThread() invoked on a discarded shader");
}

/*virtual*/ void HyOpenGLShader::OnDelete(IHyRenderer &rendererRef)
{
	// Delete shader objects, shader program, and uniforms allocated in the constructor

	// ... Delete shader objects, etc 
	if(m_uiGlHandle > 0)
		glDeleteProgram(m_uiGlHandle);
}
