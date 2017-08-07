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

/*static*/ QString HyGlobal::sm_sItemNames[NUMITEM];
/*static*/ QString HyGlobal::sm_sItemExt[NUMITEM];
/*static*/ QString HyGlobal::sm_sSubIconNames[NUM_SUBICONS];
/*static*/ QIcon HyGlobal::sm_ItemIcons[NUMITEM][NUM_SUBICONS];

/*static*/ QRegExpValidator *HyGlobal::sm_pCodeNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFileNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFilePathValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pVector2dValidator = nullptr;

/*static*/ QString HyGlobal::sm_ErrorStrings[NUMGUIFRAMEERROR];

/*static*/ void HyGlobal::Initialize()
{
    sm_sItemNames[ITEM_Project] = "Project";
    sm_sItemNames[ITEM_DirAudio] = sm_sItemNames[ITEM_Audio] = "Audio";
    sm_sItemNames[ITEM_DirParticles] = sm_sItemNames[ITEM_Particles] = "Particles";
    sm_sItemNames[ITEM_DirFonts] = "Fonts";
    sm_sItemNames[ITEM_Font] = "Font";
    sm_sItemNames[ITEM_DirSpine] = sm_sItemNames[ITEM_Spine] = "Spine";
    sm_sItemNames[ITEM_DirSprites] = "Sprites";
    sm_sItemNames[ITEM_Sprite] = "Sprite";
    sm_sItemNames[ITEM_DirShaders] = "Shaders";
    sm_sItemNames[ITEM_Shader] = "Shader";
    sm_sItemNames[ITEM_DirEntities] = "Entities";
    sm_sItemNames[ITEM_Entity] = "Entity";
    sm_sItemNames[ITEM_DirAtlases] = "Atlases";
    sm_sItemNames[ITEM_AtlasImage] = "Atlas";
    sm_sItemNames[ITEM_DirAudioBanks] = "AudioBanks";
    sm_sItemNames[ITEM_Prefix] = "Prefix";

    sm_sItemExt[ITEM_Project] = ".hyproj";
    sm_sItemExt[ITEM_DirAudio] = "/";
    sm_sItemExt[ITEM_DirParticles] = "/";
    sm_sItemExt[ITEM_DirFonts] = "/";
    sm_sItemExt[ITEM_DirShaders] = "/";
    sm_sItemExt[ITEM_DirSpine] = "/";
    sm_sItemExt[ITEM_DirSprites] = "/";
    sm_sItemExt[ITEM_DirEntities] = "/";
    sm_sItemExt[ITEM_DirAtlases] = "/";
    sm_sItemExt[ITEM_DirAudioBanks] = "/";
    sm_sItemExt[ITEM_Prefix] = "/";
    sm_sItemExt[ITEM_AtlasImage] = "";
    sm_sItemExt[ITEM_Audio] = "";//".hyaud";
    sm_sItemExt[ITEM_Particles] = "";//".hypfx";
    sm_sItemExt[ITEM_Font] = "";//".hyfnt";
    sm_sItemExt[ITEM_Spine] = "";//".hyspi";
    sm_sItemExt[ITEM_Sprite] = "";//".hyspr";
    sm_sItemExt[ITEM_Shader] = "";
    sm_sItemExt[ITEM_Entity] = "";//".hyent";

    sm_sSubIconNames[SUBICON_None] = "";
    sm_sSubIconNames[SUBICON_New] = "-New";
    sm_sSubIconNames[SUBICON_Open] = "-Open";
    sm_sSubIconNames[SUBICON_Dirty] = "-Dirty";
    sm_sSubIconNames[SUBICON_Close] = "-Close";
    sm_sSubIconNames[SUBICON_Delete] = "-Delete";
    sm_sSubIconNames[SUBICON_Settings] = "-Settings";
    sm_sSubIconNames[SUBICON_Warning] = "-Warning";

    InitItemIcons(ITEM_Project);
    sm_ItemIcons[ITEM_DirAudio][SUBICON_None].addFile(QString(":/icons16x16/audio-folder.png"));
    sm_ItemIcons[ITEM_DirParticles][SUBICON_None].addFile(QString(":/icons16x16/particle-folder.png"));
    sm_ItemIcons[ITEM_DirFonts][SUBICON_None].addFile(QString(":/icons16x16/font-folder.png"));
    sm_ItemIcons[ITEM_DirSpine][SUBICON_None].addFile(QString(":/icons16x16/spine-folder.png"));
    sm_ItemIcons[ITEM_DirSprites][SUBICON_None].addFile(QString(":/icons16x16/sprite-folder.png"));
    sm_ItemIcons[ITEM_DirShaders][SUBICON_None].addFile(QString(":/icons16x16/shader-folder.png"));
    sm_ItemIcons[ITEM_DirEntities][SUBICON_None].addFile(QString(":/icons16x16/entity-folder.png"));
    sm_ItemIcons[ITEM_DirAtlases][SUBICON_None].addFile(QString(":/icons16x16/folder.png"));
    sm_ItemIcons[ITEM_DirAudioBanks][SUBICON_None].addFile(QString(":/icons16x16/folder.png"));
    InitItemIcons(ITEM_AtlasImage);
    InitItemIcons(ITEM_Prefix);
    InitItemIcons(ITEM_Audio);
    InitItemIcons(ITEM_Particles);
    InitItemIcons(ITEM_Font);
    InitItemIcons(ITEM_Spine);
    InitItemIcons(ITEM_Sprite);
    InitItemIcons(ITEM_Entity);

    sm_pCodeNameValidator = new QRegExpValidator(QRegExp("[A-Za-z_]+[A-Za-z0-9_]*"));
    sm_pFileNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_\\-]*"));
    sm_pFilePathValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|/_\\-]*"));
    sm_pVector2dValidator = new QRegExpValidator(QRegExp("\\([0-9]*\\.?[0-9]*,[0-9]*\\.?[0-9]*\\)"));

    sm_ErrorStrings[GUIFRAMEERROR_CannotFindMetaImg] = "Cannot find source meta-image";
    sm_ErrorStrings[GUIFRAMEERROR_CouldNotPack] = "Could not pack this frame in atlas";
}

