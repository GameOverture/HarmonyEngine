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
	HYINST_Sprite2d,
	HYINST_Spine2d,
	HYINST_TexturedQuad2d,
	HYINST_Primitive2d,
	HYINST_Text2d,
	HYINST_Mesh3d,

	HYINST_Shader
};

enum HyLoadState
{
	HYLOADSTATE_Inactive = 0,
	HYLOADSTATE_Queued,
	HYLOADSTATE_Loaded,
	HYLOADSTATE_Discarded
};

enum HyCoordinateType
{
	HYCOORD_Default = -1,
	HYCOORD_Pixel = 0,
	HYCOORD_Meter,
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
};

#define HY_MAXWINDOWS 6

// Client supplies these initialization parameters to the engine
class HY_GFX_API;
struct HarmonyInit
{
	const char *			szGameName;
	const char *			szDataDir;
	uint32					uiNumWindows;
	HyWindowInfo			windowInfo[HY_MAXWINDOWS];
	HyCoordinateType		eDefaultCoordinateType;
	float					fPixelsPerMeter;
	uint32					uiNumInputMappings;
	HY_GFX_API *			pSuppliedGfx;
	const char *			szDefaultFont;

	HarmonyInit() :	szGameName("Untitled Game"),
					szDataDir("./data"),
					uiNumWindows(1),
					eDefaultCoordinateType(HYCOORD_Pixel),
					fPixelsPerMeter(80),
					uiNumInputMappings(1),
					pSuppliedGfx(NULL),
					szDefaultFont("Vera.ttf")
	{
		for(int i = 0; i < HY_MAXWINDOWS; ++i)
		{
			windowInfo[i].sName = "Window: " + std::to_string(i);
			windowInfo[i].eType = HYWINDOW_WindowedFixed;
			windowInfo[i].vResolution.x = 512;
			windowInfo[i].vResolution.x = 256;
			windowInfo[i].vLocation.x = i * windowInfo[i].vResolution.x;
			windowInfo[i].vLocation.y = 0;
		}
	}
};

#endif // __HyStdAfx_h__
