/**************************************************************************
 *	AuxTileSet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AuxTileSet_H
#define AuxTileSet_H

#include <QWidget>
#include <QDataWidgetMapper>

class EntityTreeItemData;
class EntityStateData;

namespace Ui {
class AuxTileSet;
}

class AuxTileSet : public QWidget
{
	Q_OBJECT

public:
	explicit AuxTileSet(QWidget *pParent = nullptr);
	virtual ~AuxTileSet();

	//void Init(AtlasTileSet *pTileSet);

private:
	Ui::AuxTileSet *ui;

private Q_SLOTS:
	void on_actionImportTileSheet_triggered();

};

#endif // WIDGETOUTPUTLOG_H
