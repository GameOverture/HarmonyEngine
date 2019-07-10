/**************************************************************************
 *	HyOpenGL.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Renderer/OpenGL/HyOpenGL.h"
#include "Renderer/OpenGL/HyOpenGLShaderSrc.h"
#include "Window/HyWindow.h"
#include "Renderer/Effects/HyStencil.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Scene/Nodes/Objects/HyCamera.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"

HyOpenGL::HyOpenGL(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef) :
	IHyRenderer(diagnosticsRef, windowListRef),
	m_mtxView(1.0f),
	m_mtxProj(1.0f)
{
	HyLog("OpenGL is initializing...");

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
		m_VaoMapList.push_back(std::map<HyShaderHandle, GLuint>());

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

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	}

	if(m_WindowListRef.empty() == false)
	{
		SetCurrentWindow(0);
		if(glewIsSupported("GL_VERSION_3_1") == false) {
			HyError("At least OpenGL 3.1 must be supported");
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

	// 2D vertex buffer setup
	m_VertexBuffer.Initialize2d();
}

HyOpenGL::~HyOpenGL(void)
{
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

/*virtual*/ void HyOpenGL::Begin_3d()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glEnable");

	//if(m_bVBO3dDirty)
	//{
	//	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO3d);
	//	HyErrorCheck_OpenGL("HyOpenGL:Begin_2d", "glBindBuffer");

	//	glBufferData(GL_ARRAY_BUFFER, m_VertexBuffer3d.m_uiNumUsedBytes, m_VertexBuffer3d.m_pBUFFER, GL_STATIC_DRAW);
	//	HyErrorCheck_OpenGL("HyOpenGL:Begin_2d", "glBufferData");

	//	m_bVBO3dDirty = false;
	//}
}

/*virtual*/ void HyOpenGL::DrawRenderState_3d(HyRenderBuffer::State *pRenderState) /*override*/
{
}

/*virtual*/ void HyOpenGL::Begin_2d()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer.GetGfxApiHandle2d());
	HyErrorCheck_OpenGL("HyOpenGL:Begin_2d", "glBindBuffer");

	glBufferData(GL_ARRAY_BUFFER, m_VertexBuffer.GetNumUsedBytes2d(), m_VertexBuffer.GetData2d(), GL_DYNAMIC_DRAW);
	HyErrorCheck_OpenGL("HyOpenGL:Begin_2d", "glBufferData");

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glBlendFunc");
}

/*virtual*/ void HyOpenGL::DrawRenderState_2d(HyRenderBuffer::State *pRenderState, IHyCamera<IHyNode2d> *pCamera) /*override*/
{
	//////////////////////////////////////////////////////////////////////////
	// Setup stencil buffer if required
	if(pRenderState->hSTENCIL != HY_UNUSED_HANDLE)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);									// This mask allows any 8bit value to be written to the stencil buffer (and allows clears to work)

		glDisable(GL_SCISSOR_TEST);								// Ensure scissor test isn't affecting our initial stencil clear
		glClear(GL_STENCIL_BUFFER_BIT);							// Clear stencil buffer by writing default stencil value '0' to entire buffer.
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);	// Disable rendering color while we determine the stencil buffer

		glStencilFunc(GL_ALWAYS, 1, 0xFF);						// All fragments rendered next will "pass" the stencil test, and 'ref' is set to '1'
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);				// Fragments that "passed" will write the 'ref' value in the stencil buffer

		// Render all instances stored in the HyStencil, and affect their pixel locations in stencil buffer
		HyStencil *pStencil = FindStencil(pRenderState->hSTENCIL);
		uint32 uiNumStencilInstance = static_cast<uint32>(pStencil->GetInstanceList().size());
		char *pStencilRenderStateBufferPos = reinterpret_cast<char *>(pStencil->GetRenderStatePtr());
		for(uint32 i = 0; i < uiNumStencilInstance; ++i)
		{
			HyRenderBuffer::State *pCurRenderState = reinterpret_cast<HyRenderBuffer::State *>(pStencilRenderStateBufferPos);
			if(pCurRenderState->iCOORDINATE_SYSTEM < 0 || pCurRenderState->iCOORDINATE_SYSTEM == m_pCurWindow->GetIndex())
				RenderPass2d(pCurRenderState, pCurRenderState->iCOORDINATE_SYSTEM < 0 ? pCamera : nullptr);

			pStencilRenderStateBufferPos += pCurRenderState->m_uiExDataSize + sizeof(HyRenderBuffer::State);
		}

		switch(pStencil->GetBehavior())
		{
		case HYSTENCILBEHAVIOR_Mask:
			glStencilFunc(GL_EQUAL, 1, 0xFF);					// Only render pixels that pass this check while stencil test is enabled
			break;
		case HYSTENCILBEHAVIOR_InvertedMask:
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);				// Only render pixels that pass this check while stencil test is enabled
			break;
		}

		glStencilMask(0x00);									// Disable further writing to the the stencil buffer
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);		// Re-enable the color buffer
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glDisable");
	}

	RenderPass2d(pRenderState, pRenderState->iCOORDINATE_SYSTEM < 0 ? pCamera : nullptr);
}

