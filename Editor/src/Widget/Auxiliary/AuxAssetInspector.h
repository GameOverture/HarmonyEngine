/**************************************************************************
 *	AuxAssetInspector.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUXASSETINSPECTOR_H
#define AUXASSETINSPECTOR_H

#include "Global.h"
#include "IAssetItemData.h"
#include "ManagerWidget.h"
#include "SoundClip.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QWheelEvent>
#include <QMediaPlayer>
#include <QAudioDecoder>

class WgtCodeEditor;

namespace Ui {
class AuxAssetInspector;
}

class AuxAssetInspector : public QWidget
{
	Q_OBJECT

	QGraphicsScene				m_AtlasesGfxScene;
	IAssetItemData *			m_pCurAtlasesAsset;

	QGraphicsScene				m_AudioGfxScene;
	SoundClip *					m_pCurAudioAsset;
	QAudioDecoder				m_AudioDecoder;
	QList<QAudioBuffer>			m_AudioBuffers;

	QMediaPlayer *				m_pMediaPlayer;

	QTimer						m_PanTimer;
	uint32						m_uiPanFlags;
	HyLocomotion2d				m_PanLocomotion;

public:
	explicit AuxAssetInspector(QWidget *parent = 0);
	virtual ~AuxAssetInspector();

	WgtCodeEditor &GetCodeEditor();

	void SetAssetManager(AssetManagerType eAssetManager);
	void SetFocusedAssets(AssetManagerType eAssetManager, const QList<IAssetItemData *> &selectedAssetsList);

	void Clear(AssetManagerType eAssetManager);

protected:
	virtual bool event(QEvent *pEvent) override;

	virtual void keyPressEvent(QKeyEvent *pEvent) override;
	virtual void keyReleaseEvent(QKeyEvent *pEvent) override;

private:
	Ui::AuxAssetInspector *ui;

private Q_SLOTS:
	void OnAudioBufferReady();
	void OnAudioBufferFinished();
	void OnPanTimer();

	void on_btnPlay_clicked();
};

#endif // AUXASSETINSPECTOR_H
