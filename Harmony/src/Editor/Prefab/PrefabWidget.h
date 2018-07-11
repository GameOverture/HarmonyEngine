/**************************************************************************
 *	PrefabWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PREFABWIDGET_H
#define PREFABWIDGET_H

#include "PrefabModel.h"

#include <QWidget>

namespace Ui {
class PrefabWidget;
}

class PrefabWidget : public QWidget
{
    Q_OBJECT

	ProjectItem &           m_ItemRef;

public:
	explicit PrefabWidget(ProjectItem &itemRef, QWidget *parent = 0);
	~PrefabWidget();

	void OnGiveMenuActions(QMenu *pMenu);

	void FocusState(int iStateIndex, QVariant subState);

private:
	Ui::PrefabWidget *ui;
};

#endif // PREFABWIDGET_H
