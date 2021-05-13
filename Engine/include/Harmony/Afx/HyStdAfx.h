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

#include "rapidjson/document.h"
typedef rapidjson::Document HyJsonDoc;
typedef rapidjson::Value HyJsonValue;
typedef rapidjson::GenericArray<false, rapidjson::Value> HyJsonArray;
typedef rapidjson::GenericObject<false, rapidjson::Value> HyJsonObj;

#include "box2d/box2d.h"

//#define GLM_SWIZZLE
#define GLM_FORCE_CTOR_INIT
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/quaternion.hpp"

#ifdef HY_USE_GLFW
	#include <GLFW/glfw3.h>
#endif

// STL includes (TODO: to be replaced/benchmarked with EASTL)
#include <iostream>
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
#include <memory>

#define HY_UNUSED_HANDLE 0
typedef uint32 HyTextureHandle;
typedef uint32 HyStencilHandle;
typedef uint8 HyShaderHandle;
typedef uint32 HyVertexBufferHandle;

// Tags aren't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)
// They increase the memory footprint of every node/instance by 8 bytes
#define HY_ENABLE_USER_TAGS

#define HY_SIZEFLAG_GROW (1 << 0)
#define HY_SIZEFLAG_EXPAND (1 << 1)
#define HY_SIZEFLAG_SHRINK (1 << 2)
#define HY_SIZEFLAG_IGNORE (1 << 3)

enum HyType
{
	HYTYPE_Unknown = 0,

	HYTYPE_Audio,
	HYTYPE_Particles,
	HYTYPE_Sprite,
	HYTYPE_Spine,
	HYTYPE_TexturedQuad,
	HYTYPE_Primitive,
	HYTYPE_Text,
	HYTYPE_Prefab,
	HYTYPE_Entity,
	HYTYPE_Camera,

	HYNUM_TYPES
};
static_assert((int)HYNUM_TYPES < 16, "HyType cannot exceed 15 items due to IHyNode only reserving 4 bits to store this value");

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

enum HyPhysicsType
{
	HYPHYS_Unknown = -1,

	HYPHYS_Static = 0,
	HYPHYS_Kinematic,
	HYPHYS_Dynamic
};
static_assert((int)HYPHYS_Static == (int)b2_staticBody && (int)HYPHYS_Kinematic == (int)b2_kinematicBody && (int)HYPHYS_Dynamic == (int)b2_dynamicBody, "Harmony and Box2d types don't match!");

enum HyFileType
{
	HYFILE_Atlas = 0,
	HYFILE_GLTF,
	HYFILE_AudioBank,
	HYFILE_Shader,

	HYNUM_FILETYPES
};

enum HyAnimCtrl
{
	HYANIMCTRL_Play = 0,
	HYANIMCTRL_Reset,
	HYANIMCTRL_Reverse,
	HYANIMCTRL_DontReverse,
	HYANIMCTRL_Loop,
	HYANIMCTRL_DontLoop,
	HYANIMCTRL_Bounce,
	HYANIMCTRL_DontBounce
};

enum HyAlignment
{
	HYALIGN_Left = 0,
	HYALIGN_HCenter,
	HYALIGN_Right,
	HYALIGN_Justify
};

enum HyPlayListMode
{
	// NOTE: Order matters, mode is saved as int
	HYPLAYLIST_Shuffle = 0,
	HYPLAYLIST_Weighted,
	HYPLAYLIST_SequentialLocal,
	HYPLAYLIST_SequentialGlobal
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
	HYTEXTURE_Unknown = -1,

	HYTEXTURE_R8G8B8A8 = 0,	// Uncompressed with alpha (32-bpp)
	HYTEXTURE_R8G8B8,		// Uncompressed no alpha (24-bpp)
	HYTEXTURE_RGB_DTX1,
	HYTEXTURE_RGBA_DTX1,
	HYTEXTURE_DTX3,
	HYTEXTURE_DTX5,

	HYNUM_TEXTUREFORMATS
};

enum HyTextureFiltering
{
	HYTEXFILTER_Unknown = -1,

	HYTEXFILTER_NEAREST = 0,
	HYTEXFILTER_NEAREST_MIPMAP,
	HYTEXFILTER_LINEAR_MIPMAP,
	HYTEXFILTER_BILINEAR,
	HYTEXFILTER_BILINEAR_MIPMAP,
	HYTEXFILTER_TRILINEAR,

