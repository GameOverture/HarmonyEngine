/**************************************************************************
 *	WidgetAtlasManager.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETATLASMANAGER_H
#define WIDGETATLASMANAGER_H

#include <QWidget>
#include <QDir>
#include <QMouseEvent>
#include <QStringListModel>
#include <QStackedWidget>
#include <QThread>

#include "ItemProject.h"
#include "WidgetAtlasGroup.h"

namespace Ui {
class WidgetAtlasManager;
}

class WidgetAtlasManagerLoadThread : public QThread
{
    Q_OBJECT

    QString m_sSettingsPath;

public:
    WidgetAtlasManagerLoadThread(QString sPath, QObject *pParent) : QThread(pParent),
                                                                    m_sSettingsPath(sPath)
    { }

    void run() Q_DECL_OVERRIDE
    {
        /* ... here is the expensive or blocking operation ... */
        ItemProject *pNewItemProject = new ItemProject(m_sPath);
        Q_EMIT LoadFinished(pNewItemProject);
    }

Q_SIGNALS:
    void LoadFinished(ItemProject *pLoadedItemProject);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetAtlasGroupTreeWidget : public QTreeWidget
{
    WidgetAtlasManager *      m_pOwner;

public:
    WidgetAtlasGroupTreeWidget(QWidget *parent = Q_NULLPTR);
    void SetOwner(WidgetAtlasManager *pOwner);

protected:
    virtual void dropEvent(QDropEvent *e);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetAtlasGroupTreeWidgetItem : public QTreeWidgetItem
{
public:
    WidgetAtlasGroupTreeWidgetItem(int type = Type) : QTreeWidgetItem(type)
    { }

    WidgetAtlasGroupTreeWidgetItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type)
    { }

    WidgetAtlasGroupTreeWidgetItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type)
    { }

    bool operator<(const QTreeWidgetItem& other) const;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetAtlasManager : public QWidget
{
    Q_OBJECT

    friend class WidgetAtlasGroup;

    ItemProject *                   m_pProjOwner;

    QDir                            m_MetaDir;
    QDir                            m_DataDir;
    
    QTreeWidgetItem *               m_pMouseHoverItem;


    DlgAtlasGroupSettings       m_dlgSettings;


    ImagePacker                 m_Packer;

public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlasManager();
    
    ItemProject *GetProjOwner()     { return m_pProjOwner; }

    QSize GetAtlasDimensions();
    void WriteMetaSettings();
    void WriteMetaSettings(QJsonArray frameArray);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HyGuiFrame *GenerateFrame(ItemWidget *pItem, QString sName, QImage &newImage, eAtlasNodeType eType);
    void ReplaceFrame(HyGuiFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

    QList<HyGuiFrame *> RequestFrames(ItemWidget *pItem);
    QList<HyGuiFrame *> RequestFrames(ItemWidget *pItem, QList<HyGuiFrame *> requestList);
    QList<HyGuiFrame *> RequestFrames(ItemWidget *pItem, QList<quint32> requestList);

    void RelinquishFrames(ItemWidget *pItem, QList<HyGuiFrame *> relinquishList);
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    friend void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);

private Q_SLOTS:
    void on_btnAddImages_clicked();

    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();

    void on_actionDeleteImages_triggered();

    void on_actionReplaceImages_triggered();

    void on_actionAddFilter_triggered();

    void on_atlasList_itemSelectionChanged();

private:
    Ui::WidgetAtlasManager *ui;

    void PreviewAtlasGroup();
    void HideAtlasGroup();

    void SaveData();

    void SetDependency(HyGuiFrame *pFrame, ItemWidget *pItem);
    void RemoveDependency(HyGuiFrame *pFrame, ItemWidget *pItem);

    void CreateTreeItem(WidgetAtlasGroupTreeWidgetItem *pParent, HyGuiFrame *pFrame);

    void GetAtlasInfoForGameData(QJsonObject &atlasObjOut);

    QSet<HyGuiFrame *> ImportImages(QStringList sImportImgList);
    HyGuiFrame *ImportImage(QString sName, QImage &newImage, eAtlasNodeType eType);

    void RepackAll();
    void Repack(QSet<int> repackTexIndicesSet, QSet<HyGuiFrame *> newFramesSet);
    void ConstructAtlasTexture(int iPackerBinIndex, int iTextureArrayIndex);
    void Refresh();

protected:
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // WIDGETATLASMANAGER_H
