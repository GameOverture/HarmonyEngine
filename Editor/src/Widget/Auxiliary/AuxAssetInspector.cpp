/**************************************************************************
 *	AuxAssetInspector.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxAssetInspector.h"
#include "ui_AuxAssetInspector.h"
#include "IManagerModel.h"
#include "SourceModel.h"
#include "SourceFile.h"

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QScrollBar>

AuxAssetInspector::AuxAssetInspector(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AuxAssetInspector),
	m_AtlasesGfxScene(this),
	m_pCurAtlasesAsset(nullptr),
	m_AudioGfxScene(this),
	m_pCurAudioAsset(nullptr),
	m_AudioDecoder(this),
	m_PanTimer(this),
	m_uiPanFlags(0)
{
	ui->setupUi(this);

	ui->atlasesGfxView->setScene(&m_AtlasesGfxScene);
	ui->atlasesGfxView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->atlasesGfxView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	ui->audioGfxView->setScene(&m_AudioGfxScene);
	ui->audioGfxView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->audioGfxView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	ui->wgtCodeEditor->SetReadOnly(true);

	connect(&m_AudioDecoder, SIGNAL(bufferReady()), this, SLOT(OnAudioBufferReady()));
	connect(&m_AudioDecoder, SIGNAL(finished()), this, SLOT(OnAudioBufferFinished()));

	m_pMediaPlayer = new QMediaPlayer(this);

	connect(&m_PanTimer, SIGNAL(timeout()), this, SLOT(OnPanTimer()));
}

/*virtual*/ AuxAssetInspector::~AuxAssetInspector()
{
	delete ui;
}

WgtCodeEditor &AuxAssetInspector::GetCodeEditor()
{
	return *ui->wgtCodeEditor;
}

void AuxAssetInspector::SetAssetManager(AssetManagerType eAssetManager)
{
	if(ui->stackedWidget->count() < eAssetManager)
	{
		HyGuiLog("Missing Asset Page for Stacked Widget in AuxAssetInspector: ui->stackedWidget->count() >= eAssetType", LOGTYPE_Error);
		return;
	}
	ui->stackedWidget->setCurrentIndex(eAssetManager);
}

void AuxAssetInspector::SetFocusedAssets(AssetManagerType eAssetManager, const QList<IAssetItemData *> &selectedAssetsList)
{
	SetAssetManager(eAssetManager);

	switch(eAssetManager)
	{
	case ASSETMAN_Source:
		ui->wgtCodeEditor->CloseUnmodified();
		if(selectedAssetsList.empty() == false)
		{
			HyAssert(selectedAssetsList[0]->GetType() == ITEM_Header || selectedAssetsList[0]->GetType() == ITEM_Source, "AuxAssetInspector::SetFocusedAssets() - selectedAssetsList[0] is not a SourceFile");
			SourceFile *pSrcFile = static_cast<SourceFile *>(selectedAssetsList[0]);
			HyAssert(pSrcFile->GetManagerModel().GetAssetType() == eAssetManager, "AuxAssetInspector::SetFocusedAssets() - SourceFile's ManagerModel is not ASSETMAN_Source");

			ui->wgtCodeEditor->Open(pSrcFile);
		}
		break;

	case ASSETMAN_Atlases:
		if(selectedAssetsList.empty())
		{
			m_AtlasesGfxScene.clear();
			m_pCurAtlasesAsset = nullptr;
		}
		else if(m_pCurAtlasesAsset != selectedAssetsList[0])
		{
			m_pCurAtlasesAsset = selectedAssetsList[0];

			m_AtlasesGfxScene.clear();
			QGraphicsPixmapItem *pNewPixmapItem = m_AtlasesGfxScene.addPixmap(QPixmap(m_pCurAtlasesAsset->GetAbsMetaFilePath()));

			const float fMargins = 1420.0f;
			QRectF pixmapRect = pNewPixmapItem->boundingRect();
			QRectF sceneRect;
			sceneRect.setSize(pixmapRect.size().grownBy(QMarginsF(fMargins, fMargins, fMargins, fMargins)));
			m_AtlasesGfxScene.setSceneRect(sceneRect);

			QPointF ptCenter = sceneRect.center();
			pNewPixmapItem->setPos(ptCenter.x() - (pixmapRect.width() * 0.5f), ptCenter.y() - (pixmapRect.height() * 0.5f));

			ui->atlasesGfxView->centerOn(ptCenter);
		}
		break;

	case ASSETMAN_Audio:
		if(selectedAssetsList.empty())
		{
			m_AudioGfxScene.clear();
			m_pCurAudioAsset = nullptr;
		}
		else if(m_pCurAudioAsset != selectedAssetsList[0] && selectedAssetsList[0]->GetAssetManagerType() == ASSETMAN_Audio)
		{
			m_AudioGfxScene.clear();
			m_pCurAudioAsset = static_cast<SoundClip *>(selectedAssetsList[0]);

			m_AudioBuffers.clear();

			QAudioFormat audioFormat;
			audioFormat.setSampleRate(m_pCurAudioAsset->GetWaveHeader().SamplesPerSec);
			audioFormat.setChannelCount(m_pCurAudioAsset->GetWaveHeader().NumOfChan);
			audioFormat.setSampleSize(m_pCurAudioAsset->GetWaveHeader().BitsPerSample);
			audioFormat.setCodec("audio/pcm");
			audioFormat.setByteOrder(QAudioFormat::LittleEndian);
			audioFormat.setSampleType(QAudioFormat::SignedInt);

			m_AudioDecoder.setAudioFormat(audioFormat);
			m_AudioDecoder.setSourceFilename(m_pCurAudioAsset->GetAbsMetaFilePath());
			m_AudioDecoder.start();
		}
		break;

	default:
		HyGuiLog("AuxAssetInspector::SetFocusedAssets() - Unhandled Asset Type " % QString::number(eAssetManager), LOGTYPE_Error);
		break;
	}
}

