/**************************************************************************
 *	HyAudioData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyAudioData.h"


HyAudioData::HyAudioData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef) :
	IHyNodeData(sPath)
{
	const jsonxx::Object &audioObj = dataValueRef.get<jsonxx::Object>();

	std::string sBankPath = audioObj.get<jsonxx::String>("BankPath");

	m_pAudioBank = assetsRef.GetAudioBank(sBankPath);
}

HyAudioData::~HyAudioData(void)
{
}
