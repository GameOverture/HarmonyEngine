/**************************************************************************
*	HyConsole_Win.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Diagnostics/Console/Interop/HyConsole_Win.h"

#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

HyConsole_Win::HyConsole_Win(bool bCreateConsole, HyWindowInfo &consoleInfo)
{
	if(bCreateConsole == false)
		return;

	if(!AllocConsole())
		return;

	FILE *fp;
	int hConHandle;
	long long lStdHandle;

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);

	// redirect unbuffered STDIN to the console
	lStdHandle = (long long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);

	// redirect unbuffered STDERR to the console
	lStdHandle = (long long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio(true);

	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

	coninfo.dwSize.X = consoleInfo.vSize.x;
	coninfo.dwSize.Y = consoleInfo.vSize.y;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	HWND hConsole = GetConsoleWindow();
	MoveWindow(hConsole, consoleInfo.ptLocation.x, consoleInfo.ptLocation.y, consoleInfo.vSize.x * 10, consoleInfo.vSize.y * 10, TRUE); // Super hack!

	// Disable the exit button on console window
	HMENU  hm = GetSystemMenu(hConsole, false);
	DeleteMenu(hm, SC_CLOSE, MF_BYCOMMAND);
}

HyConsole_Win::~HyConsole_Win()
{
}

/*virtual*/ void HyConsole_Win::Log(std::ostream &os, const char *szMsg, LogType eType)
{
	switch(eType)
	{
	case LOG_Regular:
		os << szMsg;
		break;

	case LOG_Warning:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
		os << "[W]: " << szMsg;
		break;
	case LOG_Error:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		os << "[E]: " << szMsg;
		break;
	case LOG_Info:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		os << "[I]: " << szMsg;
		break;

	case LOG_Title: {
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE);
		for(int i = 0; i < coninfo.dwSize.X; i++)
			os << "~";

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		os << '\t' << szMsg << std::endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE);
		for(int i = 0; i < coninfo.dwSize.X; i++)
			os << "~";
		break; }

	case LOG_Section: {
		os << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		os << "-==== " << szMsg << " ====-";
		break; }
	}

	// Reset console color and start a newline for next Write()
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
	os << std::endl;
}
