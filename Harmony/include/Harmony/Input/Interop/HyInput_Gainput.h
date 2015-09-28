/**************************************************************************
 *	HyInput_Gainput.h
 *
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyInput_Gainput_h__
#define __HyInput_Gainput_h__

#include "Input/IHyInput.h"

#include "gainput/gainput.h"

class HyInput_Gainput : public IHyInput
{
	gainput::InputManager			m_Manager;

	gainput::DeviceId				m_idKB;
	gainput::DeviceId				m_idMouse;
	vector<gainput::DeviceId>		m_idGamePads;
	gainput::DeviceId				m_idTouch;

	gainput::InputMap *				m_pInputMap;

public:
	HyInput_Gainput(vector<IHyInputMap> &vInputMapsRef);
	virtual ~HyInput_Gainput();

	virtual void ProcessInput();
};

#endif /* __HyInput_Gainput_h__ */
