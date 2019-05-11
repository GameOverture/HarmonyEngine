/**************************************************************************
 *	AudioWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include "AudioWidgetState.h"
#include "ProjectItem.h"
#include "IWidget.h"

namespace Ui {
class AudioWidget;
}

class AudioWidget : public IWidget
{
	Q_OBJECT
	
	AudioWidgetState *      m_pCurAudioState;

	QList<QAction *>        m_StateActionsList;
	
public:
	explicit AudioWidget(ProjectItem &itemRef, QWidget *pParent /*= nullptr*/);
	~AudioWidget();
	
	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;
	
private Q_SLOTS:

private:
	Ui::AudioWidget *ui;
};

#endif // AUDIOWIDGET_H
