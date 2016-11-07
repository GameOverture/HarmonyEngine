/**************************************************************************
 *	HyGlobal.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGLOBALS_H
#define HYGLOBALS_H

#include <QString>
#include <QIcon>
#include <QValidator>
#include <QAction>
#include <QStringBuilder>
#include <QDir>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QStack>

#include "WidgetOutputLog.h"

#define HyDesignerToolName "Harmony Designer Tool"

//#define HYGUI_UseBinaryMetaFiles

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
    ITEM_DirAtlases,
    ITEM_DirAudioBanks,
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

enum eProjDrawState
{
    PROJDRAWSTATE_Nothing = 0,
    PROJDRAWSTATE_AtlasManager,
    
    NUMPROJDRAWSTATE
};

enum eGuiFrameError
{
    GUIFRAMEERROR_CannotFindMetaImg = 0,
    GUIFRAMEERROR_CouldNotPack,
    GUIFRAMEERROR_Duplicate,

    NUMGUIFRAMEERROR
};

#define HYGUIPATH_TempDir                       "temp/"
#define HYGUIPATH_DataAtlases                   "atlasInfo.json"
#define HYGUIPATH_MetaAtlasSettings             "settings.hygui"
#define HYGUIPATH_DataAudioCategories           "categories.json"

QAction *FindAction(QList<QAction *> list, QString sName);
#define FINDACTION(str) FindAction(this->actions(), str)

#define HyGuiLog(msg, type) { QString sHyLogTmpStr = msg; WidgetOutputLog::Log(sHyLogTmpStr, type); }

#define JSONOBJ_TOINT(obj, key) obj.value(key).toVariant().toLongLong()

char *QStringToCharPtr(QString sString);

QString PointToQString(QPointF ptPoint);
QPointF StringToPoint(QString sPoint);

template <typename T>
void SetStateNamingConventionInComboBox(QComboBox *pCmb)
{
    // Ensure that all the entry names in the combobox match their index
    for(int i = 0; i < pCmb->count(); ++i)
    {
        QString sName(QString::number(i) % " - " % pCmb->itemData(i).value<T *>()->GetName());
        pCmb->setItemText(i, sName);
    }
}

class HyGlobal
{
    static QString                  sm_sItemNames[NUMITEM];
    static QString                  sm_sItemExt[NUMITEM];
    static QIcon                    sm_ItemIcons[NUMITEM];
    
    static QIcon                    sm_AtlasIcons[NUMATLAS];

    static QRegExpValidator *       sm_pCodeNameValidator;
    static QRegExpValidator *       sm_pFileNameValidator;
    static QRegExpValidator *       sm_pFilePathValidator;
    static QRegExpValidator *       sm_pVector2dValidator;

    static QString                  sm_ErrorStrings[NUMGUIFRAMEERROR];

public:
    static void Initialize()
    {
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
        sm_sItemNames[ITEM_DirAudioBanks] = "AudioBanks";
        
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
        //sm_ItemIcons[ITEM_DirAtlases].addFile();
        //sm_ItemIcons[ITEM_DirAudioBanks].addFile();
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

        sm_pCodeNameValidator = new QRegExpValidator(QRegExp("[A-Za-z_]+[A-Za-z0-9_]*"));
        sm_pFileNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_-]*"));
        sm_pFilePathValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|/_-]*"));
        sm_pVector2dValidator = new QRegExpValidator(QRegExp("\\([0-9]*\\.?[0-9]*,[0-9]*\\.?[0-9]*\\)"));

        sm_ErrorStrings[GUIFRAMEERROR_CannotFindMetaImg] = "Cannot find source meta-image";
        sm_ErrorStrings[GUIFRAMEERROR_CouldNotPack] = "Could not pack this frame in atlas";
        sm_ErrorStrings[GUIFRAMEERROR_Duplicate] = "Duplicate frame exists";
    }

    static eItemType GetCorrespondingDirItem(eItemType eItem)
    {
        switch(eItem)
        {
        case ITEM_Audio:        return ITEM_DirAudio;
        case ITEM_Particles:    return ITEM_DirParticles;
        case ITEM_Font:         return ITEM_DirFonts;
        case ITEM_Spine:        return ITEM_DirSpine;
        case ITEM_Sprite:       return ITEM_DirSprites;
        case ITEM_Shader:       return ITEM_DirShaders;
        default:
            HyGuiLog("HyGlobal::GetCorrespondingDirItem() could not find the proper directory item", LOGTYPE_Warning);
        }
        
        return ITEM_Unknown;
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
        list.append(ITEM_DirAtlases);
        list.append(ITEM_DirAudioBanks);
        
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
    
    static const QString &ItemName(eItemType eItm)                  { return sm_sItemNames[eItm]; }

    static const QString &ItemExt(int iIndex)                       { Q_ASSERT(iIndex >= 0 && iIndex < NUMITEM); return sm_sItemExt[iIndex]; }
    static const QString &ItemExt(eItemType eItm)                   { return sm_sItemExt[eItm]; }

    static const QIcon &ItemIcon(int iIndex)                        { Q_ASSERT(iIndex >= 0 && iIndex < NUMITEM); return sm_ItemIcons[iIndex]; }
    static const QIcon &ItemIcon(eItemType eItm)                    { return sm_ItemIcons[eItm]; }

    static const QIcon &AtlasIcon(eAtlasNodeType eNode)             { return sm_AtlasIcons[eNode]; }

    static const QRegExpValidator *CodeNameValidator()              { return sm_pCodeNameValidator; }
    static const QRegExpValidator *FileNameValidator()              { return sm_pFileNameValidator; }
    static const QRegExpValidator *FilePathValidator()              { return sm_pFilePathValidator; }
    static const QRegExpValidator *Vector2dValidator()              { return sm_pVector2dValidator; }

    static const QString GetGuiFrameErrors(uint uiErrorFlags)
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
    
    static bool IsEngineDirValid(const QDir &engineDir)
    {
        QDir dir(engineDir);
        if(!dir.exists())
            return false;
        
        if(!dir.exists("include/") ||
           !dir.exists("lib/") ||
           !dir.exists("src/") ||
           !dir.exists("templates/") ||
           (!dir.exists("Harmony_vs2013.sln") && !dir.exists("Harmony_vs2015.sln")) ||
           (!dir.exists("Harmony_vs2013.vcxproj") && !dir.exists("Harmony_vs2015.vcxproj")))
        {
            return false;
        }
        
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

    static void RecursiveFindOfFileExt(QString sExt, QStringList &appendList, QDir dirEntry)
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
};

struct SortTreeWidgetsPredicate
{
    bool operator()(const QTreeWidgetItem *pA, const QTreeWidgetItem *pB) const
    {
        return QString::compare(pA->text(0), pB->text(0)) < 0;
    }
};

#endif // HYGLOBALS_H
