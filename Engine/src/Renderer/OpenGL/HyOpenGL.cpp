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
#ifdef HY_PLATFORM_BROWSER
	#include "Renderer/OpenGL/HyOpenGLESShaderSrc.h"
#else
	#include "Renderer/OpenGL/HyOpenGLShaderSrc.h"
#endif
#include "Window/HyWindow.h"
#include "Renderer/Effects/HyStencil.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Scene/Nodes/Objects/HyCamera.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"

HyOpenGL::HyOpenGL(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef) :
	IHyRenderer(diagnosticsRef, windowListRef),
	m_mtxView(1.0f),
	m_mtxProj(1.0f),
	m_pPboHandles(nullptr),
	m_pPboStates(nullptr)
{
	HyLog("OpenGL is initializing...");

#if defined(HY_USE_SDL2) && !defined(HY_USE_GLFW)
	m_Context = nullptr;

	// TODO: CHECK TO SEE IF THIS IS THE REASON MULTIPLE WINDOWS LEAK WITH SDL2 ON WINDOWS 7

	// Create the context with the first window, and share it between any other windows
	if(m_WindowListRef.empty() == false)
	{
		HyLog("Creating SDL Context");
		m_Context = SDL_GL_CreateContext(m_WindowListRef[0]->GetInterop());
		if(m_Context)
			HyLog("SDL_GL_CreateContext passed");
		else
			HyLog("SDL_GL_CreateContext failed");
	}
#endif

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
		m_VaoMapList.push_back(std::map<HyShaderHandle, GLuint>());

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		SetCurrentWindow(i);

#if defined(HY_USE_SDL2) && !defined(HY_USE_GLFW)
		// Prep GL extensions with GLAD+SDL2
		#if defined(HY_USE_GLAD)
			#if defined(HY_PLATFORM_BROWSER)
				if(!gladLoadGLES2Loader((GLADloadproc) SDL_GL_GetProcAddress)) {
					HyError("gladLoadGLES2Loader failed to initialize GLES2 context with SDL2");
				}
			#else
				if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
					HyError("gladLoadGLLoader failed to initialize OpenGL context with SDL2");
				}
			#endif
		#endif
#endif

		// Load GL extensions with either GLAD or GLEW
#if defined(HY_USE_GLAD)
		if(!gladLoadGL()) {
			HyError("glad failed to initalize");
		}
		HyLog("glad initalized");
#elif defined(HY_USE_GLEW)
		GLenum err = glewInit();

		if(err != GLEW_OK) {
			HyError("glewInit() failed: " << err);
		}
		else {
			// Flush the OpenGL error state, as glew is known to bork it
			while(GL_NO_ERROR != glGetError());
		}
		HyErrorCheck_OpenGL("HyOpenGL:Initialize", "glewInit");
		HyLog("glew initalized");
#endif

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	if(m_WindowListRef.empty() == false)
		SetCurrentWindow(0);
	else
		HyLog("No windows created to render to");

	// Check for PBO hardware support
	bool bPboSupport = false;
	#ifdef HY_USE_GLFW
		bPboSupport = (GLFW_TRUE == glfwExtensionSupported("GL_ARB_pixel_buffer_object") || GLFW_TRUE == glfwExtensionSupported("GLEW_EXT_pixel_buffer_object"));
	#elif defined(HY_USE_SDL2)
		bPboSupport = (SDL_TRUE == SDL_GL_ExtensionSupported("GL_ARB_pixel_buffer_object") || SDL_TRUE == SDL_GL_ExtensionSupported("GLEW_EXT_pixel_buffer_object"));
	#endif
	if(false)//bPboSupport)
	{
		m_pPboHandles = HY_NEW GLuint[HY_NUM_PBO];
		glGenBuffers(HY_NUM_PBO, m_pPboHandles);
		HyErrorCheck_OpenGL("HyOpenGL::HyOpenGL", "glGenBuffers");

		m_pPboStates = HY_NEW PboState[HY_NUM_PBO];
		for(uint32 i = 0; i < HY_NUM_PBO; ++i)
			m_pPboStates[i] = PBO_Free;
	}

	GLint iFormatCount = 0;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &iFormatCount);
	GLint *pFormatArray = HY_NEW GLint[iFormatCount];
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, pFormatArray);
	std::string sCompressedTextureFormats;
	for(int32 i = 0; i < iFormatCount; ++i)
	{
		switch(pFormatArray[i])
		{
#ifndef HY_PLATFORM_BROWSER // emscripten compiled these before when I used glew.h
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			sCompressedTextureFormats += "RGB_DXT1 ";
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			sCompressedTextureFormats += "RGBA_DXT1 ";
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			sCompressedTextureFormats += "DXT3 ";
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			sCompressedTextureFormats += "DXT5 ";
			break;

		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
			sCompressedTextureFormats += "ASTC_RGBA";
			break;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
			sCompressedTextureFormats += "ASTC_SRGB8";
			break;
#endif
		}
	}
	delete[] pFormatArray;

	//const GLubyte *szExtensions = glGetString(GL_EXTENSIONS);
	//HyLog("GL EXTENSIONS: " << szExtensions);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);		// 4-byte pixel alignment

	m_VertexBuffer.Initialize2d();				// vertex buffer for 2D scene nodes

	GLint iMaxTextureSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxTextureSize);
	SetRendererInfo("OpenGL",
					reinterpret_cast<const char *>(glGetString(GL_VERSION)),
					reinterpret_cast<const char *>(glGetString(GL_VENDOR)),
					reinterpret_cast<const char *>(glGetString(GL_RENDERER)),
					reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)),
					iMaxTextureSize,
					sCompressedTextureFormats);
}

