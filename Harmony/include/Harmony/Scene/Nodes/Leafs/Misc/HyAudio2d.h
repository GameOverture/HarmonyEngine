/**************************************************************************
 *	HyAudio2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAudio2d_h__
#define __HyAudio2d_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/Nodes/HyAudioData.h"

class HyAudio2d : public IHyAudioInstInterop
{
	HyAudioData *						m_pDataPtr;

	enum CueType
	{
		SNDCUETYPE_Unknown = -1,
		SNDCUETYPE_Single = 0,
		SNDCUETYPE_WeightTable = 1,
		SNDCUETYPE_Cycle = 2,
	};
	CueType		m_eCueType;

public:
	HyAudio2d();
	virtual ~HyAudio2d(void);
};

#endif /* __HyAudio2d_h__ */
