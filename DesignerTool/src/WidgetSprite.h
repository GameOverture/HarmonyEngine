/**************************************************************************
 *	WidgetSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITE_H
#define WIDGETSPRITE_H

#include "WidgetSpriteState.h"
#include "HyGuiFrame.h"

#include <QWidget>
#include <QMenu>
#include <QComboBox>

namespace Ui {
class WidgetSprite;
}

class ItemSprite;
class WidgetAtlasManager;

class WidgetSprite : public QWidget
{
    Q_OBJECT
    
    ItemSprite *            m_pItemSprite;

    QList<QAction *>        m_StateActionsList;
    WidgetSpriteState *     m_pCurSpriteState;

public:
    explicit WidgetSprite(ItemSprite *pItemSprite, QWidget *parent = 0);
    ~WidgetSprite();
    
    ItemSprite *Owner();
    
    void LoadAndInit();
    
    void AppendActionsForToolBar(QList<QAction *> &actionList);

    void GetSpriteStateInfo(QJsonArray &spriteStateArrayRef);
    
    WidgetSpriteState *GetCurSpriteState();
    
    QList<HyGuiFrame *> GetAllDrawInsts();
    
    void UpdateActions();
    
private slots:
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
    
private:
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H
