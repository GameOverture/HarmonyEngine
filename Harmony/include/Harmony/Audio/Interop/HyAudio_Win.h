/**************************************************************************
*	HyAudio_Win.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyAudio_Win_h__
#define __HyAudio_Win_h__

#include "Afx/HyStdAfx.h"
#include "Audio/IHyAudio.h"

class HyAudio_Win : public IHyAudio
{
public:
	HyAudio_Win(HyRendererInterop &rendererRef);
	virtual ~HyAudio_Win();
};

#endif /* __HyAudio_Win_h__ */
