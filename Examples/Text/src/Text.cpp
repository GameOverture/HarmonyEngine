#include "Text.h"


Text::Text(HarmonyInit &initStruct) :	IHyApplication(initStruct),
										m_Text(nullptr, "NewFont"),
										m_Text2(nullptr, "NewFont")
{
}

Text::~Text()
{
}

/*virtual*/ bool Text::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	std::vector<glm::vec2> vertexList;
	vertexList.push_back(glm::vec2(-1000.0f, 0.0f));
	vertexList.push_back(glm::vec2(+1000.0f, 0.0f));
	m_Origin.SetAsLineChain(vertexList);

	m_Text.pos.X(-200.0f);
	m_Text.TextSet(".");
	m_Text.SetAsScaleBox(100.0f, 350.0f, true, false);
	m_Text.TextSetAlignment(HYALIGN_Center);

	m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	m_Text.AddChild(m_TextBox);



	m_Text2.pos.X(200.0f);
	m_Text2.TextSet(".");
	m_Text2.SetAsScaleBox(100.0f, 350.0f, true, true);
	m_Text2.TextSetAlignment(HYALIGN_Center);

	m_TextBox2.SetAsQuad(m_Text2.TextGetBox().x, m_Text2.TextGetBox().y, true);
	m_Text2.AddChild(m_TextBox2);



	m_Origin.Load();
	m_Text.Load();
	m_Text2.Load();

	return true;
}

/*virtual*/ bool Text::Update()
{
	return true;
}

/*virtual*/ void Text::Shutdown()
{
}
