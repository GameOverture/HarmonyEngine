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


// Client supplies these initialization parameters to the engine
class HY_GFX_API;
struct HarmonyInit
{
	const char *			szGameName;
	const char *			szDataDir;
	HyWindowType			eWindowType;
	glm::ivec2				vStartResolution;
	HyCoordinateType		eDefaultCoordinateType;
	float					fPixelsPerMeter;
	uint32					uiNumInputMappings;
	HY_GFX_API *			pSuppliedGfx;
	const char *			szDefaultFont;

	HarmonyInit() :	szGameName("Untitled Game"),
					szDataDir("./data"),
					eWindowType(HYWINDOW_WindowedFixed),
					vStartResolution(512, 256),
					eDefaultCoordinateType(HYCOORD_Pixel),
					fPixelsPerMeter(80),
					uiNumInputMappings(1),
					pSuppliedGfx(NULL),
					szDefaultFont("Vera.ttf")
	{ }
};

#endif // __HyStdAfx_h__