HyOpenGL::~HyOpenGL(void)
{
	if(m_pPboHandles)
		glDeleteBuffers(HY_NUM_PBO, m_pPboHandles);

	delete [] m_pPboHandles;
	delete [] m_pPboStates;

#if defined(HY_USE_SDL2) && !defined(HY_USE_GLFW)
	SDL_GL_DeleteContext(m_Context);
#endif
}

/*virtual*/ void HyOpenGL::SetCurrentWindow(uint32 uiIndex) /*override*/
{
	IHyRenderer::SetCurrentWindow(uiIndex);

#ifdef HY_USE_GLFW
	glfwMakeContextCurrent(m_pCurWindow->GetInterop());
#elif defined(HY_USE_SDL2)
	SDL_GL_MakeCurrent(m_pCurWindow->GetInterop(), m_Context);
#endif
}

/*virtual*/ void HyOpenGL::StartRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	HyErrorCheck_OpenGL("HyOpenGL:StartRender", "glClear");

	if(m_pPboHandles)
	{
		for(uint32 i = 0; i < HY_NUM_PBO; ++i)
		{
			if(m_pPboStates[i] == PBO_Pending3)
				m_pPboStates[i] = PBO_Pending2;
			if(m_pPboStates[i] == PBO_Pending2)
				m_pPboStates[i] = PBO_Pending1;
			if(m_pPboStates[i] == PBO_Pending1)
				m_pPboStates[i] = PBO_Free;
		}
	}
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
	if(pRenderState->m_hSTENCIL != HY_UNUSED_HANDLE)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);									// This mask allows any 8bit value to be written to the stencil buffer (and allows clears to work)

		glDisable(GL_SCISSOR_TEST);								// Ensure scissor test isn't affecting our initial stencil clear
		glClear(GL_STENCIL_BUFFER_BIT);							// Clear stencil buffer by writing default stencil value '0' to entire buffer.
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);	// Disable rendering color while we determine the stencil buffer

		glStencilFunc(GL_ALWAYS, 1, 0xFF);						// All fragments rendered next will "pass" the stencil test, and 'ref' is set to '1'
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);				// Fragments that "passed" will write the 'ref' value in the stencil buffer

		// Render all instances stored in the HyStencil, and affect their pixel locations in stencil buffer
		HyStencil *pStencil = FindStencil(pRenderState->m_hSTENCIL);
		uint32 uiNumStencilInstance = static_cast<uint32>(pStencil->GetInstanceList().size());
		char *pStencilRenderStateBufferPos = reinterpret_cast<char *>(pStencil->GetRenderStatePtr());
		for(uint32 i = 0; i < uiNumStencilInstance; ++i)
		{
			HyRenderBuffer::State *pCurRenderState = reinterpret_cast<HyRenderBuffer::State *>(pStencilRenderStateBufferPos);
			if(pCurRenderState->m_iCOORDINATE_SYSTEM < 0 || pCurRenderState->m_iCOORDINATE_SYSTEM == m_pCurWindow->GetIndex())
				RenderPass2d(pCurRenderState, pCurRenderState->m_iCOORDINATE_SYSTEM < 0 ? pCamera : nullptr);

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

	RenderPass2d(pRenderState, pRenderState->m_iCOORDINATE_SYSTEM < 0 ? pCamera : nullptr);
}

