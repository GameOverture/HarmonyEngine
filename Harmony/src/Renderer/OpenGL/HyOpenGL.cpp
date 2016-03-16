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
#include "Renderer/OpenGL/HyOpenGLShaderSrc.h"

HyOpenGL::HyOpenGL(HyGfxComms &gfxCommsRef, vector<HyWindow> &viewportsRef) :	IHyRenderer(gfxCommsRef, viewportsRef),
																				m_mtxView(1.0f)
{
#ifdef HY_PLATFORM_GUI
	Initialize();
#endif
}

HyOpenGL::~HyOpenGL(void)
{
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
	// Only draw cameras that are apart of this render surface
	while(m_RenderSurfaceIter->m_iID != GetCameraWindowIndex2d(m_iCurCamIndex) && m_iCurCamIndex < GetNumCameras2d())
		m_iCurCamIndex++;

	if(GetNumRenderStates2d() == 0 || m_iCurCamIndex >= GetNumCameras2d())
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

	if(renderState.IsEnabled(HyRenderState::USINGSCREENCOORDS))
		SetCameraMatrices_2d(MTX_SCREENVIEW);
	else
		SetCameraMatrices_2d(MTX_CAMVIEW);

	if(renderState.IsEnabled(HyRenderState::SHADER_QUADBATCH))
	{
		glBindVertexArray(m_pVAO2d[QUADBATCH]);
		m_pShader2d[QUADBATCH].Use();

		m_pShader2d[QUADBATCH].SetUniform("mtxWorldToCameraMatrix", m_mtxView);
		m_pShader2d[QUADBATCH].SetUniform("mtxCameraToClipMatrix", m_mtxProj);

		size_t uiDataOffset = renderState.GetDataOffset();
		GLuint mtx = m_pShader2d[QUADBATCH].GetAttribLocation("mtxLocalToWorld");

		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("size"),			2, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset));
		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("offset"),		2, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (2*sizeof(GLfloat))));
		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("tint"),			4, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (4*sizeof(GLfloat))));
		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("textureIndex"),	1, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (8*sizeof(GLfloat))));
		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord0"),		2, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (9*sizeof(GLfloat))));
		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord1"),		2, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (11*sizeof(GLfloat))));
		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord2"),		2, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (13*sizeof(GLfloat))));
		glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord3"),		2, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (15*sizeof(GLfloat))));
		glVertexAttribPointer(mtx + 0,													4, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (17*sizeof(GLfloat))));
		glVertexAttribPointer(mtx + 1,													4, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (21*sizeof(GLfloat))));
		glVertexAttribPointer(mtx + 2,													4, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (25*sizeof(GLfloat))));
		glVertexAttribPointer(mtx + 3,													4, GL_FLOAT, GL_FALSE, 132, (void *)(uiDataOffset + (29*sizeof(GLfloat))));

		glBindTexture(GL_TEXTURE_2D_ARRAY, renderState.GetTextureHandle());

		int iNumInsts = renderState.GetNumInstances();
		glDrawArraysInstanced(m_eDrawMode, 0, 4, iNumInsts);
	}
	else if(renderState.IsEnabled(HyRenderState::SHADER_PRIMITIVEDRAW))
	{
		glBindVertexArray(m_pVAO2d[PRIMITIVE]);
		m_pShader2d[PRIMITIVE].Use();

		m_pShader2d[PRIMITIVE].SetUniform("worldToCameraMatrix", m_mtxView);
		m_pShader2d[PRIMITIVE].SetUniform("cameraToClipMatrix", m_mtxProj);

		char *pDrawData = GetVertexData2d();
		
		size_t uiDataOffset = renderState.GetDataOffset();
		pDrawData += uiDataOffset;

		for(uint32 i = 0; i < renderState.GetNumInstances(); ++i)
		{
			m_pShader2d[PRIMITIVE].SetUniform("primitiveColor", *reinterpret_cast<vec4 *>(pDrawData));
			pDrawData += sizeof(vec4);
			uiDataOffset += sizeof(vec4);
			
			m_pShader2d[PRIMITIVE].SetUniform("transformMtx", *reinterpret_cast<mat4 *>(pDrawData));
			pDrawData += sizeof(mat4);
			uiDataOffset += sizeof(mat4);

			uint32 iNumVerts = *reinterpret_cast<uint32 *>(pDrawData);
			pDrawData += sizeof(uint32);
			uiDataOffset += sizeof(uint32);
			
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)uiDataOffset);
			glDrawArrays(m_eDrawMode, 0, iNumVerts);

			pDrawData += (sizeof(GLfloat) * 4) * iNumVerts;
			uiDataOffset += (sizeof(GLfloat) * 4) * iNumVerts;
		}
	}
}

