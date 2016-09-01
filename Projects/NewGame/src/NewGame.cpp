#include "NewGame.h"


NewGame::NewGame(HarmonyInit &initStruct) :	IHyApplication(initStruct),
											m_Tv("Feature", "Tv")
{
}

NewGame::~NewGame()
{
}

/*virtual*/ bool NewGame::Initialize()
{
	m_Tv.Load();

	return true;
}

/*virtual*/ bool NewGame::Update()
{
	return true;
}

/*virtual*/ void NewGame::Shutdown()
{
}
