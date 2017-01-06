/**************************************************************************
 *	WidgetSpriteState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITESTATE_H
#define WIDGETSPRITESTATE_H

#include "HyGuiFrame.h"
#include "WidgetSpriteModelView.h"

#include <QWidget>
#include <QCheckBox>

namespace Ui {
class WidgetSpriteState;
}

class SpriteFrame;
class WidgetSprite;

class WidgetSpriteState : public QWidget
{
    Q_OBJECT

    WidgetSprite *                      m_pOwner;
    WidgetSpriteModel *                 m_pSpriteFramesModel;
    
    QString                             m_sName;
    
    bool                                m_bPlayActive;
    float                               m_fElapsedTime;
    bool                                m_bIsBounced;

public:
    explicit WidgetSpriteState(WidgetSprite *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetSpriteState();
    
    QString GetName();
    void SetName(QString sNewName);

    void InsertFrame(HyGuiFrame *pFrame);
    void RefreshFrame(HyGuiFrame *pFrame);
    void RemoveFrame(HyGuiFrame *pFrame);

    QCheckBox *GetChkBox_Reverse();
    QCheckBox *GetChkBox_Looping();
    QCheckBox *GetChkBox_Bounce();
    WidgetSpriteTableView *GetFrameView();
    SpriteFrame *GetSelectedFrame();
    int GetSelectedIndex();
    int GetNumFrames();
    
    void AppendFramesToListRef(QList<HyGuiFrame *> &drawInstListRef);
    
    void GetStateFrameInfo(QJsonObject &stateObjOut);
    
    void UpdateTimeStep();
    
    void UpdateActions();

private Q_SLOTS:
    void on_framesView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);
    
    void on_actionPlay_triggered();
    
    void on_btnHz10_clicked();
    
    void on_btnHz20_clicked();
    
    void on_btnHz30_clicked();
    
    void on_btnHz40_clicked();
    
    void on_btnHz50_clicked();
    
    void on_btnHz60_clicked();
    
    void on_actionFirstFrame_triggered();
    
    void on_actionLastFrame_triggered();
    
    void on_chkReverse_clicked();

    void on_chkLoop_clicked();

    void on_chkBounce_clicked();
    
private:
    Ui::WidgetSpriteState *ui;
};
Q_DECLARE_METATYPE(WidgetSpriteState *)

#endif // WIDGETSPRITESTATE_H
