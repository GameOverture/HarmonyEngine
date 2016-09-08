#include "NewGame.h"


NewGame::NewGame(HarmonyInit &initStruct) : IHyApplication(initStruct),
											m_Wild("Symbols", "Wild")
{
}


NewGame::~NewGame()
{
}

/*virtual*/ bool NewGame::Initialize()
{
	m_Wild.Load();

	return true;
}

/*virtual*/ bool NewGame::Update()
{
	return true;
}

/*virtual*/ void NewGame::Shutdown()
{
}
