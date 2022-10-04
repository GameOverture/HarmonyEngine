/**************************************************************************
*	HyMain.cpp
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Utilities/HyMain.h"
#include <codecvt>

#if !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)
	#include <shellapi.h> // CommandLineToArgvW()

	#ifdef main
		#undef main
	#endif /* main */

	//#define WIN_WStringToUTF8(S) SDL_iconv_string("UTF-8", "UTF-16LE", (char *)(S), (SDL_wcslen(S)+1)*sizeof(WCHAR))

	// The VC++ compiler needs main/wmain defined
	#if defined(HY_COMPILER_MSVC)
		#define console_ansi_main main
		#if UNICODE
			#define console_wmain wmain
		#endif
	#endif

	#define HYMAIN_OutOfMemDlg	MessageBoxA(NULL, "Fatal Error", "Out of memory - aborting", MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON2)

	// Gets the arguments with GetCommandLine, converts them to argc and argv and calls the game's main()
	static int main_getcmdline(void)
	{
		LPWSTR *argvw;
		int argc, result;
		argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
		if(argvw == NULL)
		{
			HYMAIN_OutOfMemDlg;
			return FALSE;
		}

		// Parse and convert cmd line to UTF8
		char **argv = HY_NEW char *[argc + 1];
		if(!argv)
		{
			HYMAIN_OutOfMemDlg;
			return FALSE;
		}
		int i;
		for(i = 0; i < argc; ++i)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			std::string sConvStr = myconv.to_bytes(argvw[i]);

			argv[i] = HY_NEW char[sConvStr.size() + 1];
			if(!argv[i])
			{
				HYMAIN_OutOfMemDlg;
				return FALSE;
			}
			strcpy(argv[i], sConvStr.c_str());
		}
		argv[i] = NULL;
		LocalFree(argvw);

		// Run the application main() code
		result = Hy_main(argc, argv);

		// Free argv, to avoid memory leak
		for(i = 0; i < argc; ++i)
			delete argv[i];
		delete argv;

		return result;
	}

	// This is where execution begins [console apps, ansi]
	int console_ansi_main(int argc, char *argv[])
	{
		return main_getcmdline();
	}
	#if UNICODE
		// This is where execution begins [console apps, unicode]
		int console_wmain(int argc, wchar_t *wargv[], wchar_t *wenvp)
		{
			return main_getcmdline();
		}
	#endif

	// This is where execution begins [windowed apps]
	int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
	{
		return main_getcmdline();
	}
#endif /* !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS) */
