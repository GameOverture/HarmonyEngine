/**************************************************************************
*	TextWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include "TextModel.h"

#include <QWidget>
#include <QTableView>
#include "ui_TextWidget.h"

class TextWidget : public IWidget
{
	Q_OBJECT

public:
	TextWidget(ProjectItem &itemRef, QWidget *parent = nullptr);
	~TextWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

private Q_SLOTS:
	void on_cmbFont_currentIndexChanged(int index);

	void on_actionAddFill_triggered();


private:
	Ui::TextWidget ui;
};

#endif // TEXTWIDGET_H
