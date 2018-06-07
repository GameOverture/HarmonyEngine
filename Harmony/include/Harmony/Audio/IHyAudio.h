/**************************************************************************
 *	IHyAudio.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyAudio_h__
#define IHyAudio_h__

#include "Afx/HyStdAfx.h"

class HyWindow;

class IHyAudio
{
	std::vector<HyWindow *> &	m_WindowListRef;

public:
	IHyAudio(std::vector<HyWindow *> &windowListRef);
	~IHyAudio(void);
};

#endif /* IHyAudio_h__ */
