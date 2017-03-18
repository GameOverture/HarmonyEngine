/**************************************************************************
 *	WidgetSpriteState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEWIDGETSTATE_H
#define SPRITEWIDGETSTATE_H

#include "AtlasFrame.h"
#include "SpriteModelView.h"

#include <QWidget>
#include <QCheckBox>

namespace Ui {
class SpriteWidgetState;
}

class SpriteFrame;
class SpriteWidget;
class SpriteFramesModel;

class SpriteWidgetState : public QWidget
{
    Q_OBJECT

    SpriteWidget *                      m_pOwner;
    SpriteFramesModel *                  m_pSpriteFramesModel;
    
    QString                             m_sName;
    
    bool                                m_bPlayActive;
    float                               m_fElapsedTime;
    bool                                m_bIsBounced;

public:
    explicit SpriteWidgetState(SpriteWidget *pOwner, SpriteFramesModel *pTableModel, QList<QAction *> stateActionList, QWidget *parent = 0);
    ~SpriteWidgetState();
    
    QString GetName();
    void SetName(QString sNewName);

    void InsertFrame(AtlasFrame *pFrame);
    void RefreshFrame(AtlasFrame *pFrame);
    void RemoveFrame(AtlasFrame *pFrame);

//    QCheckBox *GetChkBox_Reverse();
//    QCheckBox *GetChkBox_Looping();
//    QCheckBox *GetChkBox_Bounce();
    SpriteTableView *GetFrameView();
    SpriteFrame *GetSelectedFrame();
    int GetSelectedIndex();
    int GetNumFrames();
    
    void AppendFramesToListRef(QList<AtlasFrame *> &drawInstListRef);
    
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
    Ui::SpriteWidgetState *ui;
};
Q_DECLARE_METATYPE(SpriteWidgetState *)

#endif // SPRITEWIDGETSTATE_H
