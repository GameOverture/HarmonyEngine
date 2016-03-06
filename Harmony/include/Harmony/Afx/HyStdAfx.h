/**************************************************************************
 *	HyStdAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyStdAfx_h__
#define __HyStdAfx_h__

// Order of these #includes matter!
#include "Compilers/HyCompiler.h"
#include "Platforms/HyPlatform.h"

#define HY_TEMP_TEXTBUFFER_SIZE 1024

enum HyInstanceType
{
	HYINST_Unknown = 0,
	HYINST_Sound2d,
	HYINST_Particles2d,
	HYINST_Sprite2d,
	HYINST_Spine2d,
	HYINST_TexturedQuad2d,
	HYINST_Primitive2d,
	HYINST_Text2d,
	HYINST_Mesh3d,

	HYINST_Shader
};

enum HyDataType
{
	HYDATA_Regular = 0,
	HYDATA_2d,
	HYDATA_3d
};

enum HyLoadState
{
	HYLOADSTATE_Inactive = 0,
	HYLOADSTATE_Queued,
	HYLOADSTATE_Loaded,
	HYLOADSTATE_Discarded,
	HYLOADSTATE_ReloadGfx
};

enum HyCoordinateType
{
	HYCOORDTYPE_Default = 0,

	HYCOORDTYPE_Camera,
	HYCOORDTYPE_Screen
};

enum HyCoordinateUnit
{
	HYCOORDUNIT_Default = 0,

	HYCOORDUNIT_Pixels,
	HYCOORDUNIT_Meters
};

enum HyWindowType
{
	HYWINDOW_WindowedFixed,
	HYWINDOW_WindowedSizeable,
	HYWINDOW_FullScreen,
	HYWINDOW_BorderlessWindow
};

struct HyWindowInfo
{
	std::string		sName;
	glm::ivec2		vResolution;
	glm::ivec2		vLocation;
	HyWindowType	eType;

	enum eDirtyFlags
	{
		FLAG_Title = 1 << 0,
		FLAG_Resolution = 1 << 1,
		FLAG_Location = 1 << 2,
		FLAG_Type = 1 << 3
	};
	uint32			uiDirtyFlags;
};

#define HY_MAXWINDOWS 6

// Client supplies these initialization parameters to the engine
struct HarmonyInit
{
	const char *			szGameName;
	const char *			szDataDir;
	uint32					uiNumWindows;
	HyWindowInfo			windowInfo[HY_MAXWINDOWS];
	HyCoordinateType		eDefaultCoordinateType;
	HyCoordinateUnit		eDefaultCoordinateUnit;
	float					fPixelsPerMeter;
	uint32					uiNumInputMappings;
	const char *			szDefaultFont;
	uint16					uiDebugPort;

	HarmonyInit(const char *szGameName, const char *szDataDir) :	szGameName(szGameName),
																	szDataDir(szDataDir)
	{
		CtorInit();
	}

	HarmonyInit() :	szGameName("Untitled Game"),
					szDataDir("/data")
	{
		CtorInit();
	}

private:
	void CtorInit()
	{
		uiNumWindows = 1;
		eDefaultCoordinateType = HYCOORDTYPE_Camera;
		eDefaultCoordinateUnit = HYCOORDUNIT_Pixels;
		fPixelsPerMeter = 80.0f;
		uiNumInputMappings = 1;
		szDefaultFont = "Vera.ttf";
		uiDebugPort = 1313;

		for(int i = 0; i < HY_MAXWINDOWS; ++i)
		{
			windowInfo[i].sName = "Window: " + std::to_string(i);
			windowInfo[i].eType = HYWINDOW_WindowedFixed;
			windowInfo[i].vResolution.x = 512;
			windowInfo[i].vResolution.x = 256;
			windowInfo[i].vLocation.x = i * windowInfo[i].vResolution.x;
			windowInfo[i].vLocation.y = 0;
			
			windowInfo[i].uiDirtyFlags = 0;
		}
	}
};

#endif // __HyStdAfx_h__
