/**************************************************************************
 *	WidgetFontState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTWIDGETSTATE_H
#define FONTWIDGETSTATE_H

#include "FontModelView.h"

#include <QWidget>

namespace Ui {
class FontWidgetState;
}

class FontWidget;

class FontWidgetState : public QWidget
{
    Q_OBJECT
    
    FontWidget *                    m_pOwner;
    FontTableModel *               m_pFontModel;
    
    QString                         m_sName;

    int                             m_iPrevFontCmbIndex;
    double                          m_dPrevFontSize;
    
public:
    explicit FontWidgetState(FontWidget *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    virtual ~FontWidgetState();
    
    QString GetName();
    void SetName(QString sNewName);
    
    void SetSelectedFont(QString sFontName);
    
    FontTableModel *GetFontModel();
    FontTableView *GetFontLayerView();
    
    QString GetFontFilePath();
    
    rendermode_t GetCurSelectedRenderMode();
    
    float GetSize();
    void SetSize(double dSize);
    
    float GetThickness();
    
    int GetSelectedStageId();
    
    void UpdateActions();
    
private Q_SLOTS:
    void on_cmbFontList_currentIndexChanged(int index);

    void on_cmbRenderMode_currentIndexChanged(int index);
    
    void on_sbSize_editingFinished();
    
    void on_layersView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);
    
private:
    Ui::FontWidgetState *ui;
};

#endif // FONTWIDGETSTATE_H
