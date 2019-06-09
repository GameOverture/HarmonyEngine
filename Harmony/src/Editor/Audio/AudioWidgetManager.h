/**************************************************************************
 *	AudioWidgetManager.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWIDGETMANAGER_H
#define AUDIOWIDGETMANAGER_H

#include "Project.h"

#include <QWidget>
#include <QMap>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class AudioWidgetManager;
}

class AudioWidgetManager : public QWidget
{
	Q_OBJECT
	
	Project *						m_pProjOwner;
	
	QDir							m_MetaDir;
	QDir							m_DataDir;

	QTcpSocket						m_Socket;

public:
	explicit AudioWidgetManager(QWidget *parent = 0);
	explicit AudioWidgetManager(Project *pProjOwner, QWidget *parent = 0);
	~AudioWidgetManager();

	Project *GetItemProject();

public Q_SLOTS:
	void on_btnScanAudio_pressed();

	void on_AudioMiddleware_currentIndexChanged(int index);

	void ReadData();
	void OnError(QAbstractSocket::SocketError socketError);
	
private:
	Ui::AudioWidgetManager *ui;
};

#endif // AUDIOWIDGETMANAGER_H
