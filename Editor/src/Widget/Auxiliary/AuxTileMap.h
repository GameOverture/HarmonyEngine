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

#include "Global.h"
#include "TileMapGfxScene.h"

#include <QWidget>
#include <QDataWidgetMapper>
#include <QToolBar>
#include <QTableView>

namespace Ui {
class AuxTileMap;
}

class AtlasManager;
class TileMapModel;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class TileSetTreeView : public QTreeView
//{
//	Q_OBJECT
//
//public:
//	TileSetTreeView(QWidget *pParent = nullptr);
//
//protected:
//	virtual void resizeEvent(QResizeEvent *pResizeEvent);
//};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AuxTileMap : public QWidget
{
	Q_OBJECT

	TileMapModel *				m_pTileMapModel;

	QActionGroup *				m_pToolActionGroup;
	QToolBar *					m_pToolBar;

	TileMapGfxScene *			m_pTileMapGfxScene;

public:
	explicit AuxTileMap(QWidget *pParent = nullptr);
	virtual ~AuxTileMap();

	void Init(AtlasManager &atlasManagerRef, TileMapModel *pTileMapModel);

	TileMapTool GetSelectedTool() const;

private:
	Ui::AuxTileMap *ui;

private Q_SLOTS:
	void on_tileSetsTreeView_clicked(QModelIndex index);
};

#endif // AuxTileMap_H
