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

#include "IWidget.h"
#include "PrefabModel.h"

#include <QWidget>

namespace Ui {
class PrefabWidget;
}

class PrefabWidget : public IWidget
{
    Q_OBJECT

public:
	explicit PrefabWidget(ProjectItem &itemRef, QWidget *pParent = nullptr);
	~PrefabWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void FocusState(int iStateIndex, QVariant subState) override;

private:
	Ui::PrefabWidget *ui;
};

#endif // PREFABWIDGET_H
