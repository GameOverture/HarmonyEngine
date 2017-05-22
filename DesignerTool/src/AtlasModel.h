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

    quint32                                         m_uiNextFrameId;
    quint32                                         m_uiNextAtlasId;

    QDir                                            m_MetaDir;
    QDir                                            m_DataDir;

    struct AtlasGrp
    {
        QJsonObject                                 m_PackerSettings;
        ImagePacker                                 m_Packer;
        QList<AtlasFrame *>                         m_FrameList;
        
        void SetPackerSettings()
        {
            m_Packer.sortOrder = m_PackerSettings["cmbSortOrder"].toInt();// m_iSortOrderIndex;//ui->cmbSortOrder->currentIndex();
            m_Packer.border.t = m_PackerSettings["sbFrameMarginTop"].toInt();// m_iFrameMarginTop;//ui->sbFrameMarginTop->value();
            m_Packer.border.l = m_PackerSettings["sbFrameMarginLeft"].toInt();// m_iFrameMarginLeft;//ui->sbFrameMarginLeft->value();
            m_Packer.border.r = m_PackerSettings["sbFrameMarginRight"].toInt();// m_iFrameMarginRight;//ui->sbFrameMarginRight->value();
            m_Packer.border.b = m_PackerSettings["sbFrameMarginBottom"].toInt();// m_iFrameMarginBottom;//ui->sbFrameMarginBottom->value();
            m_Packer.extrude = m_PackerSettings["extrude"].toInt();// m_iExtrude;//ui->extrude->value();
            m_Packer.merge = m_PackerSettings["chkMerge"].toBool();// m_bMerge;//ui->chkMerge->isChecked();
            m_Packer.square = m_PackerSettings["chkSquare"].toBool();// m_bSquare;//ui->chkSquare->isChecked();
            m_Packer.autosize = m_PackerSettings["chkAutosize"].toBool();// m_bAutoSize;//ui->chkAutosize->isChecked();
            m_Packer.minFillRate = m_PackerSettings["minFillRate"].toInt();// m_iFillRate;//ui->minFillRate->value();
            m_Packer.mergeBF = false;
            m_Packer.rotate = ImagePacker::NEVER;
        }
    };
    QList<AtlasGrp *>                               m_AtlasGrpList;

    QList<AtlasTreeItem *>                          m_TopLevelTreeItemList;
    
    class FrameLookup
    {
        QMap<quint32, AtlasFrame *>                 m_FrameIdMap;
        QMap<quint32, QList<AtlasFrame *> >         m_FrameChecksumMap;
        
    public:
        void AddLookup(AtlasFrame *pFrame);
        bool RemoveLookup(AtlasFrame *pFrame);  // Returns true if no remaining duplicates exist
        AtlasFrame *Find(quint32 uiId);
    };
    FrameLookup                                     m_FrameLookup;

public:
    AtlasModel(Project *pProjOwner);
    virtual ~AtlasModel();

    int GetNumAtlasGroups();
    
    QList<AtlasFrame *> GetFrames(uint uiGrpIndex);

    QJsonObject GetPackerSettings(uint uiGrpIndex);

    void TakeTreeWidgets(QList<AtlasTreeItem *> treeItemList);
    QList<AtlasTreeItem *> GetTopLevelTreeItemList();

    QSize GetAtlasDimensions(uint uiGrpIndex);

    int GetNumTextures();

    void WriteMetaSettings();
    void WriteMetaSettings(QJsonArray frameArray);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    AtlasFrame *CreateFrame(quint32 uiId, quint32 uiCRC, quint32 uiAtlasGrpId, QString sN, QRect rAlphaCrop, eAtlasNodeType eType, int iW, int iH, int iX, int iY, int iAtlasIndex, uint uiErrors);
    void RemoveFrame(AtlasFrame *pFrame);

    AtlasFrame *GenerateFrame(ProjectItem *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpId, eAtlasNodeType eType);
    void ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

    QList<AtlasFrame *> RequestFrames(ProjectItem *pItem);
    QList<AtlasFrame *> RequestFrames(ProjectItem *pItem, QList<AtlasFrame *> requestList);
    QList<AtlasFrame *> RequestFramesById(ProjectItem *pItem, QList<quint32> requestList);

    void RelinquishFrames(ProjectItem *pItem, QList<AtlasFrame *> relinquishList);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QSet<AtlasFrame *> ImportImages(QStringList sImportImgList, quint32 uiAtlasGrpId);
    AtlasFrame *ImportImage(QString sName, QImage &newImage, quint32 uiAtlasGrpId, eAtlasNodeType eType);

    void SaveData();

    //void GetAtlasInfoForGameData(QJsonObject &atlasObjOut);
    
    QFileInfoList GetExistingTextureInfoList();

    void RepackAll(uint uiGrpIndex, bool bForceRepack);
    void Repack(QSet<QPair<int, int> > repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet);
    void ConstructAtlasTexture(int iPackerBinIndex, int iTextureArrayIndex);
    void Refresh();
};

#endif // ATLASMODEL_H
