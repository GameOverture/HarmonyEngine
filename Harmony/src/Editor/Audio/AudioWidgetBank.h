/**************************************************************************
 *	AudioWidgetBank.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOWIDGETBANK_H
#define AUDIOWIDGETBANK_H

#include "Global.h"
#include "AudioModelView.h"

#include <QWidget>

namespace Ui {
class AudioWidgetBank;
}

class AudioWidgetManager;

class AudioWidgetBank : public QWidget
{
	Q_OBJECT
	
	// NOTE: Order of these member variables matter here for the member initializer list
	AudioWidgetManager *        m_pManager;
	
	QDir                        m_MetaDir;
	QDir                        m_DataDir;
	
	AudioBankTableModel *      m_pModel;

public:
	explicit AudioWidgetBank(QWidget *parent = 0);
	explicit AudioWidgetBank(QDir metaDir, QDir dataDir, AudioWidgetManager *pManager, QWidget *pParent = 0);
	~AudioWidgetBank();
	
	QString GetName();
	void SetName(QString sName);
	
	int GetId();

private Q_SLOTS:
	void on_btnAddWaves_pressed();

	void on_btnAddDir_pressed();

private:
	Ui::AudioWidgetBank *ui;

	void ImportWaves(QStringList sWaveFileList);

	void Refresh();
};

#endif // AUDIOWIDGETBANK_H
