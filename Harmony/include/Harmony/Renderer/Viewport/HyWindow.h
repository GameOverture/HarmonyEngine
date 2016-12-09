/**************************************************************************
 *	HyWindow.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyWindow_h__
#define __HyWindow_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/Viewport/HyCamera.h"

#include "Threading/BasicSync.h"

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
	std::vector<Resolution>	vResolutions;
};

// TODO: Make this class threadsafe between game app and renderer
class HyWindow
{
	friend class HyScene;
	friend class IHyRenderer;

	static std::vector<HyMonitorDeviceInfo>	sm_MonitorInfoList;
	static BasicSection						sm_csInfo;

	HyWindowInfo							m_Info_Update;	// On the application's update thread
	HyWindowInfo							m_Info_Shared;
	HyWindowInfo							m_Info_Render;	// Read-only contents for render thread
	
	std::vector<HyCamera2d *>				m_Cams2dList;
	std::vector<HyCamera3d *>				m_Cams3dList;

	bool									m_bTakeInput;

	BasicSection							m_cs;

public:
	HyWindow();
	~HyWindow(void);

	const HyWindowInfo &GetWindowInfo();

	std::string			GetTitle();
	void				SetTitle(std::string sTitle);

	glm::ivec2			GetResolution();
	void				SetResolution(glm::ivec2 vResolution);

	glm::ivec2			GetLocation();
	void				SetLocation(glm::ivec2 ptLocation);

	bool				IsInputEnabled();
	void				SetInputEnabled(bool bEnabled);

	HyWindowType		GetType();
	void				SetType(HyWindowType eType);

	int32				GetBitsPerPixel();
	void				SetBitsPerPixel(int32 iBitsPerPixel);

	HyCamera2d *		CreateCamera2d();
	HyCamera3d *		CreateCamera3d();

	void				RemoveCamera(HyCamera2d *&pCam);
	void				RemoveCamera(HyCamera3d *&pCam);

	static void			MonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &vDeviceInfoOut);

private:
	static void			SetMonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &info);

	void				Update();

	HyWindowInfo &		Update_Render();	// Only invoked on the render thread
};

#endif /* __HyWindow_h__ */