/*virtual*/ void HyOpenGL::FinishRender()
{
#ifdef HY_USE_GLFW
	//glfwSwapInterval(0); // This function will block if glfwSwapInterval is set to '1' (AKA VSync enabled)
	glfwSwapBuffers(m_pCurWindow->GetInterop());
#elif defined(HY_USE_SDL2)
	//SDL_GL_SetSwapInterval(0); // 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync
	SDL_GL_SwapWindow(m_pCurWindow->GetInterop());
#endif
}

/*virtual*/ void HyOpenGL::UploadShader(HyShader *pShader) /*override*/
{
	std::vector<HyShaderVertexAttribute> &shaderVertexAttribListRef = pShader->GetVertextAttributes();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// If unassigned vertex shader, fill in defaults
	if(pShader->GetSourceCode(HYSHADER_Vertex).empty())
	{
		shaderVertexAttribListRef.clear();

		switch(pShader->GetDefaults())
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
		switch(pShader->GetDefaults())
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
	HyLog("DEBUG: Compiling shader: " << static_cast<int>(pShader->GetHandle()));

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
	HyLog("DEBUG: Linking shader: " << static_cast<int>(pShader->GetHandle()));

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

			HyLogError("Shader program failed to link!\n" << szlog);
			HyError("Shader program failed to link!\n" << szlog);
			delete [] szlog;	// Not that this matters
		}
	}
#endif
	// TODO: After linking (whether successfully or not), it is a good idea to detach all shader objects from the program. Call glDetachShader and glDeleteShader (If not intended to use shader object to link another program)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// For each context/window, setup the vertex attributes per VAO
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		SetCurrentWindow(i);

		//BindVao(this);
		GLuint uiVao = m_VaoMapList[m_pCurWindow->GetIndex()][pShader->GetHandle()];
		glBindVertexArray(uiVao);
		HyErrorCheck_OpenGL("HyOpenGLShader::Use", "glBindVertexArray");

#ifndef HY_PLATFORM_BROWSER // emscripten also compiled with ARB when I used glew.h
		#define hyVertexAttribDivisor glVertexAttribDivisorARB
#else
		#define hyVertexAttribDivisor glVertexAttribDivisor
