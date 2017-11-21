/**************************************************************************
 *	HyWindow.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyWindow_h__
#define HyWindow_h__

#include "Afx/HyInteropAfx.h"
#include "Scene/Nodes/Misc/HyCamera.h"

class HyWindow
{
	friend class IHyRenderer;
	friend class HyScene;

	const uint32							m_uiINDEX;

	HyWindowInfo							m_Info;
	glm::ivec2								m_vFramebufferSize;
	
	std::vector<HyCamera2d *>				m_Cams2dList;
	std::vector<HyCamera3d *>				m_Cams3dList;

	HyWindowHandle							m_hData;

public:
	class CameraIterator2d
	{
		const std::vector<HyCamera2d *> &			m_CamsListRef;
		std::vector<HyCamera2d *>::const_iterator	m_iter;

	public:
		CameraIterator2d(const std::vector<HyCamera2d *> &camListRef) :	m_CamsListRef(camListRef),
																		m_iter(m_CamsListRef.begin())
		{
			while(m_iter != m_CamsListRef.end() && (*m_iter)->IsEnabled() == false)
				++m_iter;
		}

		CameraIterator2d(CameraIterator2d &iterRef) :	m_CamsListRef(iterRef.m_CamsListRef),
														m_iter(m_CamsListRef.begin())
		{
			while(m_iter != m_CamsListRef.end() && (*m_iter)->IsEnabled() == false)
				++m_iter;
		}

		CameraIterator2d &operator++()	// Prefix increment operator
		{
			do 
			{
				++m_iter;
			} while(m_iter != m_CamsListRef.end() && (*m_iter)->IsEnabled() == false);

			return *this;
		}

		CameraIterator2d operator++(int)	// Postfix increment operator
		{
			do 
			{
				++m_iter;
			} while(m_iter != m_CamsListRef.end() && (*m_iter)->IsEnabled() == false);

			return *this;
		}

		bool IsEnd()		{ return m_iter == m_CamsListRef.end(); }
		HyCamera2d *Get()	{ return *m_iter; }
	};

public:
	HyWindow(uint32 uiIndex, const HyWindowInfo &windowInfoRef, bool bShowCursor, HyWindowHandle hSharedContext);
	~HyWindow(void);

	uint32							GetIndex() const;

	std::string						GetTitle();
	void							SetTitle(const std::string &sTitle);

	glm::ivec2						GetWindowSize();
	void							SetWindowSize(glm::ivec2 vResolutionHint);

	glm::ivec2						GetFramebufferSize();

	glm::ivec2						GetLocation();
	void							SetLocation(glm::ivec2 ptLocation);

	HyCamera2d *					CreateCamera2d();
	HyCamera3d *					CreateCamera3d();

	uint32							GetNumCameras2d();
	HyCamera2d *					GetCamera2d(uint32 uiIndex);
	const std::vector<HyCamera2d *> &GetCamera2dList();

	uint32							GetNumCameras3d();
	HyCamera3d *					GetCamera3d(uint32 uiIndex);

	void							RemoveCamera(HyCamera2d *&pCam);
	void							RemoveCamera(HyCamera3d *&pCam);

	glm::vec2						ConvertViewportCoordinateToWorldPos(glm::vec2 ptViewportCoordinate);

	HyWindowHandle					GetHandle();

#ifdef HY_PLATFORM_DESKTOP
	// Returns the monitor this window is currently associated with.
	// (Determined by the monitor closest to window's center)
	GLFWmonitor *					GetGlfwMonitor();

	bool							IsFullScreen();
	void							SetFullScreen(bool bFullScreen);

	friend void glfw_WindowSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight);
	friend void glfw_FramebufferSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight);
	friend void glfw_WindowPosCallback(GLFWwindow *pWindow, int32 iX, int32 iY);
#endif
};

#endif /* HyWindow_h__ */
