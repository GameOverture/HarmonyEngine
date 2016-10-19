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
#include "HyGuiFrame.h"
#include "ItemProject.h"

#include "scriptum/imagepacker.h"

namespace Ui {
class WidgetAtlasGroup;
}

class WidgetAtlasGroup : public QWidget
{
    Q_OBJECT

    friend class                WidgetAtlasManager;
    
    WidgetAtlasManager *        m_pManager;

    // NOTE: Order of these member variables matter here for the member initializer list
    QDir                        m_MetaDir;
    QDir                        m_DataDir;

    DlgAtlasGroupSettings       m_dlgSettings;
    
    QList<HyGuiFrame *>         m_FrameList;
    ImagePacker                 m_Packer;

public:
    explicit WidgetAtlasGroup(QWidget *parent = 0);
    explicit WidgetAtlasGroup(QDir metaDir, QDir dataDir, WidgetAtlasManager *pManager, QWidget *parent = 0);
    ~WidgetAtlasGroup();
    
    bool IsMatching(QDir metaDir, QDir dataDir);
    
    QList<HyGuiFrame *> &GetFrameList();
    QTreeWidget *GetTreeWidget();

    QSize GetAtlasDimensions();
    void GetAtlasInfo(QJsonObject &atlasObjOut);
    
    QString GetName();
    int GetId();

    void ResizeAtlasListColumns();

private slots:
    void on_btnAddImages_clicked();
    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();

    void on_atlasList_itemSelectionChanged();

    void on_actionDeleteImages_triggered();
    
    void on_actionReplaceImages_triggered();

protected:
    virtual void enterEvent(QEvent *pEvent);
    virtual void leaveEvent(QEvent *pEvent);

    virtual void showEvent(QShowEvent * event);
    virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::WidgetAtlasGroup *ui;
    
    void ImportImages(QStringList sImportImgList);
    HyGuiFrame *ImportImage(QString sName, QImage &newImage);

    void Refresh();

    void WriteMetaSettings(QJsonArray frameArray);

    void CreateTreeItem(QTreeWidgetItem *pParent, QString sName, int iTextureIndex, eAtlasNodeType eType, HyGuiFrame *pFrame);
};

#endif // WIDGETATLASGROUP_H
