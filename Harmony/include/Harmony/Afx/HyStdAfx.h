/**************************************************************************
 *	HyStdAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyStdAfx_h__
#define HyStdAfx_h__

// Order of these #includes matter!
#include "Compilers/HyCompiler.h"
#include "Platforms/HyPlatform.h"

#include "Utilities/jsonxx.h"
#include "Utilities/HyFileIO.h"
#include "Box2D/Box2D.h"

//#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"


#if defined(HY_PLATFORM_GUI)// GUI -> Hack for now
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "GL/glew.h"
#endif

#define HY_UNUSED_HANDLE 0
typedef uint32 HyStencilHandle;

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

enum HyShapeType
{
	HYSHAPE_Unknown = -1,

	HYSHAPE_LineSegment = 0,
	HYSHAPE_LineChain,
	HYSHAPE_LineLoop,
	HYSHAPE_Circle,
	HYSHAPE_Polygon,

	HYNUM_SHAPE
};

enum HyLoadableType
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

enum HyDiagFlag
{
	HYDIAG_NONE				= 0,

	HYDIAG_FrameTimeMs		= 1 << 0,
	HYDIAG_AvgFrameTimes	= 1 << 1,
	HYDIAG_Fps				= 1 << 2,
	HYDIAG_FRAMETIMES		= (HYDIAG_FrameTimeMs | HYDIAG_AvgFrameTimes | HYDIAG_Fps),

	HYDIAG_ProfilerGraph	= 1 << 3,
	HYDIAG_ProfilerText		= 1 << 4,
	HYDIAG_PROFILER			= (HYDIAG_ProfilerGraph | HYDIAG_ProfilerText),
	
	HYDIAG_ALL				= (HYDIAG_FRAMETIMES | HYDIAG_PROFILER)
};

struct HyWindowInfo
{
	std::string		sName;
	glm::ivec2		vSize;
	glm::ivec2		ptLocation;
	HyWindowType	eType;

	HyWindowInfo &operator =(const HyWindowInfo &rhs)
	{
		sName = rhs.sName;
		vSize = rhs.vSize;
		ptLocation = rhs.ptLocation;
		eType = rhs.eType;

		return *this;
	}
};

#endif // HyStdAfx_h__
