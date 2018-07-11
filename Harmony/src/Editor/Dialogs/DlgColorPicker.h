/**************************************************************************
 *	DlgColorPicker.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGCOLORPICKER_H
#define DLGCOLORPICKER_H

#include <QDialog>

namespace Ui {
class DlgColorPicker;
}

class DlgColorPicker : public QDialog
{
	Q_OBJECT
	
public:
	explicit DlgColorPicker(QString sTitle, QColor initTopColor, QColor initBotColor, QWidget *parent = 0);
	~DlgColorPicker();
	
	bool IsSolidColor();

	QColor GetSolidColor();
	
	QColor GetVgTopColor();
	QColor GetVgBotColor();
	
private:
	Ui::DlgColorPicker *ui;
};

#endif // DLGCOLORPICKER_H
