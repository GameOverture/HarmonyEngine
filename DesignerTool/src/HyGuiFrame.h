/**************************************************************************
 *	HyGuiFrame.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUIFRAME_H
#define HYGUIFRAME_H

#include "Harmony/HyEngine.h"
#include "ItemWidget.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>

class HyGuiFrame
{
    friend class WidgetAtlasManager;

    const uint                          m_uiATLAS_GROUP_ID;
    
    eAtlasNodeType                      m_eType;

    QTreeWidgetItem *                   m_pTreeWidgetItem;

    quint32                             m_uiChecksum;
    QString                             m_sName;
    int                                 m_iWidth;
    int                                 m_iHeight;
    QRect                               m_rAlphaCrop;

    int                                 m_iTextureIndex;
    bool                                m_bRotation;

    int                                 m_iPosX;
    int                                 m_iPosY;

    QString                             m_sFilter;  // Folder filter for atlas group

    QSet<ItemWidget *>                  m_Links;

    QMap<void *, HyTexturedQuad2d *>    m_DrawInstMap;

    uint                                m_uiErrors; // '0' when there is no error

    // Private ctor as WidgetAtlasManager should only construct these
    HyGuiFrame(quint32 uiChecksum, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, eAtlasNodeType eType, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY, QString sFilter, uint uiErrors);
    ~HyGuiFrame();
    
public:
    void ReplaceImage(QString sImgPath, QDir metaDir);
    void ReplaceImage(QString sName, QImage &newImage, QDir metaDir);

    HyTexturedQuad2d *DrawInst(void *pKey);
    void DeleteDrawInst(void *pKey);
    void DeleteAllDrawInst();

    void SetTreeWidgetItem(QTreeWidgetItem *pTreeItem);

    quint32 GetChecksum()                       { return m_uiChecksum; }
    QString GetName()                           { return m_sName; }
    QSize GetSize()                             { return QSize(m_iWidth, m_iHeight); }
    QRect GetCrop()                             { return m_rAlphaCrop; }
    QPoint GetPosition()                        { return QPoint(m_iPosX, m_iPosY); }
    QSet<ItemWidget *> GetLinks()               { return m_Links; }
    int GetAtlasGroupdId()                      { return static_cast<int>(m_uiATLAS_GROUP_ID); }
    eAtlasNodeType GetType()                    { return m_eType; }

    int GetTextureIndex()                       { return m_iTextureIndex; }
    bool IsRotated()                            { return m_bRotation; }
    int GetX()                                  { return m_iPosX; }
    int GetY()                                  { return m_iPosY; }
    QString GetFilter();
    void SetFilter(QString sFilter);

    void UpdateInfoFromPacker(int iTextureIndex, bool bRotation, int iX, int iY);

    QString ConstructImageFileName();

    void GetJsonObj(QJsonObject &frameObj);

    void SetError(eGuiFrameError eError);
    void ClearError(eGuiFrameError eError);
    uint GetErrors();
};
Q_DECLARE_METATYPE(HyGuiFrame *)

#endif // HYGUIFRAME_H
