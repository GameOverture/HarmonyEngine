/**************************************************************************
 *	AudioWidgetState.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWIDGETSTATE_H
#define AUDIOWIDGETSTATE_H

#include <QWidget>

namespace Ui {
class AudioWidgetState;
}

class AudioWidget;

class AudioWidgetState : public QWidget
{
	Q_OBJECT
	
	AudioWidget *                       m_pOwner;
	QString                             m_sName;
	
	int                                 m_iPrevCategoryIndex;
	int                                 m_iPrevPlayTypeIndex;
	int                                 m_iPrevNumInst;
	
public:
	explicit AudioWidgetState(AudioWidget *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
	~AudioWidgetState();
	
	QString GetName();
	void SetName(QString sName);
	
	void UpdateActions();
	
private Q_SLOTS:
	
	void on_cmbCategory_currentIndexChanged(int index);
	
	void on_chkLimitInst_clicked();
	
	void on_chkLooping_clicked();
	
	void on_cmbPlayType_currentIndexChanged(int index);
	
	void on_sbInstMax_editingFinished();
	
	void on_radInstFail_toggled(bool checked);
	
	void on_radInstQueue_toggled(bool checked);
	
private:
	Ui::AudioWidgetState *ui;
};

#endif // AUDIOWIDGETSTATE_H
