/**************************************************************************
 *	HyWindow.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Window/HyWindow.h"
#include "Input/HyInput.h"

#ifdef HY_USE_GLFW
	void HyGlfw_WindowSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight)
	{
		HyWindow *pHyWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		pHyWindow->m_Info.vSize.x = iWidth;
		pHyWindow->m_Info.vSize.y = iHeight;
	}

	void HyGlfw_FramebufferSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight)
	{
		HyWindow *pHyWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		pHyWindow->m_vFramebufferSize.x = iWidth;
		pHyWindow->m_vFramebufferSize.y = iHeight;
	}

	void HyGlfw_WindowPosCallback(GLFWwindow *pWindow, int32 iX, int32 iY)
	{
		HyWindow *pHyWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		pHyWindow->m_Info.ptLocation.x = iX;
		pHyWindow->m_Info.ptLocation.y = iY;
	}
#endif

HyWindow::HyWindow(uint32 uiIndex, const HyWindowInfo &windowInfoRef, bool bShowCursor, HyWindowInteropPtr hSharedContext) :
	m_uiINDEX(uiIndex),
	m_uiId(m_uiINDEX),
	m_pInterop(nullptr)
{
	m_Info = windowInfoRef;
	m_vFramebufferSize = m_Info.vSize;

#if defined(HY_USE_GLFW)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	GLFWmonitor *pMonitorOwner = nullptr;
	switch(m_Info.eType)
	{
	case HYWINDOW_WindowedSizeable:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		break;
	case HYWINDOW_WindowedFixed:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		break;
	case HYWINDOW_BorderlessWindow:
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		break;
	case HYWINDOW_FullScreen:
		pMonitorOwner = GetGlfwMonitor();
		break;
	}
	
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);		// Will be shown after positioned

	m_pInterop = glfwCreateWindow(static_cast<int32>(m_Info.vSize.x),
							   static_cast<int32>(m_Info.vSize.y),
							   m_Info.sName.c_str(),
							   pMonitorOwner,
							   hSharedContext);
	if(m_pInterop == nullptr)
	{
		HyLogError("HyWindow ctor - glfwCreateWindow returned nullptr (At least OpenGL 3.1 is required, or window or OpenGL context creation failed)");
		return;
	}

	int32 iFrameBufferX, iFrameBufferY;
	glfwGetFramebufferSize(m_pInterop, &iFrameBufferX, &iFrameBufferY);
	m_vFramebufferSize.x = iFrameBufferX;
	m_vFramebufferSize.y = iFrameBufferY;

	glfwSetWindowUserPointer(m_pInterop, this);
	glfwSetWindowPos(m_pInterop, m_Info.ptLocation.x, m_Info.ptLocation.y);

#ifndef HY_PLATFORM_BROWSER
	if(bShowCursor == false)
		glfwSetInputMode(m_pInterop, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
#endif

	glfwShowWindow(m_pInterop);

	// Set callbacks
	glfwSetWindowSizeCallback(m_pInterop, HyGlfw_WindowSizeCallback);
	glfwSetFramebufferSizeCallback(m_pInterop, HyGlfw_FramebufferSizeCallback);
	glfwSetWindowPosCallback(m_pInterop, HyGlfw_WindowPosCallback);
#elif defined(HY_USE_SDL2)
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	#ifndef HY_PLATFORM_BROWSER
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	#else // Browsers set version for OpenGL ES 3.0 (ES is implicit with Emscripten? SDL_GL_CONTEXT_PROFILE_MASK not available in Emscripten's SDL2)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	#endif

	uint32 uiWindowFlags = SDL_WINDOW_OPENGL;
	switch(m_Info.eType)
	{
	case HYWINDOW_WindowedSizeable:
		uiWindowFlags |= SDL_WINDOW_RESIZABLE;
		break;
	case HYWINDOW_WindowedFixed:
		break;
	case HYWINDOW_BorderlessWindow:
		uiWindowFlags |= SDL_WINDOW_BORDERLESS;
		break;
	case HYWINDOW_FullScreen:
		uiWindowFlags |= SDL_WINDOW_FULLSCREEN;
		break;
	}

	m_pInterop = SDL_CreateWindow(m_Info.sName.c_str(),
								  m_Info.ptLocation.x, m_Info.ptLocation.y,//SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								  static_cast<int32>(m_Info.vSize.x), static_cast<int32>(m_Info.vSize.y),
								  uiWindowFlags);
	if(m_pInterop == nullptr) {
		HyLogError("SDL_CreateWindow failed: " << SDL_GetError());
	}

	int32 iFrameBufferX, iFrameBufferY;
#ifndef HY_PLATFORM_BROWSER // SDL_GL_GetDrawableSize not supported with Emscripten's SDL2
	SDL_GL_GetDrawableSize(m_pInterop, &iFrameBufferX, &iFrameBufferY);
#else
	iFrameBufferX = m_Info.vSize.x;
	iFrameBufferY = m_Info.vSize.y;
#endif

	m_vFramebufferSize.x = iFrameBufferX;
	m_vFramebufferSize.y = iFrameBufferY;

	m_uiId = SDL_GetWindowID(m_pInterop);

	//SDL_SetWindowData(m_pInterop, "class", this);
#endif
}

HyWindow::~HyWindow(void)
{
	while(m_Cams2dList.empty() == false)
		RemoveCamera(m_Cams2dList.back());

	while(m_Cams3dList.empty() == false)
		RemoveCamera(m_Cams3dList.back());

#if defined(HY_USE_GLFW)
	glfwDestroyWindow(m_pInterop);
#elif defined(HY_USE_SDL2)
	SDL_DestroyWindow(m_pInterop);
#endif
}

uint32 HyWindow::GetIndex() const
{
	return m_uiINDEX;
}

uint32 HyWindow::GetId() const
{
	return m_uiId;
}

std::string HyWindow::GetTitle()
{
	return m_Info.sName;
}

void HyWindow::SetTitle(const std::string &sTitle)
{
	m_Info.sName = sTitle;

#ifdef HY_USE_GLFW
	glfwSetWindowTitle(m_pInterop, m_Info.sName.c_str());
#endif
}

int32 HyWindow::GetWidth()
{
	return m_Info.vSize.x;
}

int32 HyWindow::GetHeight()
{
	return m_Info.vSize.y;
}

float HyWindow::GetWidthF(float fPercent /*= 1.0f*/)
{
	return static_cast<float>(m_Info.vSize.x) * fPercent;
}

