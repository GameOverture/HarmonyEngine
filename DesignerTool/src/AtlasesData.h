/**************************************************************************
 *	ItemAtlases.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASESDATA_H
#define ATLASESDATA_H

#include <QObject>

#include "AtlasFrame.h"
#include "scriptum/imagepacker.h"

class AtlasesData : public QObject
{
    Q_OBJECT

    Project *                                   m_pProjOwner;

    QDir                                            m_MetaDir;
    QDir                                            m_DataDir;

    QJsonObject                                     m_PackerSettings;
    ImagePacker                                     m_Packer;

    QList<AtlasFrame *>                             m_FrameList;
    QList<AtlasTreeItem *>                          m_TopLevelAtlasTreeItemList;
    QMap<quint32, AtlasFrame *>                     m_DependencyMap;

public:
    AtlasesData(Project *pProjOwner);
    virtual ~AtlasesData();

    QJsonObject GetPackerSettings();

    QList<AtlasTreeItem *> GetAtlasTreeItemList();

    QSize GetAtlasDimensions();

    int GetNumTextures();

    void WriteMetaSettings();
    void WriteMetaSettings(QJsonArray frameArray);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    AtlasFrame *CreateFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, eAtlasNodeType eType, int iW, int iH, int iX, int iY, uint uiAtlasIndex, uint uiErrors);
    void RemoveFrame(AtlasFrame *pFrame);

    AtlasFrame *GenerateFrame(IData *pItem, QString sName, QImage &newImage, eAtlasNodeType eType);
    void ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

    QList<AtlasFrame *> RequestFrames(IData *pItem);
    QList<AtlasFrame *> RequestFrames(IData *pItem, QList<AtlasFrame *> requestList);
    QList<AtlasFrame *> RequestFrames(IData *pItem, QList<quint32> requestList);

    void RelinquishFrames(IData *pItem, QList<AtlasFrame *> relinquishList);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QSet<AtlasFrame *> ImportImages(QStringList sImportImgList);
    AtlasFrame *ImportImage(QString sName, QImage &newImage, eAtlasNodeType eType);

    void SaveData();

    void SetDependency(AtlasFrame *pFrame, IData *pItem);
    void RemoveDependency(AtlasFrame *pFrame, IData *pItem);

    void GetAtlasInfoForGameData(QJsonObject &atlasObjOut);

    void SetPackerSettings();

    void RepackAll();
    void Repack(QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet);
    void ConstructAtlasTexture(int iPackerBinIndex, int iTextureArrayIndex);
    void Refresh();
};

#endif // ATLASESDATA_H
