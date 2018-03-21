#include "Portals2d.h"


Portals2d::Portals2d(HarmonyInit &initStruct) :	IHyApplication(initStruct),
												m_Sprite("", "Symbols", nullptr),
												m_Text(HY_SYSTEM_FONT, nullptr),
												m_Prim(nullptr)
{
}

Portals2d::~Portals2d()
{
}

/*virtual*/ bool Portals2d::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	Input().MapBtn(MoveUp, HYKEY_Up);
	Input().MapBtn(MoveDown, HYKEY_Down);
	Input().MapBtn(MoveRight, HYKEY_Right);
	Input().MapBtn(MoveLeft, HYKEY_Left);
	Input().MapBtn(RotLeft, HYKEY_PageUp);
	Input().MapBtn(RotRight, HYKEY_PageDown);
	Input().MapBtn(ScaleUp, HYKEY_Home);
	Input().MapBtn(ScaleDown, HYKEY_End);

	Hy_Diagnostics().Show(HYDIAG_ALL);

	m_Sprite.Load();
	//m_Sprite.scale.Set(1.0f, 1.0f);

	//m_Text.Load();
	//m_Text.TextSet("Testing help");

	//HyPortal2dInit portalInit;
	//portalInit.SetGateA(glm::vec2(-350.0f, 100.0f), glm::vec2(-350.0f, -100.0f), glm::vec2(0.0f, 0.0f));
	//portalInit.SetGateB(glm::vec2(350.0f, 100.0f), glm::vec2(350.0f, -100.0f), glm::vec2(0.0f, 0.0f));
	//portalInit.fStencilCullExtents = 150.0f;
	//m_pPortal = HY_NEW HyPortal2d(portalInit);
	//m_pPortal->EnableDebugDraw(true);
	//m_Sprite.SetPortal(m_pPortal);

	return true;
}

/*virtual*/ bool Portals2d::Update()
{
	float fNudgeAmt = 0.1f;

	if(Input().IsActionDown(MoveUp))
		m_Sprite.pos.Offset(0.0f, fNudgeAmt);
	if(Input().IsActionDown(MoveDown))
		m_Sprite.pos.Offset(0.0f, -fNudgeAmt);
	if(Input().IsActionDown(MoveRight))
		m_Sprite.pos.Offset(fNudgeAmt, 0.0f);
	if(Input().IsActionDown(MoveLeft))
		m_Sprite.pos.Offset(-fNudgeAmt, 0.0f);
	if(Input().IsActionDown(RotLeft))
		m_Sprite.rot.Offset(-fNudgeAmt);
	if(Input().IsActionDown(RotRight))
		m_Sprite.rot.Offset(fNudgeAmt);
	if(Input().IsActionDown(ScaleUp))
		m_Sprite.scale.Offset(fNudgeAmt, fNudgeAmt);
	if(Input().IsActionDown(ScaleDown))
		m_Sprite.scale.Offset(-fNudgeAmt, -fNudgeAmt);

	return true;
}

/*virtual*/ void Portals2d::Shutdown()
{
}