/*virtual*/ void HyOpenGL::End_2d()
{
	m_iCurCamIndex++;
	glDepthMask(true);

	glBindVertexArray(0);
	glUseProgram(0);
}

/*virtual*/ void HyOpenGL::FinishRender()
{

}

// Returns the texture ID used for API specific drawing.
/*virtual*/ uint32 HyOpenGL::AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, vector<unsigned char *> &vPixelData)
{
	GLenum eInternalFormat = uiNumColorChannels == 4 ? GL_RGBA8 : (uiNumColorChannels == 3 ? GL_RGB8 : GL_R8);
	GLenum eFormat = uiNumColorChannels == 4 ? GL_RGBA : (uiNumColorChannels == 3 ? GL_RGB : GL_RED);

	GLuint hGLTextureArray;
	glGenTextures(1, &hGLTextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, hGLTextureArray);

	uint32 uiNumTextures = static_cast<uint32>(vPixelData.size());

	// Create (blank) storage for the texture array
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, eFormat, uiWidth, uiHeight, uiNumTextures, 0, eFormat, GL_UNSIGNED_BYTE, NULL);

	for(uint32 i = 0; i != uiNumTextures; ++i)
	{
		// Write each texture into storage
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
						0,						// Mipmap number
						0, 0, i,				// xoffset, yoffset, zoffset
						uiWidth, uiHeight, 1,	// width, height, depth (of texture you're copying in)
						eFormat,				// format
						GL_UNSIGNED_BYTE,		// type
						vPixelData[i]);			// pointer to pixel data
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

