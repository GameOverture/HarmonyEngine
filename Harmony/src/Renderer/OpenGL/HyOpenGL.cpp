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
#include "Renderer/Components/HyRenderSurface.h"
#include "Renderer/Components/HyWindow.h"
#include "Diagnostics/Console/HyConsole.h"

HyOpenGL::HyOpenGL(HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef) :	IHyRenderer(diagnosticsRef, bShowCursor, windowListRef),
																												m_mtxView(1.0f)
{
	for(int i = 0; i < m_WindowListRef.size(); ++i)
		m_VaoMapList.push_back(std::map<HyOpenGLShader *, uint32>());

	HyLog("OpenGL is initializing...");

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		SetCurrentWindow(i);

		glewExperimental = GL_TRUE;	// This is required for GLFW to work
		GLenum err = glewInit();
		if(err != GLEW_OK) {
			HyError("glewInit() failed: " << err);
		}
		else {
			// Flush the OpenGL error state, as glew is known to bork it
			while(GL_NO_ERROR != glGetError());
		}
		HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glewInit");

		glEnable(GL_DEPTH_TEST);
		HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glEnable");

		glEnable(GL_BLEND);
		HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glEnable");

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glBlendFunc");

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	}

	if(m_WindowListRef.empty() == false)
	{
		SetCurrentWindow(0);
		if(glewIsSupported("GL_VERSION_3_3") == false) {
			HyError("At least OpenGL 3.3 must be supported");
		}
	}
	else
		HyLog("No windows created to render to");	

	GLint iMaxTextureSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxTextureSize);
	GLint iFormatCount = 0;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &iFormatCount);
	GLint *pFormatArray = HY_NEW GLint[iFormatCount];
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, pFormatArray);
	std::string sCompressedTextureFormats;
	for(int32 i = 0; i < iFormatCount; ++i)
	{
		switch(pFormatArray[i])
		{
			//case GL_COMPRESSED_RGBA_BPTC_UNORM:					sCompressedTextureFormats += "DXT_BC7";	break;
			//case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:		sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB:		sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB:		sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_RGB8_ETC2:						sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_SRGB8_ETC2:						sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:	sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:	sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_RGBA8_ETC2_EAC:					sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:			sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_R11_EAC:							sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_SIGNED_R11_EAC:					sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_RG11_EAC:						sCompressedTextureFormats += "";	break;
			//case GL_COMPRESSED_SIGNED_RG11_EAC:					sCompressedTextureFormats += "";	break;
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			sCompressedTextureFormats += "RGB_DXT1 ";
			m_uiSupportedTextureFormats |= HYTEXTURE_RGB_DTX1;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			sCompressedTextureFormats += "RGBA_DXT1 ";
			m_uiSupportedTextureFormats |= HYTEXTURE_RGB_DTX1;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			sCompressedTextureFormats += "DXT3 ";
			m_uiSupportedTextureFormats |= HYTEXTURE_DTX3;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			sCompressedTextureFormats += "DXT5 ";
			m_uiSupportedTextureFormats |= HYTEXTURE_DTX5;
			break;
		}
	}
	delete[] pFormatArray;

	SetRendererInfo("OpenGL",
		reinterpret_cast<const char *>(glGetString(GL_VERSION)),
		reinterpret_cast<const char *>(glGetString(GL_VENDOR)),
		reinterpret_cast<const char *>(glGetString(GL_RENDERER)),
		reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)),
		iMaxTextureSize,
		sCompressedTextureFormats);

	glEnable(GL_DEPTH_TEST);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glEnable");

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// 2D setup
	glGenBuffers(1, &m_hVBO2d);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glGenBuffers");

	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glBindBuffer");

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
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glEnable");

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glBlendFunc");
}

HyOpenGL::~HyOpenGL(void)
{
}

void HyOpenGL::GenVAOs(HyOpenGLShader *pShaderKey)
{
	for(int i = 0; i < m_WindowListRef.size(); ++i)
	{
		if(m_VaoMapList[i].find(pShaderKey) == m_VaoMapList[i].end())
		{
			SetCurrentWindow(i);

			m_VaoMapList[i][pShaderKey] = 0;
			glGenVertexArrays(1, &m_VaoMapList[i][pShaderKey]);
			HyErrorCheck_OpenGL("HyOpenGLShader::OnUpload", "glGenVertexArrays");
		}
	}
}

void HyOpenGL::BindVao(HyOpenGLShader *pShaderKey)
{
	uint32 uiVao = m_VaoMapList[m_pCurWindow->GetIndex()][pShaderKey];
	glBindVertexArray(uiVao);
	HyErrorCheck_OpenGL("HyOpenGLShader::Use", "glBindVertexArray");
}

