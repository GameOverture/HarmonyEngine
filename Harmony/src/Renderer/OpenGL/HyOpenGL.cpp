/**************************************************************************
 *	HyOpenGL.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/OpenGL/HyOpenGL.h"
#include "Diagnostics/HyConsole.h"

HyOpenGL::HyOpenGL(HyGfxComms &gfxCommsRef, std::vector<HyWindow *> &windowListRef) :	IHyRenderer(gfxCommsRef, windowListRef),
																						m_mtxView(1.0f)
{
}

HyOpenGL::~HyOpenGL(void)
{
}

/*virtual*/ bool HyOpenGL::Initialize() /*override*/
{
	//////////////////////////////////////////////////////////////////////////
	// Init GLEW
	//////////////////////////////////////////////////////////////////////////
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glewInit");
	}

	//const GLubyte *pExtStr = glGetString(GL_EXTENSIONS);
	//WriteTextFile("GLExtensions.txt", glGetString(GL_EXTENSIONS));

	if(glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else
	{
		HyError("At least OpenGL 3.3 must be supported");
	}

	HyLogTitle("OpenGL Initialization");
	HyLog("Vendor: " << glGetString(GL_VENDOR));
	HyLog("Renderer: " << glGetString(GL_RENDERER));
	HyLog("Version: " << glGetString(GL_VERSION));
	HyLog("GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// 2D setup
	//////////////////////////////////////////////////////////////////////////

	glGenBuffers(1, &m_hVBO2d);
	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);

	// Quad batch //////////////////////////////////////////////////////////////////////////
	HyOpenGLShader *pShaderQuadBatch = HY_NEW HyOpenGLShader(HYSHADERPROG_QuadBatch);
	sm_ShaderMap[HYSHADERPROG_QuadBatch] = pShaderQuadBatch;
	pShaderQuadBatch->Finalize(HYSHADERPROG_QuadBatch);
	pShaderQuadBatch->OnRenderThread(*this);

	// Primitive //////////////////////////////////////////////////////////////////////////
	HyOpenGLShader *pShaderPrimitive = HY_NEW HyOpenGLShader(HYSHADERPROG_Primitive);
	sm_ShaderMap[HYSHADERPROG_Primitive] = pShaderPrimitive;
	pShaderPrimitive->Finalize(HYSHADERPROG_Primitive);
	pShaderPrimitive->OnRenderThread(*this);

	// Line2D //////////////////////////////////////////////////////////////////////////
	HyOpenGLShader *pShaderLine2d = HY_NEW HyOpenGLShader(HYSHADERPROG_Lines2d);
	sm_ShaderMap[HYSHADERPROG_Lines2d] = pShaderLine2d;
	pShaderLine2d->Finalize(HYSHADERPROG_Lines2d);
	pShaderLine2d->OnRenderThread(*this);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glBlendFunc");

	// Line anti-aliasing on always for now.
	//glEnable(GL_LINE_SMOOTH);

	return true;
}

/*virtual*/ void HyOpenGL::StartRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*virtual*/ void HyOpenGL::Init_3d()
{
}

/*virtual*/ bool HyOpenGL::BeginPass_3d()
{
	return false;
}

/*virtual*/ void HyOpenGL::SetRenderState_3d(uint32 uiNewRenderState)
{
}

/*virtual*/ void HyOpenGL::End_3d()
{
}

/*virtual*/ void HyOpenGL::Init_2d()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);
	glBufferData(GL_ARRAY_BUFFER, m_pDrawBufferHeader->uiVertexBufferSize2d, GetVertexData2d(), GL_DYNAMIC_DRAW);

	m_iCurCamIndex = 0;
}

/*virtual*/ bool HyOpenGL::BeginPass_2d()
{
	int32 iNumCameras2d = GetNumCameras2d();
	int32 iNumRenderStates2d = GetNumRenderStates2d();

	// Only draw cameras that are apart of this render surface
	while(m_RenderSurfaceIter->m_iID != GetCameraWindowIndex2d(m_iCurCamIndex) && m_iCurCamIndex < iNumCameras2d)
		m_iCurCamIndex++;

	if(iNumRenderStates2d == 0 || m_iCurCamIndex >= iNumCameras2d)
		return false;
	
	// Without disabling glDepthMask, sprites fragments that overlap will be discarded, and primitive draws don't work
	glDepthMask(false);

	return true;
}

