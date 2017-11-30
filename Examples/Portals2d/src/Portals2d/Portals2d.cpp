#include "Portals2d.h"


Portals2d::Portals2d(HarmonyInit &initStruct) :	IHyApplication(initStruct),
												m_Sprite("", "Test", nullptr),
												m_Text(HY_SYSTEM_FONT, nullptr)
{
}

Portals2d::~Portals2d()
{
}

/*virtual*/ bool Portals2d::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	m_Sprite.Load();
	//m_Sprite.scale.Set(1.0f, 1.0f);

	//m_Text.Load();
	//m_Text.TextSet("Testing help");

	return true;
}

/*virtual*/ bool Portals2d::Update()
{
	return true;
}

/*virtual*/ void Portals2d::Shutdown()
{
}
