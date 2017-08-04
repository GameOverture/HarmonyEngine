/**************************************************************************
 *	HyInput.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyInput_h__
#define HyInput_h__

#include "Afx/HyStdAfx.h"
#include "Input/IHyInputMap.h"
#include "Threading/Threading.h"

class IHyDraw2d;
class HyWindow;
class HyGfxComms;

class IHyInput
{
protected:
	const uint32				m_uiNUM_INPUT_MAPS;
	IHyInputMap *				m_pInputMaps;

	std::vector<HyWindow *> &	m_WindowListRef;

	uint32						m_uiMouse_WindowIndex;
	glm::vec2					m_ptMouse_CurNormalizedPos;
	bool						m_bMouse_LeftBtnDown;
	bool						m_bMouse_RightBtnDown;

public:
	IHyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef);
	virtual ~IHyInput();

	IHyInputMap *GetInputMapArray();

	glm::vec2 GetWorldMousePos();
	bool IsMouseLeftDown();
	bool IsMouseRightDown();

	virtual void StartRecording() = 0;
	virtual void StopRecording() = 0;
	virtual void SerializeRecording() = 0;

	virtual void StartPlayback() = 0;
	virtual void StopPlayback() = 0;

	virtual void SetWindowIndex(uint32 uiCurrentWindowIndex) = 0;
	virtual void HandleMsg(void *pMsg) = 0;

	virtual void Update() = 0;
};

#endif /* HyInput_h__ */
