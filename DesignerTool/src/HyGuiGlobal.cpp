/**************************************************************************
 *	HyGlobal.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGuiGlobal.h"
#include "Project.h"

/*static*/ QString HyGlobal::sm_sItemNames[NUMTYPES];
/*static*/ QString HyGlobal::sm_sSubIconNames[NUM_SUBICONS];

/*static*/ QIcon HyGlobal::sm_ItemIcons[NUMITEMS][NUM_SUBICONS];

/*static*/ QRegExpValidator *HyGlobal::sm_pCodeNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFileNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFilePathValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pVector2dValidator = nullptr;

/*static*/ QString HyGlobal::sm_ErrorStrings[NUMATLASFRAMEERROR];

/*static*/ void HyGlobal::Initialize()
{
    sm_sItemNames[DIR_Audio] = "Audio";
    sm_sItemNames[DIR_Particles] = "Particles";
    sm_sItemNames[DIR_Fonts] = "Fonts";
    sm_sItemNames[DIR_Spine] = "Spine";
    sm_sItemNames[DIR_Sprites] = "Sprites";
    sm_sItemNames[DIR_Shaders] = "Shaders";
    sm_sItemNames[DIR_Entities] = "Entities";
    sm_sItemNames[DIR_Atlases] = "Atlases";
    sm_sItemNames[DIR_AudioBanks] = "AudioBanks";
    sm_sItemNames[ITEM_Project] = "Project";
    sm_sItemNames[ITEM_Primitive] = "Primitive";
    sm_sItemNames[ITEM_Audio] = "Audio";
    sm_sItemNames[ITEM_Particles] = "Particles";
    sm_sItemNames[ITEM_Font] = "Font";
    sm_sItemNames[ITEM_Spine] = "Spine";
    sm_sItemNames[ITEM_Sprite] = "Sprite";
    sm_sItemNames[ITEM_Shader] = "Shader";
    sm_sItemNames[ITEM_Entity] = "Entity";
    sm_sItemNames[ITEM_AtlasImage] = "Atlas";
    sm_sItemNames[ITEM_Prefix] = "Prefix";
    sm_sItemNames[ITEM_Physics] = "Physics";
    sm_sItemNames[ITEM_BoundingVolume] = "BoundingVolume";

    sm_sSubIconNames[SUBICON_None] = "";
    sm_sSubIconNames[SUBICON_New] = "-New";
    sm_sSubIconNames[SUBICON_Open] = "-Open";
    sm_sSubIconNames[SUBICON_Dirty] = "-Dirty";
    sm_sSubIconNames[SUBICON_Close] = "-Close";
    sm_sSubIconNames[SUBICON_Delete] = "-Delete";
    sm_sSubIconNames[SUBICON_Settings] = "-Settings";
    sm_sSubIconNames[SUBICON_Warning] = "-Warning";
    sm_sSubIconNames[SUBICON_Pending] = "-Pending";

    sm_ItemIcons[DIR_Audio][SUBICON_None].addFile(QString(":/icons16x16/audio-folder.png"));
    sm_ItemIcons[DIR_Particles][SUBICON_None].addFile(QString(":/icons16x16/particle-folder.png"));
    sm_ItemIcons[DIR_Fonts][SUBICON_None].addFile(QString(":/icons16x16/font-folder.png"));
    sm_ItemIcons[DIR_Spine][SUBICON_None].addFile(QString(":/icons16x16/spine-folder.png"));
    sm_ItemIcons[DIR_Sprites][SUBICON_None].addFile(QString(":/icons16x16/sprite-folder.png"));
    sm_ItemIcons[DIR_Shaders][SUBICON_None].addFile(QString(":/icons16x16/shader-folder.png"));
    sm_ItemIcons[DIR_Entities][SUBICON_None].addFile(QString(":/icons16x16/entity-folder.png"));
    sm_ItemIcons[DIR_Atlases][SUBICON_None].addFile(QString(":/icons16x16/folder.png"));
    sm_ItemIcons[DIR_AudioBanks][SUBICON_None].addFile(QString(":/icons16x16/folder.png"));

    for(int i = NUMDIR; i < NUMTYPES; ++i)
    {
        for(int j = 0; j < NUM_SUBICONS; ++j)
        {
            QString sUrl = ":/icons16x16/items/" % sm_sItemNames[i] % sm_sSubIconNames[j] % ".png";
            sm_ItemIcons[i][j].addFile(sUrl);
        }
    }

    sm_pCodeNameValidator = new QRegExpValidator(QRegExp("[A-Za-z_]+[A-Za-z0-9_]*"));
    sm_pFileNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_\\-]*"));
    sm_pFilePathValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|/_\\-]*"));
    sm_pVector2dValidator = new QRegExpValidator(QRegExp("\\([0-9]*\\.?[0-9]*,[0-9]*\\.?[0-9]*\\)"));

    sm_ErrorStrings[ATLASFRAMEERROR_CannotFindMetaImg] = "Cannot find source meta-image";
    sm_ErrorStrings[ATLASFRAMEERROR_CouldNotPack] = "Could not pack this frame in atlas";
}

