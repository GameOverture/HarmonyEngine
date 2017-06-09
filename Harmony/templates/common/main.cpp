#include "Harmony/HyEngine.h"
#include "HyTemplate.h"

//--------------------------------------------------------------------------------------
// Entry point to the program.
//
// Initialize a HarmonyInit structure either by hard-coding some
// values, using the command line arguments, or simply passing the path to the *.hyproj.
//
// Dynamically allocate your game class that extends from IHyApplication, and initialize it with the
// HarmonyInit structure. Then just pass your game to the engine and you're done. It will
// be deleted within RunGame() when exiting the program, and any memory leaks will be outputted
// if using a supporting IDE.
//--------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	HarmonyInit initStruct("[HyTitle].hyproj");

	HyTemplate *pGame = new HyTemplate(initStruct);
	HyEngine::RunGame(pGame);

	return 0;
}
