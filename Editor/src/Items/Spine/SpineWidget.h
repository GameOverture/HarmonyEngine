/**************************************************************************
*	SpineWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPINEWIDGET_H
#define SPINEWIDGET_H

#include "SpineModel.h"

#include <QWidget>
#include <QTableView>
#include "ui_SpineWidget.h"

class SpineWidget : public IWidget
{
	Q_OBJECT

public:
	SpineWidget(ProjectItemData &itemRef, QWidget *parent = nullptr);
	~SpineWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

private:
	Ui::SpineWidget ui;
};

#endif // SPINEWIDGET_H
