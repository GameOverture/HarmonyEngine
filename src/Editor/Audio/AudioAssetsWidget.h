/**************************************************************************
 *	AudioAssetsWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOASSETSWIDGET_H
#define AUDIOASSETSWIDGET_H

#include "Project.h"

#include <QWidget>
#include <QMap>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class AudioAssetsWidget;
}

class AudioAssetsWidget : public QWidget
{
	Q_OBJECT
	
	Project *						m_pProjOwner;
	QStringList						m_sBankNameList;

public:
	explicit AudioAssetsWidget(QWidget *parent = 0);
	explicit AudioAssetsWidget(Project *pProjOwner, QWidget *parent = 0);
	~AudioAssetsWidget();

	Project *GetItemProject();

public Q_SLOTS:
	void on_btnScanAudio_pressed();

	void on_AudioMiddleware_currentIndexChanged(int index);
	
private:
	Ui::AudioAssetsWidget *ui;

	void SetupForFMOD();
};

#endif // AUDIOASSETSWIDGET_H
