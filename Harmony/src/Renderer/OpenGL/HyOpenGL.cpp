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

HyOpenGL::HyOpenGL(HyGfxComms &gfxCommsRef, vector<HyWindow> &viewportsRef) : IHyRenderer(gfxCommsRef, viewportsRef),
																				m_mtxView(1.0f),
																				m_kmtxIdentity(1.0f)
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
	m_iNumRenderPassesLeft2d = GetNumCameras2d();
	m_iNumPasses3d = GetNumCameras3d();
}

/*virtual*/ bool HyOpenGL::Begin_3d()
{
	return false;
}

/*virtual*/ void HyOpenGL::SetRenderState_3d(uint32 uiNewRenderState)
{
}

/*virtual*/ void HyOpenGL::End_3d()
{
}

/*virtual*/ bool HyOpenGL::Begin_2d()
{
	while(m_RenderSurfaceIter->m_iID != GetCameraWindowIndex2d(GetNumCameras2d() - m_iNumRenderPassesLeft2d) && m_iNumRenderPassesLeft2d > 0)
	{
		m_iNumRenderPassesLeft2d--;
	}
	if(GetNumRenderStates2d() == 0 || m_iNumRenderPassesLeft2d == 0)
		return false;

	int iCameraIndex = GetNumCameras2d() - m_iNumRenderPassesLeft2d;
	int iWindowIndex = GetCameraWindowIndex2d(iCameraIndex);

	if(m_iNumRenderPassesLeft2d == GetNumCameras2d())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);
		glBufferData(GL_ARRAY_BUFFER, m_pDrawBufferHeader->uiVertexBufferSize2d, GetVertexData2d(), GL_DYNAMIC_DRAW);
	
		m_mtxProjLocalCoords = glm::ortho(m_RenderSurfaceIter->m_iRenderSurfaceWidth * -0.5f, m_RenderSurfaceIter->m_iRenderSurfaceWidth * 0.5f, m_RenderSurfaceIter->m_iRenderSurfaceHeight * -0.5f, m_RenderSurfaceIter->m_iRenderSurfaceHeight * 0.5f);
	}
	

	HyRectangle<float> *pViewportRect = GetCameraViewportRect2d(iCameraIndex);
	glViewport(static_cast<GLint>(pViewportRect->x * m_RenderSurfaceIter->m_iRenderSurfaceWidth), static_cast<GLint>(pViewportRect->y * m_RenderSurfaceIter->m_iRenderSurfaceHeight), static_cast<GLsizei>(pViewportRect->width * m_RenderSurfaceIter->m_iRenderSurfaceWidth), static_cast<GLsizei>(pViewportRect->height * m_RenderSurfaceIter->m_iRenderSurfaceHeight));

	float fWidth = (pViewportRect->width * m_RenderSurfaceIter->m_iRenderSurfaceWidth);
	float fHeight = (pViewportRect->height * m_RenderSurfaceIter->m_iRenderSurfaceHeight);
	
	m_mtxView = *GetCameraView2d(iCameraIndex);
	m_mtxProj = glm::ortho(fWidth * -0.5f, fWidth * 0.5f, fHeight * -0.5f, fHeight * 0.5f);

	m_iNumRenderPassesLeft2d--;
	
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

	// Change shader program based on render state flags and set uniforms
	if(m_PrevRenderState.CompareAttribute(renderState, HyRenderState::SHADERMASK) == false)
	{
		if(renderState.IsEnabled(HyRenderState::SHADER_QUADBATCH))
		{
			glBindVertexArray(m_pVAO2d[QUADBATCH]);
			m_pShader2d[QUADBATCH].Use();

			if(renderState.IsEnabled(HyRenderState::USINGLOCALCOORDS))
			{
				m_pShader2d[QUADBATCH].SetUniform("mtxWorldToCameraMatrix", m_kmtxIdentity);
				m_pShader2d[QUADBATCH].SetUniform("mtxCameraToClipMatrix", m_mtxProjLocalCoords);
			}
			else
			{
				m_pShader2d[QUADBATCH].SetUniform("mtxWorldToCameraMatrix", m_mtxView);
				m_pShader2d[QUADBATCH].SetUniform("mtxCameraToClipMatrix", m_mtxProj);
			}

			size_t uiDataOffset = renderState.GetDataOffset();

			GLuint mtx = m_pShader2d[QUADBATCH].GetAttribLocation("mtxLocalToWorld");
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("size"), 2, GL_FLOAT, GL_FALSE, 128, (void *)uiDataOffset);
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("offset"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (2*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("tint"), 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (4*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("textureIndex"), 1, GL_UNSIGNED_INT, GL_FALSE, 132, (void *)(uiDataOffset + (8 * sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord0"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (8*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord1"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (10 * sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord2"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (12 * sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord3"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (14 * sizeof(GLfloat))));
			glVertexAttribPointer(mtx + 0, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (16 * sizeof(GLfloat))));
			glVertexAttribPointer(mtx + 1, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (20 * sizeof(GLfloat))));
			glVertexAttribPointer(mtx + 2, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (24 * sizeof(GLfloat))));
			glVertexAttribPointer(mtx + 3, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + sizeof(GLuint) + (28 * sizeof(GLfloat))));

			glBindTexture(GL_TEXTURE_2D_ARRAY, renderState.GetTextureHandle());

			int iNumInsts = renderState.GetNumInstances();
			glDrawArraysInstanced(m_eDrawMode, 0, 4, iNumInsts);
		}
		else if(renderState.IsEnabled(HyRenderState::SHADER_PRIMITIVEDRAW))
		{
			glBindVertexArray(m_pVAO2d[PRIMITIVE]);
			m_pShader2d[PRIMITIVE].Use();

			if(renderState.IsEnabled(HyRenderState::USINGLOCALCOORDS))
			{
				m_pShader2d[PRIMITIVE].SetUniform("worldToCameraMatrix", m_kmtxIdentity);
				m_pShader2d[PRIMITIVE].SetUniform("cameraToClipMatrix", m_mtxProjLocalCoords);
			}
			else
			{
				m_pShader2d[PRIMITIVE].SetUniform("worldToCameraMatrix", m_mtxView);
				m_pShader2d[PRIMITIVE].SetUniform("cameraToClipMatrix", m_mtxProj);
			}


			size_t uiDataOffset = renderState.GetDataOffset();

			char *pDrawData = GetVertexData2d();
			pDrawData += uiDataOffset;
			m_pShader2d[PRIMITIVE].SetUniform("primitiveColor", *reinterpret_cast<vec4 *>(pDrawData));
			pDrawData += sizeof(vec4);
			uiDataOffset += sizeof(vec4);
			
			m_pShader2d[PRIMITIVE].SetUniform("transformMtx", *reinterpret_cast<mat4 *>(pDrawData));
			pDrawData += sizeof(mat4);
			uiDataOffset += sizeof(mat4);

			uint32 iNumVerts = *reinterpret_cast<uint32 *>(pDrawData);
			uiDataOffset += sizeof(uint32);
			
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)uiDataOffset);

			glDrawArrays(m_eDrawMode, 0, iNumVerts);
		}
	}
}

