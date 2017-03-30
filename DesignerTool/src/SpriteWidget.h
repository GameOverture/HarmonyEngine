/**************************************************************************
 *	WidgetSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEWIDGET_H
#define SPRITEWIDGET_H

#include "AtlasFrame.h"
#include "SpriteDraw.h"

#include <QWidget>
#include <QMenu>
#include <QComboBox>

namespace Ui {
class SpriteWidget;
}

class AtlasWidget;

class SpriteWidget : public QWidget
{
    Q_OBJECT
    
    ProjectItem &           m_ItemRef;
    SpriteDraw *            m_pDraw;

    bool                    m_bPlayActive;
    float                   m_fElapsedTime;
    bool                    m_bIsBounced;

public:
    explicit SpriteWidget(ProjectItem &itemRef, IHyApplication &hyAppRef, QWidget *parent = 0);
    ~SpriteWidget();
    
    ProjectItem &GetItem();

    void SetSelectedState(int iIndex);

//    SpriteFrame *GetSelectedFrame();
//    int GetSelectedIndex();
//    int GetNumFrames();
    
    void OnGiveMenuActions(QMenu *pMenu);
    void GetSaveInfo(QJsonArray &spriteStateArrayRef);

    void OnShow();
    void OnHide();
    void OnUpdate();

    void UpdateTimeStep();

    void RefreshData(QVariant param);
    void RefreshDraw(IHyApplication &hyAppRef);
    void UpdateActions();

private:
    SpriteStateData *GetCurStateData();
    
private Q_SLOTS:
    void on_framesView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);

    void on_actionAddState_triggered();
    
    void on_actionRemoveState_triggered();
    
    void on_actionRenameState_triggered();
    
    void on_actionOrderStateBackwards_triggered();
    
    void on_actionOrderStateForwards_triggered();
    
    void on_cmbStates_currentIndexChanged(int index);

    void on_actionAlignLeft_triggered();
    
    void on_actionAlignRight_triggered();
    
    void on_actionAlignUp_triggered();
    
    void on_actionAlignDown_triggered();
    
    void on_actionAlignCenterVertical_triggered();
    
    void on_actionAlignCenterHorizontal_triggered();

    void on_actionImportFrames_triggered();

    void on_actionRemoveFrames_triggered();

    void on_actionOrderFrameUpwards_triggered();

    void on_actionOrderFrameDownwards_triggered();

    void on_actionPlay_triggered();

    void on_actionFirstFrame_triggered();

    void on_actionLastFrame_triggered();

    void on_chkReverse_clicked();

    void on_chkLoop_clicked();

    void on_chkBounce_clicked();

    void on_btnHz10_clicked();

    void on_btnHz20_clicked();

    void on_btnHz30_clicked();

    void on_btnHz40_clicked();

    void on_btnHz50_clicked();

    void on_btnHz60_clicked();

private:
    Ui::SpriteWidget *ui;
};

#endif // SPRITEWIDGET_H
