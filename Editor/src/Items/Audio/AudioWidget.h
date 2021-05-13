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
#include <QTableView>

namespace Ui {
	class AudioWidget;
}

class AudioWidget : public IWidget
{
	Q_OBJECT

public:
	explicit AudioWidget(ProjectItemData &itemRef, QWidget *pParent = nullptr);
	~AudioWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

	QTableView *GetPlayListTableView();

private:
	Ui::AudioWidget *ui;
	
private Q_SLOTS:
	void on_playListTableView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);
	void on_actionAddAudio_triggered();
	void on_actionRemoveAudio_triggered();
	void on_actionOrderAudioUpwards_triggered();
	void on_actionOrderAudioDownwards_triggered();
};

#endif // AUDIOWIDGET_H
