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
/*static*/ QString HyGlobal::sm_TweenNames[NUM_TWEENS];
/*static*/ QString HyGlobal::sm_TweenPropNames[NUM_TWEENPROPS];
/*static*/ QString HyGlobal::sm_sSubIconNames[NUM_SUBICONS];
/*static*/ QStringList HyGlobal::sm_sTextStyleList;
/*static*/ QStringList HyGlobal::sm_sAlignmentList;

/*static*/ QIcon HyGlobal::sm_ItemIcons[NUM_ITEMTYPES][NUM_SUBICONS];
/*static*/ QIcon HyGlobal::sm_TweenPropIcons[NUM_TWEENPROPS];
/*static*/ QColor HyGlobal::sm_ItemColors[NUM_ITEMTYPES];

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

	sm_TweenNames[TWEEN_Linear] = "Linear";
	sm_TweenNames[TWEEN_QuadIn] = "QuadIn";
	sm_TweenNames[TWEEN_QuadOut] = "QuadOut";
	sm_TweenNames[TWEEN_QuadInOut] = "QuadInOut";
	sm_TweenNames[TWEEN_CubeIn] = "CubeIn";
	sm_TweenNames[TWEEN_CubeOut] = "CubeOut";
	sm_TweenNames[TWEEN_CubeInOut] = "CubeInOut";
	sm_TweenNames[TWEEN_QuartIn] = "QuartIn";
	sm_TweenNames[TWEEN_QuartOut] = "QuartOut";
	sm_TweenNames[TWEEN_QuartInOut] = "QuartInOut";
	sm_TweenNames[TWEEN_QuintIn] = "QuintIn";
	sm_TweenNames[TWEEN_QuintOut] = "QuintOut";
	sm_TweenNames[TWEEN_QuintInOut] = "QuintInOut";
	sm_TweenNames[TWEEN_SineIn] = "SineIn";
	sm_TweenNames[TWEEN_SineOut] = "SineOut";
	sm_TweenNames[TWEEN_SineInOut] = "SineInOut";
	sm_TweenNames[TWEEN_BounceIn] = "BounceIn";
	sm_TweenNames[TWEEN_BounceOut] = "BounceOut";
	sm_TweenNames[TWEEN_BounceInOut] = "BounceInOut";
	sm_TweenNames[TWEEN_CircIn] = "CircIn";
	sm_TweenNames[TWEEN_CircOut] = "CircOut";
	sm_TweenNames[TWEEN_CircInOut] = "CircInOut";
	sm_TweenNames[TWEEN_ExpoIn] = "ExpoIn";
	sm_TweenNames[TWEEN_ExpoOut] = "ExpoOut";
	sm_TweenNames[TWEEN_ExpoInOut] = "ExpoInOut";
	sm_TweenNames[TWEEN_BackIn] = "BackIn";
	sm_TweenNames[TWEEN_BackOut] = "BackOut";
	sm_TweenNames[TWEEN_BackInOut] = "BackInOut";

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

	sm_sTextStyleList << "Line" << "Column" << "Scale Box" << "Scale Box (top align)" << "Vertical";
	sm_sAlignmentList << "Left" << "Center" << "Right" << "Justify";

	sm_ItemColors[ITEM_Project] = QColor(9, 215, 33);
	sm_ItemColors[ITEM_Prefix] = QColor(228, 212, 128);
	sm_ItemColors[ITEM_Primitive] = QColor(101, 233, 235);
	sm_ItemColors[ITEM_Audio] = QColor(203, 233, 131);
	sm_ItemColors[ITEM_Particles] = QColor(218, 0, 0);
	sm_ItemColors[ITEM_Text] = QColor(179, 179, 179);
	sm_ItemColors[ITEM_Spine] = QColor(209, 159, 223);
	sm_ItemColors[ITEM_Sprite] = QColor(129, 166, 225);
	sm_ItemColors[ITEM_Prefab] = QColor(203, 233, 131);
	sm_ItemColors[ITEM_Entity] = QColor(128, 128, 128);
	sm_ItemColors[ITEM_Entity3d] = QColor(128, 128, 128);
	sm_ItemColors[ITEM_Filter] = QColor(228, 212, 128);
	sm_ItemColors[ITEM_AtlasFrame] = QColor(45, 131, 176);
	sm_ItemColors[ITEM_SoundClip] = QColor(203, 233, 131);
	sm_ItemColors[ITEM_Source] = QColor(225, 151, 97);
	sm_ItemColors[ITEM_Header] = QColor(225, 151, 97);
	sm_ItemColors[ITEM_BoundingVolume] = QColor(234, 232, 58);
	sm_ItemColors[ITEM_Physics] = QColor(201, 58, 203);

	sm_MimeTypes[MIMETYPE_ProjectItems] = "application/x-hyprojitems";
	sm_MimeTypes[MIMETYPE_AssetItems] = "application/x-hyassetitems";
	sm_MimeTypes[MIMETYPE_EntityItems] = "application/x-hyentityitems";

	sm_Themes[THEME_Decemberween] = "Decemberween";
	sm_Themes[THEME_CorpyNT6] = "Corpy NT6";
	sm_Themes[THEME_Lappy486] = "Lappy 486";
	sm_Themes[THEME_Compe] = "Compe";

	sm_pCodeNameValidator = new QRegExpValidator(QRegExp("[A-Za-z_]+[A-Za-z0-9_]*"));					// Can't begin with number. Alphanumberic and underscore only.
	sm_pFreeFormValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_\\-\\s]*"));				// Alphanumeric and valid filename symbols. Allows whitespace.
	sm_pFileNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_\\-]*"));					// Alphanumeric and valid filename symbols.
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

