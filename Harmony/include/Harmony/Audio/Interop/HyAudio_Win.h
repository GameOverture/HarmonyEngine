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

struct IXAudio2;
struct IXAudio2MasteringVoice;

class HyAudio_Win : public IHyAudio
{
	IXAudio2 *				m_pXAudio2;
	IXAudio2MasteringVoice *m_pMasterVoice;

public:
	HyAudio_Win(HyRendererInterop &rendererRef);
	virtual ~HyAudio_Win();

	//DWORD PlaySoundTest();
};

#endif /* __HyAudio_Win_h__ */
