#ifndef HYGLOBALS_H
#define HYGLOBALS_H

#include <QString>
#include <QIcon>
#include <QValidator>
#include <QAction>
#include <QStringBuilder>
#include <QDir>

#include "WidgetOutputLog.h"

enum eItemType
{
    ITEM_Unknown = -1,
    
    ITEM_Project = 0,
    ITEM_DirAudio,
    ITEM_DirParticles,
    ITEM_DirFonts,
    ITEM_DirSpine,
    ITEM_DirSprites,
    ITEM_DirShaders,
    ITEM_DirEntities,
    ITEM_Prefix,        // Essentially a sub-directory
    ITEM_Audio,
    ITEM_Particles,
    ITEM_Font,
    ITEM_Spine,
    ITEM_Sprite,
    ITEM_Shader,
    ITEM_Entity,

    NUMITEM
};

enum eAtlasNodeType
{
    ATLAS_Texture = 0,
    ATLAS_Frame,
    ATLAS_Frame_Warning,
    ATLAS_Spine,
    ATLAS_Font,
    
    NUMATLAS
};

#define HYGUIPATH_RelDataDir                "data/"
#define HYGUIPATH_RelMetaDataDir            "_metaData/"
#define HYGUIPATH_RelSrcDataDir             "src/"

#define HYGUIPATH_RelDataAtlasDir           HYGUIPATH_RelDataDir "Atlas/"
#define HYGUIPATH_DataAtlasFileName         "atlasInfo.json"

#define HYGUIPATH_RelMetaDataTmpDir         HYGUIPATH_RelMetaDataDir "tmp/"

#define HYGUIPATH_RelMetaAtlasDir       HYGUIPATH_RelMetaDataDir "atlas/"
#define HYGUIPATH_MetaDataAtlasFileName     "settings.hygui"

class HyGlobal
{
    static QString                  sm_sItemNames[NUMITEM];
    static QString                  sm_sItemExt[NUMITEM];
    static QIcon                    sm_ItemIcons[NUMITEM];
    
    static QIcon                    sm_AtlasIcons[NUMATLAS];

