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
	//m_pInputArray->GetGamePadIds(m_vGamePadIds);

	//if(m_vGamePadIds.empty() == false)
	//{
	//	m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(m_vGamePadIds[0], GP360_ButtonA));
	//	m_pInputArray->BindBtnMap(GP360_ButtonB, HyInputKey(m_vGamePadIds[0], GP360_ButtonB));

	//	m_pInputArray->BindAxisMap(GP_RStickX, HyInputKey(m_vGamePadIds[0], GP_RStickX));
	//	m_pInputArray->BindAxisMap(GP_RStickY, HyInputKey(m_vGamePadIds[0], GP_RStickY));
	//	m_pInputArray->BindAxisMap(GP_Triggers, HyInputKey(m_vGamePadIds[0], GP_Triggers));

	//	m_pInputArray->BindAxisMap(GP_LStickX, HyInputKey(m_vGamePadIds[0], GP_LStickX));
	//}
	//m_pInputArray->BindAxisMapPos(GP_LStickX, HyInputKey('D'));
	//m_pInputArray->BindAxisMapNeg(GP_LStickX, HyInputKey('A'));

	//m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(' '));

	m_pCam = Window().CreateCamera2d();
	m_pCam->SetViewport(0.0f, 0.0f, 0.5f, 1.0f);
	m_pCam2 = Window().CreateCamera2d();
	m_pCam2->SetViewport(0.5f, 0.0f, 0.5f, 1.0f);

	m_pCam_Viewport2 = Window(1).CreateCamera2d();
	m_pCam_Viewport2->SetZoom(1.0f);
	
	m_primBox.Load();
	m_primBox.color.Set(0.0f, 0.0f, 1.0f, 1.0f);
	m_primBox.SetAsQuad(15.0f, 15.0f, false);
	m_primBox.SetDisplayOrder(100);


	//m_spineBoy.Load();
	//m_spineBoy.SetDisplayOrder(1.0f);

	//m_txtName.Load();
	//m_txtName.SetString(L"Testing");11
	//m_txtName.Pos().Offset(0.0f, 0.0f);

	//m_spineBoy.AddChild(m_txtName);
	//m_spineBoy.AddChild(m_primBox);

	//m_spineBoy.Pos().Animate(2.0f, 0.0f, 5.0f, HyEase::linear);
	//m_primBox.Pos().Animate(-1.0f, 0.0f, 5.0f, HyEase::linear);

	m_testSprite.Load();
	m_testSprite.SetTextureSource(8, 0, 0, 180, 162);
	//m_testSprite.pos.Set(100.0f, 100.0f);


	//m_testSprite.SetUsingLocalCoordinates(true);
	m_testSprite.SetDisplayOrder(150);

	//HyGfxWindow::tResolution tRes;
	//m_Window.GetResolution(tRes);
	//m_Camera.SetOrthographic(static_cast<float>(tRes.iWidth), static_cast<float>(tRes.iHeight));

	//m_pCam->Pos().Set(1.0f, 1.0f);
	//m_pCam->Pos().Set(.5f, 1.0f);
	//m_pCam->SetZoom(0.8f);

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


	return true;
}

/*virtual*/ bool TestBed::Update()
{
	//if(m_pInputArray->SetReadKeyCallback GpBtnDown(GP360_ButtonB))
	//{
	//	if(m_pInputMapArray->GpBtnDown(GP360_ButtonB))
	//		m_EntSpineBoy.ProcTranslate(Procedrual_QuadraticIn, vec2(-100.0f, 100.0f), 1.0f);
	//}

	//m_Player.Move(m_pInputArray->GpAxis(GP_LStickX));

	//if(m_pInputArray->GpBtnDown(GP360_ButtonA) && m_Player.IsFeetOnGround())
	//	m_Player.Jump(2.75f);

	//m_pCam->Pos().Set(m_Player.GetPos());
	//m_pCam->Pos().Offset(m_pInputArray->GpAxis(GP_RStickX) * 2.0f, -m_pInputArray->GpAxis(GP_RStickY) * 2.0f);Z
	//m_pCam->SetZoom(m_pCam->GetZoom() + m_pInputArray->GpAxis(GP_Triggers) * 0.05f);


	//m_TxtInst.SetString(HyStr("CamX: %f\tCamY:%f"), m_pCam->Pos().X(), m_pCam->Pos().Y());

	m_testSprite.pos.Set(25, 25);


	if(Input().IsBtnDownBuffered(ACTION_1))
	{
		m_pCam_Viewport2->pos.Animate(40.0f, 40.0f, 5.0f, HyEase::linear);
	}

	if(Input().IsBtnDown(CAM_LEFT))
		m_pCam->pos.Offset(-0.5f, 0.0f);

	if(Input().IsBtnDown(CAM_RIGHT))
		m_pCam->pos.Offset(0.5f, 0.0f);

	if(Input().IsBtnDown(CAM_UP))
		m_pCam->pos.Offset(0.0f, 0.5f);

	if(Input().IsBtnDown(CAM_DOWN))
		m_pCam->pos.Offset(0.0f, -0.5f);

	if(Input().IsBtnDownBuffered(SEND_LOG))
	{
		HyEngine::ReloadDataDir(std::string("D:\\soft\\GitHub\\HarmonyEngine\\Projects\\NewGame\\data"));
		//HyLog("Testing message! Mo'fucka");
	}


	//if(m_pInputArray->


	return true;
}

/*virtual*/ bool TestBed::Shutdown()
{
	return true;
}