/*static*/ HyGuiItemType HyGlobal::GetDirFromItem(HyGuiItemType eItem)
{
    switch(eItem)
    {
    case ITEM_Audio:
    case DIR_Audio:
        return DIR_Audio;
    case ITEM_Particles:
    case DIR_Particles:
        return DIR_Particles;
    case ITEM_Font:
    case DIR_Fonts:
        return DIR_Fonts;
    case ITEM_Spine:
    case DIR_Spine:
        return DIR_Spine;
    case ITEM_Sprite:
    case DIR_Sprites:
        return DIR_Sprites;
    case ITEM_Shader:
    case DIR_Shaders:
        return DIR_Shaders;
    case ITEM_AtlasImage:
    case DIR_Atlases:
        return DIR_Atlases;
    case ITEM_Entity:
    case DIR_Entities:
        return DIR_Entities;
    default:
        HyGuiLog("HyGlobal::GetDirFromItem() could not find the proper directory item", LOGTYPE_Error);
    }

    return TYPE_Unknown;
}

/*static*/ HyGuiItemType HyGlobal::GetItemFromDir(HyGuiItemType eDir)
{
    switch(eDir)
    {
    case DIR_Audio:
    case ITEM_Audio:
        return ITEM_Audio;
    case DIR_Particles:
    case ITEM_Particles:
        return ITEM_Particles;
    case DIR_Fonts:
    case ITEM_Font:
        return ITEM_Font;
    case DIR_Spine:
    case ITEM_Spine:
        return ITEM_Spine;
    case DIR_Sprites:
    case ITEM_Sprite:
        return ITEM_Sprite;
    case DIR_Shaders:
    case ITEM_Shader:
        return ITEM_Shader;
    case DIR_Atlases:
    case ITEM_AtlasImage:
        return ITEM_AtlasImage;
    case DIR_Entities:
    case ITEM_Entity:
        return ITEM_Entity;
    default:
        HyGuiLog("HyGlobal::GetItemFromDir() could not find the proper item", LOGTYPE_Error);
    }

    return TYPE_Unknown;
}

/*static*/ HyGuiItemType HyGlobal::GetItemFromAtlasItem(AtlasItemType eFrameType)
{
    switch(eFrameType)
    {
    case ATLASITEM_Filter:
        return ITEM_Prefix;
    case ATLASITEM_Image:
        return ITEM_AtlasImage;
    case ATLASITEM_Font:
        return ITEM_Font;
    case ATLASITEM_Spine:
        return ITEM_Spine;
    default:
        HyGuiLog("HyGlobal::GetItemFromAtlasFrameType() could not find the proper item", LOGTYPE_Error);
    }

    return TYPE_Unknown;
}

/*static*/ AtlasItemType HyGlobal::GetAtlasItemFromItem(HyGuiItemType eItem)
{
    switch(eItem)
    {
    case ITEM_Prefix:
        return ATLASITEM_Filter;
    case ITEM_AtlasImage:
        return ATLASITEM_Image;
    case ITEM_Font:
        return ATLASITEM_Font;
    case ITEM_Spine:
        return ATLASITEM_Spine;
    default:
        HyGuiLog("HyGlobal::GetAtlasItemFromItem() could not find the atlas item", LOGTYPE_Error);
    }

    return ATLASITEM_Unknown;
}

