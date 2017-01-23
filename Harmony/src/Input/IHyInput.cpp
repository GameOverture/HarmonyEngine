///**************************************************************************
// *	HyInput.cpp
// *	
// *	Harmony Engine
// *	Copyright (c) 2013 Jason Knobler
// *
// *	The zlib License (zlib)
// *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
#include "Input/IHyInput.h"
#include "Input/IHyInputMap.h"

#include "Scene/Instances/IHyInst2d.h"

IHyInput::IHyInput(uint32 uiNumInputMappings) : m_uiNUM_INPUT_MAPS(uiNumInputMappings)
{
	IHyInst2d::sm_pInputManager = this;
}

/*virtual*/ IHyInput::~IHyInput()
{
}

void IHyInput::SetInputListener(bool bEnable, IHyInst2d *pInst)
{
	if(bEnable && std::find(m_InputListenerList.begin(), m_InputListenerList.end(), pInst) == m_InputListenerList.end())
		m_InputListenerList.push_back(pInst);
	else
	{
		auto iter = std::find(m_InputListenerList.begin(), m_InputListenerList.end(), pInst);

		m_InputListenerList.erase(iter);
	}
}
