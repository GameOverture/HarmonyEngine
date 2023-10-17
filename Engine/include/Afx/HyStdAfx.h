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
#include "HyFlags.h"

// TODO: Get rid of this #define - Upgrade RapidJson to a newer release version once it becomes available
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "vendor/box2d/include/box2d/box2d.h"

//#define GLM_SWIZZLE
#define GLM_FORCE_CTOR_INIT
#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtx/transform.hpp"
#include "vendor/glm/gtx/matrix_decompose.hpp"
#include "vendor/glm/gtc/quaternion.hpp"

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
#include <unordered_map>
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
typedef uint32 HyAudioNodeHandle;
typedef uint32 HyAudioHandle;
typedef uint32 HySpacerHandle;
typedef uint32 HyLayoutHandle;

// Tags aren't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)
// They increase the memory footprint of every node/instance by 8 bytes
#define HY_ENABLE_USER_TAGS

#define HY_SIZEFLAG_EXPAND (1 << 0)
#define HY_SIZEFLAG_SHRINK (1 << 1)

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

enum HyBodyType
{
	HYBODY_Static = 0,
	HYBODY_Kinematic,
	HYBODY_Dynamic
};
static_assert((int)HYBODY_Static == (int)b2_staticBody && (int)HYBODY_Kinematic == (int)b2_kinematicBody && (int)HYBODY_Dynamic == (int)b2_dynamicBody, "Harmony and Box2d types don't match!");

enum HyZoomLevel
{
	HYZOOM_6 = 0,	// 6.25%
	HYZOOM_12,		// 12.5%
	HYZOOM_25,
	HYZOOM_33,		// 33.333%
	HYZOOM_50,
	HYZOOM_75,
	HYZOOM_100,
	HYZOOM_200,
	HYZOOM_300,
	HYZOOM_400,
	HYZOOM_500,
	HYZOOM_600,
	HYZOOM_800,
	HYZOOM_1200,
	HYZOOM_1600,

	HYNUM_ZOOMLEVELS
};
const std::vector<float> Hy_ZoomLevelList = { 0.0625f, 0.125f, 0.25f, 0.333333f, 0.5f, 0.75f, 1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  8.0f,  12.0f,  16.0f };

enum HyAnimCtrl
{
	HYANIMCTRL_Reset = 0,
	HYANIMCTRL_ResetAndPlay,
	HYANIMCTRL_ResetAndPause,
	HYANIMCTRL_Reverse,
	HYANIMCTRL_DontReverse,
	HYANIMCTRL_Loop,
	HYANIMCTRL_DontLoop,
	HYANIMCTRL_Bounce,
	HYANIMCTRL_DontBounce
};

enum HyAlignment
{
	HYALIGN_Unknown = -1,

	HYALIGN_Left = 0,
	HYALIGN_Center,
	HYALIGN_Right,
	HYALIGN_Justify,

	HYNUM_ALIGNMENTS
};

enum HyPlayListMode
{
	// NOTE: Order matters, mode is serialized as int
	HYPLAYLIST_Shuffle = 0,
	HYPLAYLIST_Weighted,
	HYPLAYLIST_SequentialLocal,
	HYPLAYLIST_SequentialGlobal,

	HYNUM_PLAYLISTS
};
static_assert(HYNUM_PLAYLISTS < 7, "HyPlayListMode enum cannot exceed '7' entries because it's stored using '3' bits in struct AudioStateAttribs");

enum HyLoadState
{
	HYLOADSTATE_Inactive = 0,
	HYLOADSTATE_Queued,
	HYLOADSTATE_Discarded,
	HYLOADSTATE_Loaded
};

enum HyFileType
{
	HYFILE_Atlas = 0,
	HYFILE_GLTF,
	HYFILE_AudioBank,
	HYFILE_Shader,

	HYNUM_FILETYPES
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

enum HyWindowMode
{
	HYWINDOW_Unknown = -1,

	HYWINDOW_WindowedFixed = 0,
	HYWINDOW_WindowedSizeable,
	HYWINDOW_FullScreen,
	HYWINDOW_BorderlessWindow
};

enum HySizePolicy
{
	HYSIZEPOLICY_Fixed = 0,												// GetSizeHint() is the only acceptable alternative, so the widget can never grow or shrink (e.g. the vertical direction of a push button).
	HYSIZEPOLICY_Expanding = HY_SIZEFLAG_EXPAND,						// GetSizeHint() is the minimum size. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).
	HYSIZEPOLICY_Shrinkable = HY_SIZEFLAG_SHRINK,						// GetSizeHint() is the maximum size. The widget can be shrunk an amount without detriment if other widgets need the space (e.g. a separator line). It cannot be larger than the size provided by SizeHint().
	HYSIZEPOLICY_Flexible = HY_SIZEFLAG_SHRINK | HY_SIZEFLAG_EXPAND,	// GetSizeHint() is a sensible size, but the widget can be shrunk and still be useful. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).
};

enum HyOrientation
{
	HYORIEN_Horizontal = 0,
	HYORIEN_Vertical,

