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
#include "IData.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class AtlasTreeItem;

class HyGuiFrame
{
    friend class ItemAtlases;
    
    eAtlasNodeType                      m_eType;

    AtlasTreeItem *                     m_pTreeWidgetItem;

    quint32                             m_uiChecksum;
    QString                             m_sName;
    int                                 m_iWidth;
    int                                 m_iHeight;
    QRect                               m_rAlphaCrop;

    int                                 m_iTextureIndex;

    int                                 m_iPosX;
    int                                 m_iPosY;

    QSet<ItemWidget *>                  m_Links;

    QMap<void *, HyTexturedQuad2d *>    m_DrawInstMap;

    uint                                m_uiErrors; // '0' when there is no error

    // Private ctor as WidgetAtlasManager should only construct these
    HyGuiFrame(quint32 uiChecksum, QString sN, QRect rAlphaCrop, eAtlasNodeType eType, int iW, int iH, int iX, int iY, uint uiAtlasIndex, uint uiErrors);
    ~HyGuiFrame();
    
public:
    HyTexturedQuad2d *DrawInst(void *pKey);
    void DeleteDrawInst(void *pKey);
    void DeleteAllDrawInst();

    AtlasTreeItem *GetTreeItem();

    quint32 GetChecksum();
    QString GetName();
    QSize GetSize();
    QRect GetCrop();
    QPoint GetPosition();
    QSet<ItemWidget *> GetLinks();
    eAtlasNodeType GetType();

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
    bool DeleteMetaImage(QDir metaDir);
    void ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir);
};
Q_DECLARE_METATYPE(HyGuiFrame *)

QDataStream &operator<<(QDataStream &out, HyGuiFrame *const &rhs);
QDataStream &operator>>(QDataStream &in, HyGuiFrame *rhs);

#endif // HYGUIFRAME_H
