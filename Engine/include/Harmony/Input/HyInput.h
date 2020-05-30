/**************************************************************************
 *	HyInput.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyInput_h__
#define HyInput_h__

#include "Afx/HyStdAfx.h"
#include "Input/HyInputMap.h"

class HyWindow;

class HyInput
{
	friend class HyEngine;
	friend class HyWindow;

	const uint32				m_uiNUM_INPUT_MAPS;
	HyInputMap *				m_pInputMaps;

	std::vector<HyWindow *> &	m_WindowListRef;
	HyWindow *					m_pMouseWindow;
	glm::vec2					m_ptMousePos;
	uint32						m_uiMouseBtnFlags;
	uint32						m_uiMouseBtnFlags_NewlyPressed;
	uint32						m_uiMouseBtnFlags_Buffered;

	bool						m_bTouchScreenHack;	// Some touch screens do not send a 'MOUSE DOWN' message on initial touch, until you "drag" the cursor at least 1px or release the touch

	int32						m_JoystickList[HYNUM_JOYSTICK];
	uint32						m_uiJoystickCount;

public:
	HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef);
	~HyInput();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mouse

	// All clicks are guaranteed to be down for at least '1' frame, even if it is released before the next update.
	// Clicks released and then pressed down again within one frame will not be detected.
	bool IsMouseBtnDown(HyMouseBtn eBtn) const;
	uint32 GetMouseWindowIndex() const;
	glm::vec2 GetMousePos() const;
	glm::vec2 GetWorldMousePos() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup Button/Axis Mapping

	// Categorize actions (game, UI, etc.) so the same physical button can be used for multiple mappings.
	void SetActionCategory(int32 iActionId, uint8 uiCategory, uint32 uiMappingIndex = 0);

	// Returns -1, or the mapping 'eBtn' used to be assigned to
	int32 MapBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex = 0);
	int32 MapBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex = 0);
	int32 MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex = 0);
	int32 MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex = 0);

	bool MapJoystickBtn(int32 iActionId, HyGamePadBtn eBtn, uint32 uiJoystickIndex, uint32 uiMappingIndex = 0);
	bool MapJoystickAxis(int32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f, uint32 uiMappingIndex = 0);

	bool Unmap(int32 iActionId, uint32 uiMappingIndex = 0);
	bool IsMapped(int32 iActionId, uint32 uiMappingIndex = 0) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check for Input

	bool IsActionDown(int32 iUserId, uint32 uiMappingIndex = 0) const;
	bool IsActionReleased(int32 iUserId, uint32 uiMappingIndex = 0) const;	// Only true for a single frame upon button release
	float GetAxis(int32 iUserId, uint32 uiMappingIndex = 0) const;
	float GetAxisDelta(int32 iUserId, uint32 uiMappingIndex = 0) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Replay API

	void RecordingStart();
	void RecordingStop();

	void PlaybackStart();
	void PlaybackStop();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// MISC

	// Some touch screens do not send a 'MOUSE DOWN' message on initial touch, until you "drag" the cursor at least 1px or release the touch
	// Enabling this hack will artificially send a 'MOUSE DOWN' message whenever the cursor position changes as touching/clicking is typically the only way to move the cursor
	void EnableTouchScreenHack(bool bEnable);

private:
#ifdef HY_USE_SDL2
	void DoKeyDownEvent(const SDL_Event &eventRef);
	void DoKeyUpEvent(const SDL_Event &eventRef);
	void DoMouseDownEvent(const SDL_Event &eventRef);
	void DoMouseUpEvent(const SDL_Event &eventRef);
	void DoMouseMoveEvent(const SDL_Event &eventRef);
	void SetMouseWindow(HyWindow *pWindow);
#endif
	void Update();
};

#endif /* HyInput_h__ */
