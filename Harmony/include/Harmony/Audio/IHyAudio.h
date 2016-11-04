/**************************************************************************
 *	IHyAudio.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyAudio_h__
#define __IHyAudio_h__

#include "Afx/HyStdAfx.h"

#include <vector>
using std::vector;

class HyWindow;

class IHyAudio
{
	std::vector<HyWindow *> &	m_WindowListRef;

public:
	IHyAudio(vector<HyWindow *> &windowListRef);
	~IHyAudio(void);
};

#endif /* __IHyAudio_h__ */
