/**************************************************************************
 *	Global.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "Project.h"

#include "SpineModel.h"

#include <QProcess>

/*static*/ QString HyGlobal::sm_sItemNames[NUM_ITEMTYPES];
/*static*/ QString HyGlobal::sm_sItemNamesPlural[NUM_ITEMTYPES];
/*static*/ QString HyGlobal::sm_AssetNames[NUM_ASSETMANTYPES];
/*static*/ QString HyGlobal::sm_ShapeNames[NUM_SHAPES];
/*static*/ QString HyGlobal::sm_TweenFuncNames[NUM_TWEENFUNCS];
/*static*/ QString HyGlobal::sm_TweenPropNames[NUM_TWEENPROPS];
/*static*/ QString HyGlobal::sm_sSubIconNames[NUM_SUBICONS];
/*static*/ QString HyGlobal::sm_sTextTypeNames[HYNUM_TEXTTYPES];
/*static*/ QString HyGlobal::sm_sAlignmentNames[HYNUM_ALIGNMENTS];

/*static*/ QIcon HyGlobal::sm_ItemIcons[NUM_ITEMTYPES][NUM_SUBICONS];
/*static*/ QIcon HyGlobal::sm_TweenPropIcons[NUM_TWEENPROPS];
/*static*/ HyColor HyGlobal::sm_EditorColors[NUM_EDITORCOLORS];

/*static*/ QString HyGlobal::sm_MimeTypes[NUM_MIMETYPES];

/*static*/ QString HyGlobal::sm_Themes[NUM_THEMES];

/*static*/ QRegExpValidator *HyGlobal::sm_pCodeNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFreeFormValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFileNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFilePathValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pNumbersValidator  = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pVector2dValidator = nullptr;

/*static*/ QString HyGlobal::sm_ErrorStrings[NUM_ASSETERRORS];

