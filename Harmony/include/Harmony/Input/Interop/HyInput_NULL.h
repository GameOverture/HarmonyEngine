/**************************************************************************
*	HyInput_NULL.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyInput_NULL_h__
#define __HyInput_NULL_h__

#include "Input/IHyInput.h"
#include "Input/IHyInputMap.h"

class HyInput_NULL : public IHyInput
{
public:
	HyInput_NULL(uint32 uiNumInputMappings) : IHyInput(uiNumInputMappings) { }

	virtual void Update() { }

	virtual void StartRecording() { }
	virtual void StopRecording() { }
	virtual void SerializeRecording() { }

	virtual void StartPlayback() { }
	virtual void StopPlayback() { }
};

class HyInputMap_NULL : public IHyInputMap
{
public:
	virtual glm::ivec2 GetMousePos() { return glm::ivec2(0); }

	virtual bool MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn) { return false; }
	virtual bool MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn) { return false; }
	virtual bool MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn, uint32 uiGamePadIndex) { return false; }

	virtual bool MapAxis_MO(uint32 iUserId, HyMouseBtn eAxis, float fMin = 0.0f, float fMax = 1.0f) { return false; }
	virtual bool MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f) { return false; }

	virtual void Unmap(uint32 iUserId) { }
	virtual bool IsMapped(uint32 iUserId) const { return false; }

	virtual bool IsBtnDown(uint32 iUserId) const { return false; }
	virtual bool IsBtnDownBuffered(uint32 iUserId) const { return false; }
	virtual bool IsBtnReleased(uint32 iUserId) const { return false; }

	virtual float GetAxis(uint32 iUserId) const { return 0.0f; }
	virtual float GetAxisDelta(uint32 iUserId) const { return 0.0f; }

private:
	virtual void Initialize() { }
};

#endif /* __HyInput_NULL_h__ */
