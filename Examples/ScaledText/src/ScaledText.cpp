#include "ScaledText.h"


ScaledText::ScaledText(HarmonyInit &initStruct) :	IHyApplication(initStruct),
													m_Text("", "NewFont")
{
}

ScaledText::~ScaledText()
{
}

/*virtual*/ bool ScaledText::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	m_Text.TextSet("This is a test");
	m_Text.Load();

	return true;
}

/*virtual*/ bool ScaledText::Update()
{
	return true;
}

/*virtual*/ void ScaledText::Shutdown()
{
}
