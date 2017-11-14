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

HyOpenGLShader::HyOpenGLShader(int32 iId) :	IHyShader(iId),
											m_uiStride(0),
											m_hProgHandle(0),
											m_bLinked(false)
{
}

HyOpenGLShader::HyOpenGLShader(int32 iId, std::string sPrefix, std::string sName) :	IHyShader(iId, sPrefix, sName),
																					m_uiStride(0),
																					m_hProgHandle(0),
																					m_bLinked(false)
{
}

HyOpenGLShader::~HyOpenGLShader()
{
}

void HyOpenGLShader::CompileFromString(HyShaderType eType)
{
	// Create main program handle if one hasn't been created yet (first shader compile)
	if(m_hProgHandle <= 0)
	{
		m_hProgHandle = glCreateProgram();
		HyAssert(m_hProgHandle != 0, "Unable to create shader program");
		HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glCreateProgram");
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
	HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glCreateShader");

	// Compile the shader from the passed in source code
	const char *szSrc = m_sSourceCode[eType].c_str();
	glShaderSource(iShaderHandle, 1, &szSrc, NULL);
	HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glShaderSource");

	glCompileShader(iShaderHandle);
	HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glCompileShader");

#ifdef HY_DEBUG
	// Check for errors
	GLint result;
	glGetShaderiv(iShaderHandle, GL_COMPILE_STATUS, &result);
	HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glGetShaderiv");

	if(GL_FALSE == result)
	{
		// Compile failed
		GLint iLength = 0;
		glGetShaderiv(iShaderHandle, GL_INFO_LOG_LENGTH, &iLength);
		if(iLength > 0)
		{
			char *szlog = HY_NEW char[iLength];
			GLint written = 0;
			glGetShaderInfoLog(iShaderHandle, iLength, &written, szlog);
			
			HyError(szlog);
			delete [] szlog;	// Not that this matters
		}
	}
#endif
	
	// Compile succeeded, attach shader
	glAttachShader(m_hProgHandle, iShaderHandle);
	HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glAttachShader");
}

void HyOpenGLShader::Link()
{
	if(m_bLinked)
		return;

	HyAssert(m_hProgHandle > 0, "Shader has not been created yet");

	glLinkProgram(m_hProgHandle);
	HyErrorCheck_OpenGL("HyOpenGLShader::Link", "glLinkProgram");

#ifdef HY_DEBUG
	GLint status = 0;
	glGetProgramiv(m_hProgHandle, GL_LINK_STATUS, &status);
	if(GL_FALSE == status)
	{
		GLint length = 0;

		glGetProgramiv(m_hProgHandle, GL_INFO_LOG_LENGTH, &length);

		if( length > 0)
		{
			char *szlog = HY_NEW char[length];
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
	glEnableVertexAttribArray(location);
	HyErrorCheck_OpenGL("HyOpenGLShader::BindAttribLocation", "glEnableVertexAttribArray");

	glBindAttribLocation(m_hProgHandle, location, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::BindAttribLocation", "glBindAttribLocation");
}

GLint HyOpenGLShader::GetAttribLocation(const char *szName)
{
	GLint iRetVal = glGetAttribLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::GetAttribLocation", "glGetAttribLocation");

	return iRetVal;
}

void HyOpenGLShader::BindFragDataLocation(GLuint location, const char *szName)
{
	glBindFragDataLocation(m_hProgHandle, location, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::BindFragDataLocation", "glBindFragDataLocation");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::bvec2 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform2i(loc, v.x, v.y);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::bvec3 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform3i(loc, v.x, v.y, v.z);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::bvec4 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");
	
	glUniform4i(loc, v.x, v.y, v.z, v.w);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::ivec2 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform2i(loc, v.x, v.y);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::ivec3 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform3i(loc, v.x, v.y, v.z);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::ivec4 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform4i(loc, v.x, v.y, v.z, v.w);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::vec2 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform2f(loc, v.x, v.y);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::vec3 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform3f(loc, v.x, v.y, v.z);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::vec4 &v)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform4f(loc, v.x, v.y, v.z, v.w);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::mat4 &m)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix4fv");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, const glm::mat3 &m)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix3fv");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, float val)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform1f(loc, val);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1f");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, int32 val)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform1i(loc, val);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1i");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, uint32 val)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	HyAssert(loc >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szName << "\"");

	glUniform1ui(loc, val);
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1ui");
}

