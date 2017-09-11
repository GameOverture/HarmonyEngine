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
#include "Renderer/Components/HyGfxComms.h"
#include "Diagnostics/Console/HyConsole.h"

HyOpenGL::HyOpenGL(HyGfxComms &gfxCommsRef, HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef) :	IHyRenderer(gfxCommsRef, diagnosticsRef, bShowCursor, windowListRef),
																																		m_mtxView(1.0f)
{
}

HyOpenGL::~HyOpenGL(void)
{
}

/*virtual*/ bool HyOpenGL::Initialize() /*override*/
{
	HyLog("OpenGL is initializing...");

	//////////////////////////////////////////////////////////////////////////
	// Init GLEW
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glewInit");
	}

	//const GLubyte *pExtStr = glGetString(GL_EXTENSIONS);

	//WriteTextFile("GLExtensions.txt", glGetString(GL_EXTENSIONS));

	if(glewIsSupported("GL_VERSION_3_3") == false) {
		HyError("At least OpenGL 3.3 must be supported");
	}

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
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// 2D setup
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
	glBufferData(GL_ARRAY_BUFFER, HYDRAWBUFFERHEADER->uiVertexBufferSize2d, GetVertexData2d(), GL_DYNAMIC_DRAW);

	m_iCurCamIndex = 0;
}

/*virtual*/ bool HyOpenGL::BeginPass_2d()
{
	int32 iNumCameras2d = GetNumCameras2d();
	int32 iNumRenderStates2d = GetNumRenderStates2d();

	// Only draw cameras that are apart of this render surface
	while(m_RenderSurfaceIter->GetId() != GetCameraWindowIndex2d(m_iCurCamIndex) && m_iCurCamIndex < iNumCameras2d)
		m_iCurCamIndex++;

	if(iNumRenderStates2d == 0 || m_iCurCamIndex >= iNumCameras2d)
		return false;
	
	// TODO: Without disabling glDepthMask, sprites fragments that overlap will be discarded, and primitive draws don't work
	glDepthMask(false);

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
	pShader->Use();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(HYSHADERPROG_QuadBatch == renderState.GetShaderId())
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderState.GetTextureHandle());
		if(renderState.GetTextureHandle() != 0)
			pShader->SetUniformGLSL("Tex", 0);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(renderState.IsScissorRect())
	{
		const HyScreenRect<int32> &scissorRectRef = renderState.GetScissorRect();

		glScissor(static_cast<GLint>(m_mtxView[0].x * scissorRectRef.x) + static_cast<GLint>(m_mtxView[3].x) + (m_RenderSurfaceIter->GetWidth() / 2),
				  static_cast<GLint>(m_mtxView[1].y * scissorRectRef.y) + static_cast<GLint>(m_mtxView[3].y) + (m_RenderSurfaceIter->GetHeight() / 2),
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

	HyErrorCheck_OpenGL("DrawRenderState_2d", "Before DrawArrays");

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
	glDepthMask(true);	// TODO: Get rid of this once you implement proper depth

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

/*virtual*/ void HyOpenGL::FinishRender()
{

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
	glBindTexture(GL_TEXTURE_2D, hGLTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST

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

/*virtual*/ void HyOpenGL::DeleteTexture(uint32 uiTextureHandle)
{
	glDeleteTextures(1, &uiTextureHandle);
}

/*virtual*/ void HyOpenGL::OnRenderSurfaceChanged(HyRenderSurface &renderSurfaceRef, uint32 uiChangedFlags)
{
}

void HyOpenGL::SetCameraMatrices_2d(bool bUseCameraView)
{
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
		m_mtxView = glm::translate(m_mtxView, m_RenderSurfaceIter->GetWidth() * -0.5f, m_RenderSurfaceIter->GetHeight() * -0.5f, 0.0f);
	}

	float fWidth = (viewportRect.Width() * m_RenderSurfaceIter->GetWidth());
	float fHeight = (viewportRect.Height() * m_RenderSurfaceIter->GetHeight());

	glViewport(static_cast<GLint>(viewportRect.left * m_RenderSurfaceIter->GetWidth()),
			   static_cast<GLint>(viewportRect.bottom * m_RenderSurfaceIter->GetHeight()),
			   static_cast<GLsizei>(fWidth),
			   static_cast<GLsizei>(fHeight));

	m_mtxProj = glm::ortho(fWidth * -0.5f, fWidth * 0.5f, fHeight * -0.5f, fHeight * 0.5f, 0.0f, 1.0f);
}