	HYNUM_ORIENTATIONS
};
static_assert(HYORIEN_Horizontal == 0 && HYORIEN_Vertical == 1 && HYNUM_ORIENTATIONS == 2, "HyOrientation enum order matters (glm::ivec2 [] dimension values) ");

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

	HYDIAG_Fps				= 1 << 0,
	HYDIAG_FrameTimes		= 1 << 1,
	HYDIAG_FRAMERATE		= (HYDIAG_Fps | HYDIAG_FrameTimes),

	HYDIAG_Graph			= 1 << 2,
	HYDIAG_GraphKey			= 1 << 3,
	HYDIAG_GRAPH			= (HYDIAG_Graph | HYDIAG_GraphKey),

	HYDIAG_Mouse			= 1 << 4,
	HYDIAG_MouseWorld		= 1 << 5,
	HYDIAG_MouseButtons		= 1 << 6,
	HYDIAG_INPUT			= (HYDIAG_Mouse | HYDIAG_MouseWorld | HYDIAG_MouseButtons),

	HYDIAG_PhysShapes		= 1 << 7,
	HYDIAG_PhysJoints		= 1 << 8,
	HYDIAG_PhysAabb			= 1 << 9,
	HYDIAG_PhysPairs		= 1 << 10,	// Pairs within the broad-phase (objects close to each other or colliding)
	HYDIAG_PhysCenterOfMass	= 1 << 11,
	HYDIAG_PHYSICS			= (HYDIAG_PhysShapes | HYDIAG_PhysJoints | HYDIAG_PhysPairs),
	HYDIAG_PHYSICS_ALL		= (HYDIAG_PhysShapes | HYDIAG_PhysJoints | HYDIAG_PhysAabb | HYDIAG_PhysPairs | HYDIAG_PhysCenterOfMass),
	
	HYDIAG_ALL				= (HYDIAG_FRAMERATE | HYDIAG_GRAPH | HYDIAG_INPUT | HYDIAG_PHYSICS_ALL)
};

enum HyTextureFiltering
{
	// NOTE: Order cannot change without editor version patcher update. New entires may append to this list
	HYTEXFILTER_Unknown = 255,
	HYTEXFILTER_NEAREST = 0,
	HYTEXFILTER_NEAREST_MIPMAP,
	HYTEXFILTER_LINEAR_MIPMAP,
	HYTEXFILTER_BILINEAR,
	HYTEXFILTER_BILINEAR_MIPMAP,
	HYTEXFILTER_TRILINEAR,

	HYNUM_TEXTUREFILTERS
};
static_assert(HYNUM_TEXTUREFILTERS < 255, "HyTextureFiltering cannot exceed 255 values (including unknown). Needs to fit in uint8 (HyTextureInfo)");

enum HyTextureFormat
{
	// NOTE: Order cannot change without editor version patcher update. New entires may append to this list
	HYTEXTURE_Unknown = 255,
	HYTEXTURE_Uncompressed = 0,		// Param1: num channels						Param2: disk file type (PNG, ...)
	HYTEXTURE_DXT,					// Param1: num channels						Param2: DXT format (1,3,5)
	HYTEXTURE_ASTC,					// Param1: Block Size index (4x4 -> 12x12)	Param2: Color Profile (LDR linear, LDR sRGB, HDR RGB, HDR RGBA)

	HYNUM_TEXTUREFORMATS
};
static_assert(HYNUM_TEXTUREFORMATS < 255, "HyTextureFormat cannot exceed 255 values (including unknown). Needs to fit in uint8 (HyTextureInfo)");

struct HyTextureInfo
{
	enum UncompressedFileType
	{
		UNCOMPRESSEDFILE_PNG = 0
	};
	uint8				m_uiFiltering;
	uint8				m_uiFormat;
	uint8				m_uiFormatParam1;
	uint8				m_uiFormatParam2;

	HyTextureInfo();
	HyTextureInfo(HyTextureFiltering eFiltering, HyTextureFormat eFormat, uint8 uiFormatParam1, uint8 uiFormatParam2);
	HyTextureInfo(uint32 uiBucketId);

	bool operator==(const HyTextureInfo &rhs) const;
	bool operator!=(const HyTextureInfo &rhs) const;

	HyTextureFormat GetFormat() const;
	HyTextureFiltering GetFiltering() const;

	bool IsMipMaps() const;
	std::string GetFileExt() const; // Includes the dot (like ".png")
	uint32 GetBucketId() const;
};

struct HyWindowInfo
{
	std::string		sName;
	glm::ivec2		vSize;
	glm::ivec2		ptLocation;
	HyWindowMode	eMode;

	HyWindowInfo &operator =(const HyWindowInfo &rhs)
	{
		sName = rhs.sName;
		vSize = rhs.vSize;
		ptLocation = rhs.ptLocation;
		eMode = rhs.eMode;

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
	uint32					uiUpdatesPerSec;
	int32					iVSync;
	uint32					uiNumInputMaps;
	bool					bShowCursor;
	glm::vec2				vGravity2d;
	float					fPixelsPerMeter;
	bool					bUseConsole;
	HyWindowInfo			consoleInfo;

	HarmonyInit();
	HarmonyInit(std::string sHyProjFileName);
};

#endif // HyStdAfx_h__
