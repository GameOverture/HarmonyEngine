/**************************************************************************
 *	HyOpenGL.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/GfxApi/OpenGL/HyOpenGL.h"

// THIS WORKS BELOW!
const float vertexDataEmulate[] = {
	0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 0.2938f,

	0.0f, 100.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 0.0f,

	100.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.2938f,

	100.0f, 100.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f
};

float vertices[] = {
	-256.0f, -256.0f,0, 1,
	1,1,1,1,
	0,1,

	-256.0f,  256.0f,0, 1,
	1,1,1,1,
	0,0, 

	256.0f,  -256.0f,0, 1,
	1,1,1,1,
	1,1,

	256.0f,   256.0f,0, 1,
	1,1,1,1,
	1,0
};

const float primitiveVertexDataEmulate[] = {
	-50.0f, -50.0f, 0.0f, 1.0f,
	-50.0f, 50.0f, 0.0f, 1.0f,
	50.0f, 50.0f, 0.0f, 1.0f,
	50.0f, -50.0f, 0.0f, 1.0f
};


HyOpenGL::HyOpenGL() :	IGfxApi(),
						m_mtxView(1.0f),
						m_kmtxIdentity(1.0f)
{
}

HyOpenGL::~HyOpenGL(void)
{
}

/*virtual*/ bool HyOpenGL::CreateWindows()
{
	return true;
}