/*virtual*/ void HyOpenGL::SetCurrentWindow(uint32 uiIndex)
{
	IHyRenderer::SetCurrentWindow(uiIndex);

#ifdef HY_PLATFORM_DESKTOP
	glfwMakeContextCurrent(m_pCurWindow->GetHandle());
#endif
}

/*virtual*/ void HyOpenGL::StartRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	HyErrorCheck_OpenGL("HyOpenGL:StartRender", "glClear");
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
	HyErrorCheck_OpenGL("HyOpenGL:Init_2d", "glBindBuffer");

	glBufferData(GL_ARRAY_BUFFER, HYDRAWBUFFERHEADER->uiVertexBufferSize2d, GetVertexData2d(), GL_DYNAMIC_DRAW);
	HyErrorCheck_OpenGL("HyOpenGL:Init_2d", "glBufferData");

	m_iCurCamIndex = 0;
}

/*virtual*/ bool HyOpenGL::BeginPass_2d()
{
	int32 iNumCameras2d = GetNumCameras2d();
	int32 iNumRenderStates2d = GetNumRenderStates2d();

	// Only draw cameras that are apart of this HyWindow
	while(m_pCurWindow->GetIndex() != GetCameraWindowId2d(m_iCurCamIndex) && m_iCurCamIndex < iNumCameras2d)
		m_iCurCamIndex++;

	if(iNumRenderStates2d == 0 || m_iCurCamIndex >= iNumCameras2d)
		return false;
	
	// TODO: Without disabling glDepthMask, sprites fragments that overlap will be discarded, and primitive draws don't work
	glDepthMask(false);
	HyErrorCheck_OpenGL("HyOpenGL:BeginPass_2d", "glDepthMask");

	return true;
}

/*virtual*/ void HyOpenGL::DrawRenderState_2d(HyRenderState &renderState)
{
	switch(renderState.GetRenderMode())
	{
	case HYRENDERMODE_Triangles:		m_eDrawMode = GL_TRIANGLES;			break;
	case HYRENDERMODE_TriangleStrip:	m_eDrawMode = GL_TRIANGLE_STRIP;	break;
	case HYRENDERMODE_TriangleFan:		m_eDrawMode = GL_TRIANGLE_FAN;		break;
	case HYRENDERMODE_LineLoop:			m_eDrawMode = GL_LINE_LOOP;			break;
	case HYRENDERMODE_LineStrip:		m_eDrawMode = GL_LINE_STRIP;		break;

	default:
		HyError("Unknown draw mode in render state");
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SetCameraMatrices_2d(renderState.IsUsingCameraCoordinates());
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HyOpenGLShader *pShader = static_cast<HyOpenGLShader *>(sm_ShaderMap[renderState.GetShaderId()]);
	BindVao(pShader);
	pShader->Use();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glActiveTexture(GL_TEXTURE0);
	HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glActiveTexture");

	if(HYSHADERPROG_QuadBatch == renderState.GetShaderId())
	{
		glBindTexture(GL_TEXTURE_2D, renderState.GetTextureHandle());
		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glBindTexture");

		if(renderState.GetTextureHandle() != 0)
			pShader->SetUniformGLSL("Tex", 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glBindTexture");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(renderState.IsScissorRect())
	{
		const HyScreenRect<int32> &scissorRectRef = renderState.GetScissorRect();

		glScissor(static_cast<GLint>(m_mtxView[0].x * scissorRectRef.x) + static_cast<GLint>(m_mtxView[3].x) + (m_pCurWindow->GetFramebufferSize().x / 2),
			static_cast<GLint>(m_mtxView[1].y * scissorRectRef.y) + static_cast<GLint>(m_mtxView[3].y) + (m_pCurWindow->GetFramebufferSize().y / 2),
				  static_cast<GLsizei>(m_mtxView[0].x * scissorRectRef.width),
				  static_cast<GLsizei>(m_mtxView[1].y * scissorRectRef.height));

		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glScissor");

		glEnable(GL_SCISSOR_TEST);
		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glEnable");
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glDisable");
	}

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
	{
		glDrawArraysInstanced(m_eDrawMode, 0, renderState.GetNumVerticesPerInstance(), renderState.GetNumInstances());
		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glDrawArraysInstanced");
	}
	else
	{
		uint32 uiStartVertex = 0;
		for(uint32 i = 0; i < renderState.GetNumInstances(); ++i)
		{
			glDrawArrays(m_eDrawMode, uiStartVertex, renderState.GetNumVerticesPerInstance());
			HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glDrawArrays");

			uiStartVertex += renderState.GetNumVerticesPerInstance();
		}
	}

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*virtual*/ void HyOpenGL::End_2d()
{
	m_iCurCamIndex++;
	glDepthMask(true);	// TODO: Get rid of this once you implement proper depth

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

/*virtual*/ void HyOpenGL::FinishRender()
{
#ifdef HY_PLATFORM_DESKTOP
	// This function will block if glfwSwapInterval is set to '1' (AKA VSync enabled)
	glfwSwapBuffers(m_pCurWindow->GetHandle());
#endif
}

/*virtual*/ uint32 HyOpenGL::AddTexture(HyTextureFormat eDesiredFormat, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat)
{
	GLenum eInternalFormat = GL_RGBA;
	switch(eDesiredFormat)
	{
		case HYTEXTURE_R8G8B8A8: {
			eInternalFormat = GL_RGBA;
		} break;
		case HYTEXTURE_R8G8B8: {
			eInternalFormat = GL_RGB;
		} break;
		case HYTEXTURE_RGB_DTX1: {
			eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_RGB_DTX1)) ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGB;
		} break;
		case HYTEXTURE_RGBA_DTX1: {
			eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_RGBA_DTX1)) ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA;
		} break;
		case HYTEXTURE_DTX3: {
			eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_DTX3)) ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : GL_COMPRESSED_RGBA;
		} break;
		case HYTEXTURE_DTX5: {
			eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_DTX5)) ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA;
		} break;
		default: {
			HyLogError("Unknown TextureFormat used for 'eDesiredFormat'");
		} break;
	}

	GLenum eFormat = GL_RGBA;
	bool bIsPixelDataCompressed = false;
	switch(ePixelDataFormat)
	{
		case HYTEXTURE_R8G8B8A8: {
			eFormat = GL_RGBA;
		} break;
		case HYTEXTURE_R8G8B8: {
			eFormat = GL_RGB;
		} break;
		case HYTEXTURE_RGB_DTX1:
		case HYTEXTURE_RGBA_DTX1:
		case HYTEXTURE_DTX3:
		case HYTEXTURE_DTX5: {
			bIsPixelDataCompressed = true;
		} break;
		default: {
			HyLogError("Unknown TextureFormat used for 'ePixelDataFormat'");
		} break;
	}

	GLuint hGLTexture;
	glGenTextures(1, &hGLTexture);
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glGenTextures");

	glBindTexture(GL_TEXTURE_2D, hGLTexture);
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glBindTexture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glTexParameteri");

	if(bIsPixelDataCompressed == false)
	{
		glTexImage2D(GL_TEXTURE_2D, iNumLodLevels, eInternalFormat, uiWidth, uiHeight, 0, eFormat, GL_UNSIGNED_BYTE, pPixelData);
		HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glTexImage2D");
	}
	else
	{
		// TODO: Fix hardcoded size value (will need to retrieve the actual size from HyAtlas::OnLoadThread)
		glCompressedTexImage2D(GL_TEXTURE_2D, iNumLodLevels, eInternalFormat, uiWidth, uiHeight, 0, uiPixelDataSize, pPixelData);
		HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glCompressedTexImage2D");
	}

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
	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glGenTextures");

	//glActiveTexture(GL_TEXTURE0 + hGLTextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, hGLTextureArray);
	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glBindTexture");

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

		HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glTexSubImage3D");
	}

	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glTexParameteri");

	return hGLTextureArray;
}