float HyWindow::GetHeightF(float fPercent /*= 1.0f*/)
{
	return static_cast<float>(m_Info.vSize.y) * fPercent;
}

glm::ivec2 HyWindow::GetWindowSize()
{
	return m_Info.vSize;
}

void HyWindow::SetWindowSize(glm::ivec2 vSizeHint)
{
	if(GetWidth() == vSizeHint.x && GetHeight() == vSizeHint.y)
		return;

#if defined(HY_USE_GLFW)
	glfwSetWindowSize(m_pInterop, vSizeHint.x, vSizeHint.y);
#elif defined(HY_USE_SDL2)
	SDL_SetWindowSize(m_pInterop, vSizeHint.x, vSizeHint.y);
#else
	HyLog("Window " << m_uiINDEX << " Resized: " << vSizeHint.x << ", " << vSizeHint.y);
	m_Info.vSize = vSizeHint;
#endif
}

glm::ivec2 HyWindow::GetFramebufferSize() const
{
	return m_vFramebufferSize;
}

void HyWindow::SetFramebufferSize(glm::ivec2 vBufferSize)
{
	m_vFramebufferSize = vBufferSize;
	HyLog("Window " << m_uiINDEX << " Framebuffer: " << m_vFramebufferSize.x << ", " << m_vFramebufferSize.y);
}

glm::ivec2 HyWindow::GetLocation()
{
	return m_Info.ptLocation;
}

void HyWindow::SetLocation(glm::ivec2 ptLocation)
{
#if defined(HY_USE_GLFW)
	glfwSetWindowPos(m_pInterop, m_Info.ptLocation.x, m_Info.ptLocation.y);
#elif defined(HY_USE_SDL2)
	SDL_SetWindowPosition(m_pInterop, ptLocation.x, ptLocation.y);
#else
	m_Info.ptLocation = ptLocation;
#endif
}

HyCamera2d *HyWindow::CreateCamera2d()
{
	HyCamera2d *pNewCam = HY_NEW HyCamera2d(this);
	m_Cams2dList.push_back(pNewCam);

	return pNewCam;
}

HyCamera3d *HyWindow::CreateCamera3d()
{
	HyCamera3d *pNewCam = HY_NEW HyCamera3d(this);
	m_Cams3dList.push_back(pNewCam);

	return pNewCam;
}

uint32 HyWindow::GetNumCameras2d()
{
	return static_cast<uint32>(m_Cams2dList.size());
}

