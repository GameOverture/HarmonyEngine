/**************************************************************************
 *	main.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Harmony/HyEngine.h"
#include "TestBed.h"

//--------------------------------------------------------------------------------------
// Entry point to the program.
//
// Initialize a HarmonyInit structure either by hard-coding some
// values, using the commandline arguments, or parsing some init file.
//
// Create your game class that extends from IGame, and pass it the HarmonyInit 
// structure. Then just pass your game to the engine and you're done.
//--------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	//_crtBreakAlloc = 5429;

	HarmonyInit initStruct;
	initStruct.sGameName = "Test Game";
	initStruct.sDataDir = "../data";
	initStruct.uiNumWindows = 2;
	
	initStruct.windowInfo[0].sName = "Test Window";
	initStruct.windowInfo[0].vLocation = glm::vec2(1024.0f, 100.0f);
	initStruct.windowInfo[0].vResolution = glm::vec2(800.0f, 600.0f);
	initStruct.windowInfo[0].eType = HYWINDOW_WindowedSizeable;

	initStruct.windowInfo[1].sName = "Test Window 2";
	initStruct.windowInfo[1].vLocation = glm::vec2(0.0f, 100.0f);
	initStruct.windowInfo[1].vResolution = glm::vec2(1024.0f, 768.0f);
	initStruct.windowInfo[1].eType = HYWINDOW_WindowedSizeable;

	initStruct.fPixelsPerMeter = 180.0f;
	initStruct.uiNumInputMappings = 1;

	TestBed test(initStruct);
	HyEngine::RunGame(test);

	//_CrtDumpMemoryLeaks();

	return 0;
}

