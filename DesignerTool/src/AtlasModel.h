/**************************************************************************
 *	AtlasModel.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASMODEL_H
#define ATLASMODEL_H

#include <QObject>

#include "AtlasFrame.h"
#include "scriptum/imagepacker.h"

class AtlasModel : public QObject
{
    Q_OBJECT

    Project *                                       m_pProjOwner;

    QDir                                            m_MetaDir;
    QDir                                            m_DataDir;

    QJsonObject                                     m_PackerSettings;
    ImagePacker                                     m_Packer;

    QList<AtlasFrame *>                             m_FrameList;
    QList<AtlasTreeItem *>                          m_TopLevelTreeItemList;
    
    class ChecksumLookup
    {
        QMap<quint32, QList<AtlasFrame *> >         m_FrameChecksumMap;
        
    public:
        void AddLookup(AtlasFrame *pFrame);
        bool RemoveLookup(AtlasFrame *pFrame);
        AtlasFrame *Find(quint32 uiChecksum);
    };
    ChecksumLookup                                  m_ChecksumLookup;

public:
    AtlasModel(Project *pProjOwner);
    virtual ~AtlasModel();

    QList<AtlasFrame *> GetFrames();

    QJsonObject GetPackerSettings();

    QList<AtlasTreeItem *> GetTopLevelTreeItemList();

    QSize GetAtlasDimensions();

    int GetNumTextures();

    void WriteMetaSettings();
    void WriteMetaSettings(QJsonArray frameArray);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    AtlasFrame *CreateFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, eAtlasNodeType eType, int iW, int iH, int iX, int iY, uint uiAtlasIndex, uint uiErrors);
    void RemoveFrame(AtlasFrame *pFrame);

    AtlasFrame *GenerateFrame(ProjectItem *pItem, QString sName, QImage &newImage, eAtlasNodeType eType);
    void ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

    QList<AtlasFrame *> RequestFrames(ProjectItem *pItem);
    QList<AtlasFrame *> RequestFrames(ProjectItem *pItem, QList<AtlasFrame *> requestList);
    QList<AtlasFrame *> RequestFrames(ProjectItem *pItem, QList<quint32> requestList);

    void RelinquishFrames(ProjectItem *pItem, QList<AtlasFrame *> relinquishList);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QSet<AtlasFrame *> ImportImages(QStringList sImportImgList);
    AtlasFrame *ImportImage(QString sName, QImage &newImage, eAtlasNodeType eType);

    void SaveData();

    void GetAtlasInfoForGameData(QJsonObject &atlasObjOut);

    void SetPackerSettings();
    
    QFileInfoList GetExistingTextureInfoList();

    void RepackAll();
    void Repack(QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet);
    void ConstructAtlasTexture(int iPackerBinIndex, int iTextureArrayIndex);
    void Refresh();
};

#endif // ATLASMODEL_H
