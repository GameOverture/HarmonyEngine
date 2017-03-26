/**************************************************************************
 *	WidgetFont.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include "FontDraw.h"

#include <QWidget>
#include <QDir>
#include <QJsonArray>

#include "freetype-gl/freetype-gl.h"

namespace Ui {
class FontWidget;
}

class FontItem;
class FontTableModel;

class FontWidget : public QWidget
{
    Q_OBJECT

    ProjectItem &               m_ItemRef;
    FontDraw                    m_Draw;
    
    QSize                       m_PrevAtlasSize;
    
public:
    explicit FontWidget(ProjectItem &itemRef, IHyApplication &hyApp, QWidget *parent = 0);
    ~FontWidget();

    ProjectItem &GetItem();

    void SetSelectedState(int iIndex);

    void OnGiveMenuActions(QMenu *pMenu);

    void DrawShow();
    void DrawHide();
    void DrawUpdate();
    

    QString GetFullItemName();

    QComboBox *GetCmbStates();
    
    void SetGlyphsDirty();
    


    void Refresh(QVariant param);
    void UpdateActions();

    FontStateData *GetCurStateData();
    int GetSelectedStageId();

private Q_SLOTS:
    void on_chk_09_clicked();

    void on_chk_az_clicked();

    void on_chk_AZ_clicked();

    void on_chk_symbols_clicked();

    void on_txtAdditionalSymbols_editingFinished();

    void on_cmbStates_currentIndexChanged(int index);

    void on_actionAddState_triggered();

    void on_actionRemoveState_triggered();
    
    void on_actionRenameState_triggered();
    
    void on_actionOrderStateBackwards_triggered();
    
    void on_actionOrderStateForwards_triggered();
    
    void on_actionAddLayer_triggered();
    
    void on_actionRemoveLayer_triggered();

    void on_actionOrderLayerDownwards_triggered();

    void on_actionOrderLayerUpwards_triggered();

    void on_cmbRenderMode_currentIndexChanged(int index);

    void on_sbSize_editingFinished();

private:
    Ui::FontWidget *ui;
};

#endif // FONTWIDGET_H