/*virtual*/ void HyOpenGL::DrawRenderState_2d(HyRenderState &renderState)
{
	if(m_PrevRenderState.CompareAttribute(renderState, HyRenderState::DRAWMODEMASK) == false)
	{
		if(renderState.IsEnabled(HyRenderState::DRAWMODE_TRIANGLESTRIP))
			m_eDrawMode = GL_TRIANGLE_STRIP;
		else if(renderState.IsEnabled(HyRenderState::DRAWMODE_LINELOOP))
			m_eDrawMode = GL_LINE_LOOP;
		else if(renderState.IsEnabled(HyRenderState::DRAWMODE_LINESTRIP))
			m_eDrawMode = GL_LINE_STRIP;
		else if(renderState.IsEnabled(HyRenderState::DRAWMODE_TRIANGLEFAN))
			m_eDrawMode = GL_TRIANGLE_FAN;
		else
		{
			HyError("Unknown draw mode in render state");
			return;
		}
	}

	glLineWidth(renderState.GetLineThickness());

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(renderState.IsEnabled(HyRenderState::USINGSCREENCOORDS))
		SetCameraMatrices_2d(MTX_SCREENVIEW);
	else
		SetCameraMatrices_2d(MTX_CAMVIEW);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	HyOpenGLShader *pShader = static_cast<HyOpenGLShader *>(sm_ShaderMap[renderState.GetShaderId()]);
	pShader->Use();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderState.GetTextureHandle());
	if(renderState.GetTextureHandle() != 0)
		pShader->SetUniformGLSL("Tex", 0);
	//glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(renderState.IsScissorRect())
	{
		const HyScreenRect<int32> &scissorRectRef = renderState.GetScissorRect();

		glScissor(static_cast<GLint>(m_mtxView[0].x * scissorRectRef.x) + static_cast<GLint>(m_mtxView[3].x) + (m_RenderSurfaceIter->m_iRenderSurfaceWidth / 2),
				  static_cast<GLint>(m_mtxView[1].y * scissorRectRef.y) + static_cast<GLint>(m_mtxView[3].y) + (m_RenderSurfaceIter->m_iRenderSurfaceHeight / 2),
				  static_cast<GLsizei>(m_mtxView[0].x * scissorRectRef.width),
				  static_cast<GLsizei>(m_mtxView[1].y * scissorRectRef.height));

		glEnable(GL_SCISSOR_TEST);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Always attempt to assign these uniforms if the shader chooses to use them
	pShader->SetUniformGLSL("u_mtxWorldToCamera", m_mtxView);
	pShader->SetUniformGLSL("u_mtxCameraToClip", m_mtxProj);
	//pShader->SetUniformGLSL("Tex", 0);//renderState.GetTextureHandle());

	char *pDrawBuffer = GetVertexData2d();
	uint32 uiDataOffset = static_cast<uint32>(renderState.GetDataOffset());
	pDrawBuffer += uiDataOffset;

	uint32 uiNumUniforms = *reinterpret_cast<uint32 *>(pDrawBuffer);
	pDrawBuffer += sizeof(uint32);
	uiDataOffset += sizeof(uint32);

	for(uint32 i = 0; i < uiNumUniforms; ++i)
	{
		const char *szUniformName = pDrawBuffer;
		size_t uiStrLen = strlen(szUniformName) + 1;	// +1 for NULL terminator
		pDrawBuffer += uiStrLen;
		uiDataOffset += static_cast<uint32>(uiStrLen);

		HyShaderVariable eVarType = static_cast<HyShaderVariable>(*reinterpret_cast<uint32 *>(pDrawBuffer));
		pDrawBuffer += sizeof(uint32);
		uiDataOffset += sizeof(uint32);

		switch(eVarType)
		{
		case HYSHADERVAR_bool:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<bool *>(pDrawBuffer));
			pDrawBuffer += sizeof(bool);
			uiDataOffset += sizeof(bool);
			break;
		case HYSHADERVAR_int:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<int32 *>(pDrawBuffer));
			pDrawBuffer += sizeof(int32);
			uiDataOffset += sizeof(int32);
			break;
		case HYSHADERVAR_uint:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<uint32 *>(pDrawBuffer));
			pDrawBuffer += sizeof(uint32);
			uiDataOffset += sizeof(uint32);
			break;
		case HYSHADERVAR_float:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<float *>(pDrawBuffer));
			pDrawBuffer += sizeof(float);
			uiDataOffset += sizeof(float);
			break;
		case HYSHADERVAR_double:
			HyError("GLSL Shader uniform does not support type double yet!");
			//pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<double *>(pDrawBuffer));
			//pDrawBuffer += sizeof(double);
			//uiDataOffset += sizeof(double);
			break;
		case HYSHADERVAR_bvec2:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::bvec2 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::bvec2);
			uiDataOffset += sizeof(glm::bvec2);
			break;
		case HYSHADERVAR_bvec3:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::bvec3 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::bvec3);
			uiDataOffset += sizeof(glm::bvec3);
			break;
		case HYSHADERVAR_bvec4:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::bvec4 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::bvec4);
			uiDataOffset += sizeof(glm::bvec4);
			break;
		case HYSHADERVAR_ivec2:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::ivec2 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::ivec2);
			uiDataOffset += sizeof(glm::ivec2);
			break;
		case HYSHADERVAR_ivec3:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::ivec3 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::ivec3);
			uiDataOffset += sizeof(glm::ivec3);
			break;
		case HYSHADERVAR_ivec4:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::ivec4 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::ivec4);
			uiDataOffset += sizeof(glm::ivec4);
			break;
		case HYSHADERVAR_vec2:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::vec2 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::vec2);
			uiDataOffset += sizeof(glm::vec2);
			break;
		case HYSHADERVAR_vec3:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::vec3 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::vec3);
			uiDataOffset += sizeof(glm::vec3);
			break;
		case HYSHADERVAR_vec4:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::vec4 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::vec4);
			uiDataOffset += sizeof(glm::vec4);
			break;
		case HYSHADERVAR_dvec2:
			HyError("GLSL Shader uniform does not support type double yet!");
			//pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::dvec2 *>(pDrawBuffer));
			//pDrawBuffer += sizeof(glm::dvec2);
			//uiDataOffset += sizeof(glm::dvec2);
			break;
		case HYSHADERVAR_dvec3:
			HyError("GLSL Shader uniform does not support type double yet!");
			//pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::dvec3 *>(pDrawBuffer));
			//pDrawBuffer += sizeof(glm::dvec3);
			//uiDataOffset += sizeof(glm::dvec3);
			break;
		case HYSHADERVAR_dvec4:
			HyError("GLSL Shader uniform does not support type double yet!");
			//pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::dvec4 *>(pDrawBuffer));
			//pDrawBuffer += sizeof(glm::dvec4);
			//uiDataOffset += sizeof(glm::dvec4);
			break;
		case HYSHADERVAR_mat3:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::mat3 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::mat3);
			uiDataOffset += sizeof(glm::mat3);
			break;
		case HYSHADERVAR_mat4:
			pShader->SetUniformGLSL(szUniformName, *reinterpret_cast<glm::mat4 *>(pDrawBuffer));
			pDrawBuffer += sizeof(glm::mat4);
			uiDataOffset += sizeof(glm::mat4);
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	pShader->SetVertexAttributePtrs(uiDataOffset);

	if(renderState.IsEnabled(HyRenderState::DRAWINSTANCED))
		glDrawArraysInstanced(m_eDrawMode, 0, renderState.GetNumVerticesPerInstance(), renderState.GetNumInstances());
	else
	{
		uint32 uiStartVertex = 0;
		for(uint32 i = 0; i < renderState.GetNumInstances(); ++i)
		{
			glDrawArrays(m_eDrawMode, uiStartVertex, renderState.GetNumVerticesPerInstance());
			uiStartVertex += renderState.GetNumVerticesPerInstance();
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*virtual*/ void HyOpenGL::End_2d()
{
	m_iCurCamIndex++;
	glDepthMask(true);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

/*virtual*/ void HyOpenGL::FinishRender()
{

}

/*virtual*/ uint32 HyOpenGL::AddTexture(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData)
{
	GLenum eInternalFormat = uiNumColorChannels == 4 ? GL_RGBA : (uiNumColorChannels == 3 ? GL_RGB : GL_ALPHA);
	GLenum eFormat = uiNumColorChannels == 4 ? GL_RGBA : (uiNumColorChannels == 3 ? GL_RGB : GL_RED);

	GLuint hGLTexture;
	glGenTextures(1, &hGLTexture);
	glBindTexture(GL_TEXTURE_2D, hGLTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST

	glTexImage2D(GL_TEXTURE_2D, 0, eInternalFormat, uiWidth, uiHeight, 0, eFormat, GL_UNSIGNED_BYTE, pPixelData);
	HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glTexImage2D");

	glBindTexture(GL_TEXTURE_2D, 0);

	return hGLTexture;
}

// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
/*virtual*/ uint32 HyOpenGL::AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut)
{
	GLenum eInternalFormat = uiNumColorChannels == 4 ? GL_RGBA8 : (uiNumColorChannels == 3 ? GL_RGB8 : GL_R8);
	GLenum eFormat = uiNumColorChannels == 4 ? GL_RGBA : (uiNumColorChannels == 3 ? GL_RGB : GL_RED);

	GLuint hGLTextureArray;
	glGenTextures(1, &hGLTextureArray);

	//glActiveTexture(GL_TEXTURE0 + hGLTextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, hGLTextureArray);

	// Create (blank) storage for the texture array
	GLenum eError = GL_NO_ERROR;
	uiNumTexturesUploadedOut = static_cast<uint32>(pixelDataList.size());

	// TODO: Don't upload huge texture arrays. Actually calculate required bytes, and then size array accordingly to hardware constraints
	if(uiNumTexturesUploadedOut > 8)
		uiNumTexturesUploadedOut = 8;

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, eInternalFormat, uiWidth, uiHeight, uiNumTexturesUploadedOut, 0, eFormat, GL_UNSIGNED_BYTE, NULL);
	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glTexImage3D");
	eError = glGetError();

	while (eError)
	{
		uiNumTexturesUploadedOut /= 2;
		if(uiNumTexturesUploadedOut == 0)
			HyError("Could not allocate texture array.");

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, eInternalFormat, uiWidth, uiHeight, uiNumTexturesUploadedOut, 0, eFormat, GL_UNSIGNED_BYTE, NULL);
		eError = glGetError();
	}

	for(uint32 i = 0; i != uiNumTexturesUploadedOut; ++i)
	{
		// Write each texture into storage
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
						0,						// Mipmap number
						0, 0, i,				// xoffset, yoffset, zoffset
						uiWidth, uiHeight, 1,	// width, height, depth (of texture you're copying in)
						eFormat,				// format
						GL_UNSIGNED_BYTE,		// type
						pixelDataList[i]);		// pointer to pixel data
	}

	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return hGLTextureArray;
}