/*static*/ void HyGlobal::Initialize()
{
	sm_sItemNames[ITEM_Project] = "Project";
	sm_sItemNames[ITEM_Prefix] = "Prefix";
	sm_sItemNames[ITEM_Primitive] = "Primitive";
	sm_sItemNames[ITEM_Audio] = "Audio";
	sm_sItemNames[ITEM_Particles] = "Particles";
	sm_sItemNames[ITEM_Text] = "Text";
	sm_sItemNames[ITEM_Spine] = "Spine";
	sm_sItemNames[ITEM_Sprite] = "Sprite";
	sm_sItemNames[ITEM_Shader] = "Shader";
	sm_sItemNames[ITEM_Prefab] = "Prefab";
	sm_sItemNames[ITEM_Entity] = "Entity";
	sm_sItemNames[ITEM_Entity3d] = "Entity3d";
	sm_sItemNames[ITEM_Filter] = "Filter";
	sm_sItemNames[ITEM_AtlasFrame] = "AtlasFrame";
	sm_sItemNames[ITEM_SoundClip] = "SoundClip";
	sm_sItemNames[ITEM_Source] = "Source";
	sm_sItemNames[ITEM_Header] = "Header";
	sm_sItemNames[ITEM_BoundingVolume] = "BoundingVolume";
	sm_sItemNames[ITEM_Physics] = "Physics";

	sm_sItemNamesPlural[ITEM_Project] = "Projects";
	sm_sItemNamesPlural[ITEM_Prefix] = "Prefixes";
	sm_sItemNamesPlural[ITEM_Primitive] = "Primitives";
	sm_sItemNamesPlural[ITEM_Audio] = "Audio";
	sm_sItemNamesPlural[ITEM_Particles] = "Particles";
	sm_sItemNamesPlural[ITEM_Text] = "Texts";
	sm_sItemNamesPlural[ITEM_Spine] = "Spine";
	sm_sItemNamesPlural[ITEM_Sprite] = "Sprites";
	sm_sItemNamesPlural[ITEM_Shader] = "Shaders";
    sm_sItemNamesPlural[ITEM_Prefab] = "Prefabs";
	sm_sItemNamesPlural[ITEM_Entity] = "Entities";
	sm_sItemNamesPlural[ITEM_Entity3d] = "Entities3d";
	sm_sItemNamesPlural[ITEM_Filter] = "Filters";
	sm_sItemNamesPlural[ITEM_AtlasFrame] = "AtlasFrames";
	sm_sItemNamesPlural[ITEM_SoundClip] = "SoundClips";
	sm_sItemNamesPlural[ITEM_Source] = "Source";
	sm_sItemNamesPlural[ITEM_Header] = "Headers";
	sm_sItemNamesPlural[ITEM_BoundingVolume] = "BoundingVolumes";
	sm_sItemNamesPlural[ITEM_Physics] = "Physics";

	sm_AssetNames[ASSETMAN_Source] = "Source";
	sm_AssetNames[ASSETMAN_Atlases] = "Atlases";
	sm_AssetNames[ASSETMAN_Prefabs] = "Prefabs";
	sm_AssetNames[ASSETMAN_Audio] = "Audio";

	sm_ShapeNames[SHAPE_None] = "None";
	sm_ShapeNames[SHAPE_Box] = "Box";
	sm_ShapeNames[SHAPE_Circle] = "Circle";
	sm_ShapeNames[SHAPE_Polygon] = "Polygon";
	sm_ShapeNames[SHAPE_LineSegment] = "Line Segment";
	sm_ShapeNames[SHAPE_LineChain] = "Line Chain";
	sm_ShapeNames[SHAPE_LineLoop] = "Line Loop";

	sm_TweenFuncNames[TWEENFUNC_Linear] = "Linear";
	sm_TweenFuncNames[TWEENFUNC_QuadIn] = "QuadIn";
	sm_TweenFuncNames[TWEENFUNC_QuadOut] = "QuadOut";
	sm_TweenFuncNames[TWEENFUNC_QuadInOut] = "QuadInOut";
	sm_TweenFuncNames[TWEENFUNC_CubeIn] = "CubeIn";
	sm_TweenFuncNames[TWEENFUNC_CubeOut] = "CubeOut";
	sm_TweenFuncNames[TWEENFUNC_CubeInOut] = "CubeInOut";
	sm_TweenFuncNames[TWEENFUNC_QuartIn] = "QuartIn";
	sm_TweenFuncNames[TWEENFUNC_QuartOut] = "QuartOut";
	sm_TweenFuncNames[TWEENFUNC_QuartInOut] = "QuartInOut";
	sm_TweenFuncNames[TWEENFUNC_QuintIn] = "QuintIn";
	sm_TweenFuncNames[TWEENFUNC_QuintOut] = "QuintOut";
	sm_TweenFuncNames[TWEENFUNC_QuintInOut] = "QuintInOut";
	sm_TweenFuncNames[TWEENFUNC_SineIn] = "SineIn";
	sm_TweenFuncNames[TWEENFUNC_SineOut] = "SineOut";
	sm_TweenFuncNames[TWEENFUNC_SineInOut] = "SineInOut";
	sm_TweenFuncNames[TWEENFUNC_BounceIn] = "BounceIn";
	sm_TweenFuncNames[TWEENFUNC_BounceOut] = "BounceOut";
	sm_TweenFuncNames[TWEENFUNC_BounceInOut] = "BounceInOut";
	sm_TweenFuncNames[TWEENFUNC_CircIn] = "CircIn";
	sm_TweenFuncNames[TWEENFUNC_CircOut] = "CircOut";
	sm_TweenFuncNames[TWEENFUNC_CircInOut] = "CircInOut";
	sm_TweenFuncNames[TWEENFUNC_ExpoIn] = "ExpoIn";
	sm_TweenFuncNames[TWEENFUNC_ExpoOut] = "ExpoOut";
	sm_TweenFuncNames[TWEENFUNC_ExpoInOut] = "ExpoInOut";
	sm_TweenFuncNames[TWEENFUNC_BackIn] = "BackIn";
	sm_TweenFuncNames[TWEENFUNC_BackOut] = "BackOut";
	sm_TweenFuncNames[TWEENFUNC_BackInOut] = "BackInOut";

	sm_TweenPropNames[TWEENPROP_Position] = "Position";
	sm_TweenPropNames[TWEENPROP_Rotation] = "Rotation";
	sm_TweenPropNames[TWEENPROP_Scale] = "Scale";
	sm_TweenPropNames[TWEENPROP_Alpha] = "Alpha";

	sm_sSubIconNames[SUBICON_None] = "";
	sm_sSubIconNames[SUBICON_New] = "-New";
	sm_sSubIconNames[SUBICON_Open] = "-Open";
	sm_sSubIconNames[SUBICON_Dirty] = "-Dirty";
	sm_sSubIconNames[SUBICON_Close] = "-Close";
	sm_sSubIconNames[SUBICON_Delete] = "-Delete";
	sm_sSubIconNames[SUBICON_Settings] = "-Settings";
	sm_sSubIconNames[SUBICON_Warning] = "-Warning";
	sm_sSubIconNames[SUBICON_Activated] = "-Pending";

	for(int i = 0; i < NUM_ITEMTYPES; ++i)
	{
		for(int j = 0; j < NUM_SUBICONS; ++j)
		{
			QString sUrl = ":/icons16x16/items/" % sm_sItemNames[i] % sm_sSubIconNames[j] % ".png";
			sm_ItemIcons[i][j].addFile(sUrl);
		}
	}

	for(int i = 0; i < NUM_TWEENPROPS; ++i)
	{
		QString sUrl = ":/icons16x16/tween-" % sm_TweenPropNames[i] % ".png";
		sm_TweenPropIcons[i].addFile(sUrl);
	}

	sm_sTextTypeNames[HYTEXT_Line] = "Lines";
	sm_sTextTypeNames[HYTEXT_Column] = "Column";
	sm_sTextTypeNames[HYTEXT_Box] = "Box";
	sm_sTextTypeNames[HYTEXT_ScaleBox] = "Scale Box";
	sm_sTextTypeNames[HYTEXT_Vertical] = "Vertical";

	sm_sAlignmentNames[HYALIGN_Left] = "Left";
	sm_sAlignmentNames[HYALIGN_Center] = "Center";
	sm_sAlignmentNames[HYALIGN_Right] = "Right";
	sm_sAlignmentNames[HYALIGN_Justify] = "Justify";

	sm_EditorColors[EDITORCOLOR_Origin] = HyColor::White;
	sm_EditorColors[EDITORCOLOR_OriginStroke] = HyColor::Black;
	sm_EditorColors[EDITORCOLOR_GridColor1] = HyColor(106, 105, 113, 255);
	sm_EditorColors[EDITORCOLOR_GridColor2] = HyColor(93, 93, 97, 255);
	sm_EditorColors[EDITORCOLOR_GridOverlay] = HyColor::Black;
	sm_EditorColors[EDITORCOLOR_Rulers] = HyColor::PanelContainer;
	sm_EditorColors[EDITORCOLOR_RulersNotch] = HyColor::FrameWidget;
	sm_EditorColors[EDITORCOLOR_RulersText] = HyColor::FrameWidget;
	sm_EditorColors[EDITORCOLOR_RulersTextShadow] = HyColor::Black;
	sm_EditorColors[EDITORCOLOR_RulersMousePos] = HyColor::Cyan;
	sm_EditorColors[EDITORCOLOR_PropertiesCategory] = HyColor::PanelWidget;
	sm_EditorColors[EDITORCOLOR_PropertiesItem] = HyColor::FrameWidget;
	sm_EditorColors[EDITORCOLOR_Marquee] = HyColor::Blue.Lighten();
	sm_EditorColors[EDITORCOLOR_TransformBoundingVolume] = HyColor::Orange;// HyColor::Blue.Lighten();
	sm_EditorColors[EDITORCOLOR_TransformGrabPointOutline] = HyColor::White;
	sm_EditorColors[EDITORCOLOR_TransformGrabPointFill] = HyColor::Blue.Lighten();
	sm_EditorColors[EDITORCOLOR_TransformGrabPointSelectedOutline] = HyColor::White;
	sm_EditorColors[EDITORCOLOR_TransformGrabPointSelectedFill] = HyColor::Blue.Lighten();
	sm_EditorColors[EDITORCOLOR_Shape] = HyColor::DarkMagenta;
	sm_EditorColors[EDITORCOLOR_ShapeGrabPointOutline] = HyColor::White;
	sm_EditorColors[EDITORCOLOR_ShapeGrabPointFill] = HyColor::Red;
	sm_EditorColors[EDITORCOLOR_ShapeGrabPointSelectedOutline] = HyColor::White;
	sm_EditorColors[EDITORCOLOR_ShapeGrabPointSelectedFill] = HyColor::Blue.Lighten();
	sm_EditorColors[EDITORCOLOR_DopeSheet] = HyColor::PanelWidget;
	sm_EditorColors[EDITORCOLOR_DopeSheetTimeline] = HyColor::PanelContainer;
	sm_EditorColors[EDITORCOLOR_DopeSheetNotch] = HyColor::FrameWidget;
	sm_EditorColors[EDITORCOLOR_DopeSheetCurFrameIndicator] = HyColor::Cyan;
	sm_EditorColors[EDITORCOLOR_DopeSheetText] = HyColor::FrameWidget;
	sm_EditorColors[EDITORCOLOR_DopeSheetTextSelected] = HyColor::LightGray;
	sm_EditorColors[EDITORCOLOR_DopeSheetTextHover] = HyColor::White;
	sm_EditorColors[EDITORCOLOR_DopeSheetTextShadow] = HyColor::Black;
	sm_EditorColors[EDITORCOLOR_DopeSheetItemsColumn] = HyColor::PanelContainer;
	sm_EditorColors[EDITORCOLOR_DopeSheetKeyFrameOutline] = HyColor::Black;
	sm_EditorColors[EDITORCOLOR_DopeSheetKeyFrameFill] = HyColor::LightGray;
	sm_EditorColors[EDITORCOLOR_DopeSheetKeyFrameHover] = HyColor::White;
	sm_EditorColors[EDITORCOLOR_DopeSheetTweenFrameOutline] = HyColor::Black;
	sm_EditorColors[EDITORCOLOR_DopeSheetTweenFrameFill] = HyColor::Green;
	sm_EditorColors[EDITORCOLOR_DopeSheetTweenFrameHover] = HyColor::White;

	sm_MimeTypes[MIMETYPE_ProjectItems] = "application/x-hyprojitems";
	sm_MimeTypes[MIMETYPE_AssetItems] = "application/x-hyassetitems";
	sm_MimeTypes[MIMETYPE_EntityItems] = "application/x-hyentityitems";
	sm_MimeTypes[MIMETYPE_EntityFrames] = "application/x-hyentityframes";

	sm_Themes[THEME_Decemberween] = "Decemberween";
	sm_Themes[THEME_CorpyNT6] = "Corpy NT6";
	sm_Themes[THEME_Lappy486] = "Lappy 486";
	sm_Themes[THEME_Compe] = "Compe";

	sm_pCodeNameValidator = new QRegExpValidator(QRegExp("^[A-Za-z_]+[A-Za-z0-9_]*$"));					// Can't begin with number. Alphanumberic and underscore only.
	sm_pFreeFormValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_\\-\\s]*"));				// Alphanumeric and valid filename symbols. Allows whitespace.
	sm_pFileNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\.\\(\\)|_\\-]*"));				// Alphanumeric and valid filename symbols. Allows periods '.'
	sm_pFilePathValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|/_\\-]*"));					// Alphanumeric and valid filename symbols. Allows forward slashes.
	sm_pNumbersValidator  = new QRegExpValidator(QRegExp("[0-9]*"));									// Numbers only
	sm_pVector2dValidator = new QRegExpValidator(QRegExp("\\([0-9]*\\.?[0-9]*,[0-9]*\\.?[0-9]*\\)"));	// When user needs to input values for a 2d vector

	sm_ErrorStrings[ASSETERROR_CannotFindMetaFile] = "Cannot find source meta-image";
	sm_ErrorStrings[ASSETERROR_CouldNotPack] = "Could not pack this frame in atlas";
}

