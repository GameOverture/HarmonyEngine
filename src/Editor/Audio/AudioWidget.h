/**************************************************************************
*	AudioWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include "Global.h"
#include "IWidget.h"

#include <QWidget>
#include <QMenu>
#include <QComboBox>

namespace Ui {
	class AudioWidget;
}

class AudioWidget : public IWidget
{
	Q_OBJECT

public:
	explicit AudioWidget(ProjectItem &itemRef, QWidget *pParent = nullptr);
	~AudioWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

private:
	Ui::AudioWidget *ui;
};

#endif // AUDIOWIDGET_H