/*virtual*/ void HyOpenGL::DeleteTexture(uint32 uiTextureHandle)
{
	glDeleteTextures(1, &uiTextureHandle);
	HyErrorCheck_OpenGL("HyOpenGL:DeleteTexture", "glDeleteTextures");
}

void HyOpenGL::SetCameraMatrices_2d(bool bUseCameraView)
{
	glm::ivec2 vFramebufferSize = m_pCurWindow->GetFramebufferSize();

	HyRectangle<float> viewportRect;
	if(bUseCameraView)
	{
		viewportRect = *GetCameraViewportRect2d(m_iCurCamIndex);
		m_mtxView = *GetCameraView2d(m_iCurCamIndex);
	}
	else // Using window coordinates
	{
		viewportRect.left = 0.0f;
		viewportRect.bottom = 0.0f;
		viewportRect.right = 1.0f;
		viewportRect.top = 1.0f;

		m_mtxView = glm::mat4(1.0f);
		m_mtxView = glm::translate(m_mtxView, vFramebufferSize.x * -0.5f, vFramebufferSize.y * -0.5f, 0.0f);
	}

	float fWidth = (viewportRect.Width() * vFramebufferSize.x);
	float fHeight = (viewportRect.Height() * vFramebufferSize.y);

	glViewport(static_cast<GLint>(viewportRect.left * vFramebufferSize.x),
			   static_cast<GLint>(viewportRect.bottom * vFramebufferSize.y),
			   static_cast<GLsizei>(fWidth),
			   static_cast<GLsizei>(fHeight));

	HyErrorCheck_OpenGL("HyOpenGLShader::SetCameraMatrices_2d", "glViewport");

	m_mtxProj = glm::ortho(fWidth * -0.5f, fWidth * 0.5f, fHeight * -0.5f, fHeight * 0.5f, 0.0f, 1.0f);
}
