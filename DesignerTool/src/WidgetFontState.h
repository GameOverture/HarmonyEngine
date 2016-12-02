/**************************************************************************
 *	WidgetFontState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETFONTSTATE_H
#define WIDGETFONTSTATE_H

#include "WidgetFontModelView.h"

#include <QWidget>

namespace Ui {
class WidgetFontState;
}

class WidgetFont;

class WidgetFontState : public QWidget
{
    Q_OBJECT
    
    WidgetFont *                    m_pOwner;
    WidgetFontModel *               m_pFontModel;
    
    QString                         m_sName;

    int                             m_iPrevFontCmbIndex;
    double                          m_dPrevFontSize;
    
public:
    explicit WidgetFontState(WidgetFont *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    virtual ~WidgetFontState();
    
    QString GetName();
    void SetName(QString sNewName);
    
    void SetSelectedFont(QString sFontName);
    
    WidgetFontModel *GetFontModel();
    WidgetFontTableView *GetFontLayerView();
    
    QString GetFontFilePath();
    
    rendermode_t GetCurSelectedRenderMode();
    
    float GetSize();
    void SetSize(double dSize);
    
    float GetThickness();
    
    int GetSelectedStageId();
    
    void UpdateActions();
    
private slots:
    void on_cmbFontList_currentIndexChanged(int index);

    void on_cmbRenderMode_currentIndexChanged(int index);
    
    void on_sbSize_editingFinished();
    
    void on_layersView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);
    
private:
    Ui::WidgetFontState *ui;
};
Q_DECLARE_METATYPE(WidgetFontState *)

#endif // WIDGETFONTSTATE_H
