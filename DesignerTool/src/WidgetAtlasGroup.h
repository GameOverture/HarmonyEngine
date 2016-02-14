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
        m_DrawOutline.Color().Set(1.0f, 0.0f, 0.0f, 1.0f);
        m_DrawOutline.SetAsQuad(iW, iH, true);
        m_DrawOutline.SetDisplayOrder(0);

        m_DrawTexture.SetTextureSource(iTexIndex, iX, iY, iW, iH);
        m_DrawTexture.Pos().Set(iX, iY);
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
    void SetPackerInfo(int iTextureIndex, bool bRotation, int iX, int iY)
    {
        m_iTextureIndex = iTextureIndex;
        m_bRotation = bRotation;
        m_iPosX = iX;
        m_iPosY = iY;
    }
    
    QString ConstructImageFileName()
    {
        QString sMetaImgName;
        sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiHASH, m_sNAME.toStdString().c_str());
        sMetaImgName += ".png";
        
        return sMetaImgName;
    }
    
    void LoadDrawInst()
    {
        m_DrawOutline.Load();
        m_DrawTexture.Load();
    }

    void SetVisible(bool bOutline, bool bFrame)
    {
        m_DrawOutline.SetEnabled
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
    
    //QList<QTreeWidgetItem *>    m_TextureList;
    QList<HyGuiFrame *>         m_FrameList;
    ImagePacker                 m_Packer;
    
    // Draw members
    HyCamera2d *                m_pCam;

    QPoint                      m_MouseLocalCoords;

public:
    explicit WidgetAtlasGroup(QWidget *parent = 0);
    explicit WidgetAtlasGroup(QDir metaDir, QDir dataDir, QWidget *parent = 0);
    ~WidgetAtlasGroup();

    void GetAtlasInfo(QJsonObject &atlasObj);
    
    int GetId();
    
    virtual void Show(IHyApplication &hyApp);
    virtual void Hide(IHyApplication &hyApp);

    virtual void Draw(IHyApplication &hyApp);

    void ResizeAtlasListColumns();

private slots:
    void on_btnAddImages_clicked();
    void on_btnAddDir_clicked();
    
protected:
    virtual void enterEvent(QEvent *pEvent);
    virtual void leaveEvent(QEvent *pEvent);

    virtual void mouseMoveEvent(QMouseEvent *pEvent);

private:
    Ui::WidgetAtlasGroup *ui;
    
    void ImportImages(QStringList sImportImgList);
    void Refresh();
    
    void LoadDrawInst();

    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, QString sName, int iTextureIndex, eAtlasNodeType eType);
};

#endif // WIDGETATLASGROUP_H