bool HyOpenGL::Initialize()
{
	//////////////////////////////////////////////////////////////////////////
	// Init GLEW
	//////////////////////////////////////////////////////////////////////////
	GLenum err = glewInit();

	if(err != GLEW_OK)
	{
		GLenum eError = glGetError();
		//const GLubyte *szErrorStr;
		//szErrorStr = gluErrorString(eError);
		HyError("OpenGL Error: " << eError/* << std::endl << szErrorStr*/);
	}

	//const GLubyte *pExtStr = glGetString(GL_EXTENSIONS);
	//WriteTextFile("GLExtensions.txt", glGetString(GL_EXTENSIONS));

	//if (glewIsSupported("GL_VERSION_3_3"))
	//	printf("Ready for OpenGL 3.3\n");
	//else {
	//	printf("OpenGL 3.3 not supported\n");
	//	exit(1);
	//}
	//printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	//printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	//printf ("Version: %s\n", glGetString (GL_VERSION));
	//printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	GLint iMaxVertexAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &iMaxVertexAttribs);
	HyAssert(iMaxVertexAttribs >= 12, "GL_MAX_VERTEX_ATTRIBS is < 12");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 0.5f, 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// 2D setup
	//////////////////////////////////////////////////////////////////////////

	glGenVertexArrays(NUM_VAOTYPES, m_pVAO2d);

	glGenBuffers(1, &m_hVBO2d);
	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);

	// Quad batch //////////////////////////////////////////////////////////////////////////
	m_pShader2d[QUADBATCH].CompileFromString(szHYQUADBATCH_VERTEXSHADER, HyOpenGLShader::VERTEX);
	m_pShader2d[QUADBATCH].CompileFromString(szHYQUADBATCH_FRAGMENTSHADER, HyOpenGLShader::FRAGMENT);

	m_pShader2d[QUADBATCH].Link();

	glBindVertexArray(m_pVAO2d[QUADBATCH]);

	GLuint size = m_pShader2d[QUADBATCH].GetAttribLocation("size");
	GLuint offset = m_pShader2d[QUADBATCH].GetAttribLocation("offset");
	GLuint tint = m_pShader2d[QUADBATCH].GetAttribLocation("tint");
	GLuint textureIndex = m_pShader2d[QUADBATCH].GetAttribLocation("textureIndex");
	GLuint uv0 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord0");
	GLuint uv1 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord1");
	GLuint uv2 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord2");
	GLuint uv3 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord3");
	GLuint mtx = m_pShader2d[QUADBATCH].GetAttribLocation("mtxLocalToWorld");

	glEnableVertexAttribArray(size);
	glEnableVertexAttribArray(offset);
	glEnableVertexAttribArray(tint);
	glEnableVertexAttribArray(textureIndex);
	glEnableVertexAttribArray(uv0);
	glEnableVertexAttribArray(uv1);
	glEnableVertexAttribArray(uv2);
	glEnableVertexAttribArray(uv3);
	glEnableVertexAttribArray(mtx + 0);
	glEnableVertexAttribArray(mtx + 1);
	glEnableVertexAttribArray(mtx + 2);
	glEnableVertexAttribArray(mtx + 3);

	////////////////////////////////////////////////////////////////////////////
	//// ALL HERE IS PROBABLY NOT NEEDED
	//glVertexAttribPointer(size, 2, GL_FLOAT, GL_FALSE, 132, (void *)0);
	//glVertexAttribPointer(offset, 2, GL_FLOAT, GL_FALSE, 132, (void *)(2 * sizeof(GLfloat)));
	//glVertexAttribPointer(tint, 4, GL_FLOAT, GL_FALSE, 132, (void *)(4 * sizeof(GLfloat)));
	//glVertexAttribPointer(textureIndex, 1, GL_UNSIGNED_INT, GL_FALSE, 132, (void *)(sizeof(GLuint)));
	//glVertexAttribPointer(uv0, 2, GL_FLOAT, GL_FALSE, 132, (void *)(8 * sizeof(GLfloat)));
	//glVertexAttribPointer(uv1, 2, GL_FLOAT, GL_FALSE, 132, (void *)(10 * sizeof(GLfloat)));
	//glVertexAttribPointer(uv2, 2, GL_FLOAT, GL_FALSE, 132, (void *)(12 * sizeof(GLfloat)));
	//glVertexAttribPointer(uv3, 2, GL_FLOAT, GL_FALSE, 132, (void *)(14 * sizeof(GLfloat)));
	//glVertexAttribPointer(mtx + 0, 4, GL_FLOAT, GL_FALSE, 132, (void *)(16 * sizeof(GLfloat)));
	//glVertexAttribPointer(mtx + 1, 4, GL_FLOAT, GL_FALSE, 132, (void *)(20 * sizeof(GLfloat)));
	//glVertexAttribPointer(mtx + 2, 4, GL_FLOAT, GL_FALSE, 132, (void *)(24 * sizeof(GLfloat)));
	//glVertexAttribPointer(mtx + 3, 4, GL_FLOAT, GL_FALSE, 132, (void *)(28 * sizeof(GLfloat)));
	////////////////////////////////////////////////////////////////////////////

	glVertexAttribDivisor(size, 1);
	glVertexAttribDivisor(offset, 1);
	glVertexAttribDivisor(tint, 1);
	glVertexAttribDivisor(textureIndex, 1);
	glVertexAttribDivisor(uv0, 1);
	glVertexAttribDivisor(uv1, 1);
	glVertexAttribDivisor(uv2, 1);
	glVertexAttribDivisor(uv3, 1);
	glVertexAttribDivisor(mtx + 0, 1);
	glVertexAttribDivisor(mtx + 1, 1);
	glVertexAttribDivisor(mtx + 2, 1);
	glVertexAttribDivisor(mtx + 3, 1);

	// Primitive //////////////////////////////////////////////////////////////////////////
	m_pShader2d[PRIMITIVE].CompileFromString(szHYPRIMATIVE_VERTEXSHADER, HyOpenGLShader::VERTEX);
	m_pShader2d[PRIMITIVE].CompileFromString(szHYPRIMATIVE_FRAGMENTSHADER, HyOpenGLShader::FRAGMENT);

	m_pShader2d[PRIMITIVE].Link();

	glBindVertexArray(m_pVAO2d[PRIMITIVE]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);


	//const float fUnitQuadVertPos[16] = {
	//	0.0f, 0.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 0.0f, 1.0f,
	//	0.0f, 1.0f, 0.0f, 1.0f,
	//	1.0f, 1.0f, 0.0f, 1.0f
	//};
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, fUnitQuadVertPos, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexDataEmulate), vertexDataEmulate, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(primitiveVertexDataEmulate), primitiveVertexDataEmulate, GL_STATIC_DRAW);
	//-----------------------------------------------------------------------------------------------------------------

	// Initialize 2d quad index bufferSet up index data
	//const short g_i16UnitQuadVertIndices[6] = {
	//	0, 1, 2, 2, 1, 3
	//};
	//glGenBuffers(1, &m_hIBO2d);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO2d);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short)*6, g_i16UnitQuadVertIndices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//m_pGenericIndexBuffer2d = new uint16[HY_INDEX_BUFFER_SIZE/sizeof(uint16)];
	//uint16 *pCurWriteShort = m_pGenericIndexBuffer2d;
	//uint16 uiIndexCount = 0;
	//uint32 uiResetIndexCount = 0;
	//for(int i = 0; i < (HY_INDEX_BUFFER_SIZE/2); ++i)
	//{
	//	uiResetIndexCount++;
	//	if(uiResetIndexCount == 5)
	//	{
	//		*pCurWriteShort = HY_RESTART_INDEX;
	//		uiResetIndexCount = 0;
	//	}
	//	else
	//	{
	//		*pCurWriteShort = uiIndexCount;
	//		uiIndexCount++;
	//	}

	//	pCurWriteShort++;
	//}
	//glPrimitiveRestartIndex(HY_RESTART_INDEX);
	//glEnable(GL_PRIMITIVE_RESTART);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void HyOpenGL::SetCameraMatrices_2d(eMatrixStack eMtxStack)
{
	HyRectangle<float> viewportRect;
	if(eMtxStack == MTX_CAMVIEW)
		viewportRect = *GetCameraViewportRect2d(m_iCurCamIndex);
	else
		viewportRect.Set(0.0f, 0.0f, 1.0f, 1.0f);

	float fWidth = (viewportRect.Width() * m_RenderSurfaceIter->m_iRenderSurfaceWidth);
	float fHeight = (viewportRect.Height() * m_RenderSurfaceIter->m_iRenderSurfaceHeight);

	glViewport(static_cast<GLint>(viewportRect.left * m_RenderSurfaceIter->m_iRenderSurfaceWidth),
			   static_cast<GLint>(viewportRect.top * m_RenderSurfaceIter->m_iRenderSurfaceHeight),
			   static_cast<GLsizei>(fWidth),
			   static_cast<GLsizei>(fHeight));

	if(eMtxStack == MTX_CAMVIEW)
		m_mtxView = *GetCameraView2d(m_iCurCamIndex);
	else
	{
		m_mtxView = mat4(1.0f);
		m_mtxView = glm::translate(m_mtxView, fWidth * -0.5f, fHeight * -0.5f, 0.0f);
	}
	m_mtxProj = glm::ortho(fWidth * -0.5f, fWidth * 0.5f, fHeight * -0.5f, fHeight * 0.5f);
}
