/**************************************************************************
 *	TileMapWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILEMAPWIDGET_H
#define TILEMAPWIDGET_H

#include "TileMapModels.h"
#include "IWidget.h"

#include <QWidget>
#include <QMenu>
#include <QComboBox>

namespace Ui {
class TileMapWidget;
}

class TileMapWidget : public IWidget
{
	Q_OBJECT

public:
	explicit TileMapWidget(ProjectItemData &itemRef, QWidget *pParent = nullptr);
	virtual ~TileMapWidget();
	
	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

private Q_SLOTS:

private:
	Ui::TileMapWidget *ui;
};

#endif // TILEMAPWIDGET_H
