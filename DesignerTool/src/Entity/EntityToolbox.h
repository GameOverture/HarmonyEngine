/**************************************************************************
*	EntityToolbox.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYTOOLBOX_H
#define ENTITYTOOLBOX_H

#include <QWidget>

namespace Ui {
class EntityToolbox;
}

class EntityToolbox : public QWidget
{
	Q_OBJECT

public:
	explicit EntityToolbox(QWidget *parent = 0);
	~EntityToolbox();

private:
	Ui::EntityToolbox *ui;
};

#endif // ENTITYTOOLBOX_H
