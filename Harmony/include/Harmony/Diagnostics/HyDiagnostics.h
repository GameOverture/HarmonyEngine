/**************************************************************************
*	HyDiagnostics.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyDiagnostics_h__
#define HyDiagnostics_h__

#include "Afx/HyStdAfx.h"

class HyDiagnostics
{
	std::string			m_sPlatform;
	uint32				m_uiNumCPUs;
	uint64				m_uiRamSize;

public:
	HyDiagnostics();
	~HyDiagnostics();
};

#endif /* HyDiagnostics_h__ */
