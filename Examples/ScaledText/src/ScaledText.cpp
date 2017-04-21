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

	m_Text.TextSet("$1.00");
	m_Text.SetAsScaleBox(100.0f, 30.0f, true);
	m_Text.Load();

	m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	m_TextBox.SetTint(1.0f, 1.0f, 1.0f);
	m_TextBox.Load();

	return true;
}

/*virtual*/ bool ScaledText::Update()
{
	return true;
}

/*virtual*/ void ScaledText::Shutdown()
{
}
