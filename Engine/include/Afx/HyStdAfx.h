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
typedef uint32 HyBufferHandle;
typedef uint32 HyStencilHandle;
typedef uint32 HyShaderHandle;
typedef uint32 HyVertexBufferHandle;
typedef uint32 HyTileSetHandle;							// 32bit CRC derived from the tile set's name in lowercase
typedef uint32 HyAudioNodeHandle;
typedef std::pair<uint32, uint32> HyTextureQuadHandle;	// FIRST is the checksum, SECOND is the bank ID ---OR--- FIRST is '0', indicating SECOND is a 'HyAuxiliaryFileHandle'
typedef std::pair<uint32, uint32> HyAudioHandle;		// FIRST is the checksum, SECOND is the bank ID ---OR--- FIRST is '0', indicating SECOND is a 'HyAuxiliaryFileHandle'
typedef uint32 HyAuxiliaryFileHandle;
typedef uint32 HySpacerHandle;
typedef uint32 HyLayoutHandle;

// Tags aren't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)
// Default 32bit user tags fill a memory fragmented gap and don't contribute to increasing the memory footprint of nodes
// Enabling 64bit user tags increase the memory footprint of every node/instance by 8 bytes due to alignment (on x64 builds)
#define HY_64BIT_USER_TAGS 0

#define HY_SIZEFLAG_EXPAND (1 << 0)
#define HY_SIZEFLAG_SHRINK (1 << 1)

enum HyType
{
	HYTYPE_Unknown = 0,

	HYTYPE_Audio,
	HYTYPE_Particles,
	HYTYPE_Sprite,
	HYTYPE_TileMapBatch,
	HYTYPE_Spine,
	HYTYPE_TexturedQuad,
	HYTYPE_Primitive,
	HYTYPE_Text,
	HYTYPE_Prefab,
	HYTYPE_Entity,
	HYTYPE_Camera,

	HYNUM_TYPES
};
static_assert((int)HYNUM_TYPES <= 16, "HyType cannot exceed 16 items due to IHyNode only reserving 4 bits to store this value");

enum HyWidgetType
{
	HYWIDGET_Unknown = -1,

	HYWIDGET_Label = 0,
	HYWIDGET_RackMeter,
	HYWIDGET_BarMeter,
	HYWIDGET_Button,
	HYWIDGET_CheckBox,
	HYWIDGET_RadioButton,
	HYWIDGET_Slider,
	HYWIDGET_ComboBox,
	HYWIDGET_TextField,
	HYWIDGET_RichText,

	HYNUM_WIDGETS
};

enum HyBodyType
{
	HYBODY_Static = 0,
	HYBODY_Kinematic,
	HYBODY_Dynamic
};
static_assert((int)HYBODY_Static == (int)b2_staticBody && (int)HYBODY_Kinematic == (int)b2_kinematicBody && (int)HYBODY_Dynamic == (int)b2_dynamicBody, "Harmony and Box2d types don't match!");

enum HyCollisionCategory
{
	HYCOLLISION_Default			= 1 << 0,
	HYCOLLISION_Actor			= 1 << 1,
	HYCOLLISION_Dynamic			= 1 << 2,
};

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

enum HyTextType
{
	HYTEXT_Unknown = -1,

	HYTEXT_Line = 0,
	HYTEXT_Column,
	HYTEXT_Box,
	HYTEXT_ScaleBox,
	HYTEXT_Vertical,

	HYNUM_TEXTTYPES,

	HYTEXT_MASK = 0x07	// 3 bits
};
static_assert((int)HYNUM_TEXTTYPES <= 8, "HyTextType cannot exceed 8 items due to 'IHyText' and 'HyLabel' only reserving 3 bits to store this value");

enum HyAlignment
{
	HYALIGN_Unknown = -1,

	HYALIGN_Left = 0,
	HYALIGN_Center,
	HYALIGN_Right,
	HYALIGN_Justify,

	HYNUM_ALIGNMENTS
};

enum HyTileMapLayout
{
	HYTILEMAPLAYOUT_Unknown = -1,

	HYTILEMAPLAYOUT_Square = 0,
	HYTILEMAPLAYOUT_HalfOffsetSquare,
	HYTILEMAPLAYOUT_Isometric,
	HYTILEMAPLAYOUT_IsometricStaggerX,
	HYTILEMAPLAYOUT_IsometricStaggerY,
	HYTILEMAPLAYOUT_HexagonFlatTop,
	HYTILEMAPLAYOUT_HexagonPointTop,

	HYNUM_TILEMAPLAYOUTS
};

enum HyPlaylistMode
{
	// NOTE: Order matters, mode is serialized as int
	HYPLAYLIST_Shuffle = 0,
	HYPLAYLIST_Weighted,
	HYPLAYLIST_SequentialLocal,
	HYPLAYLIST_SequentialGlobal,

