#include "HyTemplate.h"


HyTemplate::HyTemplate(HarmonyInit &initStruct) : IHyApplication(initStruct)
{
}


HyTemplate::~HyTemplate()
{
}

/*virtual*/ bool HyTemplate::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	return true;
}

/*virtual*/ bool HyTemplate::Update()
{
	return true;
}

/*virtual*/ void HyTemplate::Shutdown()
{
}
