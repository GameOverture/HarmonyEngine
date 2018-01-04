/**************************************************************************
*	EntSpriteFrame.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTSPRITEFRAME_H
#define ENTSPRITEFRAME_H

#include <QFrame>

namespace Ui {
class EntSpriteFrame;
}

class EntSpriteFrame : public QFrame
{
	Q_OBJECT

public:
	explicit EntSpriteFrame(QWidget *parent = 0);
	~EntSpriteFrame();

private:
	Ui::EntSpriteFrame *ui;
};

#endif // ENTSPRITEFRAME_H
