#include "pch.h"
#include "%HY_FILENAME%.h"

%HY_CLASS%::%HY_CLASS%(HyInit &initStruct) :
	HyEngine(initStruct),
	m_pCamera(HyEngine::Window().CreateCamera2d())
{
	HyEngine::Input().MapBtn(INPUT_ExitGame, HYKEY_Escape);
	HyEngine::Input().MapBtn(INPUT_CameraUp, HYKEY_W);
	HyEngine::Input().MapBtn(INPUT_CameraLeft, HYKEY_A);
	HyEngine::Input().MapBtn(INPUT_CameraDown, HYKEY_S);
	HyEngine::Input().MapBtn(INPUT_CameraRight, HYKEY_D);
}

%HY_CLASS%::~%HY_CLASS%()
{
}

/*virtual*/ bool %HY_CLASS%::OnUpdate() /*override*/
{
	if(HyEngine::Input().IsActionDown(INPUT_CameraUp))
		m_pCamera->PanUp();
	if(HyEngine::Input().IsActionDown(INPUT_CameraLeft))
		m_pCamera->PanLeft();
	if(HyEngine::Input().IsActionDown(INPUT_CameraDown))
		m_pCamera->PanDown();
	if(HyEngine::Input().IsActionDown(INPUT_CameraRight))
		m_pCamera->PanRight();
	
	return !HyEngine::Input().IsActionReleased(INPUT_ExitGame);
}
