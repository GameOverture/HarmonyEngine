/**************************************************************************
 *	HyGuiFrame.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASFRAME_H
#define ATLASFRAME_H

#include "ProjectItem.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class AtlasTreeItem;

class AtlasFrame
{
    friend class AtlasModel;
    
    const quint32                       m_uiUNIQUE_ID;
    HyGuiItemType                       m_eType;
    
    quint32                             m_uiAtlasGrpId;

    AtlasTreeItem *                     m_pTreeWidgetItem;

    quint32                             m_uiImageChecksum;
    QString                             m_sName;
    int                                 m_iWidth;
    int                                 m_iHeight;
    QRect                               m_rAlphaCrop;

    int                                 m_iTextureIndex;

    int                                 m_iPosX;
    int                                 m_iPosY;

    QSet<ProjectItem *>                 m_DependencySet;

    uint                                m_uiErrors; // '0' when there is no error

    // Private ctor as WidgetAtlasManager should only construct these
    AtlasFrame(quint32 uiId, quint32 uiChecksum, quint32 uiAtlasGrpId, QString sN, QRect rAlphaCrop, HyGuiItemType eType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
    ~AtlasFrame();
    
public:
    AtlasTreeItem *GetTreeItem();

    quint32 GetId();
    
    quint32 GetAtlasGrpId();
    void SetAtlasGrpId(quint32 uiNewAtlasGrpId);
    
    quint32 GetImageChecksum();
    QString GetName();
    void SetName(QString sNewName);
    QSize GetSize();
    QRect GetCrop();
    QPoint GetPosition();
    QSet<ProjectItem *> GetLinks();
    HyGuiItemType GetType();

    int GetTextureIndex();
    int GetX();
    int GetY();
    QString GetFilter();
    void SetFilter(QString sFilter);

    void UpdateInfoFromPacker(int iTextureIndex, int iX, int iY);

    QString ConstructImageFileName();

    void GetJsonObj(QJsonObject &frameObj);

    void SetError(eGuiFrameError eError);
    void ClearError(eGuiFrameError eError);
    uint GetErrors();

private:
    void UpdateTreeItemIconAndToolTip();
    bool DeleteMetaImage(QDir metaDir);
    void ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir);
};
Q_DECLARE_METATYPE(AtlasFrame *)

QDataStream &operator<<(QDataStream &out, AtlasFrame *const &rhs);
QDataStream &operator>>(QDataStream &in, AtlasFrame *rhs);

#endif // ATLASFRAME_H
