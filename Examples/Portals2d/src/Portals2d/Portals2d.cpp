#include "Portals2d.h"


Portals2d::Portals2d(HarmonyInit &initStruct) : IHyApplication(initStruct)
{
}


Portals2d::~Portals2d()
{
}

/*virtual*/ bool Portals2d::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	return true;
}

/*virtual*/ bool Portals2d::Update()
{
	return true;
}

/*virtual*/ void Portals2d::Shutdown()
{
}