#endif

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

				hyVertexAttribDivisor(uiLocation + 0, shaderVertexAttribListRef[i].uiInstanceDivisor);
				hyVertexAttribDivisor(uiLocation + 1, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}
			else if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::mat3)
			{
				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);
				glEnableVertexAttribArray(uiLocation + 2);

				hyVertexAttribDivisor(uiLocation + 0, shaderVertexAttribListRef[i].uiInstanceDivisor);
				hyVertexAttribDivisor(uiLocation + 1, shaderVertexAttribListRef[i].uiInstanceDivisor);
				hyVertexAttribDivisor(uiLocation + 2, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}
			else if(shaderVertexAttribListRef[i].eVarType == HyShaderVariable::mat4)
			{
				glEnableVertexAttribArray(uiLocation + 0);
				glEnableVertexAttribArray(uiLocation + 1);
				glEnableVertexAttribArray(uiLocation + 2);
				glEnableVertexAttribArray(uiLocation + 3);

				hyVertexAttribDivisor(uiLocation + 0, shaderVertexAttribListRef[i].uiInstanceDivisor);
				hyVertexAttribDivisor(uiLocation + 1, shaderVertexAttribListRef[i].uiInstanceDivisor);
				hyVertexAttribDivisor(uiLocation + 2, shaderVertexAttribListRef[i].uiInstanceDivisor);
				hyVertexAttribDivisor(uiLocation + 3, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}
			else
			{
				glEnableVertexAttribArray(uiLocation);
				hyVertexAttribDivisor(uiLocation, shaderVertexAttribListRef[i].uiInstanceDivisor);
			}

			HyErrorCheck_OpenGL("HyOpenGLShader::OnUpload", "glEnableVertexAttribArray or glVertexAttribDivisorARB");
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

/*virtual*/ uint32 HyOpenGL::AddTexture(const HyTextureInfo &infoRef, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, uint32 hPBO) /*override*/
{
	GLenum eInternalFormat = GL_RGBA;
	GLenum eFormat = GL_RGBA; // Used in uncompressed
	bool bIsPixelDataCompressed = false;
	switch(infoRef.m_uiFormat)
	{
	case HYTEXTURE_Uncompressed:
		// Param1: num channels
		// Param2: disk file type (PNG, ...)
		if(infoRef.m_uiFormatParam1 == 4)
		{
			eInternalFormat = GL_RGBA;
			eFormat = GL_RGBA;
		}
		else if(infoRef.m_uiFormatParam1 == 3)
		{
			eInternalFormat = GL_RGB;
			eFormat = GL_RGB;
		}
		break;

	case HYTEXTURE_DXT:
		bIsPixelDataCompressed = true;
		// Param1: num channels
		// Param2: DXT format (1,3,5)
		if(infoRef.m_uiFormatParam2 == 1) // DXT1
		{
			if(infoRef.m_uiFormatParam1 == 4)
				eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			else
				eInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		}
		else if(infoRef.m_uiFormatParam2 == 3) // DXT3
			eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		else // DXT5
			eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;

	case HYTEXTURE_ASTC:
		bIsPixelDataCompressed = true;
		// Param1: Block Size index (4x4 -> 12x12)
		// Param2: Color Profile (LDR linear, LDR sRBG, HDR RGB, HDR RGBA)
		if(infoRef.m_uiFormatParam2 == 0)
		{
			switch(infoRef.m_uiFormatParam1)
			{
			case 0:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;   // 8.00 bpp
			case 1:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR; break;   // 6.40 bpp
			case 2:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR; break;   // 5.12 bpp
			case 3:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR; break;   // 4.27 bpp
			case 4:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR; break;   // 3.56 bpp
			case 5:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR; break;   // 3.20 bpp
			case 6:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR; break;   // 2.67 bpp
			case 7:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR; break;  // 2.56 bpp
			case 8:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR; break;  // 2.13 bpp
			case 9:  eInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR; break;   // 2.00 bpp
			case 10: eInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR; break;  // 1.60 bpp
			case 11: eInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR; break; // 1.28 bpp
			case 12: eInternalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR; break; // 1.07 bpp
			case 13: eInternalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR; break; // 0.89 bpp
			default:
				HyError("AddTexture() - Invalid ASTC m_uiFormatParam1");
				eInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
				break;
			}
		}
		else if(infoRef.m_uiFormatParam2 == 1)
		{
			switch(infoRef.m_uiFormatParam1)
			{
			case 0:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR; break;   // 8.00 bpp
			case 1:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR; break;   // 6.40 bpp
			case 2:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR; break;   // 5.12 bpp
			case 3:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR; break;   // 4.27 bpp
			case 4:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR; break;   // 3.56 bpp
			case 5:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR; break;   // 3.20 bpp
			case 6:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR; break;   // 2.67 bpp
			case 7:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR; break;  // 2.56 bpp
			case 8:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR; break;  // 2.13 bpp
			case 9:  eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR; break;   // 2.00 bpp
			case 10: eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR; break;  // 1.60 bpp
			case 11: eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR; break; // 1.28 bpp
			case 12: eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR; break; // 1.07 bpp
			case 13: eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR; break; // 0.89 bpp
			default:
				HyError("AddTexture() - Invalid SRGB8 ASTC m_uiFormatParam1");
				eInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
				break;
			}
		}
		else
		{
			HyError("AddTexture() - Invalid ASTC m_uiFormatParam2");
			eInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
		}
		break;
		
	default:
		HyLogError("Unknown TextureFormat used for 'eDesiredFormat'");
		break;
	}

	GLuint hGLTexture;
	glGenTextures(1, &hGLTexture);
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glGenTextures");

	glBindTexture(GL_TEXTURE_2D, hGLTexture);
	HyErrorCheck_OpenGL("HyOpenGLShader::AddTexture", "glBindTexture");

	if(hPBO != 0)
	{
		HyAssert(pPixelData == nullptr, "AddTexture() has a valid PBO handle as well as pixel data");
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, hPBO);
		HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glBindBuffer");
	}

	int32 iNumMipLevels = 0;
	if(infoRef.IsMipMaps())
		iNumMipLevels = static_cast<int32>(std::floor(std::log2(std::max(uiWidth, uiHeight)))) + 1;
	
	if(bIsPixelDataCompressed == false)
	{ 
		glTexImage2D(GL_TEXTURE_2D, iNumMipLevels, eInternalFormat, uiWidth, uiHeight, 0, eFormat, GL_UNSIGNED_BYTE, pPixelData);
		HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glTexImage2D");
	}
	else
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, iNumMipLevels, eInternalFormat, uiWidth, uiHeight, 0, uiPixelDataSize, pPixelData);
		HyErrorCheck_OpenGL("HyOpenGL::AddTexture", "glCompressedTexImage2D");
	}

	if(hPBO != 0)
	{
		for(uint32 i = 0; i < HY_NUM_PBO; ++i)
		{
			if(m_pPboHandles[i] == hPBO)
				m_pPboStates[i] = PBO_Pending3;
		}
		//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	switch(infoRef.m_uiFiltering)
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
///*virtual*/ uint32 HyOpenGL::AddTextureArray(const HyTextureInfo &infoRef, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) /*override*/
//{
//	GLenum eInternalFormat = uiNumColorChannels == 4 ? GL_RGBA8 : (uiNumColorChannels == 3 ? GL_RGB8 : GL_R8);
//	GLenum eFormat = uiNumColorChannels == 4 ? GL_RGBA : (uiNumColorChannels == 3 ? GL_RGB : GL_RED);
//
//	GLuint hGLTextureArray;
//	glGenTextures(1, &hGLTextureArray);
//	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glGenTextures");
//
//	//glActiveTexture(GL_TEXTURE0 + hGLTextureArray);
//	glBindTexture(GL_TEXTURE_2D_ARRAY, hGLTextureArray);
//	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glBindTexture");
//
//	// Create (blank) storage for the texture array
//	GLenum eError = GL_NO_ERROR;
//	uiNumTexturesUploadedOut = static_cast<uint32>(pixelDataList.size());
//
//	// TODO: Don't upload huge texture arrays. Actually calculate required bytes, and then size array accordingly to hardware constraints
//	if(uiNumTexturesUploadedOut > 8)
//		uiNumTexturesUploadedOut = 8;
//
//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, eInternalFormat, uiWidth, uiHeight, uiNumTexturesUploadedOut, 0, eFormat, GL_UNSIGNED_BYTE, NULL);
//	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glTexImage3D");
//	eError = glGetError();
//
//	while (eError)
//	{
//		uiNumTexturesUploadedOut /= 2;
//		if(uiNumTexturesUploadedOut == 0)
//			HyError("Could not allocate texture array.");
//
//		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, eInternalFormat, uiWidth, uiHeight, uiNumTexturesUploadedOut, 0, eFormat, GL_UNSIGNED_BYTE, NULL);
//		eError = glGetError();
//	}
//
//	for(uint32 i = 0; i != uiNumTexturesUploadedOut; ++i)
//	{
//		// Write each texture into storage
//		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
//						0,						// Mipmap number
//						0, 0, i,				// xoffset, yoffset, zoffset
//						uiWidth, uiHeight, 1,	// width, height, depth (of texture you're copying in)
//						eFormat,				// format
//						GL_UNSIGNED_BYTE,		// type
//						pixelDataList[i]);		// pointer to pixel data
//
//		HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glTexSubImage3D");
//	}
//
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	HyErrorCheck_OpenGL("HyOpenGL:AddTextureArray", "glTexParameteri");
//
//	return hGLTextureArray;
//}

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

/*virtual*/ uint8 *HyOpenGL::GetPixelBufferPtr(uint32 uiMaxBufferSize, uint32 &hPboOut) /*override*/
{
	hPboOut = 0;

	if(m_pPboHandles == nullptr)
		return nullptr;

#ifndef HY_PLATFORM_BROWSER
	for(uint32 i = 0; i < HY_NUM_PBO; ++i)
	{
		if(m_pPboStates[i] == PBO_Free)
		{
			m_pPboStates[i] = PBO_Mapped;

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pPboHandles[i]);
			HyErrorCheck_OpenGL("HyOpenGL::GetPixelBufferPtr", "glBindBuffer");
			glBufferData(GL_PIXEL_UNPACK_BUFFER, uiMaxBufferSize, nullptr, GL_STREAM_DRAW); // Reserve size
			HyErrorCheck_OpenGL("HyOpenGL::GetPixelBufferPtr", "glBufferData");

			uint8 *pMappedPtr = static_cast<uint8 *>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
			HyErrorCheck_OpenGL("HyOpenGL::GetPixelBufferPtr", "glMapBuffer");

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			hPboOut = m_pPboHandles[i];
			return pMappedPtr;
		}
	}
#endif

	return nullptr;
}

