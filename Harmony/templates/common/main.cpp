#include "Harmony/HyEngine.h"
#include "HyTemplate.h"

//--------------------------------------------------------------------------------------
// Entry point to the program.
//
// Initialize a HarmonyInit structure either by hard-coding some
// values, using the command line arguments, or simply passing the path to the *.hyproj.
//
// Create your game class that extends from IHyApplication, and initialize it with the
// HarmonyInit structure. Then just pass your game to the engine and you're done.
//--------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	HarmonyInit initStruct("[HyTitle].hyproj");

	HyTemplate game(initStruct);
	HyEngine::RunGame(game);

	return 0;
}
