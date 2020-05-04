#include "pch.h"
#include "%HY_CLASS%.h"

%HY_CLASS%::%HY_CLASS%(HarmonyInit &initStruct) :
	HyEngine(initStruct),
	m_pCamera(Hy_Window().CreateCamera2d())
{
	Hy_Input().MapBtn(INPUT_ExitGame, HYKEY_Escape);
}

%HY_CLASS%::~%HY_CLASS%()
{
}

/*virtual*/ bool %HY_CLASS%::OnUpdate() /*override*/
{
	return !Hy_Input().IsActionReleased(INPUT_ExitGame);
}