/*static*/ QList<ItemType> HyGlobal::GetItemTypeList()
{
	QList<ItemType> list;

	list.append(ITEM_Project);
	list.append(ITEM_Prefix);
	list.append(ITEM_Primitive);
	list.append(ITEM_Audio);
	list.append(ITEM_Particles);
	list.append(ITEM_Text);
	list.append(ITEM_Spine);
	list.append(ITEM_Sprite);
	list.append(ITEM_Shader);
	list.append(ITEM_Prefab);
	list.append(ITEM_Entity);
	list.append(ITEM_Entity3d);
	list.append(ITEM_Filter);
	list.append(ITEM_AtlasFrame);
	list.append(ITEM_SoundClip);
	list.append(ITEM_Source);
	list.append(ITEM_Header);
	list.append(ITEM_BoundingVolume);
	list.append(ITEM_Physics);

	if(list.size() != NUM_ITEMTYPES)
		HyGuiLog("HyGlobal::GetTypeList missing a type!", LOGTYPE_Error);

	return list;
}

/*static*/ QList<ItemType> HyGlobal::GetItemTypeList_Project()
{
	QList<ItemType> list;
	list.append(ITEM_Audio);
	list.append(ITEM_Particles);
	list.append(ITEM_Text);
	list.append(ITEM_Spine);
	list.append(ITEM_Sprite);
	list.append(ITEM_Shader);
	list.append(ITEM_Prefab);
	list.append(ITEM_Entity);
	list.append(ITEM_Entity3d);

	return list;
}

