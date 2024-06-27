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
	m_PanTimer(this),
	m_uiPanFlags(0)
{
	ui->setupUi(this);

	ui->atlasesGfxView->setScene(&m_AtlasesGfxScene);
	ui->audioGfxView->setScene(&m_AudioGfxScene);
	ui->wgtCodeEditor->SetReadOnly(true);

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
			m_AtlasesGfxScene.clear();
			m_pCurAtlasesAsset = selectedAssetsList[0];
			//pNewPixmapItem->setPixmap(QPixmap(m_pCurInspectorAsset->GetAbsMetaFilePath()));
			QGraphicsPixmapItem *pNewPixmapItem = m_AtlasesGfxScene.addPixmap(QPixmap(m_pCurAtlasesAsset->GetAbsMetaFilePath()));
		}
		break;

	case ASSETMAN_Audio:
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

	m_PanLocomotion.Update();

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