/*static*/ QList<TweenType> HyGlobal::GetTweenList()
{
	QList<TweenType> list;
	list.append(TWEEN_Linear);
	list.append(TWEEN_QuadIn);
	list.append(TWEEN_QuadOut);
	list.append(TWEEN_QuadInOut);
	list.append(TWEEN_CubeIn);
	list.append(TWEEN_CubeOut);
	list.append(TWEEN_CubeInOut);
	list.append(TWEEN_QuartIn);
	list.append(TWEEN_QuartOut);
	list.append(TWEEN_QuartInOut);
	list.append(TWEEN_QuintIn);
	list.append(TWEEN_QuintOut);
	list.append(TWEEN_QuintInOut);
	list.append(TWEEN_SineIn);
	list.append(TWEEN_SineOut);
	list.append(TWEEN_SineInOut);
	list.append(TWEEN_BounceIn);
	list.append(TWEEN_BounceOut);
	list.append(TWEEN_BounceInOut);
	list.append(TWEEN_CircIn);
	list.append(TWEEN_CircOut);
	list.append(TWEEN_CircInOut);
	list.append(TWEEN_ExpoIn);
	list.append(TWEEN_ExpoOut);
	list.append(TWEEN_ExpoInOut);
	list.append(TWEEN_BackIn);
	list.append(TWEEN_BackOut);
	list.append(TWEEN_BackInOut);

	if(list.size() != NUM_TWEENS)
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

/*static*/ QStringList HyGlobal::GetTweenNameList()
{
	QList<TweenType> tweenList = GetTweenList();

	QStringList list;
	for(int i = 0; i < tweenList.size(); ++i)
		list.append(TweenName(tweenList[i]));

	return list;
}

/*static*/ TweenType HyGlobal::GetTweenFromString(QString sTween)
{
	for(int i = 0; i < NUM_TWEENS; ++i)
	{
		if(sTween.compare(TweenName(static_cast<TweenType>(i)), Qt::CaseInsensitive) == 0)
			return static_cast<TweenType>(i);
	}

	return TWEEN_Unknown;
}

/*static*/ TweenProperty HyGlobal::GetTweenPropFromString(QString sTweenProp)
{
	for(int i = 0; i < NUM_TWEENPROPS; ++i)
	{
		if(sTweenProp.compare(TweenPropName(static_cast<TweenProperty>(i)), Qt::CaseInsensitive) == 0)
			return static_cast<TweenProperty>(i);
	}

	return TWEENPROP_Unknown;
}

/*static*/ HyTweenFunc HyGlobal::GetTweenFunc(TweenType eTween)
{
	switch(eTween)
	{
	case TWEEN_Linear:		return HyTween::Linear;
	case TWEEN_QuadIn:		return HyTween::QuadIn;
	case TWEEN_QuadOut:		return HyTween::QuadOut;
	case TWEEN_QuadInOut:	return HyTween::QuadInOut;
	case TWEEN_CubeIn:		return HyTween::CubeIn;
	case TWEEN_CubeOut:		return HyTween::CubeOut;
	case TWEEN_CubeInOut:	return HyTween::CubeInOut;
	case TWEEN_QuartIn:		return HyTween::QuartIn;
	case TWEEN_QuartOut:	return HyTween::QuartOut;
	case TWEEN_QuartInOut:	return HyTween::QuartInOut;
	case TWEEN_QuintIn:		return HyTween::QuintIn;
	case TWEEN_QuintOut:	return HyTween::QuintOut;
	case TWEEN_QuintInOut:	return HyTween::QuintInOut;
	case TWEEN_SineIn:		return HyTween::SineIn;
	case TWEEN_SineOut:		return HyTween::SineOut;
	case TWEEN_SineInOut:	return HyTween::SineInOut;
	case TWEEN_BounceIn:	return HyTween::BounceIn;
	case TWEEN_BounceOut:	return HyTween::BounceOut;
	case TWEEN_BounceInOut:	return HyTween::BounceInOut;
	case TWEEN_CircIn:		return HyTween::CircIn;
	case TWEEN_CircOut:		return HyTween::CircOut;
	case TWEEN_CircInOut:	return HyTween::CircInOut;
	case TWEEN_ExpoIn:		return HyTween::ExpoIn;
	case TWEEN_ExpoOut:		return HyTween::ExpoOut;
	case TWEEN_ExpoInOut:	return HyTween::ExpoInOut;
	case TWEEN_BackIn:		return HyTween::BackIn;
	case TWEEN_BackOut:		return HyTween::BackOut;
	case TWEEN_BackInOut:	return HyTween::BackInOut;

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

/*static*/ QStringList HyGlobal::GetTextStyleNameList()
{
	return sm_sTextStyleList;
}

/*static*/ TextStyle HyGlobal::GetTextStyleFromString(QString sTextStyle)
{
	for(int i = 0; i < NUM_TEXTSTYLES; ++i)
	{
		if(sTextStyle.compare(sm_sTextStyleList[i], Qt::CaseInsensitive) == 0)
			return static_cast<TextStyle>(i);
	}

	return TEXTSTYLE_Unknown;
}


/*static*/ QStringList HyGlobal::GetAlignmentNameList()
{
	return sm_sAlignmentList;
}

/*static*/ HyAlignment HyGlobal::GetAlignmentFromString(QString sAlignment)
{
	for(int i = 0; i < HYNUM_ALIGNMENTS; ++i)
	{
		if(sAlignment.compare(sm_sAlignmentList[i], Qt::CaseInsensitive) == 0)
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

/*static*/ QPair<QString, QString> HyGlobal::GetTweenCategoryProperty(TweenProperty eTweenProp)
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