/*static*/ QList<ItemType> HyGlobal::GetItemTypeList_Asset()
{
	QList<ItemType> list;
	list.append(ITEM_AtlasFrame);
	list.append(ITEM_SoundClip);
	list.append(ITEM_Source);
	list.append(ITEM_Header);

	return list;
}

/*static*/ QList<EditorShape> HyGlobal::GetShapeList()
{
	QList<EditorShape> list;
	list.append(SHAPE_None);
	list.append(SHAPE_Box);
	list.append(SHAPE_Circle);
	list.append(SHAPE_Polygon);
	list.append(SHAPE_LineSegment);
	list.append(SHAPE_LineChain);
	list.append(SHAPE_LineLoop);

	if(list.size() != NUM_SHAPES)
		HyGuiLog("HyGlobal::GetShapeList missing a type!", LOGTYPE_Error);

	return list;
}

/*static*/ QList<TweenFuncType> HyGlobal::GetTweenFuncList()
{
	QList<TweenFuncType> list;
	list.append(TWEENFUNC_Linear);
	list.append(TWEENFUNC_QuadIn);
	list.append(TWEENFUNC_QuadOut);
	list.append(TWEENFUNC_QuadInOut);
	list.append(TWEENFUNC_CubeIn);
	list.append(TWEENFUNC_CubeOut);
	list.append(TWEENFUNC_CubeInOut);
	list.append(TWEENFUNC_QuartIn);
	list.append(TWEENFUNC_QuartOut);
	list.append(TWEENFUNC_QuartInOut);
	list.append(TWEENFUNC_QuintIn);
	list.append(TWEENFUNC_QuintOut);
	list.append(TWEENFUNC_QuintInOut);
	list.append(TWEENFUNC_SineIn);
	list.append(TWEENFUNC_SineOut);
	list.append(TWEENFUNC_SineInOut);
	list.append(TWEENFUNC_BounceIn);
	list.append(TWEENFUNC_BounceOut);
	list.append(TWEENFUNC_BounceInOut);
	list.append(TWEENFUNC_CircIn);
	list.append(TWEENFUNC_CircOut);
	list.append(TWEENFUNC_CircInOut);
	list.append(TWEENFUNC_ExpoIn);
	list.append(TWEENFUNC_ExpoOut);
	list.append(TWEENFUNC_ExpoInOut);
	list.append(TWEENFUNC_BackIn);
	list.append(TWEENFUNC_BackOut);
	list.append(TWEENFUNC_BackInOut);

	if(list.size() != NUM_TWEENFUNCS)
		HyGuiLog("HyGlobal::GetTweenList missing a type!", LOGTYPE_Error);

	return list;
}

/*static*/ QList<TweenProperty> HyGlobal::GetTweenPropList()
{
	QList<TweenProperty> list;
	list.append(TWEENPROP_Position);
	list.append(TWEENPROP_Rotation);
	list.append(TWEENPROP_Scale);
	list.append(TWEENPROP_Alpha);
	
	if(list.size() != NUM_TWEENPROPS)
		HyGuiLog("HyGlobal::GetTweenPropList missing a type!", LOGTYPE_Error);

	return list;
}

