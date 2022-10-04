/**************************************************************************
*	IHyNetworking.cpp
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Networking/IHyNetworking.h"
#include "Networking/IHyNetworkClass.h"

IHyNetworking::IHyNetworking()
{
	IHyNetworkClass::sm_pNetworking = this;
}

/*virtual*/ IHyNetworking::~IHyNetworking()
{
	IHyNetworkClass::sm_pNetworking = nullptr;
}
