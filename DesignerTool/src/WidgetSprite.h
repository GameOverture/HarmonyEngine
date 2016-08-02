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

    void Save();
    
    WidgetSpriteState *GetCurSpriteState();
    
private slots:
    void on_actionAddState_triggered();
    
    void on_actionRemoveState_triggered();
    
    void on_actionRenameState_triggered();
    
    void on_actionOrderStateBackwards_triggered();
    
    void on_actionOrderStateForwards_triggered();

    void on_actionImportFrames_triggered();

    void on_actionRemoveFrames_triggered();
    
    void on_actionOrderFrameUpwards_triggered();
    
    void on_actionOrderFrameDownwards_triggered();
    
    void on_cmbStates_currentIndexChanged(int index);

private:
    void UpdateActions();
    
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H