/*static*/ QStringList HyGlobal::GetTypeNameList()
{
	QList<ItemType> dirList = GetItemTypeList();

	QStringList list;
	for(int i = 0; i < dirList.size(); ++i)
		list.append(sm_sItemNames[dirList[i]]);

	return list;
}

/*static*/ ItemType HyGlobal::GetTypeFromString(QString sType)
{
	QStringList sTypeList = GetTypeNameList();
	for(int i = 0; i < sTypeList.size(); ++i)
	{
		if(sType.compare(sTypeList[i], Qt::CaseInsensitive) == 0)
			return GetItemTypeList()[i];
	}

	return ITEM_Unknown;
}

/*static*/ QStringList HyGlobal::GetShapeNameList()
{
	QList<EditorShape> shapeList = GetShapeList();

	QStringList list;
	for(int i = 0; i < shapeList.size(); ++i)
		list.append(ShapeName(shapeList[i]));

	return list;
}

/*static*/ EditorShape HyGlobal::GetShapeFromString(QString sShape)
{
	for(int i = 0; i < NUM_SHAPES; ++i)
	{
		if(sShape.compare(ShapeName(static_cast<EditorShape>(i)), Qt::CaseInsensitive) == 0)
			return static_cast<EditorShape>(i);
	}

	return SHAPE_None;
}

/*static*/ QStringList HyGlobal::GetTweenFuncNameList()
{
	QList<TweenFuncType> tweenList = GetTweenFuncList();

	QStringList list;
	for(int i = 0; i < tweenList.size(); ++i)
		list.append(TweenFuncName(tweenList[i]));

	return list;
}

/*static*/ TweenFuncType HyGlobal::GetTweenFuncFromString(QString sTween)
{
	for(int i = 0; i < NUM_TWEENFUNCS; ++i)
	{
		if(sTween.compare(TweenFuncName(static_cast<TweenFuncType>(i)), Qt::CaseInsensitive) == 0)
			return static_cast<TweenFuncType>(i);
	}

	return TWEENFUNC_Unknown;
}

/*static*/ TweenProperty HyGlobal::GetTweenPropFromString(QString sTweenProp)
{
	for(int i = 0; i < NUM_TWEENPROPS; ++i)
	{
		if(sTweenProp.compare(TweenPropName(static_cast<TweenProperty>(i)), Qt::CaseInsensitive) == 0)
			return static_cast<TweenProperty>(i);
	}

	return TWEENPROP_None;
}

/*static*/ HyTweenFunc HyGlobal::GetTweenFunc(TweenFuncType eTween)
{
	switch(eTween)
	{
	case TWEENFUNC_Linear:		return HyTween::Linear;
	case TWEENFUNC_QuadIn:		return HyTween::QuadIn;
	case TWEENFUNC_QuadOut:		return HyTween::QuadOut;
	case TWEENFUNC_QuadInOut:	return HyTween::QuadInOut;
	case TWEENFUNC_CubeIn:		return HyTween::CubeIn;
	case TWEENFUNC_CubeOut:		return HyTween::CubeOut;
	case TWEENFUNC_CubeInOut:	return HyTween::CubeInOut;
	case TWEENFUNC_QuartIn:		return HyTween::QuartIn;
	case TWEENFUNC_QuartOut:	return HyTween::QuartOut;
	case TWEENFUNC_QuartInOut:	return HyTween::QuartInOut;
	case TWEENFUNC_QuintIn:		return HyTween::QuintIn;
	case TWEENFUNC_QuintOut:	return HyTween::QuintOut;
	case TWEENFUNC_QuintInOut:	return HyTween::QuintInOut;
	case TWEENFUNC_SineIn:		return HyTween::SineIn;
	case TWEENFUNC_SineOut:		return HyTween::SineOut;
	case TWEENFUNC_SineInOut:	return HyTween::SineInOut;
	case TWEENFUNC_BounceIn:	return HyTween::BounceIn;
	case TWEENFUNC_BounceOut:	return HyTween::BounceOut;
	case TWEENFUNC_BounceInOut:	return HyTween::BounceInOut;
	case TWEENFUNC_CircIn:		return HyTween::CircIn;
	case TWEENFUNC_CircOut:		return HyTween::CircOut;
	case TWEENFUNC_CircInOut:	return HyTween::CircInOut;
	case TWEENFUNC_ExpoIn:		return HyTween::ExpoIn;
	case TWEENFUNC_ExpoOut:		return HyTween::ExpoOut;
	case TWEENFUNC_ExpoInOut:	return HyTween::ExpoInOut;
	case TWEENFUNC_BackIn:		return HyTween::BackIn;
	case TWEENFUNC_BackOut:		return HyTween::BackOut;
	case TWEENFUNC_BackInOut:	return HyTween::BackInOut;

	default:
		HyGuiLog("HyGlobal::GetTweenFunc missing a type!", LOGTYPE_Error);
		break;
	}

	return nullptr;
}

/*static*/ QStringList HyGlobal::GetTweenPropNameList()
{
	QList<TweenProperty> tweenPropList = GetTweenPropList();

	QStringList list;
	for(int i = 0; i < tweenPropList.size(); ++i)
		list.append(TweenPropName(tweenPropList[i]));

	return list;
}