/*static*/ HyGuiItemType HyGlobal::GetCorrespondingDirItem(HyGuiItemType eItem)
{
    switch(eItem)
    {
    case ITEM_Audio:
    case ITEM_DirAudio:
        return ITEM_DirAudio;
    case ITEM_Particles:
    case ITEM_DirParticles:
        return ITEM_DirParticles;
    case ITEM_Font:
    case ITEM_DirFonts:
        return ITEM_DirFonts;
    case ITEM_Spine:
    case ITEM_DirSpine:
        return ITEM_DirSpine;
    case ITEM_Sprite:
    case ITEM_DirSprites:
        return ITEM_DirSprites;
    case ITEM_Shader:
    case ITEM_DirShaders:
        return ITEM_DirShaders;
    case ITEM_AtlasImage:
    case ITEM_DirAtlases:
        return ITEM_DirAtlases;
    case ITEM_Entity:
    case ITEM_DirEntities:
        return ITEM_DirEntities;
    default:
        HyGuiLog("HyGlobal::GetCorrespondingDirItem() could not find the proper directory item", LOGTYPE_Warning);
    }

    return ITEM_Unknown;
}

/*static*/ HyGuiItemType HyGlobal::GetCorrespondingItemFromDir(HyGuiItemType eItem)
{
    switch(eItem)
    {
    case ITEM_Audio:
    case ITEM_DirAudio:
        return ITEM_Audio;
    case ITEM_Particles:
    case ITEM_DirParticles:
        return ITEM_Particles;
    case ITEM_Font:
    case ITEM_DirFonts:
        return ITEM_Font;
    case ITEM_Spine:
    case ITEM_DirSpine:
        return ITEM_Spine;
    case ITEM_Sprite:
    case ITEM_DirSprites:
        return ITEM_Sprite;
    case ITEM_Shader:
    case ITEM_DirShaders:
        return ITEM_Shader;
    case ITEM_AtlasImage:
    case ITEM_DirAtlases:
        return ITEM_AtlasImage;
    case ITEM_Entity:
    case ITEM_DirEntities:
        return ITEM_Entity;
    default:
        HyGuiLog("HyGlobal::GetCorrespondingItemFromDir() could not find the proper directory item", LOGTYPE_Warning);
    }

    return ITEM_Unknown;
}


/*static*/ QList<HyGuiItemType> HyGlobal::SubDirList()
{
    QList<HyGuiItemType> list;
    list.append(ITEM_DirAudio);
    list.append(ITEM_DirParticles);
    list.append(ITEM_DirFonts);
    list.append(ITEM_DirSpine);
    list.append(ITEM_DirSprites);
    list.append(ITEM_DirShaders);
    list.append(ITEM_DirEntities);
    list.append(ITEM_DirAtlases);
    list.append(ITEM_DirAudioBanks);

    return list;
}

/*static*/ QStringList HyGlobal::SubDirNameList()
{
    QList<HyGuiItemType> itemList = SubDirList();

    QStringList list;
    for(int i = 0; i < itemList.size(); ++i)
        list.append(sm_sItemNames[itemList[i]]);

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

/*static*/ const QString HyGlobal::GetGuiFrameErrors(uint uiErrorFlags)
{
    QString sErrorString;
    sErrorString.clear();

    for(int i = 0; i < NUMGUIFRAMEERROR; ++i)
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

QByteArray JsonValueToSrc(QJsonValue &valueRef)
{
    QByteArray src;
    if(valueRef.isArray())
    {
        QJsonDocument tmpDoc(valueRef.toArray());
        src = tmpDoc.toJson();
    }
    else
    {
        QJsonDocument tmpDoc(valueRef.toObject());
        src = tmpDoc.toJson();
    }

    return src;
}

/*static*/ void HyGlobal::InitItemIcons(HyGuiItemType eItemType)
{
    for(int i = 0; i < NUM_SUBICONS; ++i)
    {
        QString sFileUrl = ":/icons16x16/items/" % sm_sItemNames[eItemType] % sm_sSubIconNames[i] % ".png";
        sm_ItemIcons[eItemType][i].addFile(sFileUrl);
    }
}
