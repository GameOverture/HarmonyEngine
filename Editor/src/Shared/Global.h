/**************************************************************************
 *	Global.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#include "HyEngine.h"
#ifdef HY_COMPILER_MSVC
	#pragma warning ( push )
	#pragma warning( disable: 4100 )
#endif

#include <QString>
#include <QIcon>
#include <QValidator>
#include <QAction>
#include <QStringBuilder>
#include <QDir>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QStack>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "AuxOutputLog.h"


class Project;

#define HyOrganizationName "Game Overture"
#define HyEditorToolName "Harmony Editor Tool"
#define HyGuiInternalCharIndicator '+'

#define HySrcEntityFilter "_hy_"
#define HySrcEntityNamespace "hy"

#define HYGUI_FILE_VERSION 16

#define DISPLAYORDER_SnapGuide		99999999
#define DISPLAYORDER_TransformCtrl	9999999
//#define HYGUI_UseBinaryMetaFiles

#define DEFAULT_GRID_SIZE 25.0f

enum ItemType
{
	ITEM_Unknown = -1,

	// Project items
	ITEM_Project,
	ITEM_Prefix,
	ITEM_Primitive,
	ITEM_Audio,
	ITEM_Particles,
	ITEM_Text,
	ITEM_Spine,
	ITEM_Sprite,
	ITEM_Shader,
	ITEM_Prefab,
	ITEM_Entity,
	ITEM_Entity3d, // TODO: Determine whether to remove this
	// Asset items
	ITEM_Filter,
	ITEM_AtlasFrame,
	ITEM_SoundClip,
	ITEM_Source,
	ITEM_Header,
	// Entity items
	ITEM_BoundingVolume,
	ITEM_Physics,

	NUM_ITEMTYPES
};

enum AssetManagerType
{
	ASSETMAN_Unknown = -1,

	ASSETMAN_Source = 0,
	ASSETMAN_Atlases,
	ASSETMAN_Prefabs,
	ASSETMAN_Audio,

	NUM_ASSETMANTYPES
};

enum AssetErrorType
{
	ASSETERROR_CannotFindMetaFile = 0,
	ASSETERROR_CouldNotPack,

	NUM_ASSETERRORS
};

enum AuxTab
{
	AUXTAB_Log = 0,			// Always visible
	AUXTAB_AssetInspector,	// Visible when Asset Manager Docking Window is visible
	AUXTAB_DopeSheet,		// Only visible when an Entity is the current item opened
	AUXTAB_ShaderEditor		// Only visible when a Shader is the current item opened
};

enum LoadingType
{
	LOADINGTYPE_Unknown = -1,

	LOADINGTYPE_Project = 0,
	LOADINGTYPE_ImportAssets,
	LOADINGTYPE_AtlasManager,
	LOADINGTYPE_AudioManager,
	LOADINGTYPE_ReloadHarmony,
	LOADINGTYPE_HarmonyStreaming
};

enum MimeType
{
	MIMETYPE_ProjectItems = 0,
	MIMETYPE_AssetItems,
	MIMETYPE_EntityItems,
	MIMETYPE_EntityFrames,

	NUM_MIMETYPES
};

enum EditorShape
{
	SHAPE_None = 0,
	SHAPE_Box,
	SHAPE_Circle,
	SHAPE_Polygon,
	SHAPE_LineSegment,
	SHAPE_LineChain,
	SHAPE_LineLoop,

	NUM_SHAPES
};

enum TweenFuncType
{
	TWEENFUNC_Unknown = -1,

	TWEENFUNC_Linear = 0,
	TWEENFUNC_QuadIn,
	TWEENFUNC_QuadOut,
	TWEENFUNC_QuadInOut,
	TWEENFUNC_CubeIn,
	TWEENFUNC_CubeOut,
	TWEENFUNC_CubeInOut,
	TWEENFUNC_QuartIn,
	TWEENFUNC_QuartOut,
	TWEENFUNC_QuartInOut,
	TWEENFUNC_QuintIn,
	TWEENFUNC_QuintOut,
	TWEENFUNC_QuintInOut,
	TWEENFUNC_SineIn,
	TWEENFUNC_SineOut,
	TWEENFUNC_SineInOut,
	TWEENFUNC_BounceIn,
	TWEENFUNC_BounceOut,
	TWEENFUNC_BounceInOut,
	TWEENFUNC_CircIn,
	TWEENFUNC_CircOut,
	TWEENFUNC_CircInOut,
	TWEENFUNC_ExpoIn,
	TWEENFUNC_ExpoOut,
	TWEENFUNC_ExpoInOut,
	TWEENFUNC_BackIn,
	TWEENFUNC_BackOut,
	TWEENFUNC_BackInOut,

	NUM_TWEENFUNCS
};

enum TweenProperty
{
	TWEENPROP_None = -1,

	TWEENPROP_Position = 0,
	TWEENPROP_Rotation,
	TWEENPROP_Scale,
	TWEENPROP_Alpha,
	//TWEENPROP_TopColor, // TODO: Add these tweens, and implement all the logic and global data associated with them
	//TWEENPROP_BotColor,
	//TWEENPROP_TextLayerColor,
	//TWEENPROP_Volume,
	//TWEENPROP_Pitch,

	NUM_TWEENPROPS
};

enum TextStyle
{
	TEXTSTYLE_Unknown = -1,

	TEXTSTYLE_Line,
	TEXTSTYLE_Column,
	TEXTSTYLE_ScaleBox,
	TEXTSTYLE_ScaleBoxTopAlign,
	TEXTSTYLE_Vertical,

	NUM_TEXTSTYLES
};

enum SnappingSetting
{
	SNAPSETTING_ToleranceMask	= 0x0000FFFF,
	SNAPSETTING_Enabled			= 1 << 16,
	SNAPSETTING_Grid			= 1 << 17,
	SNAPSETTING_Guides			= 1 << 18,
	SNAPSETTING_Origin			= 1 << 19,
	SNAPSETTING_Items			= 1 << 20,
	SNAPSETTING_ItemMidPoints	= 1 << 21,
	// NOTE: Cannot exceed 32 bits (SnappingSetting is stored as a uint32)

	SNAPSETTING_DefaultSettings = 20 | SNAPSETTING_Enabled | SNAPSETTING_Guides | SNAPSETTING_Origin | SNAPSETTING_Items | SNAPSETTING_ItemMidPoints
};

enum SubIcon
{
	SUBICON_None = 0,
	SUBICON_New,
	SUBICON_Open,
	SUBICON_Dirty,
	SUBICON_Close,
	SUBICON_Delete,
	SUBICON_Settings,
	SUBICON_Warning,
	SUBICON_Activated,

	NUM_SUBICONS
};

enum TemplateFileType
{
	TEMPLATE_Main = 0,
	TEMPLATE_Pch,
	TEMPLATE_MainClassCpp,
	TEMPLATE_MainClassH,
	TEMPLATE_ClassCpp,
	TEMPLATE_ClassH,

	TEMPLATE_EntityCpp,
	TEMPLATE_EntityH
};

enum Theme
{
	THEME_Decemberween = 0,
	THEME_CorpyNT6,
	THEME_Lappy486,
	THEME_Compe,

	NUM_THEMES
};

//#define HYTREEWIDGETITEM_IsFilter				"HyTreeFilter"

#define HYGUIPATH_TempDir						"temp/"
#define HYGUIPATH_TEMPSUBDIR_ImportTileSheet	"TileSheets"
#define HYGUIPATH_TEMPSUBDIR_PasteAssets		"PasteAssets"

#define HYGUIPATH_EditorDataDir					"Editor/data/"
#define HYGUIPATH_ProjGenDir					"Editor/data/_projGen/"
#define HYGUIPATH_AstcEncDir					"Editor/external/astcenc/"

#define HYGUIPATH_ItemsFileName					"Items"
#define HYGUIPATH_MetaExt						".meta"
#define HYGUIPATH_DataExt						".data"
#define HYGUIPATH_UserExt						".user"

#define HYMETA_FontsDir							"Fonts/"

#define HYMETA_ImageFilterList					{"*.png", "*.*"}
#define HYMETA_FontFilterList					{"*.ttf", "*.otf"}

QAction *FindAction(QList<QAction *> list, QString sName);
#define FINDACTION(str) FindAction(this->actions(), str)

#define HyGuiLog(msg, type) AuxOutputLog::Log(QString(msg), type)

#define JSONOBJ_TOINT(obj, key) obj.value(key).toVariant().toLongLong()

char *QStringToCharPtr(QString sString);

QString PointToQString(QPointF ptPoint);
QPointF StringToPoint(QString sPoint);
QByteArray JsonValueToSrc(QJsonValue value);

struct FileDataPair
{
	QJsonObject	m_Data;
	QJsonObject	m_Meta;
};

enum DopeSheetEventType
{
	DOPEEVENT_Callback = -1,
	DOPEEVENT_PauseTimeline,
	DOPEEVENT_GotoFrame,

	NUM_DOPEEVENTS
};
struct DopeSheetEvent
{
	const QString DOPEEVENT_STRINGS[NUM_DOPEEVENTS] = { "_PauseTimeline", "_GotoFrame=" };

	DopeSheetEventType	m_eType;
	QString				m_sData;

	DopeSheetEvent(QString sSerializedEvent) :
		m_eType(DOPEEVENT_Callback),
		m_sData(sSerializedEvent)
	{
		for(int iDopeEventIndex = 0; iDopeEventIndex < NUM_DOPEEVENTS; ++iDopeEventIndex)
		{
			if(sSerializedEvent.startsWith(DOPEEVENT_STRINGS[iDopeEventIndex]))
			{
				m_eType = static_cast<DopeSheetEventType>(iDopeEventIndex);
				// Parse data after '=' character
				m_sData = sSerializedEvent.split('=').last();
				break;
			}
		}
	}
};

enum EntityItemDeclarationType
{
	ENTDECLTYPE_Unknown = -1,

	ENTDECLTYPE_Static = 0,
	ENTDECLTYPE_Dynamic,
	ENTDECLTYPE_DynamicLeaked,

	NUM_ENTDECLTYPES
};
const QString ENTITYITEMDECLARATIONTYPE_STRINGS[NUM_ENTDECLTYPES] = { "Static", "Dynamic", "DynamicLeaked" };

class HyGlobal
{
	static QString														sm_sItemNames[NUM_ITEMTYPES];
	static QString														sm_sItemNamesPlural[NUM_ITEMTYPES];
	static QString														sm_AssetNames[NUM_ASSETMANTYPES];
	static QString														sm_ShapeNames[NUM_SHAPES];
	static QString														sm_TweenFuncNames[NUM_TWEENFUNCS];
	static QString														sm_TweenPropNames[NUM_TWEENPROPS];
	static QString														sm_sSubIconNames[NUM_SUBICONS];
	static QStringList													sm_sTextStyleList;
	static QStringList													sm_sAlignmentList;

	static QIcon														sm_ItemIcons[NUM_ITEMTYPES][NUM_SUBICONS];
	static QIcon														sm_TweenPropIcons[NUM_TWEENPROPS];
	static QColor														sm_ItemColors[NUM_ITEMTYPES];

	static QString														sm_MimeTypes[NUM_MIMETYPES];

	static QString														sm_Themes[NUM_THEMES];

	static QRegExpValidator *											sm_pCodeNameValidator;
	static QRegExpValidator *											sm_pFreeFormValidator;
	static QRegExpValidator *											sm_pFileNameValidator;
	static QRegExpValidator *											sm_pFilePathValidator;
	static QRegExpValidator *											sm_pNumbersValidator;
	static QRegExpValidator *											sm_pVector2dValidator;

	static QString														sm_ErrorStrings[NUM_ASSETERRORS];

public:
	static void Initialize();

	static QList<ItemType> GetItemTypeList();
	static QList<ItemType> GetItemTypeList_Project();
	static QList<ItemType> GetItemTypeList_Asset();
	static QList<EditorShape> GetShapeList();
	static QList<TweenFuncType> GetTweenFuncList();
	static QList<TweenProperty> GetTweenPropList();
	static QStringList GetTypeNameList();
	static ItemType GetTypeFromString(QString sType);
	static QStringList GetShapeNameList();
	static EditorShape GetShapeFromString(QString sShape);
	static QStringList GetTweenFuncNameList();
	static TweenFuncType GetTweenFuncFromString(QString sTween);
	static TweenProperty GetTweenPropFromString(QString sTweenProp);
	static QStringList GetTweenPropNameList();
	static HyTweenFunc GetTweenFunc(TweenFuncType eTween);
	static QStringList GetTextStyleNameList();
	static TextStyle GetTextStyleFromString(QString sTextStyle);
	static QStringList GetAlignmentNameList();
	static HyAlignment GetAlignmentFromString(QString sAlignment);

	static bool IsItemType_Project(ItemType eType);
	static bool IsItemType_Asset(ItemType eType);

	static const QString ItemName(ItemType eItem, bool bPlural)			{ return bPlural ? sm_sItemNamesPlural[eItem] : sm_sItemNames[eItem]; }
	static const QString AssetName(AssetManagerType eAsset)				{ return sm_AssetNames[eAsset]; }
	static const QString ShapeName(EditorShape eShape)					{ return sm_ShapeNames[eShape]; }
	static const QString TweenFuncName(TweenFuncType eTween)			{ return sm_TweenFuncNames[eTween]; }
	static const QString TweenPropName(TweenProperty eTweenProp)		{ return sm_TweenPropNames[eTweenProp]; }
	static const QIcon TweenPropIcon(TweenProperty eTweenProp)			{ return sm_TweenPropIcons[eTweenProp]; }
	static const QString ItemExt(ItemType eItem);
	static const QIcon ItemIcon(ItemType eItm, SubIcon eSubIcon)		{ return sm_ItemIcons[eItm][eSubIcon]; }
	static const QIcon AssetIcon(AssetManagerType eAsset, SubIcon eSubIcon);
	static const QColor ItemColor(ItemType eItem)						{ return sm_ItemColors[eItem]; }
	static const QString MimeTypeString(MimeType eMimeType)				{ return sm_MimeTypes[eMimeType]; }
	static const QString ThemeString(Theme eTheme)						{ return sm_Themes[eTheme]; }

	static const QRegExpValidator *CodeNameValidator()					{ return sm_pCodeNameValidator; }
	static const QRegExpValidator *FreeFormValidator()					{ return sm_pFreeFormValidator; }
	static const QRegExpValidator *FileNameValidator()					{ return sm_pFileNameValidator; }
	static const QRegExpValidator *FilePathValidator()					{ return sm_pFilePathValidator; }
	static const QRegExpValidator *NumbersValidator()					{ return sm_pNumbersValidator; }
	static const QRegExpValidator *Vector2dValidator()					{ return sm_pVector2dValidator; }

	static const QString GetGuiFrameErrors(uint uiErrorFlags);

	static bool IsEngineDirValid(const QDir &engineDir);

	static quint32 CRCData(quint32 crc, const uchar *buf, size_t len);

	static QString MakeFileNameFromCounter(int iCount);

	static void RecursiveFindFileOfExt(QString sExt, QStringList &appendList, QDir dirEntry);

	static QString GetTreeWidgetItemPath(const QTreeWidgetItem *pItem);

	// Includes 'pParentItem' in returned list
	static QList<QTreeWidgetItem *> RecursiveTreeChildren(QTreeWidgetItem *pParentItem);

	static QPair<QString, QString> ConvertTweenPropToRegularPropPair(TweenProperty eTweenProp);

	static QDir PrepTempDir(Project &projectRef, QString sDirName);
	static void CleanAllTempDirs(Project &projectRef);

	static bool IsItemFileDataValid(const FileDataPair &itemfileDataRef);
	static FileDataPair GenerateNewItemFileData(QString sImportPath = "");

	static void ModifyJsonObject(QJsonObject &objRef, const QString &path, const QJsonValue &newValue);
	static void ModifyJsonObject(QJsonDocument &docRef, const QString &sPath, const QJsonValue &newValue);

	static void OpenFileInExplorer(QString sFilePath);

	static QColor ConvertHyColor(HyColor color);

	static EntityItemDeclarationType GetEntityDeclType(QString sType);
};

struct SortTreeWidgetsPredicate
{
	bool operator()(const QTreeWidgetItem *pA, const QTreeWidgetItem *pB) const
	{
		return QString::compare(pA->text(0), pB->text(0)) < 0;
	}
};

#endif // GLOBAL_H