/*static*/ QStringList HyGlobal::GetTextTypeNameList()
{
	QStringList list;
	for(int i = 0; i < HYNUM_TEXTTYPES; ++i)
		list.append(sm_sTextTypeNames[i]);
	
	return list;
}

/*static*/ HyTextType HyGlobal::GetTextTypeFromString(QString sTextStyle)
{
	for(int i = 0; i < HYNUM_TEXTTYPES; ++i)
	{
		if(sTextStyle.compare(sm_sTextTypeNames[i], Qt::CaseInsensitive) == 0)
			return static_cast<HyTextType>(i);
	}

	return HYTEXT_Unknown;
}


/*static*/ QStringList HyGlobal::GetAlignmentNameList()
{
	QStringList list;
	for(int i = 0; i < HYNUM_ALIGNMENTS; ++i)
		list.append(sm_sAlignmentNames[i]);
	
	return list;
}

/*static*/ HyAlignment HyGlobal::GetAlignmentFromString(QString sAlignment)
{
	for(int i = 0; i < HYNUM_ALIGNMENTS; ++i)
	{
		if(sAlignment.compare(sm_sAlignmentNames[i], Qt::CaseInsensitive) == 0)
			return static_cast<HyAlignment>(i);
	}

	return HYALIGN_Unknown;
}

/*static*/ bool HyGlobal::IsItemType_Project(ItemType eType)
{
	QList<ItemType> projItemList = GetItemTypeList_Project();
	return projItemList.contains(eType);
}

/*static*/ bool HyGlobal::IsItemType_Asset(ItemType eType)
{
	QList<ItemType> assetItemList = GetItemTypeList_Asset();
	return assetItemList.contains(eType);
}

/*static*/ const QString HyGlobal::ItemExt(ItemType eItem)
{
	switch(eItem)
	{
	case ITEM_Project:
		return ".hyproj";
	}

	return "";
}

/*static*/ const QIcon HyGlobal::AssetIcon(AssetManagerType eAsset, SubIcon eSubIcon)
{
	switch(eAsset)
	{
	case ASSETMAN_Source:
		return sm_ItemIcons[ITEM_Source][eSubIcon];
	case ASSETMAN_Atlases:
		return sm_ItemIcons[ITEM_AtlasFrame][eSubIcon];
	case ASSETMAN_Prefabs:
		return sm_ItemIcons[ITEM_Prefab][eSubIcon];
	case ASSETMAN_Audio:
		return sm_ItemIcons[ITEM_SoundClip][eSubIcon];

	case ASSETMAN_Unknown:
	default:
		return QIcon();
	}
}

/*static*/ const QString HyGlobal::GetGuiFrameErrors(uint uiErrorFlags)
{
	QString sErrorString;
	sErrorString.clear();

	for(int i = 0; i < NUM_ASSETERRORS; ++i)
	{
		if((uiErrorFlags & (1 << i)) != 0)
		{
			if(sErrorString.isEmpty())
				sErrorString += sm_ErrorStrings[i];
			else
				sErrorString += "; \n" % sm_ErrorStrings[i];
		}
	}

	return sErrorString;
}

/*static*/ bool HyGlobal::IsEngineDirValid(const QDir &engineDir)
{
	QDir dir(engineDir);
	if(!dir.exists())
		return false;

	if(!dir.exists("Editor/") ||
	   !dir.exists("Engine/") ||
	   !dir.exists("CMakeLists.txt"))
	{
		return false;
	}

	return true;
}