	HYNUM_PLAYLISTS
};
static_assert(HYNUM_PLAYLISTS <= 8, "HyPlayListMode enum cannot exceed '8' entries because it's stored using '3' bits in struct AudioStateAttribs");

enum HyFixtureType
{
	HYFIXTURE_Nothing = -1,

	HYFIXTURE_Circle = 0,
	HYFIXTURE_LineSegment,
	HYFIXTURE_Polygon,
	HYFIXTURE_Capsule,

	HYFIXTURE_LineChain
};

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
	HYFILE_TileSet,
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

enum HyBlendMode
{
	HYBLENDMODE_Normal = 0,
	HYBLENDMODE_Additive,
	HYBLENDMODE_Multiply,
	HYBLENDMODE_Screen
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
	HYSHADERPROG_Spine,
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
	mat4,
	color // 4 uint8 RGBA - becomes a vec4 in shader
};

enum HyWindowMode
{
	HYWINDOW_Unknown = -1,

	HYWINDOW_WindowedFixed = 0,
	HYWINDOW_WindowedSizeable,
	HYWINDOW_FullScreen,
	HYWINDOW_BorderlessWindow
};

enum HyPanelState
{
	HYPANELSTATE_NotUsed = -1,

	HYPANELSTATE_Idle = 0,
	HYPANELSTATE_Down,
	HYPANELSTATE_Hover,
	HYPANELSTATE_Highlighted,
	HYPANELSTATE_HighlightedDown,
	HYPANELSTATE_HighlightedHover,

	HYNUM_PANELSTATES
};

enum HySizePolicy
{
	HYSIZEPOLICY_Unknown = -1,

	HYSIZEPOLICY_Fixed = 0,												// GetSizeHint() is the only acceptable alternative, so the widget can never grow or shrink (e.g. the vertical direction of a push button).
	HYSIZEPOLICY_Expanding = HY_SIZEFLAG_EXPAND,						// GetSizeHint() is the minimum size. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).
	HYSIZEPOLICY_Shrinkable = HY_SIZEFLAG_SHRINK,						// GetSizeHint() is the maximum size. The widget can be shrunk an amount without detriment if other widgets need the space (e.g. a separator line). It cannot be larger than the size provided by SizeHint().
	HYSIZEPOLICY_Flexible = HY_SIZEFLAG_SHRINK | HY_SIZEFLAG_EXPAND,	// GetSizeHint() is a sensible size, but the widget can be shrunk and still be useful. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).

	HYNUM_SIZEPOLICIES = 4
};

enum HyOrientation
{
	HYORIENT_Null = -1,

	HYORIENT_Horizontal = 0,
	HYORIENT_Vertical,

	HYNUM_ORIENTATIONS
};
static_assert(HYORIENT_Horizontal == 0 && HYORIENT_Vertical == 1 && HYNUM_ORIENTATIONS == 2, "HyOrientation enum order matters (glm::ivec2 [] dimension values) ");

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
	HYDIAG_PhysContacts		= 1 << 10,	// Pairs within the broad-phase (objects close to each other or colliding)
	HYDIAG_PhysCenterOfMass	= 1 << 11,
	HYDIAG_PHYSICS			= (HYDIAG_PhysShapes | HYDIAG_PhysJoints | HYDIAG_PhysContacts),
	HYDIAG_PHYSICS_ALL		= (HYDIAG_PhysShapes | HYDIAG_PhysJoints | HYDIAG_PhysAabb | HYDIAG_PhysContacts | HYDIAG_PhysCenterOfMass),
	
	HYDIAG_ALL				= (HYDIAG_FRAMERATE | HYDIAG_GRAPH | HYDIAG_INPUT | HYDIAG_PHYSICS_ALL)
};

enum HyTextureFilter
{
	// NOTE: Order cannot change without editor version patcher update. New entires may append to this list
	HYTEXFILTER_Unknown = 255,

	HYTEXFILTER_NEAREST = 0,
	HYTEXFILTER_NEAREST_MIPMAP,
	HYTEXFILTER_LINEAR_MIPMAP,
	HYTEXFILTER_BILINEAR,
	HYTEXFILTER_BILINEAR_MIPMAP,
	HYTEXFILTER_TRILINEAR_MIPMAP,

	HYNUM_TEXTUREFILTERS
};
static_assert(HYNUM_TEXTUREFILTERS < 255, "HyTextureFilter cannot exceed 254 values. Needs to fit in uint8 (HyTextureInfo::m_uiFilter)");

enum HyTextureWrap
{
	// NOTE: Order cannot change without editor version patcher update. New entires may append to this list
	HYTEXWRAP_Unknown = 15,

