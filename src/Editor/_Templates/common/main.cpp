#include "pch.h"
#include "%HY_CLASS%.h"

//--------------------------------------------------------------------------------------
// Entry point to %HY_TITLE%
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
	HarmonyInit initStruct("%HY_CLASS%.hyproj");

	%HY_CLASS% *pGame = new %HY_CLASS%(initStruct);
	pGame->RunGame();

	return 0;
}
