/**************************************************************************
 *	AuxToolBox.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUXTOOLBOX_H
#define AUXTOOLBOX_H

#include <QWidget>

namespace Ui {
class AuxToolBox;
}

class AuxToolBox : public QWidget
{
	Q_OBJECT

public:
	explicit AuxToolBox(QWidget *pParent = nullptr);
	/*virtual*/ ~AuxToolBox();

private:
	Ui::AuxToolBox *ui;
};

#endif // WIDGETOUTPUTLOG_H
