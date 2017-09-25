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
#include "Scene/Nodes/Leafs/Misc/HyCamera.h"

struct HyMonitorDeviceInfo
{
	bool					bIsPrimaryMonitor;
	std::wstring			sDeviceName;
	std::wstring			sDeviceDescription;

	struct Resolution
	{
		int32 iWidth;
		int32 iHeight;

		Resolution(int32 iW, int32 iH) : iWidth(iW), iHeight(iH)
		{ }

		bool operator <(const Resolution &right) const
		{
			return (this->iWidth + this->iHeight) < (right.iWidth + right.iHeight);
		}

		bool operator ==(const Resolution &right) const
		{
			return this->iWidth == right.iWidth && this->iHeight == right.iHeight;
		}
	};
	std::vector<Resolution>	resolutionList;
};

class HyWindow
{
	friend class IHyRenderer;
	friend class HyScene;

	static uint32							sm_uiIdCounter;
	const uint32							m_uiID;

	HyWindowInfo							m_Info;
	glm::ivec2								m_vFramebufferSize;
	
	std::vector<HyCamera2d *>				m_Cams2dList;
	std::vector<HyCamera3d *>				m_Cams3dList;

	HyRenderSurfaceHandleInterop			m_hData;

public:
	HyWindow(const HyWindowInfo &windowInfoRef, HyRenderSurfaceHandleInterop hSharedContext);
	~HyWindow(void);

	uint32							GetId() const;

	std::string						GetTitle();
	void							SetTitle(const std::string &sTitle);

	glm::ivec2						GetWindowSize();
	void							SetWindowSize(glm::ivec2 vResolutionHint);

	glm::ivec2						GetFramebufferSize();

	glm::ivec2						GetLocation();
	void							SetLocation(glm::ivec2 ptLocation);

	HyWindowType					GetType();
	void							SetType(HyWindowType eType);

	int32							GetBitsPerPixel();
	void							SetBitsPerPixel(int32 iBitsPerPixel);

	HyCamera2d *					CreateCamera2d();
	HyCamera3d *					CreateCamera3d();

	void							RemoveCamera(HyCamera2d *&pCam);
	void							RemoveCamera(HyCamera3d *&pCam);

	glm::vec2						ConvertViewportCoordinateToWorldPos(glm::vec2 ptViewportCoordinate);

	HyRenderSurfaceHandleInterop	GetHandle();

	friend void glfw_WindowSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight);
	friend void glfw_FramebufferSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight);
	friend void glfw_WindowPosCallback(GLFWwindow *pWindow, int32 iX, int32 iY);
};

#endif /* HyWindow_h__ */
