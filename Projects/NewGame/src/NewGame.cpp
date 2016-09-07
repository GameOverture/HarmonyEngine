#include "NewGame.h"


NewGame::NewGame(HarmonyInit &initStruct) : IHyApplication(initStruct)
{
}


NewGame::~NewGame()
{
}

/*virtual*/ bool NewGame::Initialize()
{
	return true;
}

/*virtual*/ bool NewGame::Update()
{
	return true;
}

/*virtual*/ void NewGame::Shutdown()
{
}
