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

class HyWindow;

class HyInput_NULL : public IHyInput
{
public:
	HyInput_NULL(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef) : IHyInput(uiNumInputMappings, windowListRef) { }

	virtual void Update() override { }

	void HandleMsg(uint32 uiWindowIndex, int32 iWidth, int32 iHeight, const MSG &msg) { }

	virtual void StartRecording() override { }
	virtual void StopRecording() override { }
	virtual void SerializeRecording() override { }

	virtual void StartPlayback() override { }
	virtual void StopPlayback() override { }
};

class HyInputMap_NULL : public IHyInputMap
{
public:
	HyInputMap_NULL(IHyInput *pInputManager) : IHyInputMap() { }

	virtual bool MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn) override { return false; }
	virtual bool MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn) override { return false; }
	virtual bool MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn, uint32 uiGamePadIndex) override { return false; }

	virtual bool MapAxis_MO(uint32 iUserId, HyMouseAxis eAxis, float fMin = 0.0f, float fMax = 1.0f) override { return false; }
	virtual bool MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f) override { return false; }

	virtual void Unmap(uint32 iUserId) { }
	virtual bool IsMapped(uint32 iUserId) const override { return false; }

	virtual bool IsBtnDown(uint32 iUserId) const override { return false; }
	virtual bool IsBtnDownBuffered(uint32 iUserId) const override { return false; }
	virtual bool IsBtnReleased(uint32 iUserId) const override { return false; }

	virtual float GetAxis(uint32 iUserId) const override { return 0.0f; }
	virtual float GetAxisDelta(uint32 iUserId) const override { return 0.0f; }
};

#endif /* __HyInput_NULL_h__ */
