/**************************************************************************
*	EntityCommon.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYCOMMON_H
#define ENTITYCOMMON_H

#include <QToolBox>

namespace Ui {
class EntityCommon;
}

class EntityCommon : public QToolBox
{
	Q_OBJECT

public:
	explicit EntityCommon(QWidget *parent = 0);
	~EntityCommon();

private:
	Ui::EntityCommon *ui;
};

#endif // ENTITYCOMMON_H
