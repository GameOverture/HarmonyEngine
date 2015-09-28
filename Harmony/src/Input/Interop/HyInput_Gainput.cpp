/**************************************************************************
 *	HyInput_Gainput.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Input/Interop/HyInput_Gainput.h"

HyInput_Gainput::HyInput_Gainput(vector<IHyInputMap> &vInputMapsRef) : IHyInput(vInputMapsRef)
{
}

HyInput_Gainput::~HyInput_Gainput()
{
}

/*virtual*/ void HyInput_Gainput::ProcessInput()
{
	m_Manager.Update();
}
