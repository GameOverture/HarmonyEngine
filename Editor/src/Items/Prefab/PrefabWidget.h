/**************************************************************************
 *	PrefabWidget.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2018 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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
	explicit PrefabWidget(ProjectItemData &itemRef, QWidget *pParent = nullptr);
	~PrefabWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

private:
	Ui::PrefabWidget *ui;
};

#endif // PREFABWIDGET_H
