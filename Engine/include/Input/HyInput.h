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
class IHyController;
class HyGamePad;
class HyJoystick;

union HyControllerInputValue
{
	float			fAxisValue;
	HyBtnPressState eBtnValue;
};
using HyControllerConnectionFunc = std::function<void(IHyController *pController, bool bWasConnected)>;
using HyControllerInputFunc = std::function<void(IHyController *pController, bool bIsAxis, int32 iKey, HyControllerInputValue value)>;

class HyInput
{
	friend class HyEngine;
	friend class HyWindow;

	const uint32						m_uiNUM_INPUT_MAPS;
	HyInputMap *						m_pInputMaps;

	uint32								m_uiUserInputOccured;

	std::vector<HyWindow *> &			m_WindowListRef;
	HyWindow *							m_pMouseWindow;
	glm::vec2							m_ptMousePos;
	uint32								m_uiMouseBtnFlags;
	uint32								m_uiMouseBtnFlags_NewlyPressed;
	uint32								m_uiMouseBtnFlags_Buffered;
	glm::ivec2							m_vMouseScroll_LiveCount;
	glm::ivec2							m_vMouseScroll_ThisFrame;

	bool								m_bTextInputActive;
	std::string							m_sTextComposition;
	int32								m_iTextCursorIndex;
	int32								m_iTextSelectLength;

	bool								m_bTouchScreen;							// Whether the user has a touch screen instead of a mouse
	bool								m_bTouchActive;
	int32								m_iTouchId;

	bool								m_bControllerBackgroundInputEnabled;	// Whether to process GamePad/Joystick input when the window is not in focus
	HyControllerConnectionFunc			m_fpControllerConnectionCallback;		// A callback listener for when any controller is connected/disconnected
	HyControllerInputFunc				m_fpControllerInputCallback;			// A callback listener for when any controller input is detected (doesn't require to be assigned to an input map)
	std::vector<HyGamePad *>			m_GamePadList;							// Currently connected GamePads. Holds the actual dynamically allocated data
	std::vector<HyJoystick *>			m_JoystickList;							// Currently connected Joysticks. Holds the actual dynamically allocated data
	std::map<int32, IHyController *>	m_LookupControllerMap;					// Maps a controller's device ID to the controller pointer

	std::map<int, HyMouseCursorPtr>		m_LoadedCursorsMap;
	bool								m_bCursorWasSet;

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

	bool IsControllerBackgroundInput() const;			// Get whether to process GamePad/Joystick input when the window is not in focus
	void SetControllerBackgroundInput(bool bEnable);	// Set whether to process GamePad/Joystick input when the window is not in focus

	void SetControllerConnectionListener(HyControllerConnectionFunc fpConnectionCallback);
	void SetControllerInputListener(HyControllerInputFunc fpInputCallback);

	uint32 GetNumGamePads() const;
	HyGamePad *GetGamePad(uint32 uiIndex) const;

	uint32 GetNumJoysticks() const;
	HyJoystick *GetJoystick(uint32 uiIndex) const;

	std::vector<IHyController *> GetAssignedControllers(uint32 uiMappingIndex = 0) const;
	void AssignGamePad(HyGamePad *pGamePad, uint32 uiMappingIndex = 0);
	void RemoveGamePad(uint32 uiMappingIndex = 0);
	void AssignJoystick(HyJoystick *pJoystick, uint32 uiMappingIndex = 0);
	void RemoveJoystick(HyJoystick *pJoystick, uint32 uiMappingIndex = 0);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check for Input

	bool IsActionDown(int32 iUserId, uint32 uiMappingIndex = 0) const;
	bool IsActionReleased(int32 iUserId, uint32 uiMappingIndex = 0) const;	// Only true for a single frame upon button release
	float GetGamePadAxis(HyGamePadAxis eAxis, uint32 uiMappingIndex = 0) const;

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
	void AllocateController(int32 iDeviceIndex);
	void RemoveController(int32 iId);

#ifdef HY_USE_GLFW
	friend void HyGlfw_MouseButtonCallback(GLFWwindow *pWindow, int32 iButton, int32 iAction, int32 iMods);
	friend void HyGlfw_CursorPosCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void HyGlfw_ScrollCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void HyGlfw_KeyCallback(GLFWwindow *pWindow, int32 iKey, int32 iScancode, int32 iAction, int32 iMods);
	friend void HyGlfw_CharCallback(GLFWwindow *pWindow, uint32 uiCodepoint);
	friend void HyGlfw_JoystickCallback(int32 iJoyId, int32 iEvent);

	void OnGlfwKey(int32 iKey, int32 iAction);
	void GlfwUpdateControllers();														// Update all assigned controllers (GamePads/Joysticks). Dispatches calls to OnEventGamePadAxis/OnEventGamePadButton
	void GlfwProcessGamePad(HyGamePad *pGamePad, GLFWgamepadstate &gamePadStateRef);	// Use GlfwUpdateControllers() instead of GlfwProcessGamePad() directly
	void GlfwProcessJoystick(HyJoystick *pJoystick);									// Use GlfwUpdateControllers() instead of GlfwProcessJoystick() directly
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
	void DoTouchDownEvent(const SDL_Event &eventRef);
	void DoTouchMoveEvent(const SDL_Event &eventRef);
	void DoTouchUpEvent(const SDL_Event &eventRef);
#elif defined(HY_PLATFORM_GUI)
	void SetWidgetMousePos(glm::vec2 ptMousePos);
#endif

	// All controller input is fed through these ApplyController* functions
	void ApplyControllerAxis(int32 iId, HyGamePadAxis eAxis, float fAxisValue);
	void ApplyControllerButton(int32 iId, HyGamePadBtn eButtonType, HyBtnPressState ePressState);

	void DistrubuteTextInput(std::string sNewText);
	void DistrubuteKeyboardInput(HyKeyboardBtn eBtn);

	void Update();		// This update occurs AFTER event polls are processed
};

#endif /* HyInput_h__ */
