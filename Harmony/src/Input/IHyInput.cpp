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

IHyInput::IHyInput(vector<IHyInputMap *> &vInputMapsRef)
{
	IHyInputMap::SetManagerPtr(this);

	for(uint32 i = 0; i < vInputMapsRef.size(); ++i)
		vInputMapsRef[i]->Initialize();
}

/*virtual*/ IHyInput::~IHyInput()
{
}
