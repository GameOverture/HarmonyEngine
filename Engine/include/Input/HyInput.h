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

#include "Afx/HyInteropAfx.h"
#include "Input/HyInputMap.h"

class HyWindow;

class HyInput
{
	friend class HyEngine;
	friend class HyWindow;

	const uint32					m_uiNUM_INPUT_MAPS;
	HyInputMap *					m_pInputMaps;

	uint32							m_uiUserInputOccured;

	std::vector<HyWindow *> &		m_WindowListRef;
	HyWindow *						m_pMouseWindow;
	glm::vec2						m_ptMousePos;
	uint32							m_uiMouseBtnFlags;
	uint32							m_uiMouseBtnFlags_NewlyPressed;
	uint32							m_uiMouseBtnFlags_Buffered;
	glm::ivec2						m_vMouseScroll_LiveCount;
	glm::ivec2						m_vMouseScroll_ThisFrame;

	bool							m_bTextInputActive;
	std::string						m_sTextComposition;
	int32							m_iTextCursorIndex;
	int32							m_iTextSelectLength;

	bool							m_bTouchScreen;	// Whether the user has a touch screen instead of a mouse
	bool							m_bTouchActive;
	int32							m_iTouchId;

	int32							m_JoystickList[HYNUM_JOYSTICK];
	uint32							m_uiJoystickCount;

	std::map<int, HyMouseCursorPtr>	m_LoadedCursorsMap;
	bool							m_bCursorWasSet;

public:
	HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef);
	~HyInput();

	bool UserInputOccured() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mouse

	// All clicks are guaranteed to be down for at least '1' frame, even if it is released before the next update.
	// Clicks released and then pressed down again within one frame will not be detected.
	bool IsMouseBtnDown(HyMouseBtn eBtn) const;
	uint32 GetMouseWindowIndex() const;
	glm::vec2 GetMousePos() const;
	bool GetWorldMousePos(glm::vec2 &ptWorldPosOut) const;
	glm::ivec2 GetMouseScroll() const;

	// Change the mouse cursor icon
	void SetMouseCursor(HyMouseCursor eCursor);
	void ResetMouseCursor();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup Button/Axis Mapping

	// Categorize actions (game, UI, etc.) so the same physical button can be used for multiple mappings.
	void SetActionCategory(int32 iActionId, uint8 uiCategory, uint32 uiMappingIndex = 0);

	// Returns -1, or the mapping 'eBtn' used to be assigned to
	int32 MapBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex = 0);
	int32 MapBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex = 0);
	int32 MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex = 0);
	int32 MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex = 0);

	bool MapGamePadBtn(int32 iActionId, HyGamePadBtn eBtn, uint32 uiMappingIndex = 0);

	bool Unmap(int32 iActionId, uint32 uiMappingIndex = 0);
	bool IsMapped(int32 iActionId, uint32 uiMappingIndex = 0) const;

	bool AssignGamePadIndex(int32 iGamePadIndex, uint32 uiMappingIndex = 0);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check for Input
	bool IsActionDown(int32 iUserId, uint32 uiMappingIndex = 0) const;
	bool IsActionReleased(int32 iUserId, uint32 uiMappingIndex = 0) const;	// Only true for a single frame upon button release
	float GetGamePadAxis(HyGamePadAxis eAxis, uint32 uiMappingIndex = 0) const;
	float GetGamePadAxisDelta(HyGamePadAxis eAxis, uint32 uiMappingIndex = 0) const;

	bool IsTextInputActive();
	void StartTextInput();
	void StopTextInput();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Replay API

	void RecordingStart();
	void RecordingStop();

	void PlaybackStart();
	void PlaybackStop();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Touch Screen

	void UsingTouchScreen(bool bEnable);
	bool IsUsingTouchScreen() const;

	void DoTouchStart(int32 iId, int32 iX, int32 iY);
	void DoTouchMove(int32 iId, int32 iX, int32 iY);
	void DoTouchEnd(int32 iId, int32 iX, int32 iY);
	void DoTouchCancel(int32 iId);

private:
#ifdef HY_USE_GLFW
	friend void HyGlfw_MouseButtonCallback(GLFWwindow *pWindow, int32 iButton, int32 iAction, int32 iMods);
	friend void HyGlfw_CursorPosCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void HyGlfw_ScrollCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void HyGlfw_KeyCallback(GLFWwindow *pWindow, int32 iKey, int32 iScancode, int32 iAction, int32 iMods);
	friend void HyGlfw_CharCallback(GLFWwindow *pWindow, uint32 uiCodepoint);
	friend void HyGlfw_JoystickCallback(int32 iJoyId, int32 iEvent);

	void OnGlfwKey(int32 iKey, int32 iAction);
	void UpdateGlfwGamepads();
#elif defined(HY_USE_SDL2)
	void DoKeyDownEvent(const SDL_Event &eventRef);
	void DoKeyUpEvent(const SDL_Event &eventRef);
	void DoTextInputEvent(const SDL_Event &eventRef);		// Add new text onto the end of our text
	void DoTextEditEvent(const SDL_Event &eventRef);		// Update the composition text; Update the cursor position; Update the selection length
	void DoMouseMoveEvent(const SDL_Event &eventRef);
	void DoMouseDownEvent(const SDL_Event &eventRef);
	void DoMouseUpEvent(const SDL_Event &eventRef);
	void DoMouseWheelEvent(const SDL_Event &eventRef);
	void SetMouseWindow(HyWindow *pWindow);
	void DoPadAxis(const SDL_Event &eventRef);
	void DoPadBtnDown(const SDL_Event &eventRef);
	void DoPadBtnUp(const SDL_Event &eventRef);
	void DoTouchDownEvent(const SDL_Event &eventRef);
	void DoTouchMoveEvent(const SDL_Event &eventRef);
	void DoTouchUpEvent(const SDL_Event &eventRef);
#elif defined(HY_PLATFORM_GUI)
	void SetWidgetMousePos(glm::vec2 ptMousePos);
#endif

	void DistrubuteTextInput(std::string sNewText);
	void DistrubuteKeyboardInput(HyKeyboardBtn eBtn);

	void Update();		// This update occurs AFTER event polls are processed
};

#endif /* HyInput_h__ */
