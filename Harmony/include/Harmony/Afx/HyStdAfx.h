/**************************************************************************
 *	HyStdAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyStdAfx_h__
#define HyStdAfx_h__

// Order of these #includes matter!
#include "Compilers/HyCompiler.h"
#include "Platforms/HyPlatform.h"

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
//#define TINYGLTF_NOEXCEPTION
#include "Utilities/tiny_gltf.h"

#include "Utilities/jsonxx.h"
#include "Utilities/HyFileIO.h"
#include "Box2D/Box2D.h"

//#define GLM_SWIZZLE
#define GLM_FORCE_CTOR_INIT
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

// STL includes (TODO: to be replaced with EASTL)
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cctype>
#include <sstream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>

#define HY_UNUSED_HANDLE 0
typedef uint32 HyTextureHandle;
typedef uint32 HyStencilHandle;
typedef uint8 HyShaderHandle;
typedef uint32 HyVertexBufferHandle;

enum HyType
{
	HYTYPE_Unknown = 0,

	HYTYPE_Sound,
	HYTYPE_Particles,
	HYTYPE_Sprite,
	HYTYPE_Spine,
	HYTYPE_TexturedQuad,
	HYTYPE_Primitive,
	HYTYPE_Text,
	HYTYPE_Prefab,
	HYTYPE_Entity,
	HYTYPE_Camera
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
	HYLOADABLE_Atlas = 0,
	HYLOADABLE_Shader,
	HYLOADABLE_GLTF
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

enum HyRenderMode
{
	HYRENDERMODE_Unknown = -1,

	HYRENDERMODE_Triangles = 0,
	HYRENDERMODE_TriangleStrip,
	HYRENDERMODE_TriangleFan,
	HYRENDERMODE_LineLoop,
	HYRENDERMODE_LineStrip
};

enum HyShaderProgramDefaults
{
	HYSHADERPROG_QuadBatch = 1,
	HYSHADERPROG_Primitive,
	HYSHADERPROG_Lines2d,
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

enum class HyShaderVariable : uint32
{
	boolean = 0,
	int32,
	uint32,
	float32,
	double64,
	bvec2,
	bvec3,
	bvec4,
	ivec2,
	ivec3,
	ivec4,
	vec2,
	vec3,
	vec4,
	dvec2,
	dvec3,
	dvec4,
	mat3,
	mat4
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
	HYTHREADSTATE_Inactive = 0,
	HYTHREADSTATE_Run,
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
