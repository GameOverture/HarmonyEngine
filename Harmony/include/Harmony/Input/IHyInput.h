/**************************************************************************
 *	HyInput.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyInput_h__
#define __HyInput_h__

#include "Afx/HyStdAfx.h"

#include "Input/IHyInputMap.h"

#include <vector>
using std::vector;

class IHyInput
{
	vector<IHyInputMap> &		m_vInputMapsRef;

	enum eReplayState
	{
		REPLAY_Off = 0,
		REPLAY_Saving,
		REPLAY_Replaying
	};
	eReplayState					m_eReplayState;
	uint64							m_uiRecordCount;

public:
	IHyInput(vector<IHyInputMap> &vInputMapsRef);
	virtual ~IHyInput();

	virtual void ProcessInput() = 0;

	static void StartRecording();
	static void PlayRecording();

	void Update();
};

#endif /* __HyInput_h__ */