void AuxAssetInspector::Clear(AssetManagerType eAssetManager)
{
	m_AtlasesGfxScene.clear();
	m_pCurAtlasesAsset = nullptr;
}

/*virtual*/ bool AuxAssetInspector::event(QEvent *pEvent) /*override*/
{
	if(pEvent->type() == QEvent::HoverEnter || pEvent->type() == QEvent::HoverLeave)
	{
		if(pEvent->type() == QEvent::HoverEnter)
			setFocus();
		else if(pEvent->type() == QEvent::HoverLeave)
		{
			m_uiPanFlags = 0;
			clearFocus();
		}
	}

	return QWidget::event(pEvent);
}

/*virtual*/ void AuxAssetInspector::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->key() == Qt::Key_A)
	{
		m_uiPanFlags |= PAN_LEFT;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
	else if(pEvent->key() == Qt::Key_D)
	{
		m_uiPanFlags |= PAN_RIGHT;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
	else if(pEvent->key() == Qt::Key_W)
	{
		m_uiPanFlags |= PAN_UP;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
	else if(pEvent->key() == Qt::Key_S)
	{
		m_uiPanFlags |= PAN_DOWN;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
}

/*virtual*/ void AuxAssetInspector::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->key() == Qt::Key_A)
		m_uiPanFlags &= ~PAN_LEFT;
	else if(pEvent->key() == Qt::Key_D)
		m_uiPanFlags &= ~PAN_RIGHT;
	else if(pEvent->key() == Qt::Key_W)
		m_uiPanFlags &= ~PAN_UP;
	else if(pEvent->key() == Qt::Key_S)
		m_uiPanFlags &= ~PAN_DOWN;
}

void AuxAssetInspector::OnAudioBufferReady()
{
	m_AudioBuffers.push_back(m_AudioDecoder.read());
}

void AuxAssetInspector::OnAudioBufferFinished()
{
	int iPixelWidth = 1000;// ui->audioGfxView->width();

	//struct SoundPixelGroup
	//{
	//	float		m_fChannel1Peak = 0.0f;
	//	float		m_fChannel1Rms = 0.0f;
	//	
	//	float		m_fChannel2Peak = 0.0f;
	//	float		m_fChannel2Rms = 0.0f;
	//};
	//QList<SoundPixelGroup> soundPixelGroupsList;

	//for(QAudioBuffer audBuff : m_AudioBuffers)
	//{
	//	QAudioFormat audioFormat = audBuff.format();
	//	int iNumChannels = audioFormat.channelCount();
	//	int iNumSamplePulls = audBuff.sampleCount() / iNumChannels;
	//	int iNumFrames = audBuff.frameCount();

	//	int iNumSamplesPerPixel = iNumFrames / iPixelWidth;

	//	float fSampleDiv;
	//	switch(audioFormat.sampleSize())
	//	{
	//	case 8:
	//		if(audioFormat.sampleType() == QAudioFormat::UnSignedInt)
	//			fSampleDiv = 255.0f;
	//		else
	//			fSampleDiv = 127.0f;
	//		break;

	//	case 16:
	//		if(audioFormat.sampleType() == QAudioFormat::UnSignedInt)
	//			fSampleDiv = 65535.0f;
	//		else
	//			fSampleDiv = 32767.0f;
	//		break;

	//	case 32:
	//		if(audioFormat.sampleType() == QAudioFormat::UnSignedInt)
	//			fSampleDiv = 4294967295.0f;
	//		else if(audioFormat.sampleType() == QAudioFormat::SignedInt)
	//			fSampleDiv = 2147483647.0f;
	//		else if(audioFormat.sampleType() == QAudioFormat::Float)
	//			fSampleDiv = 1.0f;
	//		break;
	//	}

	//	const qint16 *data = audBuff.constData<qint16>();
	//	for(int iSamplePull = 0; iSamplePull < iNumSamplePulls; iSamplePull += iNumSamplesPerPixel)
	//	{
	//		SoundPixelGroup soundPixelGroup;
	//		if(iNumChannels == 1)
	//		{
	//			for(int iSample = 0; iSample < iNumSamplesPerPixel; ++iSample)
	//			{
	//				float fSample = static_cast<float>(data[iSamplePull + iSample]);
	//				fSample /= fSampleDiv;

	//				soundPixelGroup.m_fChannel1Peak = HyMath::Max(soundPixelGroup.m_fChannel1Peak, fSample);
	//				soundPixelGroup.m_fChannel1Rms += fSample * fSample;
	//			}
	//			soundPixelGroup.m_fChannel1Rms = sqrt(soundPixelGroup.m_fChannel1Rms / iNumSamplesPerPixel);
	//		}
	//		else if(iNumChannels == 2)
	//		{
	//			for(int iSample = 0; iSample < iNumSamplesPerPixel; ++iSample)
	//			{
	//				float fSample1 = static_cast<float>(data[iSamplePull + iSample * 2]);
	//				fSample1 /= fSampleDiv;
	//				
	//				soundPixelGroup.m_fChannel1Peak = HyMath::Max(soundPixelGroup.m_fChannel1Peak, fSample1);
	//				soundPixelGroup.m_fChannel1Rms += fSample1 * fSample1;

	//				float fSample2 = static_cast<float>(data[iSamplePull + iSample * 2 + 1]);
	//				fSample2 /= fSampleDiv;

	//				soundPixelGroup.m_fChannel2Peak = HyMath::Max(soundPixelGroup.m_fChannel2Peak, fSample2);
	//				soundPixelGroup.m_fChannel2Rms += fSample2 * fSample2;
	//			}
	//			soundPixelGroup.m_fChannel1Rms = sqrt(soundPixelGroup.m_fChannel1Rms / iNumSamplesPerPixel);
	//			soundPixelGroup.m_fChannel2Rms = sqrt(soundPixelGroup.m_fChannel2Rms / iNumSamplesPerPixel);
	//		}

	//		soundPixelGroupsList.push_back(soundPixelGroup);
	//	}
	//	
	//} // for(m_AudioBuffers)

	//m_AudioGfxScene.setSceneRect(0, 0, iPixelWidth, 500);
	//for(int i = 0; i < iPixelWidth; ++i)
	//{
	//	SoundPixelGroup &soundPixelGroup = soundPixelGroupsList[i];
	//	//m_AudioGfxScene.addLine(i, 250 - soundPixelGroup.m_fChannel1Peak * 250, i, 250 + soundPixelGroup.m_fChannel1Peak * 250, QPen(Qt::darkBlue));
	//	m_AudioGfxScene.addLine(i, 250 - soundPixelGroup.m_fChannel1Rms * 250, i, 250 + soundPixelGroup.m_fChannel1Rms * 250, QPen(Qt::blue));
	//}
}

void AuxAssetInspector::OnPanTimer()
{
	if(m_uiPanFlags & PAN_LEFT)
		m_PanLocomotion.GoLeft();
	if(m_uiPanFlags & PAN_RIGHT)
		m_PanLocomotion.GoRight();
	if(m_uiPanFlags & PAN_UP)
		m_PanLocomotion.GoUp();
	if(m_uiPanFlags & PAN_DOWN)
		m_PanLocomotion.GoDown();

	m_PanLocomotion.UpdateSimple();

	if(m_PanLocomotion.IsMoving())
	{
		AssetManagerType eAssetManager = static_cast<AssetManagerType>(ui->stackedWidget->currentIndex());

		QScrollBar *pHorzScrollBar = nullptr;
		QScrollBar *pVertScrollBar = nullptr;
		switch(eAssetManager)
		{
		case ASSETMAN_Source:
		case ASSETMAN_Prefabs:
			break;

		case ASSETMAN_Atlases:
			pHorzScrollBar = ui->atlasesGfxView->horizontalScrollBar();
			pVertScrollBar = ui->atlasesGfxView->verticalScrollBar();
			break;

		case ASSETMAN_Audio:
			pHorzScrollBar = ui->audioGfxView->horizontalScrollBar();
			pVertScrollBar = ui->audioGfxView->verticalScrollBar();
			break;

		default:
			HyGuiLog("AuxAssetInspector::OnPanTimer() - Unhandled Asset Type " % QString::number(eAssetManager), LOGTYPE_Error);
			break;
		}

		pHorzScrollBar->setValue(pHorzScrollBar->value() + m_PanLocomotion.GetVelocity().x);
		pVertScrollBar->setValue(pVertScrollBar->value() + (m_PanLocomotion.GetVelocity().y * -1.0f));
		
		if(pHorzScrollBar->value() == pHorzScrollBar->minimum() || pHorzScrollBar->value() == pHorzScrollBar->maximum())
			m_PanLocomotion.StopX();
		if(pVertScrollBar->value() == pVertScrollBar->minimum() || pVertScrollBar->value() == pVertScrollBar->maximum())
			m_PanLocomotion.StopY();
	}
	else
		m_PanTimer.stop();
}

void AuxAssetInspector::on_btnPlay_clicked()
{
	if(m_pCurAudioAsset == nullptr)
		return;

	m_pMediaPlayer->setMedia(QUrl::fromLocalFile(m_pCurAudioAsset->GetAbsMetaFilePath()));
	m_pMediaPlayer->setVolume(50);
	m_pMediaPlayer->play();
}
