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

#include <vector>
using std::vector;

class IHyInputMap;

class IHyInput
{
public:
	IHyInput(vector<IHyInputMap *> &vInputMapsRef);
	virtual ~IHyInput();

	virtual void Update() = 0;

	virtual void StartRecording() = 0;
	virtual void StopRecording() = 0;
	virtual void SerializeRecording() = 0;

	virtual void StartPlayback() = 0;
	virtual void StopPlayback() = 0;
};

#endif /* __HyInput_h__ */
