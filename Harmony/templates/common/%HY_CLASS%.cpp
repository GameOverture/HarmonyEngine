#include "%HY_CLASS%.h"


%HY_CLASS%::%HY_CLASS%(HarmonyInit &initStruct) : IHyApplication(initStruct)
{
}


%HY_CLASS%::~%HY_CLASS%()
{
}

/*virtual*/ bool %HY_CLASS%::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	return true;
}

/*virtual*/ bool %HY_CLASS%::Update()
{
	return true;
}

/*virtual*/ void %HY_CLASS%::Shutdown()
{
}