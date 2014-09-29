/**************************************************************************
 *	HyTLSEntry.h
 *	
 *	Copyright (c) 2012 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#ifndef __HyTLSEntry_h__
#define __HyTLSEntry_h__

#include "Afx/HyThreadAfx.h"

class CTLSEntry
{
public:
	// default constructor - allocats a new TLS entry
	CTLSEntry();

	// frees the TLS entry allocated in the constructor
	~CTLSEntry();

private:
	// hide copy constructor
	CTLSEntry(const CTLSEntry& p_Entry) { UNREFERENCED_PARAMETER(p_Entry); }

public:
	// set the TLS entry's value
	void Set(const PVOID p_Value);

	// get the TLS entry's value
	PVOID Get() const;

private:
	// platform dependend TLS index
#if defined(HY_PLATFORM_WINDOWS)
	DWORD m_TLSId;
#elif defined(HY_PLATFORM_UNIX)
	pthread_key_t m_TLSKey;
#endif
};

#endif /* __HyTLSEntry_h__ */
