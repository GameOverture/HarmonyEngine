/**************************************************************************
 *	HyOpenGL.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Interop/OpenGL/HyOpenGL.h"

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

/*virtual*/ bool HyOpenGL::Initialize()
{
	//////////////////////////////////////////////////////////////////////////
	// Init GLEW
	//////////////////////////////////////////////////////////////////////////
	GLenum err = glewInit();

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


	//////////////////////////////////////////////////////////////////////////
	// Init Built-in Shaders
	//////////////////////////////////////////////////////////////////////////
	m_ShaderQuadBatch.CompileFromFile("HyQuadBatch", HyOpenGLShader::VERTEX);
	m_ShaderQuadBatch.CompileFromFile("HyQuadBatch", HyOpenGLShader::FRAGMENT);

	m_ShaderQuadBatch.BindAttribLocation(0, "position");
	m_ShaderQuadBatch.BindAttribLocation(1, "color");
	m_ShaderQuadBatch.BindAttribLocation(2, "uv");

	if(!m_ShaderQuadBatch.Link())
		HyError("Shader program failed to link!\n" << m_ShaderQuadBatch.Log().c_str() << "\n");

	//////////////////////////////////////////////////////////////////////////

	m_ShaderPrimitive2d.CompileFromFile("Prim2d", HyOpenGLShader::VERTEX);
	m_ShaderPrimitive2d.CompileFromFile("Prim2d", HyOpenGLShader::FRAGMENT);

	if(!m_ShaderPrimitive2d.Link())
		HyError("Shader program failed to link!\n" << m_ShaderPrimitive2d.Log().c_str() << "\n");

	//////////////////////////////////////////////////////////////////////////

	//m_ShaderText2d.CompileFromFile("Txt2d", HyOpenGLShader::VERTEX);
	//m_ShaderText2d.CompileFromFile("Txt2d", HyOpenGLShader::FRAGMENT);

	//if(!m_ShaderText2d.Link())
	//	HyError("Shader program failed to link!\n" << m_ShaderText2d.Log().c_str() << "\n");


	//////////////////////////////////////////////////////////////////////////
	// 2D setup
	//////////////////////////////////////////////////////////////////////////

	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Set up vertex array object
	glGenVertexArrays(1, &m_hVAO2d);
	glBindVertexArray(m_hVAO2d);

	//// Initialize 2d quad buffer
	//glGenBuffers(1, &m_hVBO2d_UnitQuad);
	//glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d_UnitQuad);
	//const float fUnitQuadVertPos[16] = {
	//	0.0f, 0.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 0.0f, 1.0f,
	//	0.0f, 1.0f, 0.0f, 1.0f,
	//	1.0f, 1.0f, 0.0f, 1.0f
	//};
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, fUnitQuadVertPos, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	glBindVertexArray(m_hVAO2d);

	
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
			glUseProgram(0);
			m_ShaderQuadBatch.Use();

			if(renderState.IsEnabled(HyRenderState::USINGLOCALCOORDS))
				m_ShaderQuadBatch.SetUniform("worldToCameraMatrix", m_kmtxIdentity);
			else
				m_ShaderQuadBatch.SetUniform("worldToCameraMatrix", m_mtxView);

			m_ShaderQuadBatch.SetUniform("cameraToClipMatrix", m_mtxProj);

			//glEnableVertexAttribArray(m_ShaderQuadBatch.GetAttribLocation("position"));
			//glEnableVertexAttribArray(m_ShaderQuadBatch.GetAttribLocation("color"));
			//glEnableVertexAttribArray(m_ShaderQuadBatch.GetAttribLocation("uv"));

			glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d_UnitQuad);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO2d);
			//glPrimitiveRestartIndex(HY_RESTART_INDEX);
			//glEnable(GL_PRIMITIVE_RESTART);


			//m_ShaderQuadBatch.SetUniform("localToWorld", pInst->GetTransformMtx());


			//layout(location = 0) in vec2 size					8
			//layout(location = 1) in vec2 offset;				8
			//layout(location = 2) in vec4 tint;				16
			//layout(location = 3) in vec2 UVcoord0;			8
			//layout(location = 4) in vec2 UVcoord1;			8
			//layout(location = 5) in vec2 UVcoord2;			8
			//layout(location = 6) in vec2 UVcoord3;			8
			//layout(location = 7) in mat4 mtxLocalToWorld;		64

			uint32 uiDataOffset = renderState.GetDataOffset();

			GLuint size = m_ShaderQuadBatch.GetAttribLocation("size");
			GLuint offset = m_ShaderQuadBatch.GetAttribLocation("offset");
			GLuint tint = m_ShaderQuadBatch.GetAttribLocation("tint");
			GLuint uv0 = m_ShaderQuadBatch.GetAttribLocation("UVcoord0");
			GLuint uv1 = m_ShaderQuadBatch.GetAttribLocation("UVcoord1");
			GLuint uv2 = m_ShaderQuadBatch.GetAttribLocation("UVcoord2");
			GLuint uv3 = m_ShaderQuadBatch.GetAttribLocation("UVcoord3");
			GLuint mtx = m_ShaderQuadBatch.GetAttribLocation("mtxLocalToWorld");

			glVertexAttribPointer(size, 2, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)uiDataOffset);
			glVertexAttribPointer(offset, 2, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)(uiDataOffset + (2*sizeof(GLfloat))));
			glVertexAttribPointer(tint, 4, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)(uiDataOffset + (4*sizeof(GLfloat))));
			glVertexAttribPointer(uv0, 2, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)(uiDataOffset + (8*sizeof(GLfloat))));
			glVertexAttribPointer(uv1, 2, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)(uiDataOffset + (10*sizeof(GLfloat))));
			glVertexAttribPointer(uv2, 2, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)(uiDataOffset + (12*sizeof(GLfloat))));
			glVertexAttribPointer(uv3, 2, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)(uiDataOffset + (14*sizeof(GLfloat))));
			glVertexAttribPointer(mtx, 16, GL_FLOAT, GL_FALSE, 128*sizeof(GLfloat), (void *)(uiDataOffset + (16*sizeof(GLfloat))));
			
			glVertexAttribDivisor(size, 1);
			glVertexAttribDivisor(offset, 1);
			glVertexAttribDivisor(tint, 1);
			glVertexAttribDivisor(uv0, 1);
			glVertexAttribDivisor(uv1, 1);
			glVertexAttribDivisor(uv2, 1);
			glVertexAttribDivisor(uv3, 1);
			glVertexAttribDivisor(mtx, 1);

			//uint32 uiByteOffset = pInst->GetVertexDataOffset();
			//glVertexAttribPointer(m_ShaderQuadBatch.GetAttribLocation("unitQuadPos"), 4, GL_FLOAT, GL_FALSE, 0, (void *)uiByteOffset);
			//glVertexAttribPointer(m_ShaderQuadBatch.GetAttribLocation("color"), 4, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), (void *)(uiByteOffset+(4*sizeof(GLfloat))));
			//glVertexAttribPointer(m_ShaderQuadBatch.GetAttribLocation("uv"), 2, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), (void *)(uiByteOffset+(8*sizeof(GLfloat))));

			GLuint uiNumInsts = renderState.GetAux();
			glBindTexture(GL_TEXTURE_2D, renderState.GetTextureHandle(0));
			//glDrawElements(pThis->m_eDrawMode, pInst->GetNumQuads() * 5, GL_UNSIGNED_SHORT, 0);

			glDrawArraysInstanced(m_eDrawMode, 0, 4, uiNumInsts);

			//m_fpDraw2d = DrawBatchedQuads2d;
		}
		else if(renderState.IsEnabled(HyRenderState::SHADER_PRIMITIVEDRAW))
		{
			glUseProgram(0);
			m_ShaderPrimitive2d.Use();

			if(renderState.IsEnabled(HyRenderState::USINGLOCALCOORDS))
				m_ShaderPrimitive2d.SetUniform("worldToCameraMatrix", m_kmtxIdentity);
			else
				m_ShaderPrimitive2d.SetUniform("worldToCameraMatrix", m_mtxView);

			m_ShaderPrimitive2d.SetUniform("cameraToClipMatrix", m_mtxProj);

			glEnableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			//glDisable(GL_PRIMITIVE_RESTART);

			



			//m_fpDraw2d = DrawPrim2dInst;
		}
		//else if(uiNewRenderState & IObjInst2d::RS_SHADER_TEXT)
		//{
		//	glUseProgram(0);
		//	m_ShaderText2d.Use();

		//	if(uiNewRenderState & IObjInst2d::USINGLOCALCOORDS)
		//		m_ShaderText2d.SetUniform("worldToCameraMatrix", m_kmtxIdentity);
		//	else
		//		m_ShaderText2d.SetUniform("worldToCameraMatrix", m_mtxView);

		//	m_ShaderText2d.SetUniform("cameraToClipMatrix", m_mtxProj);

		//	glEnableVertexAttribArray(0);
		//	glEnableVertexAttribArray(1);
		//	glDisableVertexAttribArray(2);

		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO2d);
		//	//glPrimitiveRestartIndex(HY_RESTART_INDEX);
		//	//glEnable(GL_PRIMITIVE_RESTART);

		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//	glEnable(GL_TEXTURE_2D);

		//	m_fpDraw2d = DrawTxt2dInst;
		//}
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

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

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
	//GLint iLocation = glGetUniformLocation(m_ShaderQuadBatch.GetHandle(), "Tex");
	//glUniform1i(iLocation, 0);

	return hGLTexture;
}

/*virtual*/ void HyOpenGL::DeleteTexture(uint32 uiTextureId)
{
	glDeleteTextures(1, &uiTextureId);
}
