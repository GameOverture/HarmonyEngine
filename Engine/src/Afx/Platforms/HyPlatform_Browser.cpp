/**************************************************************************
 *	HyPlatform_Browser.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "HyEngine.h"

#ifdef HY_PLATFORM_BROWSER

EM_BOOL OnHtmlResizeCallback(int iEventType, const EmscriptenUiEvent *pUiEvent, void *pUserData)
{
	glm::ivec2 vNewWindowSize(pUiEvent->windowInnerWidth, pUiEvent->windowInnerHeight);
	//HyLog("HtmlResize Callback: " << vNewWindowSize.x << ", " << vNewWindowSize.y);

	HyEngine::Window().SetWindowSize(vNewWindowSize);

	return EM_FALSE;
}

EM_BOOL OnHtmlFocusCallback(int iEventType, const EmscriptenFocusEvent *pFocusEvent, void *pUserData)
{
	HyEngine *pEngine = reinterpret_cast<HyEngine *>(pUserData);

	switch(iEventType)
	{
	case EMSCRIPTEN_EVENT_BLUR:		// Dispatched after focus has shifted from this
		HyLog("HtmlFocus Callback: Blur");
		break;

	case EMSCRIPTEN_EVENT_FOCUS:
		HyLog("HtmlFocus Callback: Focus");
		break;

	case EMSCRIPTEN_EVENT_FOCUSOUT:
		HyLog("HtmlFocus Callback: FocusOut");
		pEngine->Audio().StopDevice();
		break;

	case EMSCRIPTEN_EVENT_FOCUSIN:	// Dispatched before focus is shifted to this
		HyLog("HtmlFocus Callback: FocusIn");
		pEngine->Audio().StartDevice();
		break;

	default:
		break;
	}

	return EM_FALSE;
}

EM_BOOL OnHtmlOrientationChangeCallback(int iEventType, const EmscriptenOrientationChangeEvent *pOrientationChangeEvent, void *pUserData)
{
	HyLog("HtmlOrientationChange Callback: " << pOrientationChangeEvent->orientationIndex);

	HyEngine *pEngine = reinterpret_cast<HyEngine *>(pUserData);
	pEngine->Window().ApiRefreshWindowSize();

	return EM_FALSE;
}

EM_BOOL OnHtmlTouchCallback(int iEventType, const EmscriptenTouchEvent *pTouchEvent, void *pUserData)
{
	//pTouchEvent->ctrlKey; pTouchEvent->shiftKey; pTouchEvent->altKey; pTouchEvent->metaKey;
	HyInput *pInput = reinterpret_cast<HyInput *>(pUserData);

	for(int32 i = 0; i < pTouchEvent->numTouches; ++i)
	{
		if(pTouchEvent->touches[i].isChanged)
		{
			switch(iEventType)
			{
			case EMSCRIPTEN_EVENT_TOUCHSTART:
				//HyLog("HtmlTouch Callback: TouchStart(" << pTouchEvent->touches[i].identifier << ")");
				pInput->DoTouchStart(pTouchEvent->touches[i].identifier, pTouchEvent->touches[i].pageX, pTouchEvent->touches[i].pageY);
				break;

			case EMSCRIPTEN_EVENT_TOUCHEND:
				//HyLog("HtmlTouch Callback: TouchEnd(" << pTouchEvent->touches[i].identifier << ")");
				pInput->DoTouchEnd(pTouchEvent->touches[i].identifier, pTouchEvent->touches[i].pageX, pTouchEvent->touches[i].pageY);
				break;

			case EMSCRIPTEN_EVENT_TOUCHMOVE:
				//HyLog("HtmlTouch Callback: TouchMove(" << pTouchEvent->touches[i].identifier << ")");
				pInput->DoTouchMove(pTouchEvent->touches[i].identifier, pTouchEvent->touches[i].pageX, pTouchEvent->touches[i].pageY);
				break;

			case EMSCRIPTEN_EVENT_TOUCHCANCEL:
				//HyLog("HtmlTouch Callback: TouchCancel(" << pTouchEvent->touches[i].identifier << ")");
				pInput->DoTouchCancel(pTouchEvent->touches[i].identifier);
				break;
			}
		}
	}

	// Callback handlers that return an EM_BOOL may specify true to signal that the handler consumed
	// the event (this suppresses the default action for that event by calling its .preventDefault(); member)
	//
	// If the preventDefault method of touchstart or touchmove is called, the user agent should not dispatch
	// any mouse event that would be a consequential result of the the prevented touch event.
	return EM_TRUE;
}

#endif // HY_PLATFORM_BROWSER
