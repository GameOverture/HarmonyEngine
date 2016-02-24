/**************************************************************************
 *	WidgetAtlasGroup.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETATLASGROUP_H
#define WIDGETATLASGROUP_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QImage>
#include <QVariant>

#include "HyGlobal.h"
#include "DlgAtlasGroupSettings.h"
#include "IHyGuiDrawItem.h"

#include "scriptum/imagepacker.h"
#include "Harmony/HyEngine.h"

namespace Ui {
class WidgetAtlasGroup;
}

class HyGuiFrame
{
    const quint32       m_uiHASH;
    const QString       m_sNAME;
    const int           m_iWIDTH;
    const int           m_iHEIGHT;
    const QRect         m_rALPHA_CROP;

    int                 m_iTextureIndex;
    bool                m_bRotation;void on_atlasList_itemEntered(QTreeWidgetItem *item, int column);

    int                 m_iPosX;
    int                 m_iPosY;
    
    QTreeWidgetItem *   m_pTreeItem;
    QStringList         m_sLinks;
    
    HyTexturedQuad2d    m_DrawTexture;
    HyPrimitive2d       m_DrawOutline;

public:
    HyGuiFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, uint uiAtlasGroupIndex, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY) : m_uiHASH(uiCRC),
                                                                                                                                                m_sNAME(sN),
                                                                                                                                                m_iWIDTH(iW),
                                                                                                                                                m_iHEIGHT(iH),
                                                                                                                                                m_rALPHA_CROP(rAlphaCrop),
                                                                                                                                                m_iTextureIndex(iTexIndex),
                                                                                                                                                m_bRotation(bRot),
                                                                                                                                                m_iPosX(iX),
                                                                                                                                                m_iPosY(iY),
                                                                                                                                                m_DrawTexture(uiAtlasGroupIndex),
                                                                                                                                                m_pTreeItem(NULL)
    {
        m_DrawOutline.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
        m_DrawOutline.SetAsQuad(m_rALPHA_CROP.width(), m_rALPHA_CROP.height(), true);
        m_DrawOutline.SetDisplayOrder(0);

        m_DrawTexture.SetTextureSource(iTexIndex, GetX(), GetY(), m_rALPHA_CROP.width(), m_rALPHA_CROP.height());
        m_DrawTexture.SetDisplayOrder(1);

        m_DrawTexture.AddChild(m_DrawOutline);
    }
    
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
    
    void SetLink(QString sFullPath)
    {
        m_sLinks.append(sFullPath);
    }
    void SetLink(eItemType eType, QString sPrefix, QString sName)
    {
        QString sLink(HyGlobal::ItemName(eType) % "/");
        sLink += sPrefix;
        sLink += sName;
        
        m_sLinks.append(sLink);
    }
    void SetTreeWidgetItem(QTreeWidgetItem *pItem)
    {
        m_pTreeItem = pItem;
        
        QVariant v; v.setValue(this);
        m_pTreeItem->setData(0, QTreeWidgetItem::UserType, v);
    }
    void SetInfoFromPacker(int iTextureIndex, bool bRotation, int iX, int iY)
    {
        m_iTextureIndex = iTextureIndex;
        m_bRotation = bRotation;
        m_iPosX = iX;
        m_iPosY = iY;

        m_DrawTexture.SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.width(), m_rALPHA_CROP.height());
    }
    
    QString ConstructImageFileName()
    {
        QString sMetaImgName;
        sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiHASH, m_sNAME.toStdString().c_str());
        sMetaImgName += ".png";
        
        return sMetaImgName;
    }
    
    void DrawLoad()
    {
        m_DrawOutline.Load();
        m_DrawOutline.SetEnabled(false);
        
        m_DrawTexture.Load();
        m_DrawTexture.SetEnabled(false);
    }

    void DrawUnload()
    {
        m_DrawOutline.Unload();
        m_DrawTexture.Unload();
    }

    void SetVisible(bool bOutline, bool bFrame)
    {
        m_DrawOutline.SetEnabled(bOutline);
        m_DrawTexture.SetEnabled(bFrame);
    }
};
Q_DECLARE_METATYPE(HyGuiFrame *)

class WidgetAtlasGroup : public QWidget, public IHyGuiDrawItem
{
    Q_OBJECT

    // NOTE: Order of these member variables matter here for the member initializer list
    QDir                        m_MetaDir;
    QDir                        m_DataDir;

    DlgAtlasGroupSettings       m_dlgSettings;
    
    QList<HyGuiFrame *>         m_FrameList;
    ImagePacker                 m_Packer;

public:
    explicit WidgetAtlasGroup(QWidget *parent = 0);
    explicit WidgetAtlasGroup(QDir metaDir, QDir dataDir, QWidget *parent = 0);
    ~WidgetAtlasGroup();

    void GetAtlasInfo(QJsonObject &atlasObj);
    
    int GetId();

    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);
    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);

    void ResizeAtlasListColumns();

private slots:
    void on_btnAddImages_clicked();
    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();
    
protected:
    virtual void enterEvent(QEvent *pEvent);
    virtual void leaveEvent(QEvent *pEvent);

private:
    Ui::WidgetAtlasGroup *ui;
    
    void ImportImages(QStringList sImportImgList);
    void Refresh();

    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, QString sName, int iTextureIndex, eAtlasNodeType eType);
};

#endif // WIDGETATLASGROUP_H
