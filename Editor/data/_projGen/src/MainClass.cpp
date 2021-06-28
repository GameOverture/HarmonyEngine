#include "pch.h"
#include "%HY_CODENAME%.h"

%HY_CODENAME%::%HY_CODENAME%(HarmonyInit &initStruct) :
	HyEngine(initStruct),
	m_pCamera(HyEngine::Window().CreateCamera2d())
{
	HyEngine::Input().MapBtn(INPUT_ExitGame, HYKEY_Escape);
}

%HY_CODENAME%::~%HY_CODENAME%()
{
}

/*virtual*/ bool %HY_CODENAME%::OnUpdate() /*override*/
{
	return !HyEngine::Input().IsActionReleased(INPUT_ExitGame);
}
