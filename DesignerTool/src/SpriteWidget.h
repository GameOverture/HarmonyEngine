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

#include "SpriteWidgetState.h"
#include "AtlasFrame.h"

#include <QWidget>
#include <QMenu>
#include <QComboBox>

namespace Ui {
class SpriteWidget;
}

class SpriteItem;
class AtlasesWidget;

class SpriteWidget : public QWidget
{
    Q_OBJECT
    
    SpriteItem *            m_pItemSprite;

    QList<QAction *>        m_StateActionsList;
    SpriteWidgetState *     m_pCurSpriteState;

public:
    explicit SpriteWidget(SpriteItem *pItemSprite, QWidget *parent = 0);
    ~SpriteWidget();
    
    SpriteItem *GetData();
    
    void OnGiveMenuActions(QMenu *pMenu);

    void GetSaveInfo(QJsonArray &spriteStateArrayRef);
    
    SpriteWidgetState *GetCurSpriteState();

    void Relink(AtlasFrame *pFrame);
    
    QList<AtlasFrame *> GetAllDrawInsts();
    
    void UpdateActions();
    
private Q_SLOTS:
    void on_actionAddState_triggered();
    
    void on_actionRemoveState_triggered();
    
    void on_actionRenameState_triggered();
    
    void on_actionOrderStateBackwards_triggered();
    
    void on_actionOrderStateForwards_triggered();

    void on_actionImportFrames_triggered();

    void on_actionRemoveFrames_triggered();
    
    void on_cmbStates_currentIndexChanged(int index);

    void on_actionAlignLeft_triggered();
    
    void on_actionAlignRight_triggered();
    
    void on_actionAlignUp_triggered();
    
    void on_actionAlignDown_triggered();
    
    void on_actionAlignCenterVertical_triggered();
    
    void on_actionAlignCenterHorizontal_triggered();
    
    void on_actionOrderFrameUpwards_triggered();

    void on_actionOrderFrameDownwards_triggered();

private:
    Ui::SpriteWidget *ui;
};

#endif // SPRITEWIDGET_H