/*static*/ QList<HyGuiItemType> HyGlobal::SubDirList()
{
    QList<HyGuiItemType> list;
    list.append(DIR_Audio);
    list.append(DIR_Particles);
    list.append(DIR_Fonts);
    list.append(DIR_Spine);
    list.append(DIR_Sprites);
    list.append(DIR_Shaders);
    list.append(DIR_Entities);
    list.append(DIR_Atlases);
    list.append(DIR_AudioBanks);

    return list;
}

/*static*/ QStringList HyGlobal::SubDirNameList()
{
    QList<HyGuiItemType> dirList = SubDirList();

    QStringList list;
    for(int i = 0; i < dirList.size(); ++i)
        list.append(sm_sItemNames[dirList[i]]);

    return list;
}

/*static*/ QString HyGlobal::AtlasTextureTypeString(HyTextureFormat eType)
{
    switch(eType)
    {
    case HYTEXTURE_R8G8B8A8:
        return "R8G8B8A8";
    case HYTEXTURE_R8G8B8:
        return "R8G8B8 (unsupported)";
    case HYTEXTURE_RGB_DTX1:
        return "RGB_DTX1 (unsupported)";
    case HYTEXTURE_RGBA_DTX1:
        return "RGBA_DTX1";
    case HYTEXTURE_DTX3:
        return "DTX3 (unsupported)";
    case HYTEXTURE_DTX5:
        return "DTX5";
    }

    return "Unknown";
}

/*static*/ const QString HyGlobal::ItemExt(HyGuiItemType eItem)
{
    switch(eItem)
    {
    case ITEM_Project:
        return ".hyproj";
    case ITEM_Prefix:
        return "/";
    }

    return "";
}

/*static*/ const QString HyGlobal::GetGuiFrameErrors(uint uiErrorFlags)
{
    QString sErrorString;
    sErrorString.clear();

    for(int i = 0; i < NUMATLASFRAMEERROR; ++i)
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

    if(!dir.exists("include/") ||
       !dir.exists("lib/") ||
       !dir.exists("src/") ||
       !dir.exists("templates/") ||
       (!dir.exists("Harmony_vs2013.sln") && !dir.exists("Harmony_vs2015.sln")))
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
    QString sNewString;
    sNewString.sprintf("%05d", iCount);

    return sNewString;
}

/*static*/ void HyGlobal::RecursiveFindFileOfExt(QString sExt, QStringList &appendList, QDir dirEntry)
{
    sExt = sExt.toLower();

    QFileInfoList list = dirEntry.entryInfoList();
    QStack<QFileInfoList> dirStack;
    dirStack.push(list);

    while(dirStack.isEmpty() == false)
    {
        list = dirStack.pop();
        for(int i = 0; i < list.count(); i++)
        {
            QFileInfo info = list[i];

            if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
            {
                QDir subDir(info.filePath());
                QFileInfoList subList = subDir.entryInfoList();

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

/*static*/ QDir HyGlobal::PrepTempDir(Project *pProject)
{
    QDir metaDir(pProject->GetMetaDataAbsPath());
    QDir metaTempDir(metaDir.absoluteFilePath(HYGUIPATH_TempDir));
    if(metaTempDir.exists())
    {
        QFileInfoList tempFileInfoList = metaTempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        for(int i = 0; i < tempFileInfoList.size(); ++i)
        {
            if(false == QFile::remove(tempFileInfoList[i].absoluteFilePath()))
                HyGuiLog("Could not remove temp file: " % tempFileInfoList[i].fileName(), LOGTYPE_Error);
        }
    }
    else if(false == metaTempDir.mkpath("."))
        HyGuiLog("Could not make meta temp directory", LOGTYPE_Error);

    return metaTempDir;
}

QAction *FindAction(QList<QAction *> list, QString sName)
{
    for(int i = 0; i < list.size(); ++i)
    {
        if(list[i]->objectName() == sName)
            return list[i];
    }
    
    return NULL;
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
