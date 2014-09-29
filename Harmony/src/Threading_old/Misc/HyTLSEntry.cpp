/**************************************************************************
 *	HyTLSEntry.cpp
 *	
 *	Copyright (c) 2012 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#include "HyTLSEntry.h"

CTLSEntry::CTLSEntry()
{
#if defined(HY_PLATFORM_WINDOWS)
	// aquire TLS
	m_TLSId = TlsAlloc();
	HyAssert(m_TLSId != TLS_OUT_OF_INDEXES, "HyTLSEntry is out of indices.");
#elif defined(HY_PLATFORM_UNIX)
	HyAssert(pthread_key_create(&m_TLSKey, NULL) == 0, "HyTLSEntry is out of indices.");
#endif
}

CTLSEntry::~CTLSEntry()
{
#if defined(HY_PLATFORM_WINDOWS)
	// free TLS
	TlsFree(m_TLSId);
#elif defined(HY_PLATFORM_UNIX)
	pthread_key_delete(m_TLSKey);
#endif
}

void CTLSEntry::Set(const PVOID p_Value)
{
#if defined(HY_PLATFORM_WINDOWS)
	TlsSetValue(m_TLSId, p_Value);
#elif defined(HY_PLATFORM_UNIX)
	pthread_setspecific(m_TLSKey, p_Value);
#endif
}

PVOID CTLSEntry::Get() const
{
	PVOID ptr;
#if defined(HY_PLATFORM_WINDOWS)
	ptr = (PVOID)TlsGetValue(m_TLSId);
#elif defined(HY_PLATFORM_UNIX)
	ptr = (PVOID)pthread_getspecific(m_TLSKey);
#endif
	return ptr;
}
