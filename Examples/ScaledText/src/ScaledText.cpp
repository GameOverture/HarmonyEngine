#include "ScaledText.h"
#include "BaseGame.h"

ScaledText::ScaledText(HarmonyInit &initStruct) :	LgSlotGame(initStruct),
													m_Text("", "NewFont"),
													m_Telnet(m_Tilt)
{
	
}

ScaledText::~ScaledText()
{
}

/*virtual*/ void ScaledText::OnConstruct() /*override*/
{
}

/*virtual*/ ILgSubGame *ScaledText::OnSubGame_Allocate(const ILgxGameData *pXmlGameData) /*override*/
{
	return new BaseGame(*this, pXmlGameData);
}

///*virtual*/ bool ScaledText::Initialize()
//{
//	m_pCamera = Window().CreateCamera2d();
//
//	
//
//	//m_Text.TextSet("$1.00");
//	//m_Text.SetAsScaleBox(100.0f, 30.0f, true);
//	//m_Text.Load();
//
//	//m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
//	//m_TextBox.SetTint(1.0f, 1.0f, 1.0f);
//	//m_TextBox.Load();
//
//	return true;
//}
//
///*virtual*/ bool ScaledText::Update()
//{
//	return true;
//}
//
///*virtual*/ void ScaledText::Shutdown()
//{
//}
