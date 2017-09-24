#include "PlatformInterop.h"
#include "Utils.h"


void Utils::SetDebugThreadName(uint32 p_ThreadId, PCSTR p_ThreadName)
{
#if defined(HY_PLATFORM_GUI_WIN)
#ifdef _DEBUG
	if(!IsDebuggerPresent())
		return;

	struct THREADNAME_INFO
	{
		DWORD dwType;		// must be 0x1000
		PCSTR szName;		// pointer to name (in user addr space)
		DWORD dwThreadID;	// thread ID (-1=caller thread)
		DWORD dwFlags;		// reserved for future use, must be zero
	};

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = p_ThreadName;
	info.dwThreadID = p_ThreadId;
	info.dwFlags = 0;

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#else
	UNREFERENCED_PARAMETER(p_ThreadId);
	UNREFERENCED_PARAMETER(p_ThreadName);
#endif
#else
	UNREFERENCED_PARAMETER(p_ThreadId);
	UNREFERENCED_PARAMETER(p_ThreadName);
#endif
}

void Utils::Beep(uint32 dwDurationMs, uint32 dwFreq)
{
#if defined(HY_PLATFORM_GUI_WIN)
	::Beep(dwFreq, dwDurationMs);
#elif defined(HY_PLATFORM_UNIX)
	int fd, arg;
	fd = open("/dev/tty0", O_RDONLY);
	arg = (dwDurationMs << 16) + (1193180 / dwFreq);
	//ioctl(fd, KDMKTONE, arg);
	ioctl(fd, 0x4B30, arg);
	InteropSleep(dwDurationMs);
	close(fd);
#endif
}

void Utils::BeepErrorWaitObject()
{
	Utils::Beep(500, 1000);
	InteropSleep(500);
	Utils::Beep(500, 4000);
}

// code from http://www.c-plusplus.de/forum/viewtopic-var-t-is-168607.html
std::wstring Utils::StringToWString(const std::string& p_Str)
{
	const std::ctype<wchar_t>& CType = std::use_facet<std::ctype<wchar_t> >(std::locale());
	std::vector<wchar_t> wideStringBuffer(p_Str.length());
	CType.widen(p_Str.data(), p_Str.data() + p_Str.length(), &wideStringBuffer[0]);
	return std::wstring(&wideStringBuffer[0], wideStringBuffer.size()); 
}

std::string Utils::WStringToString(const std::wstring& p_Str)
{
	const std::ctype<wchar_t>& CType = std::use_facet<std::ctype<wchar_t> >(std::locale());
	std::vector<char> narrowStringBuffer(p_Str.length());
	CType.narrow(p_Str.data(), p_Str.data() + p_Str.length(), ' ', &narrowStringBuffer[0]);
	return std::string(&narrowStringBuffer[0], narrowStringBuffer.size()); 
}


