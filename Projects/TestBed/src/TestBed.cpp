/**************************************************************************
 *	TestBed.cpp
 *	
 *	Copyright (c) 2013 Jason Knobler
 *	Harmony Engine
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "TestBed.h"

TestBed::TestBed(HarmonyInit &initStruct) : IHyApplication(initStruct),
											m_spineBoy(NULL, "SpineBoy"),
											m_primBox(),
											m_txtName(NULL, NULL),
											m_testSprite(1)
{
}

TestBed::~TestBed(void)
{
}

/*virtual*/ bool TestBed::Initialize()
{
	Input().MapBtn_KB(CAM_LEFT, KeyLeft);
	Input().MapBtn_KB(CAM_LEFT, KeyA);

	Input().MapBtn_KB(CAM_RIGHT, KeyRight);
	Input().MapBtn_KB(CAM_RIGHT, KeyD);

	Input().MapBtn_KB(CAM_UP, KeyUp);
	Input().MapBtn_KB(CAM_UP, KeyW);

	Input().MapBtn_KB(CAM_DOWN, KeyDown);
	Input().MapBtn_KB(CAM_DOWN, KeyS);

	Input().MapBtn_KB(SEND_LOG, KeyF);

	Input().MapBtn_KB(ACTION_1, Key1);
	Input().MapBtn_KB(ACTION_2, Key2);
	Input().MapBtn_KB(ACTION_3, Key3);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pCam = Window().CreateCamera2d();
	m_pCam->SetViewport(0.0f, 0.0f, 0.5f, 1.0f);
	m_pCam2 = Window().CreateCamera2d();
	m_pCam2->SetViewport(0.5f, 0.0f, 0.5f, 1.0f);

	m_pCam_Viewport2 = Window(1).CreateCamera2d();
	m_pCam_Viewport2->SetZoom(1.0f);
	m_pCam_Viewport2->SetZoom(0.2f);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	m_primBox.Load();
	m_primBox.color.Set(0.0f, 0.0f, 1.0f, 1.0f);
	m_primBox.SetAsQuad(180.0f, 160.0f, false);
	m_primBox.SetDisplayOrder(0);
	m_primBox.pos.Set(-45.0f, 0.0f);

	m_testSprite.Load();
	m_testSprite.SetTextureSource(8, 0, 0, 180, 162);
	m_testSprite.SetDisplayOrder(1);
	m_testSprite.pos.Set(0.0f, 0.0f);


	glm::vec2 vLinePts[2];

	vLinePts[0].x = -2048.0f;
	vLinePts[0].y = 0.0f;
	vLinePts[1].x = 2048.0f;
	vLinePts[1].y = 0.0f;
	m_HorzLine.SetAsEdgeChain(vLinePts, 2, false);

	vLinePts[0].x = 0.0f;
	vLinePts[0].y = -2048.0f;
	vLinePts[1].x = 0.0f;
	vLinePts[1].y = 2048.0f;
	m_VertLine.SetAsEdgeChain(vLinePts, 2, false);

	m_VertLine.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
	m_HorzLine.color.Set(1.0f, 0.0f, 0.0f, 1.0f);

	m_VertLine.Load();
	m_HorzLine.Load();

	//////////////////////////////////////////////////////////////////////////
	IHyShader *pShader_Checkerboard = IHyRenderer::NewCustomShader();
	char *pShaderSrc = HyReadTextFile("testFrag.glsl", NULL);
	pShader_Checkerboard->SetSourceCode(pShaderSrc, HYSHADER_Fragment);
	delete[] pShaderSrc;

//<<<<<<< HEAD
//	pShader_Checkerboard->
//
//	pShaderUniformsRef->Set("primitiveColor", color.Get());
//	pShaderUniformsRef->Set("transformMtx", mtx);
//
//	m_primBox.SetCustomShader(pShader_Checkerboard);
//=======
//	//m_primBox.SetCustomShader(pShader_Checkerboard);
//>>>>>>> 69a4a7012978a9f8d4e55702add785735390ca19
	

	return true;
}

/*virtual*/ bool TestBed::Update()
{
	if(Input().IsBtnDownBuffered(ACTION_1))
	{
		m_testSprite.SetCoordinateType(HYCOORDTYPE_Screen, NULL);//m_pCam_Viewport2);
		//m_pCam_Viewport2->pos.Tween(40.0f, 40.0f, 5.0f, HyTween::Linear);
	}
	if(Input().IsBtnDownBuffered(ACTION_2))
	{
		m_testSprite.SetCoordinateType(HYCOORDTYPE_Camera, NULL);//m_pCam_Viewport2);
		//m_pCam_Viewport2->pos.Tween(40.0f, 40.0f, 5.0f, HyTween::Linear);
	}
	if(Input().IsBtnDownBuffered(ACTION_3))
	{
		m_testSprite.rot.Tween(180.0f, 0.0f, 0.0f, 10.0f, HyTween::BounceInOut);
	}

	const float fMOVEMENTSPEED = 2.0f;

	if(Input().IsBtnDown(CAM_LEFT))
	{
		m_testSprite.pos.Offset(-fMOVEMENTSPEED, 0.0f);
		m_pCam->pos.Offset(-fMOVEMENTSPEED, 0.0f);
	}

	if(Input().IsBtnDown(CAM_RIGHT))
	{
		m_testSprite.pos.Offset(fMOVEMENTSPEED, 0.0f);
		m_pCam->pos.Offset(fMOVEMENTSPEED, 0.0f);
	}

	if(Input().IsBtnDown(CAM_UP))
	{
		m_testSprite.pos.Offset(0.0f, fMOVEMENTSPEED);
		m_pCam->pos.Offset(0.0f, fMOVEMENTSPEED);
	}

	if(Input().IsBtnDown(CAM_DOWN))
	{
		m_testSprite.pos.Offset(0.0f, -fMOVEMENTSPEED);
		m_pCam->pos.Offset(0.0f, -fMOVEMENTSPEED);
	}

	if(Input().IsBtnDownBuffered(SEND_LOG))
	{
		// Exit application
		return false;

		//HyEngine::ReloadDataDir(std::string("D:\\soft\\GitHub\\HarmonyEngine\\Projects\\NewGame\\data"));
		//HyLog("Testing message! Mo'fucka");
	}

	return true;
}

/*virtual*/ bool TestBed::Shutdown()
{
	return true;
}
