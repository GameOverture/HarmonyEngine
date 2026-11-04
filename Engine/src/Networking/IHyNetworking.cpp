/**************************************************************************
*	IHyNetworking.cpp
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	MIT License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/main/Engine/LICENSE
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
