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
	m_pInputMaps = reinterpret_cast<HyInputMapInterop *>(HY_NEW unsigned char[sizeof(HyInputMapInterop) * m_uiNUM_INPUT_MAPS]);
	HyInputMapInterop *pWriteLoc = static_cast<HyInputMapInterop *>(m_pInputMaps);
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i, ++pWriteLoc)
		new (pWriteLoc)HyInputMapInterop(this);

	IHyInst2d::sm_pInputManager = this;
}

/*virtual*/ IHyInput::~IHyInput()
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		static_cast<HyInputMapInterop *>(m_pInputMaps)[i].~HyInputMapInterop();

	unsigned char *pMemBuffer = reinterpret_cast<unsigned char *>(m_pInputMaps);
	delete[] pMemBuffer;
}

IHyInputMap *IHyInput::GetInputMapArray()
{
	return m_pInputMaps;
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

void IHyInput::Update()
{
	for(uint32 i = 0; i < m_InputListenerList.size(); ++i)
		m_InputListenerList[i]->InputUpdate(m_pInputMaps);

	OnUpdate();
}
