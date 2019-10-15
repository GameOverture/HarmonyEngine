#include "pch.h"
#include "%HY_CLASS%.h"

%HY_CLASS%::%HY_CLASS%(HarmonyInit &initStruct) :
	HyEngine(initStruct),
	m_pCamera(Hy_Window().CreateCamera2d())
{
}

%HY_CLASS%::~%HY_CLASS%()
{
}

/*virtual*/ bool %HY_CLASS%::OnUpdate() /*override*/
{
	return true;
}
