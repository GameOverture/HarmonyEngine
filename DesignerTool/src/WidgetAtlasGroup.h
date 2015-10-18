#ifndef WIDGETATLASGROUP_H
#define WIDGETATLASGROUP_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QImage>
#include <QVariant>

#include "HyGlobal.h"
#include "DlgAtlasGroupSettings.h"
#include "scriptum/imagepacker.h"

namespace Ui {
class WidgetAtlasGroup;
}

struct Image
{
    quint32             uiHash;
    QString             sName;
    int                 iWidth;
    int                 iHeight;
    bool                bRotation;
    int                 iPosX;
    int                 iPosY;
    
    QStringList         sLinks;
    QTreeWidgetItem *   pTreeItem;

    Image(quint32 uiCRC, QString sN, int iW, int iH, bool bRot, int iX, int iY) :   uiHash(uiCRC),
                                                                                    sName(sN),
                                                                                    iWidth(iW),
                                                                                    iHeight(iH),
                                                                                    bRotation(bRot),
                                                                                    iPosX(iX),
                                                                                    iPosY(iY)
    { }
    
    void SetLink(QString sPrefixAndName)
    {
        sLinks.append(sPrefixAndName);
    }
    void SetLink(QString sPrefix, QString sName)
    {
        QString sLink(sPrefix);
        sLink += sName;
        
        sLinks.append(sLink);
    }
    
    void SetTreeWidgetItem(QTreeWidgetItem *pItem)
    {
        pTreeItem = pItem;
        
        QVariant v; v.setValue(this);
        pTreeItem->setData(0, QTreeWidgetItem::UserType, v);
    }
};
Q_DECLARE_METATYPE(Image *)

class WidgetAtlasGroup : public QWidget
{
    Q_OBJECT

    QDir                    m_MetaDir;
    DlgAtlasGroupSettings   m_dlgSettings;
    
    QList<Image *>          m_ImageList;
    ImagePacker             m_Packer;

public:
    explicit WidgetAtlasGroup(QWidget *parent = 0);
    explicit WidgetAtlasGroup(QDir metaDir, QWidget *parent = 0);
    ~WidgetAtlasGroup();

    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType);

private slots:
    void on_btnAddImages_clicked();
    void on_btnAddDir_clicked();

private:
    Ui::WidgetAtlasGroup *ui;
    
    void ImportImages(QStringList sImportImgList);
    void Refresh();
};

#endif // WIDGETATLASGROUP_H
