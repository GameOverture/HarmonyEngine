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

#include <QWidget>

class HyGuiFrame
{
    const quint32       m_uiHASH;
    const uint          m_uiATLAS_GROUP_ID;
    const QString       m_sNAME;
    const int           m_iWIDTH;
    const int           m_iHEIGHT;
    const QRect         m_rALPHA_CROP;

    int                 m_iTextureIndex;
    bool                m_bRotation;

    int                 m_iPosX;
    int                 m_iPosY;

    QStringList         m_sLinks;

public:
    HyGuiFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY) :    m_uiATLAS_GROUP_ID(uiAtlasGroupId),
                                                                                                                                                m_uiHASH(uiCRC),
                                                                                                                                                m_sNAME(sN),
                                                                                                                                                m_iWIDTH(iW),
                                                                                                                                                m_iHEIGHT(iH),
                                                                                                                                                m_rALPHA_CROP(rAlphaCrop),
                                                                                                                                                m_iTextureIndex(iTexIndex),
                                                                                                                                                m_bRotation(bRot),
                                                                                                                                                m_iPosX(iX),
                                                                                                                                                m_iPosY(iY)
    {
    }

    HyGuiFrame::~HyGuiFrame();

    quint32 GetHash()       { return m_uiHASH; }
    QString GetName()       { return m_sNAME; }
    QSize GetSize()         { return QSize(m_iWIDTH, m_iHEIGHT); }
    QRect GetCrop()         { return m_rALPHA_CROP; }
    QPoint GetPosition()    { return QPoint(m_iPosX, m_iPosY); }
    QStringList GetLinks()  { return m_sLinks; }

    bool IsRotated()        { return m_bRotation; }
    int GetX()              { return m_iPosX; }
    int GetY()              { return m_iPosY; }
    int GetTextureIndex()   { return m_iTextureIndex; }

    // Who ever receieves this pointer is responsible for deleting
    HyTexturedQuad2d *CreateHyTexturedQuad2d();

    void SetLink(QString sFullPath)
    {
        m_sLinks.append(sFullPath);
    }
    void SetLink(eItemType eType, QString sPrefix, QString sName)
    {
        QString sLink(HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType)) % "/");
        sLink += sPrefix;
        sLink += sName;

        m_sLinks.append(sLink);
    }
    void SetInfoFromPacker(int iTextureIndex, bool bRotation, int iX, int iY, HyTexturedQuad2d *pHyTexQuad2d)
    {
        m_iTextureIndex = iTextureIndex;
        m_bRotation = bRotation;
        m_iPosX = iX;
        m_iPosY = iY;

        pHyTexQuad2d->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.width(), m_rALPHA_CROP.height());
    }

    QString ConstructImageFileName()
    {
        QString sMetaImgName;
        sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiHASH, m_sNAME.toStdString().c_str());
        sMetaImgName += ".png";

        return sMetaImgName;
    }

    void Reset()
    {
        SetEnabled(false);
        SetDisplayOrder(0);
        color.Set(1.0f, 1.0f, 1.0f, 1.0f);
        SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    }
};
Q_DECLARE_METATYPE(HyGuiFrame *)

#endif // HYGUIFRAME_H
