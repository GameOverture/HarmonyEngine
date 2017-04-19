#include "ScaledText.h"


ScaledText::ScaledText(HarmonyInit &initStruct) : IHyApplication(initStruct)
{
}


ScaledText::~ScaledText()
{
}

/*virtual*/ bool ScaledText::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	return true;
}

/*virtual*/ bool ScaledText::Update()
{
	return true;
}

/*virtual*/ void ScaledText::Shutdown()
{
}
