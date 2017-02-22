#include "TextTest.h"


TextTest::TextTest(HarmonyInit &initStruct) :	IHyApplication(initStruct),
												m_Text("Prefixx", "Font")
{
}


TextTest::~TextTest()
{
}

/*virtual*/ bool TextTest::Initialize()
{
	Window().CreateCamera2d();

	m_Text.TextSet("T");
	m_Text.SetAsScaleBox(500.0f, 100.0f, false);

	m_Text.TextSetAlignment(HYALIGN_Center);

	m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	m_TextBox.SetTint(1.0f, 0.0f, 0.0f);

	m_Text.AddChild(m_TextBox);
	m_Text.Load();

	return true;
}

/*virtual*/ bool TextTest::Update()
{
	return true;
}

/*virtual*/ void TextTest::Shutdown()
{
}
