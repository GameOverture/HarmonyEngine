/**************************************************************************
 *	ItemAtlases.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMATLASES_H
#define ITEMATLASES_H

#include <QObject>

#include "AtlasFrame.h"
#include "scriptum/imagepacker.h"

class ItemAtlases : public QObject
{
    Q_OBJECT

    ItemProject *                                   m_pProjOwner;

    QDir                                            m_MetaDir;
    QDir                                            m_DataDir;

    QJsonObject                                     m_PackerSettings;
    ImagePacker                                     m_Packer;

    QList<HyGuiFrame *>                             m_FrameList;
    QList<AtlasTreeItem *>                          m_TopLevelAtlasTreeItemList;
    QMap<quint32, HyGuiFrame *>                     m_DependencyMap;

public:
    ItemAtlases(ItemProject *pProjOwner);
    virtual ~ItemAtlases();

    QJsonObject GetPackerSettings();

    QList<AtlasTreeItem *> GetAtlasTreeItemList();

    QSize GetAtlasDimensions();

    int GetNumTextures();

    void WriteMetaSettings();
    void WriteMetaSettings(QJsonArray frameArray);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    HyGuiFrame *CreateFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, eAtlasNodeType eType, int iW, int iH, int iX, int iY, uint uiAtlasIndex, uint uiErrors);
    void RemoveFrame(HyGuiFrame *pFrame);

    HyGuiFrame *GenerateFrame(ItemWidget *pItem, QString sName, QImage &newImage, eAtlasNodeType eType);
    void ReplaceFrame(HyGuiFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

    QList<HyGuiFrame *> RequestFrames(ItemWidget *pItem);
    QList<HyGuiFrame *> RequestFrames(ItemWidget *pItem, QList<HyGuiFrame *> requestList);
    QList<HyGuiFrame *> RequestFrames(ItemWidget *pItem, QList<quint32> requestList);

    void RelinquishFrames(ItemWidget *pItem, QList<HyGuiFrame *> relinquishList);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QSet<HyGuiFrame *> ImportImages(QStringList sImportImgList);
    HyGuiFrame *ImportImage(QString sName, QImage &newImage, eAtlasNodeType eType);

    void SaveData();

    void SetDependency(HyGuiFrame *pFrame, ItemWidget *pItem);
    void RemoveDependency(HyGuiFrame *pFrame, ItemWidget *pItem);

    void GetAtlasInfoForGameData(QJsonObject &atlasObjOut);

    void SetPackerSettings();

    void RepackAll();
    void Repack(QSet<int> repackTexIndicesSet, QSet<HyGuiFrame *> newFramesSet);
    void ConstructAtlasTexture(int iPackerBinIndex, int iTextureArrayIndex);
    void Refresh();
};

#endif // ITEMATLASES_H
