#include "Harmony/HyEngine.h"
#include "NewGame.h"

//--------------------------------------------------------------------------------------
// Entry point to the program.
//
// Initialize a HarmonyInit structure either by hard-coding some
// values, using the command line arguments, or simply passing the path to the *.hyproj.
//
// Create your game class that extends from IGame, and pass it the HarmonyInit 
// structure. Then just pass your game to the engine and you're done.
//--------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	HarmonyInit initStruct("../NewGame.hyproj");

	NewGame game(initStruct);
	HyEngine::RunGame(game);

	return 0;
}
