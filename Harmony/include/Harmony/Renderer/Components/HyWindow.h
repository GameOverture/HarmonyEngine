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

#include "Afx/HyStdAfx.h"
#include "Renderer/Components/HyRenderSurface.h"
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

	HyWindowInfo							m_Info;
	static std::vector<HyMonitorDeviceInfo>	sm_MonitorInfoList;
	
	std::vector<HyCamera2d *>				m_Cams2dList;
	std::vector<HyCamera3d *>				m_Cams3dList;

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

	glm::vec2			ConvertViewportCoordinateToWorldPos(glm::vec2 ptViewportCoordinate);

	static void			MonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &monitorInfoListOut);

private:
	static void			SetMonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &info);
	void Update_Render(HyRenderSurface &renderSurfaceRef);
};

#endif /* HyWindow_h__ */
