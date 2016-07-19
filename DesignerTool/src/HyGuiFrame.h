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

class HyGuiFrame
{
    friend class WidgetAtlasManager;

    const quint32                       m_uiHASH;
    const QString                       m_sNAME;
    const int                           m_iWIDTH;
    const int                           m_iHEIGHT;
    const QRect                         m_rALPHA_CROP;
    const uint                          m_uiATLAS_GROUP_ID;

    int                                 m_iTextureIndex;
    bool                                m_bRotation;

    int                                 m_iPosX;
    int                                 m_iPosY;

    QSet<ItemWidget *>                  m_Links;

    QMap<void *, HyTexturedQuad2d *>    m_DrawInstMap;

    HyGuiFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY);
    ~HyGuiFrame();
    
public:
    HyTexturedQuad2d *DrawInst(void *pKey);

    quint32 GetHash()                           { return m_uiHASH; }
    QString GetName()                           { return m_sNAME; }
    QSize GetSize()                             { return QSize(m_iWIDTH, m_iHEIGHT); }
    QRect GetCrop()                             { return m_rALPHA_CROP; }
    QPoint GetPosition()                        { return QPoint(m_iPosX, m_iPosY); }
    QSet<ItemWidget *> GetLinks()               { return m_Links; }

    bool IsRotated()                            { return m_bRotation; }
    int GetX()                                  { return m_iPosX; }
    int GetY()                                  { return m_iPosY; }
    int GetTextureIndex()                       { return m_iTextureIndex; }

    void UpdateInfoFromPacker(int iTextureIndex, bool bRotation, int iX, int iY);

    QString ConstructImageFileName();
};
Q_DECLARE_METATYPE(HyGuiFrame *)

#endif // HYGUIFRAME_H