HyCamera2d *HyWindow::GetCamera2d(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_Cams2dList.size()), "HyWindow::GetCamera2d was passed an invalid index: " << uiIndex);
	return m_Cams2dList[uiIndex];
}

const std::vector<HyCamera2d *> &HyWindow::GetCamera2dList()
{
	return m_Cams2dList;
}

uint32 HyWindow::GetNumCameras3d()
{
	return static_cast<uint32>(m_Cams3dList.size());
}

HyCamera3d *HyWindow::GetCamera3d(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_Cams3dList.size()), "HyWindow::GetCamera3d was passed an invalid index: " << uiIndex);
	return m_Cams3dList[uiIndex];
}

void HyWindow::RemoveCamera(HyCamera2d *&pCam)
{
	for(std::vector<HyCamera2d *>::iterator iter = m_Cams2dList.begin(); iter != m_Cams2dList.end(); ++iter)
	{
		if((*iter) == pCam)
		{
			delete pCam;
			m_Cams2dList.erase(iter);

			return;
		}
	}
}

void HyWindow::RemoveCamera(HyCamera3d *&pCam)
{
	for(std::vector<HyCamera3d *>::iterator iter = m_Cams3dList.begin(); iter != m_Cams3dList.end(); ++iter)
	{
		if((*iter) == pCam)
		{
			delete pCam;
			m_Cams3dList.erase(iter);

			return;
		}
	}
}

glm::vec2 HyWindow::ConvertViewportCoordinateToWorldPos(glm::vec2 ptViewportCoordinate)
{
	// Y-axis goes up when working with world coordinates
	ptViewportCoordinate.y = GetWindowSize().y - ptViewportCoordinate.y;

	// Convert to normalized [0.0 - 1.0]
	ptViewportCoordinate /= GetWindowSize();

	glm::vec2 ptWorldPos(0.0f);

	// Find the first camera that encompasses this normalized point in the viewport
	for(uint32 i = 0; i < m_Cams2dList.size(); ++i)
	{
		const HyRectangle<float> &viewportRect = m_Cams2dList[i]->GetViewport();

		if(ptViewportCoordinate.x >= viewportRect.left   && ptViewportCoordinate.x <= viewportRect.right &&
		   ptViewportCoordinate.y >= viewportRect.bottom && ptViewportCoordinate.y <= viewportRect.top)
		{
			// Find local coordinate in found camera's viewport
			glm::vec2 vOffsetInViewport;
			vOffsetInViewport.x = (ptViewportCoordinate.x - viewportRect.left) / (viewportRect.right - viewportRect.left);
			vOffsetInViewport.y = (ptViewportCoordinate.y - viewportRect.bottom) / (viewportRect.top - viewportRect.bottom);

			// Now using the found camera's transformation convert to the world position
			const b2AABB &aabbWorldRef = m_Cams2dList[i]->GetWorldViewBounds();
			
			float fDeg = m_Cams2dList[i]->rot.Get();

			if(fDeg != 0.0f)
			{
				// TODO: Calculate world vector orientation
				HyError("ConvertViewportCoordinateToWorldPos() TODO: Calculate world vector orientation for camera");

				glm::vec2 vOrientation(cos(glm::radians(fDeg)), sin(glm::radians(fDeg)));
			}
			else
			{
				//const b2AABB &aabbWorldRef = m_Cams2dList[i]->GetWorldAABB();

				ptWorldPos.x = (aabbWorldRef.upperBound.x - aabbWorldRef.lowerBound.x) * vOffsetInViewport.x;
				ptWorldPos.x += aabbWorldRef.lowerBound.x;

				ptWorldPos.y = (aabbWorldRef.upperBound.y - aabbWorldRef.lowerBound.y) * vOffsetInViewport.y;
				ptWorldPos.y += aabbWorldRef.lowerBound.y;
			}

			break;
		}
	}

	return ptWorldPos;
}

HyWindowInteropPtr HyWindow::GetInterop()
{
	return m_pInterop;
}

bool HyWindow::IsFullScreen()
{
#ifdef HY_USE_GLFW
	return glfwGetWindowMonitor(m_pInterop) != nullptr;
#elif defined(HY_USE_SDL2)
	SDL_DisplayMode windowMode, desktopMode;
	SDL_GetWindowDisplayMode(m_pInterop, &windowMode);
	SDL_GetDesktopDisplayMode(0, &desktopMode);

	return (windowMode.h == desktopMode.h && windowMode.w == desktopMode.w);
#else
	HyLogWarning("HyWindow::IsFullScreen is not implemented for this build configuration");
	return false;
#endif
}