	HYTEXWRAP_Repeat = 0,
	HYTEXWRAP_ClampToEdge,
	HYTEXWRAP_ClampToBorder,
	HYTEXWRAP_MirroredRepeat,
	HYTEXWRAP_MirrorClampToEdge,
	HYTEXWRAP_MirrorClampToBorder,

	HYNUM_TEXTUREWRAPS
};
static_assert(HYNUM_TEXTUREWRAPS < 15, "HyTextureWrap cannot exceed 14 values. Needs to fit in 4 bits (HyTextureInfo::m_uiWrapAndChannels)");

enum HyTextureFormat
{
	// NOTE: Order cannot change without editor version patcher update. New uncompressed entires should use reserves, and compressed should append to this list
	HYTEXFORMAT_Unknown = 255,
	
	// UNCOMPRESSED					Format code: HYTX
	HYTEXFORMAT_UINT8 = 0,			// UI08
	HYTEXFORMAT_INT8,				// SI08
	HYTEXFORMAT_NORM8,				
	HYTEXFORMAT_SNORM8,				
	HYTEXFORMAT_UINT16,				// UI16
	HYTEXFORMAT_INT16,				// SI16
	HYTEXFORMAT_NORM16,				
	HYTEXFORMAT_SNORM16,			
	HYTEXFORMAT_UINT32,				// UI32
	HYTEXFORMAT_INT32,				// SI32
	HYTEXFORMAT_FLOAT16,			// SF16
	HYTEXFORMAT_FLOAT32,			// SF32

	// BLOCK COMPRESSION			Format code: DDS
	HYTEXFORMAT_BC1_DXT1,			// DXT1 (when used in conjunction with 4 color channels, it's the 1bit alpha channel version)
	HYTEXFORMAT_BC2_DXT3,			// DXT3 (mostly obsolete, only support for auxiliary loading)
	HYTEXFORMAT_BC3_DXT5,			// DXT5
	HYTEXFORMAT_BC4_RGTC1,			// ATI1 or BC4U
	HYTEXFORMAT_BC4_SIGNED_RGTC1,	// BC4S
	HYTEXFORMAT_BC5_RGTC2,			// ATI2 or BC5U
	HYTEXFORMAT_BC5_SIGNED_RGTC2,	// BC5S
	HYTEXFORMAT_BC6_HDR,			// DX10
	HYTEXFORMAT_BC6_SIGNED_HDR,		// DX10
	HYTEXFORMAT_BC7_LINEAR,			// DX10 (Encoding modes 0-7 specified by the format parameter)
	HYTEXFORMAT_BC7_sRGB_A8,		// DX10 (Encoding modes 0-7 specified by the format parameter)

	// ASTC COMPRESSION
	HYTEXFORMAT_ASTC_LINEAR_4x4,	// 8.00 bpp
	HYTEXFORMAT_ASTC_LINEAR_5x4,	// 6.40 bpp
	HYTEXFORMAT_ASTC_LINEAR_5x5,	// 5.12 bpp
	HYTEXFORMAT_ASTC_LINEAR_6x5,	// 4.27 bpp
	HYTEXFORMAT_ASTC_LINEAR_6x6,	// 3.56 bpp
	HYTEXFORMAT_ASTC_LINEAR_8x5,	// 3.20 bpp
	HYTEXFORMAT_ASTC_LINEAR_8x6,	// 2.67 bpp
	HYTEXFORMAT_ASTC_LINEAR_10x5,	// 2.56 bpp
	HYTEXFORMAT_ASTC_LINEAR_10x6,	// 2.13 bpp
	HYTEXFORMAT_ASTC_LINEAR_8x8,	// 2.00 bpp
	HYTEXFORMAT_ASTC_LINEAR_10x8,	// 1.60 bpp
	HYTEXFORMAT_ASTC_LINEAR_10x10,	// 1.28 bpp
	HYTEXFORMAT_ASTC_LINEAR_12x10,	// 1.07 bpp
	HYTEXFORMAT_ASTC_LINEAR_12x12,	// 0.89 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_4x4,	// 8.00 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_5x4,	// 6.40 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_5x5,	// 5.12 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_6x5,	// 4.27 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_6x6,	// 3.56 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_8x5,	// 3.20 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_8x6,	// 2.67 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_10x5,	// 2.56 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_10x6,	// 2.13 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_8x8,	// 2.00 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_10x8,	// 1.60 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_10x10,	// 1.28 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_12x10,	// 1.07 bpp
	HYTEXFORMAT_ASTC_sRGB_A8_12x12,	// 0.89 bpp

	HYNUM_TEXTUREFORMATS
};
static_assert(HYNUM_TEXTUREFORMATS < 255, "HyTextureFormat cannot exceed 254 values. Needs to fit in uint8 (HyImageInfo::m_uiFormat & HyTextureInfo::m_uiFormat)");