/*static*/ quint32 HyGlobal::CRCData(quint32 crc, const uchar *buf, size_t len)
{
	static quint32 table[256];
	static int have_table = 0;
	quint32 rem, octet;
	const uchar *p, *q;

	/* This check is not thread safe; there is no mutex. */
	if(have_table == 0)
	{
		/* Calculate CRC table. */
		for(int i = 0; i < 256; i++)
		{
			rem = i;  /* remainder from polynomial division */
			for(int j = 0; j < 8; j++)
			{
				if(rem & 1)
				{
					rem >>= 1;
					rem ^= 0xedb88320;
				}
				else
				{
					rem >>= 1;
				}
			}
			table[i] = rem;
		}
		have_table = 1;
	}

	crc = ~crc;
	q = buf + len;
	for(p = buf; p < q; p++)
	{
		octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}

/*static*/ QString HyGlobal::MakeFileNameFromCounter(int iCount)
{
	QString sNewString = QString("%1").arg(iCount, 5, 10, QChar('0'));

	return sNewString;
}

/*static*/ void HyGlobal::RecursiveFindFileOfExt(QString sExt, QStringList &appendList, QDir dirEntry)
{
	if(sExt.startsWith('.'))
		sExt.remove(0, 1);
	sExt = sExt.toLower();

	QFileInfoList list = dirEntry.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	QStack<QFileInfoList> dirStack;
	dirStack.push(list);

	while(dirStack.isEmpty() == false)
	{
		list = dirStack.pop();
		for(int i = 0; i < list.count(); i++)
		{
			QFileInfo info = list[i];
			QString sFileName = info.fileName();
			QString sFilePath = info.filePath();
			QString sSuffix = info.suffix().toLower();

			if(info.isDir())// && info.fileName() != ".." && info.fileName() != ".")
			{
				QDir subDir(info.filePath());
				QFileInfoList subList = subDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

				dirStack.push(subList);
			}
			else if(info.suffix().toLower() == sExt)
			{
				appendList.push_back(info.filePath());
			}
		}
	}
}

/*static*/ QString HyGlobal::GetTreeWidgetItemPath(const QTreeWidgetItem *pItem)
{
	QString sPath = "";
	while(pItem)
	{
		sPath = "/" % pItem->text(0) % sPath;
		pItem = pItem->parent();
	}

	return sPath;
}

// Includes 'pParentItem' in returned list
/*static*/ QList<QTreeWidgetItem *> HyGlobal::RecursiveTreeChildren(QTreeWidgetItem *pParentItem)
{
	QStack<QTreeWidgetItem *> itemStack;
	itemStack.push(pParentItem);

	QList<QTreeWidgetItem *> returnList;
	QStringList testList;
	while(!itemStack.isEmpty())
	{
		QTreeWidgetItem *pItem = itemStack.pop();
		returnList.append(pItem);
		testList.append(pItem->text(0));

		for(int i = 0; i < pItem->childCount(); ++i)
			itemStack.push(pItem->child(i));
	}

	return returnList;
}

/*static*/ QPair<QString, QString> HyGlobal::ConvertTweenPropToRegularPropPair(TweenProperty eTweenProp)
{
	switch(eTweenProp)
	{
	case TWEENPROP_Position:
	case TWEENPROP_Rotation:
	case TWEENPROP_Scale:
		return QPair<QString, QString>("Transformation", sm_TweenPropNames[eTweenProp]);
		break;

	case TWEENPROP_Alpha:
		return QPair<QString, QString>("Body", sm_TweenPropNames[eTweenProp]);
		break;

	default:
		HyGuiLog("EntityDopeSheetScene::GetUniquePropertiesList - unhandled tween property", LOGTYPE_Error);
		break;
	}

	return QPair<QString, QString>();
}

/*static*/ QDir HyGlobal::PrepTempDir(Project &projectRef, QString sDirName)
{
	QDir metaTempDir(projectRef.GetMetaAbsPath() % '/' % HYGUIPATH_TempDir);
	if(false == metaTempDir.mkpath("."))
		HyGuiLog("Could not make meta temp directory", LOGTYPE_Error);

	QDir metaTempSubDir(metaTempDir.absoluteFilePath(sDirName));
	if(metaTempSubDir.exists())
	{
		QFileInfoList tempFileInfoList = metaTempSubDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
		for(int i = 0; i < tempFileInfoList.size(); ++i)
		{
			if(false == QFile::remove(tempFileInfoList[i].absoluteFilePath()))
				HyGuiLog("Could not remove temp file: " % tempFileInfoList[i].fileName(), LOGTYPE_Error);
		}
	}
	else if(false == metaTempSubDir.mkdir("."))
		HyGuiLog("Could not make meta temp directory", LOGTYPE_Error);
	
	return metaTempSubDir;
}

/*static*/ void HyGlobal::CleanAllTempDirs(Project &projectRef)
{
	QDir metaTempDir(projectRef.GetMetaAbsPath() % '/' % HYGUIPATH_TempDir);
	if(metaTempDir.exists())
	{
		if(false == metaTempDir.removeRecursively())
			HyGuiLog("Failed to metaTempDir.removeRecursively", LOGTYPE_Error);
	}
}

/*static*/ bool HyGlobal::IsItemFileDataValid(const FileDataPair &itemfileDataRef)
{
	std::function<bool(const QJsonArray &)> fpMetaStateArrayCheck = [](const QJsonArray &metaStateArrayRef)->bool
	{
		for(int i = 0; i < metaStateArrayRef.size(); ++i)
		{
			if(metaStateArrayRef[i].isObject() == false ||
				metaStateArrayRef[i].toObject().contains("name") == false ||
				metaStateArrayRef[i].toObject()["name"].isString() == false)
			{
				return false;
			}
		}

		return true;
	};

	return (itemfileDataRef.m_Meta.contains("stateArray") && itemfileDataRef.m_Meta["stateArray"].isArray() &&
		itemfileDataRef.m_Data.contains("stateArray") && itemfileDataRef.m_Meta["stateArray"].isArray() &&
		itemfileDataRef.m_Meta["stateArray"].toArray().count() == itemfileDataRef.m_Data["stateArray"].toArray().count() &&
		fpMetaStateArrayCheck(itemfileDataRef.m_Meta["stateArray"].toArray()) &&
		itemfileDataRef.m_Meta.contains("CameraPos") && itemfileDataRef.m_Meta["CameraPos"].isArray() &&
		itemfileDataRef.m_Meta.contains("CameraZoom") && itemfileDataRef.m_Meta["CameraZoom"].isDouble() &&
		itemfileDataRef.m_Meta.contains("UUID") && itemfileDataRef.m_Meta["UUID"].isString());
}

/*static*/ FileDataPair HyGlobal::GenerateNewItemFileData(QString sImportPath /*= ""*/)
{
	FileDataPair newItemFileData;

	newItemFileData.m_Meta["stateArray"] = QJsonArray();
	newItemFileData.m_Data["stateArray"] = QJsonArray();

	QJsonArray cameraPosArray;
	cameraPosArray.append(0);
	cameraPosArray.append(0);
	newItemFileData.m_Meta["CameraPos"] = cameraPosArray;
	newItemFileData.m_Meta["CameraZoom"] = 1;

	newItemFileData.m_Meta["UUID"] = QUuid::createUuid().toString(QUuid::WithoutBraces);

	if(sImportPath.isEmpty() == false)
		newItemFileData.m_Meta["newImport"] = sImportPath;

	if(IsItemFileDataValid(newItemFileData) == false)
		HyGuiLog("ExplorerModel::GenerateNewItemFileData - IsItemFileDataValid return false", LOGTYPE_Error);

	return newItemFileData;
}

/*static*/ void HyGlobal::ModifyJsonObject(QJsonObject &objRef, const QString &path, const QJsonValue &newValue)
{
	const int indexOfDot = path.indexOf('.');
	const QString propertyName = path.left(indexOfDot);
	const QString subPath = indexOfDot>0 ? path.mid(indexOfDot+1) : QString();

	QJsonValue subValue = objRef[propertyName];

	if(subPath.isEmpty())
		subValue = newValue;
	else
	{
		QJsonObject obj = subValue.toObject();
		ModifyJsonObject(obj, subPath, newValue);
		subValue = obj;
	}

	objRef[propertyName] = subValue;
}

/*static*/ void HyGlobal::ModifyJsonObject(QJsonDocument &docRef, const QString &sPath, const QJsonValue &newValue)
{
	QJsonObject obj = docRef.object();
	ModifyJsonObject(obj, sPath, newValue);

	docRef = QJsonDocument(obj);
}

/*static*/ void HyGlobal::OpenFileInExplorer(QString sAbsFilePath)
{
	#if defined(Q_OS_MACOS)
	QStringList args;
	args << "-e";
	args << "tell application \"Finder\"";
	args << "-e";
	args << "activate";
	args << "-e";
	args << "select POSIX file \"" + sAbsFilePath + "\"";
	args << "-e";
	args << "end tell";
	QProcess::startDetached("osascript", args);
#elif defined(Q_OS_WIN)
	QStringList args;
	args << "/select," << QDir::toNativeSeparators(sAbsFilePath);
	QProcess::startDetached("explorer", args);
#endif
}

/*static*/ QColor HyGlobal::ConvertHyColor(HyColor color)
{
	return QColor(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

/*static*/ EntityItemDeclarationType HyGlobal::GetEntityDeclType(QString sType)
{
	for(int i = 0; i < NUM_ENTDECLTYPES; ++i)
	{
		if(sType == ENTITYITEMDECLARATIONTYPE_STRINGS[i])
			return static_cast<EntityItemDeclarationType>(i);
	}
	
	return ENTDECLTYPE_Static;// ENTDECLTYPE_Unknown;
}

QMap<int, QList<TimelineEvent>> HyGlobal::AssembleTimelineEvents(const QMap<int, QJsonObject> &itemKeyFrameMapRef)
{
	QMap<int, QList<TimelineEvent>> timelineEventMap;
	for(auto iter = itemKeyFrameMapRef.begin(); iter != itemKeyFrameMapRef.end(); ++iter)
	{
		QJsonObject keyFrameObj = iter.value();
		if(keyFrameObj.contains("Timeline"))
		{
			QList<TimelineEvent> eventList;

			QJsonObject timelineCategoryObj = keyFrameObj["Timeline"].toObject();

			if(timelineCategoryObj.contains("Pause"))
				eventList.append(TimelineEvent(TIMELINEEVENT_PauseTimeline, timelineCategoryObj["Pause"]));
			if(timelineCategoryObj.contains("Frame"))
				eventList.append(TimelineEvent(TIMELINEEVENT_GotoPrevFrame, timelineCategoryObj["Frame"]));
			if(timelineCategoryObj.contains("State"))
				eventList.append(TimelineEvent(TIMELINEEVENT_GotoState, timelineCategoryObj["State"]));

			timelineEventMap.insert(iter.key(), eventList);
		}
	}

	return timelineEventMap;
}

QAction *FindAction(QList<QAction *> list, QString sName)
{
	for(int i = 0; i < list.size(); ++i)
	{
		if(list[i]->objectName() == sName)
			return list[i];
	}
	
	return nullptr;
}

char *QStringToCharPtr(QString sString)
{
	QByteArray ba;
	ba.append(sString);
	return ba.data();
}

QString PointToQString(QPointF ptPoint)
{
	return "(" % QString::number(ptPoint.x(), 'g', 4) % "," % QString::number(ptPoint.y(), 'g', 4) % ")";
}

QPointF StringToPoint(QString sPoint)
{
	sPoint.replace(QString("("), QString(""));
	sPoint.replace(QString(")"), QString(""));
	QStringList sComponentList = sPoint.split(',');
	
	if(sComponentList.size() < 2)
		return QPointF(0.0f, 0.0f);
	else
		return QPointF(sComponentList[0].toFloat(), sComponentList[1].toFloat());
}

QByteArray JsonValueToSrc(QJsonValue value)
{
	QByteArray src;
	if(value.isArray())
	{
		QJsonDocument tmpDoc(value.toArray());
		src = tmpDoc.toJson();
	}
	else
	{
		QJsonDocument tmpDoc(value.toObject());
		src = tmpDoc.toJson();
	}

	return src;
}