void HyOpenGL::CompileShader(HyShader *pShader, HyShaderType eType)
{
	GLuint iShaderHandle = 0;

	switch(eType)
	{
	case HYSHADER_Vertex:			iShaderHandle = glCreateShader(GL_VERTEX_SHADER);				break;
	case HYSHADER_Fragment:			iShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);				break;
#ifndef HY_PLATFORM_BROWSER // emscripten compiled these before when I used glew.h
	case HYSHADER_Geometry:			iShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);				break;
	case HYSHADER_TessControl:		iShaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);			break;
	case HYSHADER_TessEvaluation:	iShaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);		break;
#endif
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

			HyLogError(szlog);
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
		m_mtxView = glm::inverse(pCamera->GetSceneTransform()); // View Matrix is calculated by taking the inverse of the camera transform
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

	float fFbWidth = (viewportRect.Width() * vFramebufferSize.x);
	float fFbHeight = (viewportRect.Height() * vFramebufferSize.y);

	m_mtxProj = glm::ortho(fFbWidth * -0.5f, fFbWidth * 0.5f, fFbHeight * -0.5f, fFbHeight * 0.5f, 0.0f, 1.0f);

	glViewport(static_cast<GLint>(viewportRect.left * vFramebufferSize.x),
		static_cast<GLint>(viewportRect.bottom * vFramebufferSize.y),
		m_pCurWindow->GetWidth(),
		m_pCurWindow->GetHeight());
	HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glViewport");
	// TODO: glViewport width and height are silently clamped to a range that depends on the implementation. To query this range, call glGet with argument GL_MAX_VIEWPORT_DIMS

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set the proper shader program
	GLuint uiVao = m_VaoMapList[m_pCurWindow->GetIndex()][pRenderState->m_hSHADER];
	glBindVertexArray(uiVao);
	HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glBindVertexArray");

	GLuint hGlHandle = m_GLShaderMap[pRenderState->m_hSHADER];
	glUseProgram(hGlHandle);
	HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUseProgram");

	HyShader *pShader = m_ShaderMap[pRenderState->m_hSHADER];
	HyAssert(pShader, "HyShader not found for render state: " << pRenderState->m_uiID);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Bind texture(s)
	
	// Parse texture unit/samplers & uniform data from ExBuffer
	char *pTexUnitExBuffer = reinterpret_cast<char *>(pRenderState) + sizeof(HyRenderBuffer::State);
	uint32 uiNumTextureUnits = *reinterpret_cast<uint32 *>(pTexUnitExBuffer);
	pTexUnitExBuffer += sizeof(uint32);
	for(uint32 i = 0; i < uiNumTextureUnits; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glActiveTexture");

		GLuint uiTexHandle = *reinterpret_cast<HyTextureHandle *>(pTexUnitExBuffer);
		pTexUnitExBuffer += sizeof(HyTextureHandle);
		
		glBindTexture(GL_TEXTURE_2D, uiTexHandle);
		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glBindTexture");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(pRenderState->m_SCISSOR_RECT.iTag != IHyBody::SCISSORTAG_Disabled)
	{
		const HyScreenRect<int32> &scissorRectRef = pRenderState->m_SCISSOR_RECT;

		glScissor(static_cast<GLint>(m_mtxView[0].x * scissorRectRef.x) + static_cast<GLint>(m_mtxView[3].x) + (m_pCurWindow->GetFramebufferSize().x / 2),
				  static_cast<GLint>(m_mtxView[1].y * scissorRectRef.y) + static_cast<GLint>(m_mtxView[3].y) + (m_pCurWindow->GetFramebufferSize().y / 2),
				  static_cast<GLsizei>(m_mtxView[0].x * scissorRectRef.width),
				  static_cast<GLsizei>(m_mtxView[1].y * scissorRectRef.height));

		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glScissor");

		glEnable(GL_SCISSOR_TEST);
		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glEnable");
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glDisable");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Always attempt to assign these uniforms if the shader chooses to use them
	GLint iUniLocation = glGetUniformLocation(hGlHandle, "u_mtxWorldToCamera");
	HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glGetUniformLocation");
	if(iUniLocation >= 0)
	{
		glUniformMatrix4fv(iUniLocation, 1, GL_FALSE, &m_mtxView[0][0]);
		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniformMatrix4fv");
	}

	iUniLocation = glGetUniformLocation(hGlHandle, "u_mtxCameraToClip");
	HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glGetUniformLocation");
	if(iUniLocation >= 0)
	{
		glUniformMatrix4fv(iUniLocation, 1, GL_FALSE, &m_mtxProj[0][0]);
		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniformMatrix4fv");
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assign any other uniforms by parsing the RenderState's ex buffer portion

	char *pExBuffer = reinterpret_cast<char *>(pRenderState) + sizeof(HyRenderBuffer::State) + sizeof(uint32) + (uiNumTextureUnits * sizeof(HyTextureHandle));
	uint32 uiNumUniforms = *reinterpret_cast<uint32 *>(pExBuffer);
	pExBuffer += sizeof(uint32);

	for(uint32 i = 0; i < uiNumUniforms; ++i)
	{
		const char *szUniformName = pExBuffer;
		pExBuffer += HY_SHADER_UNIFORM_NAME_LENGTH;

		iUniLocation = glGetUniformLocation(hGlHandle, szUniformName);
		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glGetUniformLocation");
		HyAssert(iUniLocation >= 0, "HyOpenGLShader::SetUniformGLSL - Uniform location returned '-1' for \"" << szUniformName << "\"");

		HyShaderVariable eVarType = static_cast<HyShaderVariable>(*reinterpret_cast<uint32 *>(pExBuffer));
		pExBuffer += sizeof(uint32);
		switch(eVarType)
		{
		case HyShaderVariable::boolean:
			glUniform1i(iUniLocation, *reinterpret_cast<bool *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform1i");
			pExBuffer += sizeof(bool);
			break;

		case HyShaderVariable::int32:
			glUniform1i(iUniLocation, *reinterpret_cast<int32 *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform1i");
			pExBuffer += sizeof(int32);
			break;

		case HyShaderVariable::uint32:
			glUniform1ui(iUniLocation, *reinterpret_cast<uint32 *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform1ui");
			pExBuffer += sizeof(uint32);
			break;

		case HyShaderVariable::float32:
			glUniform1f(iUniLocation, *reinterpret_cast<float *>(pExBuffer));
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform1f");
			pExBuffer += sizeof(float);
			break;

		case HyShaderVariable::double64:
			HyError("GLSL Shader uniform does not support type double yet!");
			break;

		case HyShaderVariable::bvec2:
			glUniform2i(iUniLocation, reinterpret_cast<glm::bvec2 *>(pExBuffer)->x, reinterpret_cast<glm::bvec2 *>(pExBuffer)->y);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform2i");
			pExBuffer += sizeof(glm::bvec2);
			break;

		case HyShaderVariable::bvec3:
			glUniform3i(iUniLocation, reinterpret_cast<glm::bvec3 *>(pExBuffer)->x,
									  reinterpret_cast<glm::bvec3 *>(pExBuffer)->y,
									  reinterpret_cast<glm::bvec3 *>(pExBuffer)->z);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform3i");
			pExBuffer += sizeof(glm::bvec3);
			break;

		case HyShaderVariable::bvec4:
			glUniform4i(iUniLocation, reinterpret_cast<glm::bvec4 *>(pExBuffer)->x,
									  reinterpret_cast<glm::bvec4 *>(pExBuffer)->y,
									  reinterpret_cast<glm::bvec4 *>(pExBuffer)->z,
									  reinterpret_cast<glm::bvec4 *>(pExBuffer)->w);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform4i");
			pExBuffer += sizeof(glm::bvec4);
			break;

		case HyShaderVariable::ivec2:
			glUniform2i(iUniLocation, reinterpret_cast<glm::ivec2 *>(pExBuffer)->x, reinterpret_cast<glm::ivec2 *>(pExBuffer)->y);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform2i");
			pExBuffer += sizeof(glm::ivec2);
			break;

		case HyShaderVariable::ivec3:
			glUniform3i(iUniLocation, reinterpret_cast<glm::ivec3 *>(pExBuffer)->x,
									  reinterpret_cast<glm::ivec3 *>(pExBuffer)->y,
									  reinterpret_cast<glm::ivec3 *>(pExBuffer)->z);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform3i");
			pExBuffer += sizeof(glm::ivec3);
			break;

		case HyShaderVariable::ivec4:
			glUniform4i(iUniLocation, reinterpret_cast<glm::ivec4 *>(pExBuffer)->x,
									  reinterpret_cast<glm::ivec4 *>(pExBuffer)->y,
									  reinterpret_cast<glm::ivec4 *>(pExBuffer)->z,
									  reinterpret_cast<glm::ivec4 *>(pExBuffer)->w);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform4i");
			pExBuffer += sizeof(glm::ivec4);
			break;

		case HyShaderVariable::vec2:
			glUniform2f(iUniLocation, reinterpret_cast<glm::vec2 *>(pExBuffer)->x, reinterpret_cast<glm::vec2 *>(pExBuffer)->y);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform2f");
			pExBuffer += sizeof(glm::vec2);
			break;

		case HyShaderVariable::vec3:
			glUniform3f(iUniLocation, reinterpret_cast<glm::vec3 *>(pExBuffer)->x,
									  reinterpret_cast<glm::vec3 *>(pExBuffer)->y,
									  reinterpret_cast<glm::vec3 *>(pExBuffer)->z);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform3f");
			pExBuffer += sizeof(glm::vec3);
			break;

		case HyShaderVariable::vec4:
			glUniform4f(iUniLocation, reinterpret_cast<glm::vec4 *>(pExBuffer)->x,
									  reinterpret_cast<glm::vec4 *>(pExBuffer)->y,
									  reinterpret_cast<glm::vec4 *>(pExBuffer)->z,
									  reinterpret_cast<glm::vec4 *>(pExBuffer)->w);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniform4f");
			pExBuffer += sizeof(glm::vec4);
			break;

		case HyShaderVariable::dvec2:
		case HyShaderVariable::dvec3:
		case HyShaderVariable::dvec4:
			HyError("GLSL Shader uniform does not support type double yet!");
			break;

		case HyShaderVariable::mat3:
			glUniformMatrix3fv(iUniLocation, 1, GL_FALSE, &(*reinterpret_cast<glm::mat3 *>(pExBuffer))[0][0]);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniformMatrix3fv");
			pExBuffer += sizeof(glm::mat3);
			break;

		case HyShaderVariable::mat4:
			glUniformMatrix4fv(iUniLocation, 1, GL_FALSE, &(*reinterpret_cast<glm::mat4 *>(pExBuffer))[0][0]);
			HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glUniformMatrix4fv");
			pExBuffer += sizeof(glm::mat4);
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set vertex attribute pointers to the vertex data locations in bound buffer
	size_t uiStartOffset = pRenderState->m_uiDATA_OFFSET;
	size_t uiOffset = 0;
	std::vector<HyShaderVertexAttribute> &shaderVertexAttribListRef = pShader->GetVertextAttributes();
	for(size_t i = 0; i < shaderVertexAttribListRef.size(); ++i)
	{
		// TODO: Cache the attribute location instead of finding it every time
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
#ifndef HY_PLATFORM_BROWSER
		case HyShaderVariable::double64:
			glVertexAttribLPointer(uiLocation, 1, GL_DOUBLE, pShader->GetStride(), reinterpret_cast<void *>(uiStartOffset + uiOffset));
			uiOffset += sizeof(GLdouble);
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
#endif
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

		default:
			HyError("Unsupported HyShaderVariable '" << static_cast<int>(shaderVertexAttribListRef[i].eVarType) << "' for this platform");
			break;
		}

		HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glVertexAttribPointer[" << static_cast<int>(shaderVertexAttribListRef[i].eVarType) << "]");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GLenum eDrawMode;
	switch(pRenderState->m_eRENDER_MODE)
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
	glDrawArraysInstanced(eDrawMode, 0, pRenderState->m_uiNUM_VERTS_PER_INSTANCE, pRenderState->m_uiNumInstances);
	HyErrorCheck_OpenGL("HyOpenGL::RenderPass2d", "glDrawArraysInstanced");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reset OpenGL states
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