/*virtual*/ void HyOpenGL::DeleteTextureArray(uint32 uiTextureHandle)
{
	glDeleteTextures(1, &uiTextureHandle);
}

/*virtual*/ void HyOpenGL::OnRenderSurfaceChanged(RenderSurface &renderSurfaceRef, uint32 uiChangedFlags)
{
}

void HyOpenGL::SetCameraMatrices_2d(eMatrixStack eMtxStack)
{
	HyRectangle<float> viewportRect;
	if(eMtxStack == MTX_CAMVIEW)
	{
		viewportRect = *GetCameraViewportRect2d(m_iCurCamIndex);
		m_mtxView = *GetCameraView2d(m_iCurCamIndex);
	}
	else
	{
		viewportRect.left = 0.0f;
		viewportRect.bottom = 0.0f;
		viewportRect.right = 1.0f;
		viewportRect.top = 1.0f;

		m_mtxView = glm::mat4(1.0f);
		m_mtxView = glm::translate(m_mtxView, m_RenderSurfaceIter->m_iRenderSurfaceWidth * -0.5f, m_RenderSurfaceIter->m_iRenderSurfaceHeight * -0.5f, 0.0f);
	}

	float fWidth = (viewportRect.Width() * m_RenderSurfaceIter->m_iRenderSurfaceWidth);
	float fHeight = (viewportRect.Height() * m_RenderSurfaceIter->m_iRenderSurfaceHeight);

	glViewport(static_cast<GLint>(viewportRect.left * m_RenderSurfaceIter->m_iRenderSurfaceWidth),
			   static_cast<GLint>(viewportRect.bottom * m_RenderSurfaceIter->m_iRenderSurfaceHeight),
			   static_cast<GLsizei>(fWidth),
			   static_cast<GLsizei>(fHeight));

	m_mtxProj = glm::ortho(fWidth * -0.5f, fWidth * 0.5f, fHeight * -0.5f, fHeight * 0.5f);
}
