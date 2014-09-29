/**************************************************************************
 *	HyQt.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Interop/Qt/HyQt.h"

#ifdef HY_PLATFORM_GUI

#define HY_INDEX_BUFFER_SIZE (128 * 1024)
#define HY_RESTART_INDEX 65535

HyQt::HyQt() :	HyOpenGL(false),
					Fl_Gl_Window(0, 0, 0, 0, NULL)
{
	HyError("HyQt's default constructor should not be used.");
}

HyQt::HyQt(int x,int y,int w,int h,const char *l) : IGfxApi(false),
														Fl_Gl_Window(x, y, w, h, l)
{
}

HyQt::~HyQt(void)
{
}

/*virtual*/ bool HyQt::Initialize()
{
	HyOpenGL::Initialize() true;
}

/*virtual*/ bool HyQt::PollApi()
{
	this->redraw();
	return true;
}

/*virtual*/ bool HyQt::CheckDevice()
{
	return true;
}

/*virtual*/ void HyQt::DrawBuffers()
{
	// Override DrawBuffers() to do nothing. We only want to do IGfxApi::DrawBuffers()
	// when Fl_Gl_Window does its draw() callback. 
}

/*virtual*/void HyQt::draw()
{
	if(m_bInitialized == false)
	{
		//////////////////////////////////////////////////////////////////////////
		// Init GLEW
		//////////////////////////////////////////////////////////////////////////
		GLenum err = glewInit();

		if (glewIsSupported("GL_VERSION_3_3"))
			printf("Ready for OpenGL 3.3\n");
		else {
			printf("OpenGL 3.3 not supported\n");
			exit(1);
		}
		printf ("Vendor: %s\n", glGetString (GL_VENDOR));
		printf ("Renderer: %s\n", glGetString (GL_RENDERER));
		printf ("Version: %s\n", glGetString (GL_VERSION));
		printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

		glEnable(GL_DEPTH_TEST);


		//////////////////////////////////////////////////////////////////////////
		// Init Shaders
		//////////////////////////////////////////////////////////////////////////
		m_ShaderEnt2d.CompileFromFile("Ent2d", HyGlfwShader::VERTEX);
		m_ShaderEnt2d.CompileFromFile("Ent2d", HyGlfwShader::FRAGMENT);

		if(!m_ShaderEnt2d.Link())
			HyError("Shader program failed to link!\n" << m_ShaderEnt2d.Log().c_str() << "\n");

		//////////////////////////////////////////////////////////////////////////

		m_ShaderPrimitive2d.CompileFromFile("Prim2d", HyGlfwShader::VERTEX);
		m_ShaderPrimitive2d.CompileFromFile("Prim2d", HyGlfwShader::FRAGMENT);

		if(!m_ShaderPrimitive2d.Link())
			HyError("Shader program failed to link!\n" << m_ShaderPrimitive2d.Log().c_str() << "\n");

		//////////////////////////////////////////////////////////////////////////

		m_ShaderText2d.CompileFromFile("Txt2d", HyGlfwShader::VERTEX);
		m_ShaderText2d.CompileFromFile("Txt2d", HyGlfwShader::FRAGMENT);

		if(!m_ShaderText2d.Link())
			HyError("Shader program failed to link!\n" << m_ShaderText2d.Log().c_str() << "\n");


		//////////////////////////////////////////////////////////////////////////
		// 2D setup
		//////////////////////////////////////////////////////////////////////////

		//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		/* Set up vertex array object */
		glGenVertexArrays(1, &m_hVAO2d);
		glBindVertexArray(m_hVAO2d);

		glGenBuffers(1, &m_hVBO2d);
		glBindBuffer(GL_ARRAY_BUFFER, m_hVBO2d);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Initialize 2d vertex buffer
		/* Set up vertex data */


		/* Set up index data */
		m_pGenericIndexBuffer2d = new uint16[HY_INDEX_BUFFER_SIZE/sizeof(uint16)];
		uint16 *pCurWriteShort = m_pGenericIndexBuffer2d;
		uint16 uiIndexCount = 0;
		uint32 uiResetIndexCount = 0;
		for(int i = 0; i < (HY_INDEX_BUFFER_SIZE/2); ++i)
		{
			uiResetIndexCount++;
			if(uiResetIndexCount == 5)
			{
				*pCurWriteShort = HY_RESTART_INDEX;
				uiResetIndexCount = 0;
			}
			else
			{
				*pCurWriteShort = uiIndexCount;
				uiIndexCount++;
			}

			pCurWriteShort++;
		}

		glGenBuffers(1, &m_hIBO2d);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO2d);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, HY_INDEX_BUFFER_SIZE, m_pGenericIndexBuffer2d, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glPrimitiveRestartIndex(HY_RESTART_INDEX);
		glEnable(GL_PRIMITIVE_RESTART);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(0);

		m_bInitialized = true;
	}

	IGfxApi::DrawBuffers();
}

#endif /* HY_PLATFORM_GUI */
