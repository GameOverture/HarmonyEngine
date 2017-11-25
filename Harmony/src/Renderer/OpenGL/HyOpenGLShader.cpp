///**************************************************************************
// *	HyOpenGLShader.cpp
// *	
// *	Harmony Engine
// *	Copyright (c) 2013 Jason Knobler
// *
// *	The zlib License (zlib)
// *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
//#include "Renderer/OpenGL/HyOpenGLShader.h"
//#include "Renderer/OpenGL/HyOpenGL.h"
//
//#include <vector>
//#include <map>
//#include <algorithm>
//
//HyOpenGLShader::HyOpenGLShader(int32 iId) :	HyShader(iId),
//											m_uiStride(0),
//											m_uiGlHandle(0),
//											m_bLinked(false)
//{
//}
//
//HyOpenGLShader::HyOpenGLShader(int32 iId, std::string sPrefix, std::string sName) :	HyShader(iId, sPrefix, sName),
//																					m_uiStride(0),
//																					m_uiGlHandle(0),
//																					m_bLinked(false)
//{
//}
//
//HyOpenGLShader::~HyOpenGLShader()
//{
//}
//
//GLuint HyOpenGLShader::GetGlHandle()
//{
//	return m_uiGlHandle;
//}
//
//bool HyOpenGLShader::IsLinked()
//{
//	return m_bLinked;
//}
//
//void HyOpenGLShader::BindAttribLocation(GLuint location, const char *szName)
//{
//
//}
//
//GLint HyOpenGLShader::GetAttribLocation(const char *szName)
//{
//	
//
//	return iRetVal;
//}
//
//void HyOpenGLShader::BindFragDataLocation(GLuint location, const char *szName)
//{
//	glBindFragDataLocation(m_uiGlHandle, location, szName);
//	HyErrorCheck_OpenGL("HyOpenGLShader::BindFragDataLocation", "glBindFragDataLocation");
//}
//
//void HyOpenGLShader::PrintActiveUniforms()
//{
//	GLint nUniforms, size, location, maxLen;
//	GLchar * name;
//	GLsizei written;
//	GLenum type;
//
//	glGetProgramiv( m_uiGlHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
//	glGetProgramiv( m_uiGlHandle, GL_ACTIVE_UNIFORMS, &nUniforms);
//
//	name = (GLchar *) malloc( maxLen );
//
//	printf(" Location | Name\n");
//	printf("------------------------------------------------\n");
//	for(GLint i = 0; i < nUniforms; ++i)
//	{
//		glGetActiveUniform( m_uiGlHandle, i, maxLen, &written, &size, &type, name );
//		location = glGetUniformLocation(m_uiGlHandle, name);
//		printf(" %-8d | %s\n",location, name);
//	}
//
//	free(name);
//}
//
//void HyOpenGLShader::PrintActiveAttribs()
//{
//	GLint written, size, location, maxLength, nAttribs;
//	GLenum type;
//	GLchar * name;
//
//	glGetProgramiv(m_uiGlHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
//	glGetProgramiv(m_uiGlHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);
//
//	name = (GLchar *) malloc( maxLength );
//
//	printf(" Index | Name\n");
//	printf("------------------------------------------------\n");
//	for(GLint i = 0; i < nAttribs; i++)
//	{
//		glGetActiveAttrib( m_uiGlHandle, i, maxLength, &written, &size, &type, name );
//		location = glGetAttribLocation(m_uiGlHandle, name);
//		printf(" %-5d | %s\n",location, name);
//	}
//
//	free(name);
//}
//
/////*virtual*/ void HyOpenGLShader::OnUpload(IHyRenderer &rendererRef)
////{
////	HyAssert(GetLoadableState() != HYLOADSTATE_Discarded, "HyOpenGLShader::OnRenderThread() invoked on a discarded shader");
////}
////
/////*virtual*/ void HyOpenGLShader::OnDelete(IHyRenderer &rendererRef)
////{
////	// Delete shader objects, shader program, and uniforms allocated in the constructor
////
////	// ... Delete shader objects, etc 
////	if(m_uiGlHandle > 0)
////		glDeleteProgram(m_uiGlHandle);
////}
