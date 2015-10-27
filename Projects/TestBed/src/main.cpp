/**************************************************************************
 *	main.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "stdafx.h"
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
	HarmonyInit initStruct;
	initStruct.szGameName = "Test Game";
	initStruct.szDataDir = "./data";
	initStruct.uiNumWindows = 2;
	
	initStruct.windowInfo[0].sName = "Test Window";
	initStruct.windowInfo[0].vLocation = vec2(100.0f, 100.0f);
	initStruct.windowInfo[0].vResolution = vec2(1024.0f, 768.0f);
	initStruct.windowInfo[0].eType = HYWINDOW_WindowedSizeable;

	initStruct.windowInfo[1].sName = "Test Window 2";
	initStruct.windowInfo[1].vLocation = vec2(2000.0f, 100.0f);
	initStruct.windowInfo[1].vResolution = vec2(1024.0f, 768.0f);
	initStruct.windowInfo[1].eType = HYWINDOW_WindowedSizeable;

	initStruct.fPixelsPerMeter = 180.0f;
	initStruct.uiNumInputMappings = 1;

	TestBed test(initStruct);
	HyEngine::RunGame(test);

	return 0;
}

