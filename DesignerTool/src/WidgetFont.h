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

#include <QWidget>

#include "freetype-gl/freetype-gl.h"

namespace Ui {
class WidgetFont;
}

class ItemFont;

class WidgetFont : public QWidget
{
    Q_OBJECT

    ItemFont *                  m_pItemFont;
    
    texture_atlas_t *           m_pAtlas;
    QList<texture_font_t *>     m_TextureFontList;
    
public:
    explicit WidgetFont(ItemFont *pOwner, QWidget *parent = 0);
    ~WidgetFont();
    
    void GeneratePreview();

private slots:
    void on_cmbAtlasGroups_currentIndexChanged(int index);
    
    void on_cmbFonts_currentIndexChanged(int index);
    
    void on_actionAddFontSize_triggered();
    
    void on_actionRemoveFontSize_triggered();
    
    void on_chk_09_stateChanged(int arg1);
    
    void on_chk_az_stateChanged(int arg1);
    
    void on_chk_AZ_stateChanged(int arg1);
    
    void on_chk_symbols_stateChanged(int arg1);
    
    void on_txtAdditionalSymbols_editingFinished();
    
private:
    Ui::WidgetFont *ui;
};

#endif // WIDGETFONT_H
