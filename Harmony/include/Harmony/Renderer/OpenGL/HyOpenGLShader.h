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

#include "Assets/Loadables/IHyShader.h"

class HyOpenGLShader : public IHyShader
{
	GLuint								m_hVAO;

	GLsizei								m_uiStride;

	// Shader creation
	GLuint								m_hProgHandle;
	bool								m_bLinked;

public:
	HyOpenGLShader(int32 iId);
	HyOpenGLShader(int32 iId, std::string sPrefix, std::string sName);

	virtual HyOpenGLShader::~HyOpenGLShader();

	void CompileFromString(HyShaderType eType);
	void Link();
	void Use();
	void SetVertexAttributePtrs(size_t uiStartOffset);

	int GetHandle();
	bool IsLinked();

	void BindAttribLocation(GLuint location, const char *szName);
	uint32 GetAttribLocation(const char *szName);

	void BindFragDataLocation(GLuint location, const char *szName);

	void SetUniformGLSL(const char *szName, const glm::bvec2 &v);
	void SetUniformGLSL(const char *szName, const glm::bvec3 &v);
	void SetUniformGLSL(const char *szName, const glm::bvec4 &v);
	void SetUniformGLSL(const char *szName, const glm::ivec2 &v);
	void SetUniformGLSL(const char *szName, const glm::ivec3 &v);
	void SetUniformGLSL(const char *szName, const glm::ivec4 &v);
	void SetUniformGLSL(const char *szName, const glm::vec2 &v);
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

	virtual void OnSetVertexAttribute(const char *szName, uint32 uiLocation) override;
	virtual void OnUpload(IHyRenderer &rendererRef) override;
	virtual void OnDelete(IHyRenderer &rendererRef) override;
};

#endif /* __HyOpenGLShader_h__ */
