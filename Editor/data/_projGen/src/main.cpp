#include "pch.h"
#include "%HY_CLASS%.h"

//--------------------------------------------------------------------------------------
// Entry point to %HY_TITLE%
//
// Initialize a HarmonyInit structure either by passing the path to the *.hyproj file,
// or override its settings using the command line arguments or simply hard-coding the values.
//
// Instantiate your game class that extends from HyEngine, and initialize it with the
// HarmonyInit structure. return RunGame() to end the program with the proper exit code, and 
// any memory leaks will be written to output if using a supported IDE.
//--------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	HarmonyInit initStruct("%HY_CLASS%.hyproj");

	// Emscripten's update loop requires all memory to be on the heap (above 'initStruct' is copied internally)
	%HY_CLASS% *pGame = HY_NEW %HY_CLASS%(initStruct);
	return pGame->RunGame();
}
