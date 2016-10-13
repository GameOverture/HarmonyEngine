/**************************************************************************
 *	WidgetFont.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETFONT_H
#define WIDGETFONT_H

#include "WidgetFontState.h"

#include <QWidget>
#include <QDir>

#include "freetype-gl/freetype-gl.h"

namespace Ui {
class WidgetFont;
}

class ItemFont;
class WidgetFontModel;

class WidgetFont : public QWidget
{
    Q_OBJECT

    ItemFont *                  m_pItemFont;

    QList<FontStagePass *>      m_MasterStageList;
    bool                        m_bGlyphsDirty;

    QList<QAction *>            m_StateActionsList;
    WidgetFontState *           m_pCurFontState;
    
    texture_atlas_t *           m_pAtlas;
    QDir                        m_FontMetaDir;
    int                         m_iPrevAtlasCmbIndex;
    
public:
    explicit WidgetFont(ItemFont *pOwner, QWidget *parent = 0);
    ~WidgetFont();
    
    ItemFont *GetItemFont();

    QString GetFullItemName();

    QComboBox *GetCmbStates();
    
    WidgetFontModel *GetCurrentFontModel();
    
    void SetGlyphsDirty();
    
    void GeneratePreview(bool bFindBestFit = false);
    
    texture_atlas_t *GetAtlas();
    
    QDir GetFontMetaDir();

    QSize GetAtlasDimensions(int iAtlasGrpIndex);

    void UpdateActions();

private slots:
    void on_cmbAtlasGroups_currentIndexChanged(int index);

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

private:
    Ui::WidgetFont *ui;
};

#endif // WIDGETFONT_H
