/**************************************************************************
*	HyConsole_Std.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Diagnostics/Console/Interop/HyConsole_Std.h"

HyConsole_Std::HyConsole_Std(bool bCreateConsole, const HyWindowInfo &consoleInfo)
{
}

/*virtual*/ HyConsole_Std::~HyConsole_Std()
{
}

/*virtual*/ void HyConsole_Std::OnLog(std::ostream &os, const char *szMsg, LogType eType) /*override*/
{
	switch(eType)
	{
	case LOG_Regular:
		os << szMsg;
		break;

	case LOG_Warning:
		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
		os << "[W]: " << szMsg;
		break;
	case LOG_Error:
		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		os << "[E]: " << szMsg;
		break;
	case LOG_Info:
		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		os << "[I]: " << szMsg;
		break;

	case LOG_Title: {
		//CONSOLE_SCREEN_BUFFER_INFO coninfo;
		//GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE);
		for(int i = 0; i < 50/*coninfo.dwSize.X*/; i++)
			os << "~";
		os << std::endl;
		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		os << '\t' << szMsg << std::endl;

		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE);
		for(int i = 0; i < 50/*coninfo.dwSize.X*/; i++)
			os << "~";
		break; }

	case LOG_Section: {
		os << std::endl;
		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		os << "-==== " << szMsg << " ====-";
		break; }
	}

	// Reset console color and start a newline for next Write()
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
	os << std::endl;
}
