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
	HyCamera2d *pCam = Window().CreateCamera2d();
	//pCam->pos.Set(Window().GetResolution().x * 0.5f, Window().GetResolution().y * 0.5f);

	pCam->pos.Offset(400, 300);
	pCam->SetZoom(0.25f);

	m_Text.TextSet("One\nTwo\nThree");
	m_Text.SetAsScaleBox(500.0f, 500.0f, false);
	m_Text.pos.Set(100.0f, 100.0f);

	m_Text.TextSetAlignment(HYALIGN_Left);
	m_Text.SetScissor(0, 0, 333, 300);

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
