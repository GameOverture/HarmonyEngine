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

#include "Utilities/jsonxx.h"
#include "Utilities/HyFileIO.h"

//#define HYSETTING_ThrottleUpdate

enum HyType
{
	HYTYPE_Unknown = 0,
	HYTYPE_Sound2d,
	HYTYPE_Particles2d,
	HYTYPE_Sprite2d,
	HYTYPE_Spine2d,
	HYTYPE_TexturedQuad2d,
	HYTYPE_Primitive2d,
	HYTYPE_Text2d,
	HYTYPE_Mesh3d,
	HYTYPE_Entity2d,
	HYTYPE_Entity3d,
	HYTYPE_Camera2d,
	HYTYPE_Camera3d
};

enum HyGfxType
{
	HYGFXTYPE_AtlasGroup = 0,
	HYGFXTYPE_CustomShader
};

enum HyAnimCtrl
{
	HYANIMCTRL_Play = 0,
	HYANIMCTRL_ReversePlay,
	HYANIMCTRL_Reset,
	HYANIMCTRL_Loop,
	HYANIMCTRL_DontLoop,
	HYANIMCTRL_Bounce,
	HYANIMCTRL_DontBounce
};

enum HyAlign
{
	HYALIGN_Left = 0,
	HYALIGN_Center,
	HYALIGN_Right,
	HYALIGN_Justify
};

enum HyLoadState
{
	HYLOADSTATE_Inactive = 0,
	HYLOADSTATE_Queued,
	HYLOADSTATE_Discarded,
	HYLOADSTATE_Loaded
};

enum HyTextureFormat
{
	HYTEXTURE_R8G8B8A8 = 0,	// Uncompressed with alpha (32-bpp)
	HYTEXTURE_R8G8B8,		// Uncompressed no alpha (24-bpp)
	HYTEXTURE_RGB_DTX1,
	HYTEXTURE_RGBA_DTX1,
	HYTEXTURE_DTX3,
	HYTEXTURE_DTX5,

	HYNUMTEXTUREFORMATS
};

enum HyShaderProgram
{
	HYSHADERPROG_QuadBatch = 0,
	HYSHADERPROG_Primitive,
	HYSHADERPROG_Lines2d,

	HYSHADERPROG_CustomStartIndex
};

enum HyShaderType
{
	HYSHADER_Vertex = 0,
	HYSHADER_Fragment,
	HYSHADER_Geometry,
	HYSHADER_TessControl,
	HYSHADER_TessEvaluation,

	HYNUMSHADERTYPES
};

enum HyShaderVariable
{
	HYSHADERVAR_bool = 0,
	HYSHADERVAR_int,
	HYSHADERVAR_uint,
	HYSHADERVAR_float,
	HYSHADERVAR_double,
	HYSHADERVAR_bvec2,
	HYSHADERVAR_bvec3,
	HYSHADERVAR_bvec4,
	HYSHADERVAR_ivec2,
	HYSHADERVAR_ivec3,
	HYSHADERVAR_ivec4,
	HYSHADERVAR_vec2,
	HYSHADERVAR_vec3,
	HYSHADERVAR_vec4,
	HYSHADERVAR_dvec2,
	HYSHADERVAR_dvec3,
	HYSHADERVAR_dvec4,
	HYSHADERVAR_mat3,
	HYSHADERVAR_mat4
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

enum HyThreadState
{
	HYTHREADSTATE_Run = 0,
	HYTHREADSTATE_ShouldExit,
	HYTHREADSTATE_HasExited
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

#endif // __HyStdAfx_h__
