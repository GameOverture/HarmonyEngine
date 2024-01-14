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
#include "SourceFile.h"

#include <QGraphicsPixmapItem>

AuxAssetInspector::AuxAssetInspector(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AuxAssetInspector),
	m_AtlasesGfxScene(this),
	m_pCurAtlasesAsset(nullptr),
	m_AudioGfxScene(this)
{
	ui->setupUi(this);

	ui->atlasesGfxView->setScene(&m_AtlasesGfxScene);
	ui->audioGfxView->setScene(&m_AudioGfxScene);
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
		if(selectedAssetsList.empty())
			ui->wgtCodeEditor->SetSourceFile(nullptr);
		else
			ui->wgtCodeEditor->SetSourceFile(static_cast<SourceFile *>(selectedAssetsList[0]));
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
