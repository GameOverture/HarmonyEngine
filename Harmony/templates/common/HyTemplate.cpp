#include "HyTemplate.h"


HyTemplate::HyTemplate(HarmonyInit &initStruct) : IHyApplication(initStruct)
{
}


HyTemplate::~HyTemplate()
{
}

/*virtual*/ bool HyTemplate::Initialize()
{
	return true;
}

/*virtual*/ bool HyTemplate::Update()
{
	return true;
}

/*virtual*/ void HyTemplate::Shutdown()
{
}
