#include "PlatformInterop.h"
#include "TLSEntry.h"

CTLSEntry::CTLSEntry()
{
#if defined(HY_PLATFORM_GUI_WIN)
	// aquire TLS
	m_TLSId = TlsAlloc();
	CHECK_EXPR(m_TLSId != TLS_OUT_OF_INDEXES);
#elif defined(HY_PLATFORM_UNIX)
	CHECK_EXPR(pthread_key_create(&m_TLSKey, NULL) == 0);
#endif
}

CTLSEntry::~CTLSEntry()
{
#if defined(HY_PLATFORM_GUI_WIN)
	// free TLS
	TlsFree(m_TLSId);
#elif defined(HY_PLATFORM_UNIX)
	pthread_key_delete(m_TLSKey);
#endif
}

void CTLSEntry::Set(const PVOID p_Value)
{
#if defined(HY_PLATFORM_GUI_WIN)
	TlsSetValue(m_TLSId, p_Value);
#elif defined(HY_PLATFORM_UNIX)
	pthread_setspecific(m_TLSKey, p_Value);
#endif
}

PVOID CTLSEntry::Get() const
{
	PVOID ptr;
#if defined(HY_PLATFORM_GUI_WIN)
	ptr = (PVOID)TlsGetValue(m_TLSId);
#elif defined(HY_PLATFORM_UNIX)
	ptr = (PVOID)pthread_getspecific(m_TLSKey);
#endif
	return ptr;
}