/*virtual*/ bool HyOpenGL::Initialize()
{
	//////////////////////////////////////////////////////////////////////////
	// Init GLEW
	//////////////////////////////////////////////////////////////////////////
	GLenum err = glewInit();

	if(err != GLEW_OK)
	{
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
			std::cout << "OpenGL Error: " << error << std::endl;

		exit(EXIT_FAILURE);
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
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	//////////////////////////////////////////////////////////////////////////
	// 2D setup
	//////////////////////////////////////////////////////////////////////////

	glGenVertexArrays(NUM_VAOTYPES, m_pVAO2d);

	glGenBuffers(1, &m_hVBO2d);
	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);

	// Quad batch //////////////////////////////////////////////////////////////////////////
	m_pShader2d[QUADBATCH].CompileFromFile("HyQuadBatch", HyOpenGLShader::VERTEX);
	m_pShader2d[QUADBATCH].CompileFromFile("HyQuadBatch", HyOpenGLShader::FRAGMENT);

	if(!m_pShader2d[QUADBATCH].Link())
		HyError("Shader program failed to link!\n" << m_pShader2d[QUADBATCH].Log().c_str() << "\n");

	glBindVertexArray(m_pVAO2d[QUADBATCH]);

	GLuint size = m_pShader2d[QUADBATCH].GetAttribLocation("size");
	GLuint offset = m_pShader2d[QUADBATCH].GetAttribLocation("offset");
	GLuint tint = m_pShader2d[QUADBATCH].GetAttribLocation("tint");
	GLuint uv0 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord0");
	GLuint uv1 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord1");
	GLuint uv2 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord2");
	GLuint uv3 = m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord3");
	GLuint mtx = m_pShader2d[QUADBATCH].GetAttribLocation("mtxLocalToWorld");

	glEnableVertexAttribArray(size);
	glEnableVertexAttribArray(offset);
	glEnableVertexAttribArray(tint);
	glEnableVertexAttribArray(uv0);
	glEnableVertexAttribArray(uv1);
	glEnableVertexAttribArray(uv2);
	glEnableVertexAttribArray(uv3);
	glEnableVertexAttribArray(mtx+0);
	glEnableVertexAttribArray(mtx+1);
	glEnableVertexAttribArray(mtx+2);
	glEnableVertexAttribArray(mtx+3);

	glVertexAttribPointer(size, 2, GL_FLOAT, GL_FALSE, 128, (void *)0);
	glVertexAttribPointer(offset, 2, GL_FLOAT, GL_FALSE, 128, (void *)(2*sizeof(GLfloat)));
	glVertexAttribPointer(tint, 4, GL_FLOAT, GL_FALSE, 128, (void *)(4*sizeof(GLfloat)));
	glVertexAttribPointer(uv0, 2, GL_FLOAT, GL_FALSE, 128, (void *)(8*sizeof(GLfloat)));
	glVertexAttribPointer(uv1, 2, GL_FLOAT, GL_FALSE, 128, (void *)(10*sizeof(GLfloat)));
	glVertexAttribPointer(uv2, 2, GL_FLOAT, GL_FALSE, 128, (void *)(12*sizeof(GLfloat)));
	glVertexAttribPointer(uv3, 2, GL_FLOAT, GL_FALSE, 128, (void *)(14*sizeof(GLfloat)));
	glVertexAttribPointer(mtx+0, 4, GL_FLOAT, GL_FALSE, 128, (void *)(16*sizeof(GLfloat)));
	glVertexAttribPointer(mtx+1, 4, GL_FLOAT, GL_FALSE, 128, (void *)(20*sizeof(GLfloat)));
	glVertexAttribPointer(mtx+2, 4, GL_FLOAT, GL_FALSE, 128, (void *)(24*sizeof(GLfloat)));
	glVertexAttribPointer(mtx+3, 4, GL_FLOAT, GL_FALSE, 128, (void *)(28*sizeof(GLfloat)));

	glVertexAttribDivisor(size, 1);
	glVertexAttribDivisor(offset, 1);
	glVertexAttribDivisor(tint, 1);
	glVertexAttribDivisor(uv0, 1);
	glVertexAttribDivisor(uv1, 1);
	glVertexAttribDivisor(uv2, 1);
	glVertexAttribDivisor(uv3, 1);
	glVertexAttribDivisor(mtx+0, 1);
	glVertexAttribDivisor(mtx+1, 1);
	glVertexAttribDivisor(mtx+2, 1);
	glVertexAttribDivisor(mtx+3, 1);

	// Primitive //////////////////////////////////////////////////////////////////////////
	m_pShader2d[PRIMITIVE].CompileFromFile("Prim2d", HyOpenGLShader::VERTEX);
	m_pShader2d[PRIMITIVE].CompileFromFile("Prim2d", HyOpenGLShader::FRAGMENT);

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

/*virtual*/ bool HyOpenGL::PollApi()
{
	return true;
}

/*virtual*/ bool HyOpenGL::CheckDevice()
{
	return true;
}

/*virtual*/ void HyOpenGL::StartRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_iNumCams2d = GetNumCameras2d();
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
	if(GetNumRenderStates2d() == 0 || m_iNumCams2d == 0)
		return false;

	// Without disabling glDepthMask, sprites fragments that overlap will be discarded
	glDepthMask(false);

	//glPrimitiveRestartIndex(HY_RESTART_INDEX);
	//glEnable(GL_PRIMITIVE_RESTART);


	glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);
	glBufferData(GL_ARRAY_BUFFER, m_DrawpBufferHeader->uiVertexBufferSize2d, GetVertexData2d(), GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexDataEmulate), vertexDataEmulate, GL_DYNAMIC_DRAW);

	//char *pTest = GetVertexData2d();
	float *pTest = reinterpret_cast<float *>(GetVertexData2d());
	
	//-----------------------------------------------------------------------------------------------------------------


	m_mtxView = *GetCameraView2d(GetNumCameras2d() - m_iNumCams2d);

	//m_mtxView = mat4(1.0f);

	m_iNumCams2d--;
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
				m_pShader2d[QUADBATCH].SetUniform("mtxWorldToCameraMatrix", m_kmtxIdentity);
			else
				m_pShader2d[QUADBATCH].SetUniform("mtxWorldToCameraMatrix", m_mtxView);

			m_pShader2d[QUADBATCH].SetUniform("mtxCameraToClipMatrix", m_mtxProj);

			size_t uiDataOffset = renderState.GetDataOffset();

			GLuint mtx = m_pShader2d[QUADBATCH].GetAttribLocation("mtxLocalToWorld");
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("size"), 2, GL_FLOAT, GL_FALSE, 128, (void *)uiDataOffset);
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("offset"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (2*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("tint"), 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (4*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord0"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (8*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord1"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (10*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord2"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (12*sizeof(GLfloat))));
			glVertexAttribPointer(m_pShader2d[QUADBATCH].GetAttribLocation("UVcoord3"), 2, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (14*sizeof(GLfloat))));
			glVertexAttribPointer(mtx+0, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (16*sizeof(GLfloat))));
			glVertexAttribPointer(mtx+1, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (20*sizeof(GLfloat))));
			glVertexAttribPointer(mtx+2, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (24*sizeof(GLfloat))));
			glVertexAttribPointer(mtx+3, 4, GL_FLOAT, GL_FALSE, 128, (void *)(uiDataOffset + (28*sizeof(GLfloat))));

			glBindTexture(GL_TEXTURE_2D, renderState.GetTextureHandle(0));

			int iNumInsts = renderState.GetNumInstances();
			glDrawArraysInstanced(m_eDrawMode, 0, 4, iNumInsts);
		}
		else if(renderState.IsEnabled(HyRenderState::SHADER_PRIMITIVEDRAW))
		{
			glBindVertexArray(m_pVAO2d[PRIMITIVE]);
			m_pShader2d[PRIMITIVE].Use();

			if(renderState.IsEnabled(HyRenderState::USINGLOCALCOORDS))
				m_pShader2d[PRIMITIVE].SetUniform("worldToCameraMatrix", m_kmtxIdentity);
			else
				m_pShader2d[PRIMITIVE].SetUniform("worldToCameraMatrix", m_mtxView);

			m_pShader2d[PRIMITIVE].SetUniform("cameraToClipMatrix", m_mtxProj);

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

//void HyOpenGL::DrawBatchedQuads2d(char *pData)
//{
//	m_ShaderQuadBatch.SetUniform("localToWorld", pInst->GetTransformMtx());
//
//	uint32 uiByteOffset = pInst->GetVertexDataOffset();
//	glVertexAttribPointer(pThis->m_ShaderQuadBatch.GetAttribLocation("position"), 4, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), (void *)uiByteOffset);
//	glVertexAttribPointer(pThis->m_ShaderQuadBatch.GetAttribLocation("color"), 4, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), (void *)(uiByteOffset+(4*sizeof(GLfloat))));
//	glVertexAttribPointer(pThis->m_ShaderQuadBatch.GetAttribLocation("uv"), 2, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), (void *)(uiByteOffset+(8*sizeof(GLfloat))));
//
//	GLuint uiTexId = pInst->GetTextureId();
//	glBindTexture(GL_TEXTURE_2D, uiTexId);
//	glDrawElements(pThis->m_eDrawMode, pInst->GetNumQuads() * 5, GL_UNSIGNED_SHORT, 0);
//}
//
///*static*/ void HyOpenGL::DrawPrim2dInst(IDraw2d *pBaseInst, void *pApi)
//{
//	HyDrawPrimitive2d *pInst = reinterpret_cast<HyDrawPrimitive2d *>(pBaseInst);
//	HyOpenGL *pThis = reinterpret_cast<HyOpenGL *>(pApi);
//
//	pThis->m_ShaderPrimitive2d.SetUniform("primitiveColor", pInst->GetColorAlpha());
//	pThis->m_ShaderPrimitive2d.SetUniform("transformMtx", pInst->GetTransformMtx());
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)pInst->GetVertexDataOffset());
//	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void *)(pInst->GetVertexDataOffset()+(4*sizeof(GLfloat))));
//	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), (void *)(pInst->GetVertexDataOffset()+(8*sizeof(GLfloat))));
//
//	glDrawArrays(pThis->m_eDrawMode, 0, pInst->GetNumVerts());
//}

