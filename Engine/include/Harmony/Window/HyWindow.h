/**************************************************************************
 *	HyWindow.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyWindow_h__
#define HyWindow_h__

#include "Afx/HyInteropAfx.h"
#include "Scene/Nodes/Objects/HyCamera.h"

class HyInput;

class HyWindow
{
	friend class IHyRenderer;
	friend class HyScene;

	const uint32							m_uiINDEX;
	uint32									m_uiId;

	HyWindowInfo							m_Info;
	glm::ivec2								m_vFramebufferSize;
	
	std::vector<HyCamera2d *>				m_Cams2dList;
	std::vector<HyCamera3d *>				m_Cams3dList;

	HyWindowInteropPtr						m_pInterop;

public:
	class CameraIterator2d
	{
		const std::vector<HyCamera2d *> &			m_CamsListRef;
		std::vector<HyCamera2d *>::const_iterator	m_iter;

	public:
		CameraIterator2d(const std::vector<HyCamera2d *> &camListRef) :
			m_CamsListRef(camListRef),
			m_iter(m_CamsListRef.begin())
		{
			Reset();
		}

		CameraIterator2d(CameraIterator2d &iterRef) :
			m_CamsListRef(iterRef.m_CamsListRef),
			m_iter(m_CamsListRef.begin())
		{
			Reset();
		}

		CameraIterator2d &operator++()	// Prefix increment operator
		{
			do
			{
				++m_iter;
			} while(m_iter != m_CamsListRef.end() && (*m_iter)->IsVisible() == false);

			return *this;
		}

		bool IsEnd()		{ return m_iter == m_CamsListRef.end(); }
		HyCamera2d *Get()	{ return *m_iter; }
		void Reset()
		{
			m_iter = m_CamsListRef.begin();
			while(m_iter != m_CamsListRef.end() && (*m_iter)->IsVisible() == false)
				++m_iter;
		}
	};

public:
	HyWindow(uint32 uiIndex, const HyWindowInfo &windowInfoRef, bool bShowCursor, HyWindowInteropPtr hSharedContext);
	~HyWindow(void);

	uint32								GetIndex() const;
	uint32								GetId() const;

	std::string							GetTitle();
	void								SetTitle(const std::string &sTitle);

	int32								GetWidth();
	int32								GetHeight();
	float								GetWidthF(float fPercent = 1.0f);
	float								GetHeightF(float fPercent = 1.0f);
	glm::ivec2							GetWindowSize() const;
	void								SetWindowSize(glm::ivec2 vSizeHint);
	glm::ivec2							GetLocation();
	void								SetLocation(glm::ivec2 ptLocation);

	glm::ivec2							GetFramebufferSize() const;
	void								SetFramebufferSize(glm::ivec2 vDrawableSize);

	HyCamera2d *						CreateCamera2d();
	HyCamera3d *						CreateCamera3d();

	uint32								GetNumCameras2d();
	HyCamera2d *						GetCamera2d(uint32 uiIndex);
	const std::vector<HyCamera2d *> &	GetCamera2dList();

	uint32								GetNumCameras3d();
	HyCamera3d *						GetCamera3d(uint32 uiIndex);

	void								RemoveCamera(HyCamera2d *&pCam);
	void								RemoveCamera(HyCamera3d *&pCam);

	bool								ProjectToWorldPos2d(const glm::vec2 &ptWindowCoordinate, glm::vec2 &ptWorldPosOut) const;
	bool								ProjectToWindow2d(const glm::vec2 &ptWorldPos, glm::vec2 &ptWindowCoordinateOut) const;

	HyWindowInteropPtr					GetInterop();

	HyWindowMode						GetMode();
	void								SetMode(HyWindowMode eMode);

#ifdef HY_USE_GLFW
	// Returns the monitor this window is currently associated with.
	// (Determined by the monitor closest to window's center)
	GLFWmonitor *						GetGlfwMonitor();

	friend void HyGlfw_WindowSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight);
	friend void HyGlfw_FramebufferSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight);
	friend void HyGlfw_WindowPosCallback(GLFWwindow *pWindow, int32 iX, int32 iY);
#elif defined(HY_USE_SDL2)
	void DoEvent(const SDL_Event &eventRef, HyInput &inputRef);
#endif
};

#endif /* HyWindow_h__ */