    static QRegExpValidator *       sm_pFileNameValidator;
    static QRegExpValidator *       sm_pFilePathValidator;

public:
    static void Initialize()
    {
        // 'Dir' and 'item' both require the same name
        sm_sItemNames[ITEM_DirAudio] = sm_sItemNames[ITEM_Audio] = "Audio";
        sm_sItemNames[ITEM_DirParticles] = sm_sItemNames[ITEM_Particles] = "Particles";
        sm_sItemNames[ITEM_DirFonts] = sm_sItemNames[ITEM_Font] = "Font";
        sm_sItemNames[ITEM_DirSpine] = sm_sItemNames[ITEM_Spine] = "Spine";
        sm_sItemNames[ITEM_DirSprites] = sm_sItemNames[ITEM_Sprite] = "Sprite";
        sm_sItemNames[ITEM_DirShaders] = sm_sItemNames[ITEM_Shader] = "Shader";
        sm_sItemNames[ITEM_DirEntities] = sm_sItemNames[ITEM_Entity] = "Entity";
        
        sm_sItemExt[ITEM_Project] = "/";
        sm_sItemExt[ITEM_DirAudio] = "/";
        sm_sItemExt[ITEM_DirParticles] = "/";
        sm_sItemExt[ITEM_DirFonts] = "/";
        sm_sItemExt[ITEM_DirShaders] = "/";
        sm_sItemExt[ITEM_DirSpine] = "/";
        sm_sItemExt[ITEM_DirSprites] = "/";
        sm_sItemExt[ITEM_DirEntities] = "/";
        sm_sItemExt[ITEM_Prefix] = "/";
        sm_sItemExt[ITEM_Audio] = ".hyaud";
        sm_sItemExt[ITEM_Particles] = ".hypfx";
        sm_sItemExt[ITEM_Font] = ".hyfnt";
        sm_sItemExt[ITEM_Spine] = ".hyspi";
        sm_sItemExt[ITEM_Sprite] = ".hyspr";
        sm_sItemExt[ITEM_Shader] = "";
        sm_sItemExt[ITEM_Entity] = ".hyent";
        
        sm_ItemIcons[ITEM_Project].addFile(QString(":/icons16x16/project.png"));
        sm_ItemIcons[ITEM_DirAudio].addFile(QString(":/icons16x16/audio-folder.png"));
        sm_ItemIcons[ITEM_DirParticles].addFile(QString(":/icons16x16/particle-folder.png"));
        sm_ItemIcons[ITEM_DirFonts].addFile(QString(":/icons16x16/font-folder.png"));
        sm_ItemIcons[ITEM_DirSpine].addFile(QString(":/icons16x16/spine-folder.png"));
        sm_ItemIcons[ITEM_DirSprites].addFile(QString(":/icons16x16/sprite-folder.png"));
        sm_ItemIcons[ITEM_DirShaders].addFile(QString(":/icons16x16/shader-folder.png"));
        sm_ItemIcons[ITEM_DirEntities].addFile(QString(":/icons16x16/entity-folder.png"));
        sm_ItemIcons[ITEM_Prefix].addFile(QString(":/icons16x16/folder.png"));
        sm_ItemIcons[ITEM_Audio].addFile(QString(":/icons16x16/audio-document.png"));
        sm_ItemIcons[ITEM_Particles].addFile(QString(":/icons16x16/particle-document.png"));
        sm_ItemIcons[ITEM_Font].addFile(QString(":/icons16x16/font-document.png"));
        sm_ItemIcons[ITEM_Spine].addFile(QString(":/icons16x16/spine-document.png"));
        sm_ItemIcons[ITEM_Sprite].addFile(QString(":/icons16x16/sprite-document.png"));
        sm_ItemIcons[ITEM_Entity].addFile(QString(":/icons16x16/entity-document.png"));
        
        sm_AtlasIcons[ATLAS_Texture].addFile(QString(":/icons16x16/atlas-file.png"));
        sm_AtlasIcons[ATLAS_Frame].addFile(QString(":/icons16x16/atlas-frame.png"));
        sm_AtlasIcons[ATLAS_Frame_Warning].addFile(QString(":/icons16x16/atlas-frameWarning.png"));
        sm_AtlasIcons[ATLAS_Spine].addFile(QString(":/icons16x16/spine-document.png"));
        sm_AtlasIcons[ATLAS_Font].addFile(QString(":/icons16x16/font-document.png"));

        sm_pFileNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_-]*"));
        sm_pFilePathValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|/_-]*"));
    }

    
    static QList<eItemType> SubDirList()
    {
        QList<eItemType> list;
        list.append(ITEM_DirAudio);
        list.append(ITEM_DirParticles);
        list.append(ITEM_DirFonts);
        list.append(ITEM_DirSpine);
        list.append(ITEM_DirSprites);
        list.append(ITEM_DirShaders);
        list.append(ITEM_DirEntities);
        
        return list;
    }
    
    static QStringList SubDirNameList()
    {
        QList<eItemType> itemList = SubDirList();
        
        QStringList list;
        foreach(eItemType eType, itemList)
            list.append(sm_sItemNames[eType]);
        
        return list;
    }
    
    static const QString &ItemName(eItemType eItm)          { return sm_sItemNames[eItm]; }

    static const QString &ItemExt(int iIndex)               { Q_ASSERT(iIndex >= 0 && iIndex < NUMITEM); return sm_sItemExt[iIndex]; }
    static const QString &ItemExt(eItemType eItm)           { return sm_sItemExt[eItm]; }
    
    static const QIcon &ItemIcon(int iIndex)                { Q_ASSERT(iIndex >= 0 && iIndex < NUMITEM); return sm_ItemIcons[iIndex]; }
    static const QIcon &ItemIcon(eItemType eItm)            { return sm_ItemIcons[eItm]; }
    
    static const QIcon &AtlasIcon(eAtlasNodeType eNode)     { return sm_AtlasIcons[eNode]; }

    static const QRegExpValidator *FileNameValidator()      { return sm_pFileNameValidator; }
    static const QRegExpValidator *FilePathValidator()      { return sm_pFilePathValidator; }
    
    static bool IsWorkspaceValid(const QDir &projDir)
    {
        QDir dir(projDir);
        if(!dir.exists(HYGUIPATH_RelDataDir))
            return false;
        
        QStringList dirList = HyGlobal::SubDirNameList();
        foreach(QString sDir, dirList)
        {
            if(!dir.exists(HYGUIPATH_RelDataDir + sDir))
                return false;
        }
        
        if(!dir.exists(HYGUIPATH_RelDataAtlasDir))
            return false;
        
        if(!dir.exists(HYGUIPATH_RelMetaDataDir))
            return false;
        
        if(!dir.exists(HYGUIPATH_RelMetaAtlasDir))
            return false;

        if(!dir.exists(HYGUIPATH_RelSrcDataDir))
            return false;
        
        return true;
    }

    static quint32 CRCData(quint32 crc, const uchar *buf, size_t len)
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

    static QString MakeFileNameFromCounter(int iCount)
    {
        QString sNewString;
        sNewString.sprintf("%05d", iCount);

        return sNewString;
    }
};

QAction *FindAction(QList<QAction *> list, QString sName);
#define FINDACTION(str) FindAction(this->actions(), str)

#define HYLOG(msg, type) { QString sHyLogTmpStr = msg; WidgetOutputLog::Log(sHyLogTmpStr, type); }

#define JSONOBJ_TOINT(obj, key) obj.value(key).toVariant().toLongLong()

#endif // HYGLOBALS_H