///*static*/ void HyOpenGL::DrawTxt2dInst(IDraw2d *pBaseInst, void *pApi)
//{
//	HyDrawText2d *pInst = reinterpret_cast<HyDrawText2d *>(pBaseInst);
//	HyOpenGL *pThis = reinterpret_cast<HyOpenGL *>(pApi);
//
//	pThis->m_ShaderText2d.SetUniform("textColor", pInst->GetColorAlpha());
//	pThis->m_ShaderText2d.SetUniform("transformMtx", pInst->GetTransformMtx());
//
//	uint32 uiByteOffset = pInst->GetVertexDataOffset();
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void *)uiByteOffset);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void *)(uiByteOffset+(4*sizeof(GLfloat))));
//
//	GLuint uiTexId = pInst->GetTextureId();
//	glBindTexture(GL_TEXTURE_2D, uiTexId);
//	glDrawElements(pThis->m_eDrawMode, pInst->GetNumChars() * 5, GL_UNSIGNED_SHORT, 0);
//}

/*virtual*/ void HyOpenGL::End_2d()
{
	glDepthMask(true);

	glBindVertexArray(0);
	glUseProgram(0);
}

/*virtual*/ void HyOpenGL::FinishRender()
{

}

/*virtual*/ bool HyOpenGL::Shutdown()
{
	return true;
}

// Returns the texture ID used for API specific drawing.
/*virtual*/ uint32 HyOpenGL::AddTexture(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, void *pPixelData)
{
	GLuint hGLTexture;
	glGenTextures(1, &hGLTexture);

	glBindTexture( GL_TEXTURE_2D, hGLTexture);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	if(uiNumColorChannels == 4 )
	{
		//	#ifdef GL_UNSIGNED_INT_8_8_8_8_REV
		//		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, uiWidth, uiHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pPixelData);
		//	#else
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, uiWidth, uiHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pPixelData);
		//	#endif
	}
	else if(uiNumColorChannels == 3 )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, uiWidth, uiHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pPixelData);
	}
	else
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, uiWidth, uiHeight, 0, GL_RED, GL_UNSIGNED_BYTE, pPixelData);
	}

	// This is probable unnecessary
	//GLint iLocation = glGetUniformLocation(m_pShader2d[QUADBATCH].GetHandle(), "Tex");
	//glUniform1i(iLocation, 0);

	return hGLTexture;
}

/*virtual*/ void HyOpenGL::DeleteTexture(HyTexture &texture)
{
	uint32 uiTexId = texture.GetId();
	glDeleteTextures(1, &uiTexId);
}