/*virtual*/ void HyOpenGL::End_2d()
{
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
	GLuint hGLTextureArray;
	glGenTextures(1, &hGLTextureArray);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, hGLTextureArray);

	GLenum eInternalFormat = uiNumColorChannels == 4 ? GL_RGBA8 : (uiNumColorChannels == 3 ? GL_RGB8 : GL_R8);
	GLenum eFormat = uiNumColorChannels == 4 ? GL_RGBA : (uiNumColorChannels == 3 ? GL_RGB : GL_RED);

	// Create storage for the texture
	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
					1,						// Number of mipmaps
					eInternalFormat,		// Internal format
					uiWidth, uiHeight,		// width, height
					static_cast<int32>(vPixelData.size()));

	for(unsigned int i = 0; i != vPixelData.size(); ++i)
	{
		// Write each texture into storage
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
						0,										// Mipmap number
						0, 0, i,								// xoffset, yoffset, zoffset
						uiWidth, uiHeight, static_cast<uint32>(vPixelData.size()),	// width, height, depth
						eFormat,								// format
						GL_UNSIGNED_BYTE,						// type
						vPixelData[i]);							// pointer to pixel data
	}

	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return hGLTextureArray;
}

/*virtual*/ void HyOpenGL::DeleteTextureArray(uint32 uiTextureHandle)
{
	glDeleteTextures(1, &uiTextureHandle);
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

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// 2D setup
	//////////////////////////////////////////////////////////////////////////

	glGenVertexArrays(NUM_VAOTYPES, m_pVAO2d);

	glGenBuffers(1, &m_hVBO2d);
	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);

	// Quad batch //////////////////////////////////////////////////////////////////////////
	m_pShader2d[QUADBATCH].CompileFromString(szHYQUADBATCH_VERTEXSHADER, HyOpenGLShader::VERTEX);
	m_pShader2d[QUADBATCH].CompileFromString(szHYQUADBATCH_FRAGMENTSHADER, HyOpenGLShader::FRAGMENT);

	if(!m_pShader2d[QUADBATCH].Link())
		HyError("Shader program failed to link!\n" << m_pShader2d[QUADBATCH].Log().c_str() << "\n");

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

	//////////////////////////////////////////////////////////////////////////
	// ALL HERE IS PROBABLY NOT NEEDED
	glVertexAttribPointer(size, 2, GL_FLOAT, GL_FALSE, 132, (void *)0);
	glVertexAttribPointer(offset, 2, GL_FLOAT, GL_FALSE, 132, (void *)(2 * sizeof(GLfloat)));
	glVertexAttribPointer(tint, 4, GL_FLOAT, GL_FALSE, 132, (void *)(4 * sizeof(GLfloat)));
	glVertexAttribPointer(textureIndex, 1, GL_UNSIGNED_INT, GL_FALSE, 132, (void *)(sizeof(GLuint)));
	glVertexAttribPointer(uv0, 2, GL_FLOAT, GL_FALSE, 132, (void *)(8 * sizeof(GLfloat)));
	glVertexAttribPointer(uv1, 2, GL_FLOAT, GL_FALSE, 132, (void *)(10 * sizeof(GLfloat)));
	glVertexAttribPointer(uv2, 2, GL_FLOAT, GL_FALSE, 132, (void *)(12 * sizeof(GLfloat)));
	glVertexAttribPointer(uv3, 2, GL_FLOAT, GL_FALSE, 132, (void *)(14 * sizeof(GLfloat)));
	glVertexAttribPointer(mtx + 0, 4, GL_FLOAT, GL_FALSE, 132, (void *)(16 * sizeof(GLfloat)));
	glVertexAttribPointer(mtx + 1, 4, GL_FLOAT, GL_FALSE, 132, (void *)(20 * sizeof(GLfloat)));
	glVertexAttribPointer(mtx + 2, 4, GL_FLOAT, GL_FALSE, 132, (void *)(24 * sizeof(GLfloat)));
	glVertexAttribPointer(mtx + 3, 4, GL_FLOAT, GL_FALSE, 132, (void *)(28 * sizeof(GLfloat)));
	//////////////////////////////////////////////////////////////////////////

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

	if(!m_pShader2d[PRIMITIVE].Link())
		HyError("Shader program failed to link!\n" << m_pShader2d[PRIMITIVE].Log().c_str() << "\n");

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