void HyOpenGLShader::SetUniformGLSL(const char *szName, bool val)
{
	GLint loc = glGetUniformLocation(m_hProgHandle, szName);
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

/*virtual*/ void HyOpenGLShader::OnSetVertexAttribute(const char *szName, uint32 uiLocation)
{
	BindAttribLocation(uiLocation, szName);
}

/*virtual*/ void HyOpenGLShader::OnUpload(IHyRenderer &rendererRef)
{
	HyAssert(GetLoadableState() != HYLOADSTATE_Discarded, "HyOpenGLShader::OnRenderThread() invoked on a discarded shader");

	HyOpenGL &gl = static_cast<HyOpenGL &>(rendererRef);

	gl.GenVAOs(this);

#ifdef HY_DEBUG
	GLint iMaxVertexAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &iMaxVertexAttribs);

	int32 iTotalVertexAttribs = 0;
	for(uint32 i = 0; i < m_VertexAttributeList.size(); ++i)
	{
		if(m_VertexAttributeList[i].eVarType == HYSHADERVAR_dvec2 || m_VertexAttributeList[i].eVarType == HYSHADERVAR_dvec3 || m_VertexAttributeList[i].eVarType == HYSHADERVAR_dvec4)
			iTotalVertexAttribs += 2;
		else if(m_VertexAttributeList[i].eVarType == HYSHADERVAR_mat3)
			iTotalVertexAttribs += 3;
		else if(m_VertexAttributeList[i].eVarType == HYSHADERVAR_mat4)
			iTotalVertexAttribs += 4;
		else
			iTotalVertexAttribs += 1;
	}

	HyAssert(iMaxVertexAttribs >= iTotalVertexAttribs, "GL_MAX_VERTEX_ATTRIBS is < " << iTotalVertexAttribs);
#endif

	CompileFromString(HYSHADER_Vertex);
	CompileFromString(HYSHADER_Fragment);

	// TODO: Explicitly bind 
	for(uint32 i = 0; i < m_VertexAttributeList.size(); ++i)
		BindAttribLocation(i, m_VertexAttributeList[i].sName.c_str());

	Link();
	
	for(uint32 i = 0; i < gl.GetNumWindows(); ++i)
	{
		gl.SetCurrentWindow(i);
		gl.BindVao(this);

		m_uiStride = 0;

		for(uint32 i = 0; i < m_VertexAttributeList.size(); ++i)
		{
			GLuint uiLocation = GetAttribLocation(m_VertexAttributeList[i].sName.c_str());

			if(m_VertexAttributeList[i].eVarType == HYSHADERVAR_dvec2 || m_VertexAttributeList[i].eVarType == HYSHADERVAR_dvec3 || m_VertexAttributeList[i].eVarType == HYSHADERVAR_dvec4)
			{
				HyError("HYSHADERVAR_dvec2, HYSHADERVAR_dvec3, or HYSHADERVAR_dvec4 is not implemented");

				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);

				glVertexAttribDivisor(uiLocation + 0, m_VertexAttributeList[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 1, m_VertexAttributeList[i].uiInstanceDivisor);
			}
			else if(m_VertexAttributeList[i].eVarType == HYSHADERVAR_mat3)
			{
				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);
				glEnableVertexAttribArray(uiLocation + 2);

				glVertexAttribDivisor(uiLocation + 0, m_VertexAttributeList[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 1, m_VertexAttributeList[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 2, m_VertexAttributeList[i].uiInstanceDivisor);
			}
			else if(m_VertexAttributeList[i].eVarType == HYSHADERVAR_mat4)
			{
				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);
				glEnableVertexAttribArray(uiLocation + 2);
				glEnableVertexAttribArray(uiLocation + 3);

				glVertexAttribDivisor(uiLocation + 0, m_VertexAttributeList[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 1, m_VertexAttributeList[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 2, m_VertexAttributeList[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 3, m_VertexAttributeList[i].uiInstanceDivisor);
			}
			else
			{
				glEnableVertexAttribArray(uiLocation);
				glVertexAttribDivisor(uiLocation, m_VertexAttributeList[i].uiInstanceDivisor);
			}

			HyErrorCheck_OpenGL("HyOpenGLShader::OnUpload", "glEnableVertexAttribArray or glVertexAttribDivisor");

			switch(m_VertexAttributeList[i].eVarType)
			{
			case HYSHADERVAR_bool:		m_uiStride += sizeof(bool);			break;
			case HYSHADERVAR_int:		m_uiStride += sizeof(int32);		break;
			case HYSHADERVAR_uint:		m_uiStride += sizeof(uint32);		break;
			case HYSHADERVAR_float:		m_uiStride += sizeof(float);		break;
			case HYSHADERVAR_double:	m_uiStride += sizeof(double);		break;
			case HYSHADERVAR_bvec2:		m_uiStride += sizeof(glm::bvec2);	break;
			case HYSHADERVAR_bvec3:		m_uiStride += sizeof(glm::bvec3);	break;
			case HYSHADERVAR_bvec4:		m_uiStride += sizeof(glm::bvec4);	break;
			case HYSHADERVAR_ivec2:		m_uiStride += sizeof(glm::ivec2);	break;
			case HYSHADERVAR_ivec3:		m_uiStride += sizeof(glm::ivec3);	break;
			case HYSHADERVAR_ivec4:		m_uiStride += sizeof(glm::ivec4);	break;
			case HYSHADERVAR_vec2:		m_uiStride += sizeof(glm::vec2);	break;
			case HYSHADERVAR_vec3:		m_uiStride += sizeof(glm::vec3);	break;
			case HYSHADERVAR_vec4:		m_uiStride += sizeof(glm::vec4);	break;
			case HYSHADERVAR_dvec2:		m_uiStride += sizeof(glm::dvec2);	break;
			case HYSHADERVAR_dvec3:		m_uiStride += sizeof(glm::dvec3);	break;
			case HYSHADERVAR_dvec4:		m_uiStride += sizeof(glm::dvec4);	break;
			case HYSHADERVAR_mat3:		m_uiStride += sizeof(glm::mat3);	break;
			case HYSHADERVAR_mat4:		m_uiStride += sizeof(glm::mat4);	break;
			}
		}

		glBindVertexArray(0);
	}

	////////////////////////////////////////////////////////////////////////////
	//// TODO: These would be nicer to use if OpenGL 4.3 was supported
	//glVertexAttribFormat(size,			2, GL_FLOAT, GL_FALSE, 0);
	//glVertexAttribBinding(size, QUADBATCH);
	//glVertexAttribFormat(offset,		2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat));
	//glVertexAttribBinding(offset, QUADBATCH);
	//glVertexAttribFormat(tint,			4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat));
	//glVertexAttribBinding(tint, QUADBATCH);
	//glVertexAttribFormat(textureIndex,	1, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat));
	//glVertexAttribBinding(textureIndex, QUADBATCH);
	//glVertexAttribFormat(uv0,			2, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat));
	//glVertexAttribBinding(uv0, QUADBATCH);
	//glVertexAttribFormat(uv1,			2, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat));
	//glVertexAttribBinding(uv1, QUADBATCH);
	//glVertexAttribFormat(uv2,			2, GL_FLOAT, GL_FALSE, 13*sizeof(GLfloat));
	//glVertexAttribBinding(uv2, QUADBATCH);
	//glVertexAttribFormat(uv3,			2, GL_FLOAT, GL_FALSE, 15*sizeof(GLfloat));
	//glVertexAttribBinding(uv3, QUADBATCH);
	//glVertexAttribFormat(mtx+0,			4, GL_FLOAT, GL_FALSE, 17*sizeof(GLfloat));
	//glVertexAttribBinding(mtx+0, QUADBATCH);
	//glVertexAttribFormat(mtx+1,			4, GL_FLOAT, GL_FALSE, 21*sizeof(GLfloat));
	//glVertexAttribBinding(mtx+1, QUADBATCH);
	//glVertexAttribFormat(mtx+2,			4, GL_FLOAT, GL_FALSE, 25*sizeof(GLfloat));
	//glVertexAttribBinding(mtx+2, QUADBATCH);
	//glVertexAttribFormat(mtx+3,			4, GL_FLOAT, GL_FALSE, 29*sizeof(GLfloat));
	//glVertexAttribBinding(mtx+3, QUADBATCH);
	////////////////////////////////////////////////////////////////////////////
}

/*virtual*/ void HyOpenGLShader::OnDelete(IHyRenderer &rendererRef)
{
	// Delete shader objects, shader program, and uniforms allocated in the constructor

	// ... Delete shader objects, etc 
	if(m_hProgHandle > 0)
		glDeleteProgram(m_hProgHandle);
}
