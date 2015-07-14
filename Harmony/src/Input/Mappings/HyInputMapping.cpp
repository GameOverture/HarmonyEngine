/**************************************************************************
 *	HyInputMapping.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Input/Mappings/HyInputMapping.h"
#include "Input/HyInput.h"

HyInputMapping::HyInputMapping() :	m_pHyInput(NULL),
									m_pInputMap(NULL)
{

}


HyInputMapping::~HyInputMapping(void)
{
	delete m_pInputMap;
}

//void HyInputMapping::BindBtnMap(eActionButton eBtn, HyInputKey &tKey)
//{
//	m_vBtnMappings.push_back(std::pair<eActionButton, HyInputKey>(eBtn, tKey));
//}
//
//void HyInputMapping::BindAxisMap(eActionAxis eAxis, HyInputKey &tKey)
//{
//	HyAssert(tKey.GetInputType() == HyInputKey::InputType_AXIS, "HyInputMapping::BindAxisMap() recieved a key that wasn't an InputType_AXIS. You must use HyInputMapping::BindAxisMapPos() or HyInputMapping::BindAxisMapNeg() with this HyInputKey");
//	m_vAxisMappings.push_back(std::pair<eActionAxis, HyInputKey>(eAxis, tKey));
//}
//
//void HyInputMapping::BindAxisMapPos(eActionAxis eAxis, HyInputKey &tKey)
//{
//	HyAssert(tKey.GetInputType() != HyInputKey::InputType_AXIS, "HyInputMapping::BindAxisMapPos() recieved a key that was an InputType_AXIS. You must use HyInputMapping::BindAxisMap with this HyInputKey");
//	tKey.SetAsPositiveAxis(true);
//	m_vAxisMappings.push_back(std::pair<eActionAxis, HyInputKey>(eAxis, tKey));
//}
//
//void HyInputMapping::BindAxisMapNeg(eActionAxis eAxis, HyInputKey &tKey)
//{
//	HyAssert(tKey.GetInputType() != HyInputKey::InputType_AXIS, "HyInputMapping::BindAxisMapNeg() recieved a key that was an InputType_AXIS. You must use HyInputMapping::BindAxisMap with this HyInputKey");
//	tKey.SetAsPositiveAxis(false);
//	m_vAxisMappings.push_back(std::pair<eActionAxis, HyInputKey>(eAxis, tKey));
//}
//
//void HyInputMapping::GetGamePadIds(vector<uint32> &vGamePadIdsOut)
//{
//	vGamePadIdsOut.clear();
//	map<uint32, tInputState>::iterator iter = m_pHyInput->GetGamePadMapRef().begin();
//	map<uint32, tInputState>::iterator iterEnd = m_pHyInput->GetGamePadMapRef().end();
//	for(; iter != iterEnd; ++iter)
//		vGamePadIdsOut.push_back(iter->first);
//}
//
//bool HyInputMapping::GpBtnDown(eActionButton eBtn)
//{
//	return (m_tCurConvertedInput.uiButtonFlags & (1 << eBtn)) != 0;
//}
//
//bool HyInputMapping::GpBtnDownBuff(eActionButton eBtn)
//{
//	return (m_tCurConvertedInput.uiButtonFlags & (1 << eBtn)) && ((m_uiPrevConvertedBtnFlags & (1 << eBtn)) == 0);
//}
//
//bool HyInputMapping::GpBtnUp(eActionButton eBtn)
//{
//	return (m_uiPrevConvertedBtnFlags & (1 << eBtn)) && ((m_tCurConvertedInput.uiButtonFlags & (1 << eBtn)) == 0);
//}
//
//float HyInputMapping::GpAxis(eActionAxis eAxis)
//{
//	return m_tCurConvertedInput.pAxisStates[eAxis];
//}

void HyInputMapping::SetHyInputPtr(HyInput *pHyInput)
{
	m_pHyInput = pHyInput;
	m_pHyInput->GiveDeviceIds(m_idKB, m_idMouse, m_idGamePad, m_idTouch);

	m_pInputMap = new gainput::InputMap(m_pHyInput->GetManager());
}
