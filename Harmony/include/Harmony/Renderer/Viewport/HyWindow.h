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

#include "Renderer/Viewport/HyCamera2d.h"
#include "Renderer/Viewport/HyCamera3d.h"

#include "Threading/BasicSync.h"

struct HyMonitorDeviceInfo
{
	bool				bIsPrimaryMonitor;
	std::wstring		sDeviceName;
	std::wstring		sDeviceDescription;

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
	vector<Resolution>	vResolutions;
};

class HyWindow
{
	friend class HyScene;
	friend class IHyRenderer;

	static vector<HyMonitorDeviceInfo>	sm_vMonitorInfo;
	static BasicSection					sm_csInfo;

	HyWindowInfo						m_Info;
	
	vector<HyCamera2d *>				m_vCams2d;
	vector<HyCamera3d *>				m_vCams3d;

	enum eDirtyFlags
	{
		FLAG_Title		= 1 << 0,
		FLAG_Resolution	= 1 << 1,
		FLAG_Location	= 1 << 2,
		FLAG_Type		= 1 << 3
	};
	uint32								m_uiDirtyFlags;

	BasicSection						m_cs;

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

	HyWindowType		GetType();
	void				SetType(HyWindowType eType);

	int32				GetBitsPerPixel();
	void				SetBitsPerPixel(int32 iBitsPerPixel);

	HyCamera2d *		CreateCamera2d();
	HyCamera3d *		CreateCamera3d();

	void				RemoveCamera(HyCamera2d *&pCam);
	void				RemoveCamera(HyCamera3d *&pCam);

	static void			MonitorDeviceInfo(vector<HyMonitorDeviceInfo> &vDeviceInfoOut);

private:
	static void			SetMonitorDeviceInfo(vector<HyMonitorDeviceInfo> &info);

	void				ClearDirtyFlag();
};

#endif /* __HyWindow_h__ */