enum HyAstcColorProfile
{
	// NOTE: This enum maps the format parameter value used in both HyImageInfo/HyTextureInfo to an ASTC color profile (all profiles support alpha)
	HYASTC_Unknown = 0,

	HYASTC_Linear = 1,			// LDR (Low Dynamic Range) linear color data
	HYASTC_Standard = 2,		// LDR Standard RGBA - Recommended for standard color textures as gamma curve better matches human perception
	HYASTC_HDR_LinearA = 3,		// HDR (High Dynamic Range) RGB data combined with Low Dynamic Range alpha
	HYASTC_HDR_RGBA = 4			// HDR RGB data combined with High Dynamic Range alpha (usually not needed)
};

enum HyImageType
{
	// NOTE: Order cannot change without editor version patcher update. New entires may append to this list
	HYIMAGE_Unknown = 255,

	HYIMAGE_PNG = 0,	// Format = HYTEXFORMAT_UINT8 or HYTEXFORMAT_UINT16
	HYIMAGE_HYTX,
	HYIMAGE_DDS,
	HYIMAGE_ASTC,		// Format = block Size (4x4 -> 12x12); FormatParam = HyAstcColorProfile (LDR linear, LDR sRGB, HDR RGB, HDR RGBA)

	HYNUM_IMAGETYPES
};
static_assert(HYNUM_IMAGETYPES < 255, "HyImageType cannot exceed 254 values. Needs to fit in uint8 (HyImageInfo::m_uiType)");

class HyImageInfo
{
	uint16				m_uiWidth;
	uint16				m_uiHeight;
	
	uint8				m_uiType;
	uint8				m_uiFlipAndChannels;
	uint8				m_uiFormat;
	uint8				m_uiFormatParam;

public:
	HyImageInfo();
	HyImageInfo(uint16 uiWidth, uint16 uiHeight, HyImageType eType, bool bVerticalFlip, int iNumChannels, HyTextureFormat eFormat, uint8 uiFormatParam);
	HyImageInfo(uint64 uiBucketId);

	uint64 GetBucketId() const;

	uint16 GetWidth() const;
	void SetWidth(uint16 uiWidth);

	uint16 GetHeight() const;
	void SetHeight(uint16 uiHeight);

	HyImageType GetType() const;
	void SetType(HyImageType eType);

	bool IsVerticalFlip() const;
	void SetVerticalFlip(bool bVerticalFlip);

	int GetNumChannels() const;
	void SetNumChannels(int iNumChannels);

	HyTextureFormat GetFormat() const;
	void SetFormat(HyTextureFormat eFormat);

	uint8 GetFormatParam() const;
	void SetFormatParam(uint8 uiFormatParam);

	static std::string GetExt(HyImageType eType); // Includes the dot (like ".png")
};

class HyTextureIn
{
	uint8				m_uiFilter;
	uint8				m_uiWrapAndChannels;
	uint8				m_uiFormat;
	uint8				m_uiFormatParam;

public:
	HyTextureIn();
	HyTextureIn(HyTextureFilter eFilter, HyTextureWrap eWrap, int iNumChannels, HyTextureFormat eFormat, uint8 uiFormatParam);
	HyTextureIn(uint32 uiBucketId);

	uint32 GetBucketId() const;

	HyTextureFilter GetFilter() const;
	void SetFilter(HyTextureFilter eFilter);

	HyTextureWrap GetWrap() const;
	void SetWrap(HyTextureWrap eWrap);

	int GetNumChannels() const;
	void SetNumChannels(int iNumChannels);

	HyTextureFormat GetFormat() const;
	void SetFormat(HyTextureFormat eFormat);

	uint8 GetFormatParam() const;
	void SetFormatParam(uint8 uiFormatParam);
};

struct HyWindowInfo
{
	std::string		sName;
	glm::ivec2		vSize;
	glm::ivec2		ptLocation;
	HyWindowMode	eMode;
	int32			iVSync;

	HyWindowInfo &operator =(const HyWindowInfo &rhs)
	{
		sName = rhs.sName;
		vSize = rhs.vSize;
		ptLocation = rhs.ptLocation;
		eMode = rhs.eMode;
		iVSync = rhs.iVSync;

		return *this;
	}
};

// Client supplies these initialization parameters to the engine
struct HyInit
{
	std::string					sProjectDir;

	std::string					sGameName;
	std::string					sDataPath;			// a directory path when loose assets are used, or file path when packed assets are used (without the extension)
	std::vector<HyWindowInfo>	windowInfoList;
	uint32						uiUpdatesPerSec;
	uint32						uiNumInputMaps;
	bool						bShowCursor;
	glm::vec2					vGravity2d;
	float						fPixelsPerMeter;
	bool						bUseConsole;
	HyWindowInfo				consoleInfo;

	HyInit();
	HyInit(std::string sHyProjFileName);
};

#endif // HyStdAfx_h__
