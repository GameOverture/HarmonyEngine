//*****************************************************************************
// Description:  Thread-local-storage class
//*****************************************************************************

#ifndef TREADINGFX_TLS_H
#define TREADINGFX_TLS_H

#include "Afx/HyStdAfx.h"

//*****************************************************************************
// class overview:
// @short:
//  TLS entry handling class
// @detailed:
//  Creating an instance of this class will allocate a slot in Thread-Local-Storage (TLS).
//  This entry can be changed and queried using the respective Set/Get functions.
//  When the instance is destroyed the allocated storage will be freed.
//-----------------------------------------------------------------------------
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

#endif // TREADINGFX_TLS_H
