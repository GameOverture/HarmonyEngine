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

class IHyInst2d;

class IHyInput
{
protected:
	const uint32				m_uiNUM_INPUT_MAPS;
	IHyInputMap *				m_pInputMaps;

	std::vector<IHyInst2d *>	m_InputListenerList;

public:
	IHyInput(uint32 uiNumInputMappings);
	virtual ~IHyInput();

	IHyInputMap *GetInputMapArray();

	virtual void StartRecording() = 0;
	virtual void StopRecording() = 0;
	virtual void SerializeRecording() = 0;

	virtual void StartPlayback() = 0;
	virtual void StopPlayback() = 0;

	void SetInputListener(bool bEnable, IHyInst2d *pInst);

	void Update();
	virtual void OnUpdate() = 0;
};

#endif /* __HyInput_h__ */
