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

#include "IHyInst2d.h"

#include "Assets/Data/HyAudioData.h"

class HyAudio2d : public IHyAudioInstInterop
{
	HySfxData *						m_pDataPtr;

	enum eCueType
	{
		SNDCUETYPE_Unknown = -1,
		SNDCUETYPE_Single = 0,
		SNDCUETYPE_WeightTable = 1,
		SNDCUETYPE_Cycle = 2,
	};
	eCueType		m_eCueType;

public:
	HyAudio2d(vector<HyWindow *> &windowListRef);
	virtual ~HyAudio2d(void);
};

#endif /* __HyAudio2d_h__ */
