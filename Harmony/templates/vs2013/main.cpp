#include "Harmony/HyEngine.h"
#include "HyTemplate.h"

//--------------------------------------------------------------------------------------
// Entry point to the program.
//
// Initialize a HarmonyInit structure either by hard-coding some
// values, using the command line arguments, or parsing some init file.
//
// Create your game class that extends from IGame, and pass it the HarmonyInit 
// structure. Then just pass your game to the engine and you're done.
//--------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	HarmonyInit initStruct;
	initStruct.sGameName = "HyTemplate";
	initStruct.sDataDir = "HyDataRelPath";

	initStruct.windowInfo[0].sName = "HyTemplate";
	initStruct.windowInfo[0].vResolution = glm::vec2(1280.0f, 720.0f);

	initStruct.fPixelsPerMeter = 180.0f;
	initStruct.uiNumInputMappings = 1;

	HyTemplate game(initStruct);
	HyEngine::RunGame(game);

	return 0;
}