	HYNUM_TEXTUREFILTERS
};

enum HyAudioFormat
{
	HYAUDFORMAT_WAV = 0,
	HYAUDFORMAT_OGG,
	HYAUDFORMAT_ADPCM,

	HYNUM_AUDIOFORMATS
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

	HYNUM_SHADERTYPES
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

enum HyLayoutType
{
	HYLAYOUT_Horizontal = 0,
	HYLAYOUT_Vertical,
	HYLAYOUT_Grid
};

enum HySizePolicy
{
	HYSIZEPOLICY_Fixed = 0,																	// GetSizeHint() is the only acceptable alternative, so the widget can never grow or shrink (e.g. the vertical direction of a push button).
	HYSIZEPOLICY_Minimum = HY_SIZEFLAG_GROW,												// GetSizeHint() is the minimum size. The widget can be expanded, but there is no advantage to it being larger (e.g. the horizontal direction of a push button).
	HYSIZEPOLICY_Maximum = HY_SIZEFLAG_SHRINK,												// GetSizeHint() is the maximum size. The widget can be shrunk any amount without detriment if other widgets need the space (e.g. a separator line). It cannot be larger than the size provided by SizeHint().
	HYSIZEPOLICY_Preferred = HY_SIZEFLAG_GROW | HY_SIZEFLAG_SHRINK,							// GetSizeHint() is best, but the widget can be shrunk and still be useful. The widget can be expanded, but there is no advantage to it being larger than SizeHint() (the default HyWidget policy).
	HYSIZEPOLICY_MinimumExpanding = HY_SIZEFLAG_GROW | HY_SIZEFLAG_EXPAND,					// GetSizeHint() is the minimum size. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).
	HYSIZEPOLICY_Expanding = HY_SIZEFLAG_GROW | HY_SIZEFLAG_SHRINK | HY_SIZEFLAG_EXPAND,	// GetSizeHint() is a sensible size, but the widget can be shrunk and still be useful. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).
	HYSIZEPOLICY_None = HY_SIZEFLAG_GROW | HY_SIZEFLAG_SHRINK | HY_SIZEFLAG_IGNORE			// GetSizeHint() is ignored. The widget will use as little or as much space as possible.
};

enum HyOrientation
{
	// WARNING: THIS ENUM ORDER MATTERS (glm::ivec2 [] dimension values)
	HYORIEN_Horizontal = 0,
	HYORIEN_Vertical,

	HYNUM_ORIENTATIONS
};

enum HyThreadPriority
{
	HYTHREAD_Lowest = -2,
	HYTHREAD_BelowNormal = -1,
	HYTHREAD_Normal = 0,
	HYTHREAD_AboveNormal,
	HYTHREAD_Highest
};

enum HyDiagFlag
{
	HYDIAG_NONE				= 0,

	HYDIAG_LastFrameTime	= 1 << 0,
	HYDIAG_AvgFrameTimes	= 1 << 1,
	HYDIAG_Fps				= 1 << 2,
	HYDIAG_FRAMETIMES		= (HYDIAG_LastFrameTime | HYDIAG_AvgFrameTimes | HYDIAG_Fps),

	HYDIAG_ProfilerGraph	= 1 << 3,
	HYDIAG_ProfilerText		= 1 << 4,
	HYDIAG_PROFILER			= (HYDIAG_ProfilerGraph | HYDIAG_ProfilerText),

	HYDIAG_Mouse			= 1 << 5,
	
	HYDIAG_ALL				= (HYDIAG_FRAMETIMES | HYDIAG_PROFILER | HYDIAG_Mouse)
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

// Client supplies these initialization parameters to the engine
struct HarmonyInit
{
	std::string				sProjectDir;

	std::string				sGameName;
	std::string				sDataDir;
	uint32					uiNumWindows;
	HyWindowInfo			windowInfo[HY_MAXWINDOWS];
	uint32					uiUpdateTickMs;
	uint32					uiNumInputMappings;
	uint16					uiDebugPort;
	bool					bUseConsole;
	bool					bShowCursor;
	HyWindowInfo			consoleInfo;

	HarmonyInit();
	HarmonyInit(std::string sHyProjFileName);
};

#endif // HyStdAfx_h__