void HyWindow::SetFullScreen(bool bFullScreen)
{
#ifdef HY_USE_GLFW
	glfwSetWindowMonitor(m_pInterop, bFullScreen ? GetGlfwMonitor() : nullptr, m_Info.ptLocation.x, m_Info.ptLocation.y, m_Info.vSize.x, m_Info.vSize.y, GLFW_DONT_CARE);
#elif defined(HY_USE_SDL2) && !defined(HY_PLATFORM_BROWSER) // SDL_SetWindowFullscreen not supported with Emscripten's SDL2
	SDL_SetWindowFullscreen(m_pInterop, bFullScreen ? SDL_WINDOW_FULLSCREEN : 0);
#else
	HyLogWarning("HyWindow::SetFullScreen is not implemented for this build configuration");
#endif
}


#ifdef HY_USE_GLFW
GLFWmonitor *HyWindow::GetGlfwMonitor()
{
	GLFWmonitor *pClosestMonitor = nullptr;

	glm::vec2 ptWindowCenter = glm::vec2(m_Info.ptLocation.x, m_Info.ptLocation.y) + (glm::vec2(m_Info.vSize.x, m_Info.vSize.y) / 2.0f);
	float fClosestDist = 999999999.0f;

	int iNumMonitors;
	GLFWmonitor **ppMonitors = glfwGetMonitors(&iNumMonitors);
	for(int i = 0; i < iNumMonitors; ++i)
	{
		int32 iX, iY;
		glfwGetMonitorPos(ppMonitors[i], &iX, &iY);
		const GLFWvidmode *pVidMode = glfwGetVideoMode(ppMonitors[i]);

		glm::vec2 ptMonitorCenter = glm::vec2(iX, iY) + (glm::vec2(pVidMode->width, pVidMode->height) / 2.0f);

		float fDist = glm::distance(ptWindowCenter, ptMonitorCenter);
		if(fClosestDist > fDist)
		{
			pClosestMonitor = ppMonitors[i];
			fClosestDist = fDist;
		}
	}

	return pClosestMonitor;
}
#elif defined(HY_USE_SDL2)
void HyWindow::DoEvent(const SDL_Event &eventRef, HyInput &inputRef)
{
	switch(eventRef.window.event)
	{
	case SDL_WINDOWEVENT_SHOWN:
		HyLog("Window " << m_uiId << " shown");
		break;
	case SDL_WINDOWEVENT_HIDDEN:
		HyLog("Window " << m_uiId << " hidden");
		break;
	case SDL_WINDOWEVENT_EXPOSED:
		// Indicates to be redrawn
		break;
	case SDL_WINDOWEVENT_MOVED:
		m_Info.ptLocation.x = eventRef.window.data1;
		m_Info.ptLocation.y = eventRef.window.data2;
		break;
	case SDL_WINDOWEVENT_RESIZED:
		// Indicating an external event, i.e. the user or the window manager
		break;
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		HyLog("Window " << m_uiINDEX << " Resized: " << eventRef.window.data1 << ", " << eventRef.window.data2);
		HySetVec(m_Info.vSize, eventRef.window.data1, eventRef.window.data2);
		break;
	case SDL_WINDOWEVENT_MINIMIZED:
		HyLog("Window " << m_uiId << " minimized");
		break;
	case SDL_WINDOWEVENT_MAXIMIZED:
		HyLog("Window " << m_uiId << " maximized");
		break;
	case SDL_WINDOWEVENT_RESTORED:
		HyLog("Window " << m_uiId << " restored");
		break;
	case SDL_WINDOWEVENT_ENTER:
		//HyLog("Mouse entered window " << m_uiId);
		inputRef.SetMouseWindow(this);
		break;
	case SDL_WINDOWEVENT_LEAVE:
		//HyLog("Mouse left window " << m_uiId);
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:
		HyLog("Window " << m_uiId << " gained keyboard focus");
		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:
		HyLog("Window " << m_uiId << " lost keyboard focus");
		break;
	case SDL_WINDOWEVENT_CLOSE:
		HyLog("Window " << m_uiId << " closed");
		break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
	case SDL_WINDOWEVENT_TAKE_FOCUS:
		HyLog("Window " << m_uiId << " is offered a focus");
		break;
	case SDL_WINDOWEVENT_HIT_TEST:
		HyLog("Window " << m_uiId << " has a special hit test");
		break;
#endif
	default:
		HyLogError("Window " << m_uiId << " got unknown event " << eventRef.window.event);
		break;
	}
}
#endif