/*virtual*/ void HyOpenGL::FinishRender()
{
#ifdef HY_PLATFORM_DESKTOP
	glfwSwapInterval(0);
	// This function will block if glfwSwapInterval is set to '1' (AKA VSync enabled)
	glfwSwapBuffers(m_pCurWindow->GetHandle());
#endif
}

/*virtual*/ void HyOpenGL::UploadShader(HyShaderProgramDefaults eDefaultsFrom, HyShader *pShader) /*override*/
{
	std::vector<HyShaderVertexAttribute> &shaderVertexAttribListRef = pShader->GetVertextAttributes();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// If unassigned vertex shader, fill in defaults
	if(pShader->GetSourceCode(HYSHADER_Vertex).empty())
	{
		shaderVertexAttribListRef.clear();

		switch(eDefaultsFrom)
		{
		case HYSHADERPROG_QuadBatch:
			pShader->SetSourceCode(szHYQUADBATCH_VERTEXSHADER, HYSHADER_Vertex);
			pShader->AddVertexAttribute("attr_vSize", HyShaderVariable::vec2, false, 1);
			pShader->AddVertexAttribute("attr_vOffset", HyShaderVariable::vec2, false, 1);
			pShader->AddVertexAttribute("attr_vTopTint", HyShaderVariable::vec4, false, 1);
			pShader->AddVertexAttribute("attr_vBotTint", HyShaderVariable::vec4, false, 1);
			pShader->AddVertexAttribute("attr_vUVcoord0", HyShaderVariable::vec2, false, 1);
			pShader->AddVertexAttribute("attr_vUVcoord1", HyShaderVariable::vec2, false, 1);
			pShader->AddVertexAttribute("attr_vUVcoord2", HyShaderVariable::vec2, false, 1);
			pShader->AddVertexAttribute("attr_vUVcoord3", HyShaderVariable::vec2, false, 1);
			pShader->AddVertexAttribute("attr_mtxLocalToWorld", HyShaderVariable::mat4, false, 1);
			break;

		case HYSHADERPROG_Primitive:
			pShader->SetSourceCode(szHYPRIMATIVE_VERTEXSHADER, HYSHADER_Vertex);
			pShader->AddVertexAttribute("attr_vPosition", HyShaderVariable::vec2);
			break;

		case HYSHADERPROG_Lines2d:
			pShader->SetSourceCode(szHYLINES2D_VERTEXSHADER, HYSHADER_Vertex);
			pShader->AddVertexAttribute("attr_vPosition", HyShaderVariable::vec2);
			pShader->AddVertexAttribute("attr_vNormal", HyShaderVariable::vec2);
			break;

		default:
			HyError("HyOpenGL::UploadShader has unknown program to generate defaults from");
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// If unassigned fragment shader, fill in defaults
	if(pShader->GetSourceCode(HYSHADER_Fragment).empty())
	{
		switch(eDefaultsFrom)
		{
		case HYSHADERPROG_QuadBatch:
			pShader->SetSourceCode(szHYQUADBATCH_FRAGMENTSHADER, HYSHADER_Fragment);
			break;

		case HYSHADERPROG_Primitive:
			pShader->SetSourceCode(szHYPRIMATIVE_FRAGMENTSHADER, HYSHADER_Fragment);
			break;

		case HYSHADERPROG_Lines2d:
			pShader->SetSourceCode(szHYLINES2D_FRAGMENTSHADER, HYSHADER_Fragment);
			break;

		default:
			HyError("HyOpenGL::UploadShader has unknown program to generate defaults from");
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Error check attribute list
#ifdef HY_DEBUG
	GLint iMaxVertexAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &iMaxVertexAttribs);

	int32 iTotalVertexAttribs = 0;
	for(uint32 i = 0; i < shaderVertexAttribListRef.size(); ++i)
	{
		if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::dvec2 || shaderVertexAttribListRef[i].eVarType == HyShaderVariable::dvec3 || shaderVertexAttribListRef[i].eVarType == HyShaderVariable::dvec4)
			iTotalVertexAttribs += 2;
		else if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::mat3)
			iTotalVertexAttribs += 3;
		else if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::mat4)
			iTotalVertexAttribs += 4;
		else
			iTotalVertexAttribs += 1;
	}

	HyAssert(iMaxVertexAttribs >= iTotalVertexAttribs, "GL_MAX_VERTEX_ATTRIBS is < " << iTotalVertexAttribs);
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate the shader program and generate the corresponding VAO(s) for each context/window
	if(m_GLShaderMap.find(pShader->GetHandle()) == m_GLShaderMap.end())
	{
		m_GLShaderMap[pShader->GetHandle()] = glCreateProgram();
		HyAssert(m_GLShaderMap[pShader->GetHandle()] != 0, "Unable to create shader program");
		HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glCreateProgram");
	}
	GLuint hGLShaderProg = m_GLShaderMap[pShader->GetHandle()];

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		if(m_VaoMapList[i].find(pShader->GetHandle()) == m_VaoMapList[i].end())
		{
			SetCurrentWindow(i);

			GLuint uiVao = 0;
			glGenVertexArrays(1, &uiVao);
			HyErrorCheck_OpenGL("HyOpenGLShader::OnUpload", "glGenVertexArrays");

			m_VaoMapList[i][pShader->GetHandle()] = uiVao;
		}
	}

	CompileShader(pShader, HYSHADER_Vertex);
	CompileShader(pShader, HYSHADER_Fragment);

	// Explicitly binding vertex attributes here to support intel's incomplete OpenGL 3.1
	for(uint32 i = 0; i < shaderVertexAttribListRef.size(); ++i)
	{
		//BindAttribLocation(i, );
		glEnableVertexAttribArray(i);
		HyErrorCheck_OpenGL("HyOpenGLShader::BindAttribLocation", "glEnableVertexAttribArray");

		glBindAttribLocation(hGLShaderProg, i, shaderVertexAttribListRef[i].sName.c_str());
		HyErrorCheck_OpenGL("HyOpenGLShader::BindAttribLocation", "glBindAttribLocation");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Link all the shaders in the program together
	glLinkProgram(hGLShaderProg);
	HyErrorCheck_OpenGL("HyOpenGLShader::Link", "glLinkProgram");

#ifdef HY_DEBUG
	GLint status = 0;
	glGetProgramiv(hGLShaderProg, GL_LINK_STATUS, &status);
	if(GL_FALSE == status)
	{
		GLint iLength = 0;
		glGetProgramiv(hGLShaderProg, GL_INFO_LOG_LENGTH, &iLength);

		if(iLength > 0)
		{
			char *szlog = HY_NEW char[iLength];
			GLint written = 0;
			glGetProgramInfoLog(hGLShaderProg, iLength, &written, szlog);

			HyError("Shader program failed to link!\n" << szlog);
			delete [] szlog;	// Not that this matters
		}
	}
#endif
	// TODO: After linking (whether successfully or not), it is a good idea to detach all shader objects from the program. Call glDetachShader and glDeleteShader (If not intended to use shader object to link another program)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// For each context/window, setup the vertex attributes per VAO
	for(uint32 i = 0; i < GetNumWindows(); ++i)
	{
		SetCurrentWindow(i);

		//BindVao(this);
		GLuint uiVao = m_VaoMapList[m_pCurWindow->GetIndex()][pShader->GetHandle()];
		glBindVertexArray(uiVao);
		HyErrorCheck_OpenGL("HyOpenGLShader::Use", "glBindVertexArray");

		for(uint32 i = 0; i < shaderVertexAttribListRef.size(); ++i)
		{
			GLuint uiLocation = glGetAttribLocation(hGLShaderProg, shaderVertexAttribListRef[i].sName.c_str());
			HyErrorCheck_OpenGL("HyOpenGLShader::GetAttribLocation", "glGetAttribLocation");
			HyAssert(static_cast<GLint>(uiLocation) >= 0, "Vertex attribute \"" << shaderVertexAttribListRef[i].sName.c_str() << "\" is not found in shader: " << static_cast<uint32>(pShader->GetHandle()));

			if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::dvec2 || shaderVertexAttribListRef[i].eVarType == HyShaderVariable::dvec3 || shaderVertexAttribListRef[i].eVarType == HyShaderVariable::dvec4)
			{
				HyAssert(false, "dvec2, dvec3, or dvec4 is not tested, remove this if it works");

				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);

				glVertexAttribDivisor(uiLocation + 0, shaderVertexAttribListRef[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 1, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}
			else if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::mat3)
			{
				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);
				glEnableVertexAttribArray(uiLocation + 2);

				glVertexAttribDivisor(uiLocation + 0, shaderVertexAttribListRef[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 1, shaderVertexAttribListRef[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 2, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}
			else if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::mat4)
			{
				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);
				glEnableVertexAttribArray(uiLocation + 2);
				glEnableVertexAttribArray(uiLocation + 3);

				glVertexAttribDivisor(uiLocation + 0, shaderVertexAttribListRef[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 1, shaderVertexAttribListRef[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 2, shaderVertexAttribListRef[i].uiInstanceDivisor);
				glVertexAttribDivisor(uiLocation + 3, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}
			else
			{
				glEnableVertexAttribArray(uiLocation);
				glVertexAttribDivisor(uiLocation, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}

			HyErrorCheck_OpenGL("HyOpenGLShader::OnUpload", "glEnableVertexAttribArray or glVertexAttribDivisor");
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

/*virtual*/ uint32 HyOpenGL::AddTexture(HyTextureFormat eDesiredFormat, HyTextureFiltering eTexFiltering, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat) /*override*/
{
	GLenum eInternalFormat = GL_RGBA;
	switch(eDesiredFormat)
	{
	case HYTEXTURE_R8G8B8A8: {
		eInternalFormat = GL_RGBA;
		break; }
	case HYTEXTURE_R8G8B8: {
		eInternalFormat = GL_RGB;
		break; }
	case HYTEXTURE_RGB_DTX1: {
		eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_RGB_DTX1)) ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGB;
		break; }
	case HYTEXTURE_RGBA_DTX1: {
		eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_RGBA_DTX1)) ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA;
		break; }
	case HYTEXTURE_DTX3: {
		eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_DTX3)) ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : GL_COMPRESSED_RGBA;
		break; }
	case HYTEXTURE_DTX5: {
		eInternalFormat = (0 != (m_uiSupportedTextureFormats & HYTEXTURE_DTX5)) ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA;
		break; }
	default: {
		HyLogError("Unknown TextureFormat used for 'eDesiredFormat'");
		break; }
	}

	GLenum eFormat = GL_RGBA;
	bool bIsPixelDataCompressed = false;
	switch(ePixelDataFormat)
	{
	case HYTEXTURE_R8G8B8A8: {
		eFormat = GL_RGBA;
		break; }
	case HYTEXTURE_R8G8B8: {
		eFormat = GL_RGB;
		break; }
	case HYTEXTURE_RGB_DTX1:
	case HYTEXTURE_RGBA_DTX1:
	case HYTEXTURE_DTX3:
	case HYTEXTURE_DTX5: {
		bIsPixelDataCompressed = true;
		break; }
	default: {
		HyLogError("Unknown TextureFormat used for 'ePixelDataFormat'");
		break; }
	}

	GLuint hGLTexture;
	glGenTextures(1, &hGLTexture);
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glGenTextures");

	glBindTexture(GL_TEXTURE_2D, hGLTexture);
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glBindTexture");

	if(bIsPixelDataCompressed == false)
	{
		glTexImage2D(GL_TEXTURE_2D, iNumLodLevels, eInternalFormat, uiWidth, uiHeight, 0, eFormat, GL_UNSIGNED_BYTE, pPixelData);
		HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glTexImage2D");
	}
	else
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, iNumLodLevels, eInternalFormat, uiWidth, uiHeight, 0, uiPixelDataSize, pPixelData);
		HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glCompressedTexImage2D");
	}

	switch(eTexFiltering)
	{
	case HYTEXFILTER_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case HYTEXFILTER_NEAREST_MIPMAP:
		glEnable(GL_TEXTURE_2D);	// An old ATI driver bug work-around requires glEnabled()
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case HYTEXFILTER_LINEAR_MIPMAP:
		glEnable(GL_TEXTURE_2D);	// An old ATI driver bug work-around requires glEnabled()
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case HYTEXFILTER_BILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case HYTEXFILTER_BILINEAR_MIPMAP:
		glEnable(GL_TEXTURE_2D);	// An old ATI driver bug work-around requires glEnabled()
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case HYTEXFILTER_TRILINEAR:
		glEnable(GL_TEXTURE_2D);	// An old ATI driver bug work-around requires glEnabled()
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	default:
		HyError("HyOpenGL::AddTexture - Improper texture filter specified");
		break;
	}
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glTexParameteri");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_REPEAT

	glBindTexture(GL_TEXTURE_2D, 0);
	return hGLTexture;
}

// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
/*virtual*/ uint32 HyOpenGL::AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) /*override*/
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

/*virtual*/ void HyOpenGL::DeleteTexture(uint32 uiTextureHandle) /*override*/
{
	glDeleteTextures(1, &uiTextureHandle);
	HyErrorCheck_OpenGL("HyOpenGL:DeleteTexture", "glDeleteTextures");
}

/*virtual*/ uint32 HyOpenGL::GenerateVertexBuffer() /*override*/
{
	GLuint hVBO;
	glGenBuffers(1, &hVBO);
	HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glGenBuffers");

	return hVBO;
}

void HyOpenGL::CompileShader(HyShader *pShader, HyShaderType eType)
{
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
	const char *szSrc = pShader->GetSourceCode(eType).c_str();
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
	glAttachShader(m_GLShaderMap[pShader->GetHandle()], iShaderHandle);
	HyErrorCheck_OpenGL("HyOpenGLShader:CompileFromString", "glAttachShader");
}

void HyOpenGL::RenderPass2d(HyRenderBuffer::State *pRenderState, IHyCamera<IHyNode2d> *pCamera)
{
	glm::ivec2 vFramebufferSize = m_pCurWindow->GetFramebufferSize();
	HyRectangle<float> viewportRect;

	//////////////////////////////////////////////////////////////////////////
	// Set glViewport based on coordinate system
	if(pCamera)
	{
		viewportRect = pCamera->GetViewport();
		m_mtxView = pCamera->GetWorldTransform();

		// Reversing X and Y because it's more intuitive (or I'm not multiplying the matrices correctly somewhere here or in the shader)
		m_mtxView[3].x *= -1;
		m_mtxView[3].y *= -1;
	}
	else // Using window coordinates (origin is bottom left corner)
	{
		viewportRect.left = 0.0f;
		viewportRect.bottom = 0.0f;
		viewportRect.right = 1.0f;
		viewportRect.top = 1.0f;

		m_mtxView = glm::mat4(1.0f);
		m_mtxView = glm::translate(m_mtxView, glm::vec3(vFramebufferSize.x * -0.5f, vFramebufferSize.y * -0.5f, 0.0f));
	}

	float fWidth = (viewportRect.Width() * vFramebufferSize.x);
	float fHeight = (viewportRect.Height() * vFramebufferSize.y);

	m_mtxProj = glm::ortho(fWidth * -0.5f, fWidth * 0.5f, fHeight * -0.5f, fHeight * 0.5f, 0.0f, 1.0f);

	glViewport(static_cast<GLint>(viewportRect.left * vFramebufferSize.x),
			   static_cast<GLint>(viewportRect.bottom * vFramebufferSize.y),
			   static_cast<GLsizei>(fWidth),
			   static_cast<GLsizei>(fHeight));
	HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glViewport");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set the proper shader program
	GLuint uiVao = m_VaoMapList[m_pCurWindow->GetIndex()][pRenderState->hSHADER];
	glBindVertexArray(uiVao);
	HyErrorCheck_OpenGL("HyOpenGLShader::Use", "glBindVertexArray");

	GLuint hGlHandle = m_GLShaderMap[pRenderState->hSHADER];
	glUseProgram(hGlHandle);
	HyErrorCheck_OpenGL("HyOpenGLShader::Use", "glUseProgram");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Bind texture(s)
	glActiveTexture(GL_TEXTURE0);
	HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glActiveTexture");

	glBindTexture(GL_TEXTURE_2D, pRenderState->hTEXTURE_0);
	HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glBindTexture");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(pRenderState->SCISSOR_RECT.iTag != IHyVisable::SCISSORTAG_Disabled)
	{
		const HyScreenRect<int32> &scissorRectRef = pRenderState->SCISSOR_RECT;

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
	GLint iUniLocation = glGetUniformLocation(hGlHandle, "u_mtxWorldToCamera");
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	if(iUniLocation >= 0)
	{
		glUniformMatrix4fv(iUniLocation, 1, GL_FALSE, &m_mtxView[0][0]);
		HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix4fv");
	}

	iUniLocation = glGetUniformLocation(hGlHandle, "u_mtxCameraToClip");
	HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
	if(iUniLocation >= 0)
	{
		glUniformMatrix4fv(iUniLocation, 1, GL_FALSE, &m_mtxProj[0][0]);
		HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix4fv");
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assign any other uniforms by parsing the RenderState's ex buffer portion
	char *pExBuffer = reinterpret_cast<char *>(pRenderState) + sizeof(HyRenderBuffer::State);

	uint32 uiNumUniforms = *reinterpret_cast<uint32 *>(pExBuffer);
	pExBuffer += sizeof(uint32);
	for(uint32 i = 0; i < uiNumUniforms; ++i)
	{
		const char *szUniformName = pExBuffer;
		size_t uiStrLen = strlen(szUniformName) + 1;	// +1 for NULL terminator
		pExBuffer += uiStrLen;

		iUniLocation = glGetUniformLocation(hGlHandle, szUniformName);
		HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glGetUniformLocation");
		HyAssert(iUniLocation >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szUniformName << "\"");

		HyShaderVariable eVarType = static_cast<HyShaderVariable>(*reinterpret_cast<uint32 *>(pExBuffer));
		pExBuffer += sizeof(uint32);
		switch(eVarType)
		{
		case HyShaderVariable::boolean:
			glUniform1i(iUniLocation, *reinterpret_cast<bool *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1i");
			pExBuffer += sizeof(bool);
			break;

		case HyShaderVariable::int32:
			glUniform1i(iUniLocation, *reinterpret_cast<int32 *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1i");
			pExBuffer += sizeof(int32);
			break;

		case HyShaderVariable::uint32:
			glUniform1ui(iUniLocation, *reinterpret_cast<uint32 *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1ui");
			pExBuffer += sizeof(uint32);
			break;

		case HyShaderVariable::float32:
			glUniform1f(iUniLocation, *reinterpret_cast<float *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform1f");
			pExBuffer += sizeof(float);
			break;

		case HyShaderVariable::double64:
			HyError("GLSL Shader uniform does not support type double yet!");
			break;

		case HyShaderVariable::bvec2:
			glUniform2i(iUniLocation, reinterpret_cast<glm::bvec2 *>(pExBuffer)->x, reinterpret_cast<glm::bvec2 *>(pExBuffer)->y);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2i");
			pExBuffer += sizeof(glm::bvec2);
			break;

		case HyShaderVariable::bvec3:
			glUniform3i(iUniLocation, reinterpret_cast<glm::bvec3 *>(pExBuffer)->x,
									  reinterpret_cast<glm::bvec3 *>(pExBuffer)->y,
									  reinterpret_cast<glm::bvec3 *>(pExBuffer)->z);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3i");
			pExBuffer += sizeof(glm::bvec3);
			break;

		case HyShaderVariable::bvec4:
			glUniform4i(iUniLocation, reinterpret_cast<glm::bvec4 *>(pExBuffer)->x,
									  reinterpret_cast<glm::bvec4 *>(pExBuffer)->y,
									  reinterpret_cast<glm::bvec4 *>(pExBuffer)->z,
									  reinterpret_cast<glm::bvec4 *>(pExBuffer)->w);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4i");
			pExBuffer += sizeof(glm::bvec4);
			break;

		case HyShaderVariable::ivec2:
			glUniform2i(iUniLocation, reinterpret_cast<glm::ivec2 *>(pExBuffer)->x, reinterpret_cast<glm::ivec2 *>(pExBuffer)->y);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2i");
			pExBuffer += sizeof(glm::ivec2);
			break;

		case HyShaderVariable::ivec3:
			glUniform3i(iUniLocation, reinterpret_cast<glm::ivec3 *>(pExBuffer)->x,
									  reinterpret_cast<glm::ivec3 *>(pExBuffer)->y,
									  reinterpret_cast<glm::ivec3 *>(pExBuffer)->z);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3i");
			pExBuffer += sizeof(glm::ivec3);
			break;

		case HyShaderVariable::ivec4:
			glUniform4i(iUniLocation, reinterpret_cast<glm::ivec4 *>(pExBuffer)->x,
									  reinterpret_cast<glm::ivec4 *>(pExBuffer)->y,
									  reinterpret_cast<glm::ivec4 *>(pExBuffer)->z,
									  reinterpret_cast<glm::ivec4 *>(pExBuffer)->w);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4i");
			pExBuffer += sizeof(glm::ivec4);
			break;

		case HyShaderVariable::vec2:
			glUniform2f(iUniLocation, reinterpret_cast<glm::vec2 *>(pExBuffer)->x, reinterpret_cast<glm::vec2 *>(pExBuffer)->y);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform2f");
			pExBuffer += sizeof(glm::vec2);
			break;

		case HyShaderVariable::vec3:
			glUniform3f(iUniLocation, reinterpret_cast<glm::vec3 *>(pExBuffer)->x,
									  reinterpret_cast<glm::vec3 *>(pExBuffer)->y,
									  reinterpret_cast<glm::vec3 *>(pExBuffer)->z);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform3f");
			pExBuffer += sizeof(glm::vec3);
			break;

		case HyShaderVariable::vec4:
			glUniform4f(iUniLocation, reinterpret_cast<glm::vec4 *>(pExBuffer)->x,
									  reinterpret_cast<glm::vec4 *>(pExBuffer)->y,
									  reinterpret_cast<glm::vec4 *>(pExBuffer)->z,
									  reinterpret_cast<glm::vec4 *>(pExBuffer)->w);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniform4f");
			pExBuffer += sizeof(glm::vec4);
			break;

		case HyShaderVariable::dvec2:
		case HyShaderVariable::dvec3:
		case HyShaderVariable::dvec4:
			HyError("GLSL Shader uniform does not support type double yet!");
			break;

		case HyShaderVariable::mat3:
			glUniformMatrix3fv(iUniLocation, 1, GL_FALSE, &(*reinterpret_cast<glm::mat3 *>(pExBuffer))[0][0]);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix3fv");
			pExBuffer += sizeof(glm::mat3);
			break;

		case HyShaderVariable::mat4:
			glUniformMatrix4fv(iUniLocation, 1, GL_FALSE, &(*reinterpret_cast<glm::mat4 *>(pExBuffer))[0][0]);
			HyErrorCheck_OpenGL("HyOpenGLShader::SetUniformGLSL", "glUniformMatrix4fv");
			pExBuffer += sizeof(glm::mat4);
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set vertex attribute pointers to the vertex data locations in bound buffer
	size_t uiStartOffset = pRenderState->uiDATA_OFFSET;
	HyShader *pShader = m_ShaderMap[pRenderState->hSHADER];
	HyAssert(pShader, "HyShader not found for render state: " << pRenderState->uiID);

	std::vector<HyShaderVertexAttribute> &shaderVertexAttribListRef = pShader->GetVertextAttributes();

	// TODO: if OpenGL 4.3 is available
	//glBindVertexBuffer(QUADBATCH, m_hVBO2d, uiDataOffset, 132);

	size_t uiOffset = 0;
	for(size_t i = 0; i < shaderVertexAttribListRef.size(); ++i)
	{
		// TODO: Cache the attribute location instead of finding it everytime
		GLuint uiLocation = glGetAttribLocation(hGlHandle, shaderVertexAttribListRef[i].sName.c_str());

		switch(shaderVertexAttribListRef[i].eVarType)
		{
		case HyShaderVariable::boolean:
			glVertexAttribPointer(uiLocation, 1, GL_BYTE, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLboolean);
			break;
		case HyShaderVariable::int32:
			glVertexAttribPointer(uiLocation, 1, GL_INT, shaderVertexAttribListRef[i].bNormalized, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLint);
			break;
		case HyShaderVariable::uint32:
			glVertexAttribPointer(uiLocation, 1, GL_UNSIGNED_INT, shaderVertexAttribListRef[i].bNormalized, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLuint);
			break;
		case HyShaderVariable::float32:
			glVertexAttribPointer(uiLocation, 1, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLfloat);
			break;
		case HyShaderVariable::double64:
			glVertexAttribLPointer(uiLocation, 1, GL_DOUBLE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLdouble);
			break;
		case HyShaderVariable::bvec2:
			glVertexAttribPointer(uiLocation, 2, GL_BYTE, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::bvec2);
			break;
		case HyShaderVariable::bvec3:
			glVertexAttribPointer(uiLocation, 3, GL_BYTE, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::bvec3);
			break;
		case HyShaderVariable::bvec4:
			glVertexAttribPointer(uiLocation, 4, GL_BYTE, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::bvec4);
			break;
		case HyShaderVariable::ivec2:
			glVertexAttribPointer(uiLocation, 2, GL_INT, shaderVertexAttribListRef[i].bNormalized ? GL_TRUE : GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::ivec2);
			break;
		case HyShaderVariable::ivec3:
			glVertexAttribPointer(uiLocation, 3, GL_INT, shaderVertexAttribListRef[i].bNormalized ? GL_TRUE : GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::ivec3);
			break;
		case HyShaderVariable::ivec4:
			glVertexAttribPointer(uiLocation, 4, GL_INT, shaderVertexAttribListRef[i].bNormalized ? GL_TRUE : GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::ivec4);
			break;
		case HyShaderVariable::vec2:
			glVertexAttribPointer(uiLocation, 2, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec2);
			break;
		case HyShaderVariable::vec3:
			glVertexAttribPointer(uiLocation, 3, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			break;
		case HyShaderVariable::vec4:
			glVertexAttribPointer(uiLocation, 4, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			break;
		case HyShaderVariable::dvec2:
			glVertexAttribLPointer(uiLocation, 2, GL_DOUBLE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::dvec2);
			break;
		case HyShaderVariable::dvec3:
			glVertexAttribLPointer(uiLocation, 3, GL_DOUBLE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::dvec3);
			break;
		case HyShaderVariable::dvec4:
			glVertexAttribLPointer(uiLocation, 4, GL_DOUBLE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::dvec4);
			break;
		case HyShaderVariable::mat3:
			glVertexAttribPointer(uiLocation, 3, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			glVertexAttribPointer(uiLocation + 1, 3, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			glVertexAttribPointer(uiLocation + 2, 3, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec3);
			break;
		case HyShaderVariable::mat4:
			glVertexAttribPointer(uiLocation, 4, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			glVertexAttribPointer(uiLocation + 1, 4, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			glVertexAttribPointer(uiLocation + 2, 4, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			glVertexAttribPointer(uiLocation + 3, 4, GL_FLOAT, GL_FALSE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(glm::vec4);
			break;
		}

		HyErrorCheck_OpenGL("HyOpenGLShader::SetVertexAttributePtrs", "glVertexAttribPointer[" << shaderVertexAttribListRef[i].eVarType << "]");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GLenum eDrawMode;
	switch(pRenderState->eRENDER_MODE)
	{
	case HYRENDERMODE_Triangles:		eDrawMode = GL_TRIANGLES;		break;
	case HYRENDERMODE_TriangleStrip:	eDrawMode = GL_TRIANGLE_STRIP;	break;
	case HYRENDERMODE_TriangleFan:		eDrawMode = GL_TRIANGLE_FAN;	break;
	case HYRENDERMODE_LineLoop:			eDrawMode = GL_LINE_LOOP;		break;
	case HYRENDERMODE_LineStrip:		eDrawMode = GL_LINE_STRIP;		break;

	default:
		HyError("Unknown draw mode in render state");
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Everything is prepared, do the drawing
	if(pRenderState->uiNUM_INSTANCES > 1)
	{
		glDrawArraysInstanced(eDrawMode, 0, pRenderState->uiNUM_VERTS_PER_INSTANCE, pRenderState->uiNUM_INSTANCES);
		HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glDrawArraysInstanced");
	}
	else
	{
		uint32 uiStartVertex = 0;
		for(uint32 i = 0; i < pRenderState->uiNUM_INSTANCES; ++i)
		{
			glDrawArrays(eDrawMode, uiStartVertex, pRenderState->uiNUM_VERTS_PER_INSTANCE);
			HyErrorCheck_OpenGL("HyOpenGLShader::DrawRenderState_2d", "glDrawArrays");

			uiStartVertex += pRenderState->uiNUM_VERTS_PER_INSTANCE;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reset OpenGL states
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
