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

#include "ItemProject.h"
#include "WidgetAtlasGroup.h"

namespace Ui {
class WidgetAtlasManager;
}

class WidgetAtlasGroupModel : public QStringListModel
{
    QStackedWidget &        m_AtlasGroupsRef;

public:
    WidgetAtlasGroupModel(QStackedWidget &atlasGroupsRef, QObject *pParent);

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetAtlasManager : public QWidget
{
    Q_OBJECT

    friend class WidgetAtlasGroup;

    ItemProject *                   m_pProjOwner;

    QDir                            m_MetaDir;
    QDir                            m_DataDir;

    QMap<quint32, HyGuiFrame *>     m_DependencyMap;
    
    QTreeWidgetItem *               m_pMouseHoverItem;

    WidgetAtlasGroupModel *         m_pCmbModel;

public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlasManager();
    
    ItemProject *GetProjOwner()     { return m_pProjOwner; }
    
    WidgetAtlasGroupModel *AllocateAtlasModelView();
    int CurrentAtlasGroupIndex();
    int GetAtlasIdFromIndex(int iIndex);
    QSize GetAtlasDimensions(int iIndex);

    QString GetSelectedAtlasGroup();
    void SetSelectedAtlasGroup(QString sName);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HyGuiFrame *GenerateFrame(ItemWidget *pItem, int iAtlasGroupId, QString sName, QImage &newImage, eAtlasNodeType eType);
    void ReplaceFrame(HyGuiFrame *pFrame, QString sName, QImage &newImage);

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
    void on_atlasGroups_currentChanged(int iIndex);

    void on_btnAddGroup_clicked();

    void on_cmbAtlasGroups_currentIndexChanged(int index);
    
    void on_actionDeleteAtlasGroup_triggered();
    
private:
    Ui::WidgetAtlasManager *ui;

    void AddAtlasGroup(int iId = -1);

    void PreviewAtlasGroup();
    void HideAtlasGroup();

    HyGuiFrame *CreateFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, eAtlasNodeType eType, int iW, int iH, int iTexIndex, int iX, int iY, uint uiErrors);
    void RemoveImage(HyGuiFrame *pFrame, QDir metaDir);

    void SaveData();

    void SetDependency(HyGuiFrame *pFrame, ItemWidget *pItem);
    void RemoveDependency(HyGuiFrame *pFrame, ItemWidget *pItem);
};

#endif // WIDGETATLASMANAGER_H
