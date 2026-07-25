/**************************************************************************
 *	AuxTileMap.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AuxTileMap_H
#define AuxTileMap_H

#include <QWidget>
#include <QDataWidgetMapper>
#include <QToolBar>

namespace Ui {
class AuxTileMap;
}

class TileMapModel;

class AuxTileMap : public QWidget
{
	Q_OBJECT

	QTabBar *				m_pTabBar;
	QToolBar *				m_pToolBar;

public:
	explicit AuxTileMap(QWidget *pParent = nullptr);
	virtual ~AuxTileMap();

	void Init(TileMapModel &tileMapModelRef);

private:
	Ui::AuxTileMap *ui;

private Q_SLOTS:
};

#endif // AuxTileMap_H
