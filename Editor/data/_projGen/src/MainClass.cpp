#include "pch.h"
#include "%HY_FILENAME%.h"

%HY_CLASS%::%HY_CLASS%(HyInit &initStruct) :
	HyEngine(initStruct),
	m_pCamera(HyEngine::Window().CreateCamera2d())
{
	HyEngine::Input().MapBtn(INPUT_ExitGame, HYKEY_Escape);
}

%HY_CLASS%::~%HY_CLASS%()
{
}

/*virtual*/ bool %HY_CLASS%::OnUpdate() /*override*/
{
	return !HyEngine::Input().IsActionReleased(INPUT_ExitGame);
}
