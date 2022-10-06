/**************************************************************************
*	HarmonyWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "HarmonyWidget.h"
#include "ExplorerModel.h"
#include "ProjectItemMimeData.h"
#include "EntityModel.h"
#include "EntityUndoCmds.h"
#include "MainWindow.h"
#include "AssetMimeData.h"
#include "GlobalUndoCmds.h"

#include <QDragEnterEvent>

bool HarmonyWidget::sm_bHarmonyLoaded = false;

HarmonyWidget::HarmonyWidget(Project *pProject) :
	QOpenGLWidget(nullptr),
	m_pProject(pProject),
	m_pHyEngine(nullptr)
{
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnBootCheck()));
	m_pTimer->start(50);

	setMouseTracking(true);
	setAcceptDrops(true);
	RestoreCursor();
}

/*virtual*/ HarmonyWidget::~HarmonyWidget()
{
	sm_bHarmonyLoaded = false;
	m_pTimer->stop();

	makeCurrent();
	delete m_pHyEngine;
}

Project *HarmonyWidget::GetProject()
{
	return m_pProject;
}

bool HarmonyWidget::IsProject(Project *pProjectToTest)
{
	return m_pProject == pProjectToTest;
}

void HarmonyWidget::CloseProject()
{
	delete m_pHyEngine;
	m_pHyEngine = nullptr;
	m_pProject = nullptr;
}

void HarmonyWidget::SetCursor(Qt::CursorShape eShape)
{
	setCursor(eShape);
}

void HarmonyWidget::RestoreCursor()
{
	setCursor(Qt::CrossCursor);
}

HyRendererInterop *HarmonyWidget::GetHarmonyRenderer()
{
	if(m_pHyEngine)
		return &m_pHyEngine->GetRenderer();
	else
		return nullptr;
}

/*virtual*/ void HarmonyWidget::initializeGL() /*override*/
{
	if(m_pProject == nullptr)
		return;

	QString glType;
	QString glProfile;

	glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
	switch (format().profile())
	{
	case QSurfaceFormat::NoProfile: glProfile = "No Profile"; break;
	case QSurfaceFormat::CoreProfile: glProfile = "Core Profile"; break;
	case QSurfaceFormat::CompatibilityProfile: glProfile = "Compatibility Profile"; break;
	}

	HyGuiLog("Initializing OpenGL", LOGTYPE_Title);
	HyGuiLog(glType % "(" % glProfile % ")", LOGTYPE_Normal);

	// TODO: Get glGetString to work with 'glad'
	//HyGuiLog("Vendor: " % QString(reinterpret_cast<const char *>(glGetString(GL_VENDOR))), LOGTYPE_Normal);
	//HyGuiLog("Renderer: " % QString(reinterpret_cast<const char *>(glGetString(GL_RENDERER))), LOGTYPE_Normal);
	//HyGuiLog("Version: " % QString(reinterpret_cast<const char *>(glGetString(GL_VERSION))), LOGTYPE_Normal);
	//HyGuiLog("GLSL: " % QString(reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION))), LOGTYPE_Normal);

	HarmonyInit initStruct;
	initStruct.sGameName = m_pProject->GetTitle().toStdString();
	initStruct.sDataDir = m_pProject->GetAssetsAbsPath().toStdString();
	m_pHyEngine = new GuiHyEngine(initStruct, m_pProject);
}

/*virtual*/ void HarmonyWidget::paintGL() /*override*/
{
	if(sm_bHarmonyLoaded && m_pProject)
	{
		if(m_pHyEngine->Update() == false)
			HyGuiLog("Harmony Gfx requested exit program.", LOGTYPE_Info);
	}
}

/*virtual*/ void HarmonyWidget::resizeGL(int w, int h) /*override*/
{
	if(m_pProject)
		m_pProject->SetRenderSize(w, h);
}

/*virtual*/ void HarmonyWidget::enterEvent(QEvent *pEvent) /*override*/
{
	this->setFocus();
}

/*virtual*/ void HarmonyWidget::leaveEvent(QEvent *pEvent) /*override*/
{
}

/*virtual*/ void HarmonyWidget::dragEnterEvent(QDragEnterEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
	{
		pEvent->ignore();
		return;
	}

	ProjectTabBar *pTabBar = m_pProject->GetTabBar();
	if(pTabBar == nullptr || pTabBar->count() == 0)
	{
		pEvent->ignore();
		return;
	}

	bool bIsValid = false;
	ProjectItemData *pCurOpenTabItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItemData *>();
	if(pCurOpenTabItem)
	{
		switch(pCurOpenTabItem->GetType())
		{
		case ITEM_Audio:
			if(pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE_ASSET))
			{
				const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
				if(pMimeData && pMimeData->GetNumAssetsOfType(ASSET_Audio) != 0)
					bIsValid = true;
			}
			break;

		case ITEM_Sprite:
			if(pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE_ASSET))
			{
				const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
				if(pMimeData && pMimeData->GetNumAssetsOfType(ASSET_Atlas) != 0)
					bIsValid = true;
			}
			break;

		case ITEM_Entity:
			if(pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE_ITEM) ||
				pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE_ASSET))
			{
				bIsValid = true;
			}
			break;
		}
	}

	if(bIsValid)
		pEvent->acceptProposedAction();
	else
		pEvent->ignore();
}

/*virtual*/ void HarmonyWidget::dropEvent(QDropEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
	{
		pEvent->ignore();
		return;
	}

	const IMimeData *pMimeData = static_cast<const IMimeData *>(pEvent->mimeData());
	ProjectTabBar *pTabBar = m_pProject->GetTabBar();

	if(pTabBar == nullptr || pTabBar->count() == 0)
	{
		pEvent->ignore();
		return;
	};
	ProjectItemData *pCurOpenTabItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItemData *>();
	if(pCurOpenTabItem == nullptr)
	{
		pEvent->ignore();
		return;
	}

	switch(pCurOpenTabItem->GetType())
	{
	case ITEM_Audio: {
		const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
		QJsonArray assetsArray = pMimeData->GetAssetsArray(ASSET_Audio);
		QList<AssetItemData *> assetsList;
		for(int i = 0; i < assetsArray.size(); ++i)
		{
			QJsonObject assetObj = assetsArray[i].toObject();

			AssetItemData *pFoundAsset = m_pProject->GetAtlasModel().FindById(assetObj["assetUUID"].toString());
			if(pFoundAsset)
				assetsList.push_back(pFoundAsset);
		}
		
		if(assetsList.isEmpty() == false)
		{
			int iStateIndex = pCurOpenTabItem->GetWidget()->GetCurStateIndex();
			QUndoCommand *pCmd = new UndoCmd_LinkStateAssets("Add Audio", *pCurOpenTabItem, iStateIndex, assetsList);
			pCurOpenTabItem->GetUndoStack()->push(pCmd);
		}
		break; }

	case ITEM_Sprite: {
		const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
		QJsonArray assetsArray = pMimeData->GetAssetsArray(ASSET_Atlas);
		QList<AssetItemData *> assetsList;
		for(int i = 0; i < assetsArray.size(); ++i)
		{
			QJsonObject assetObj = assetsArray[i].toObject();

			AssetItemData *pFoundAsset = m_pProject->GetAtlasModel().FindById(assetObj["assetUUID"].toString());
			if(pFoundAsset)
				assetsList.push_back(pFoundAsset);
		}
		
		if(assetsList.isEmpty() == false)
		{
			int iStateIndex = pCurOpenTabItem->GetWidget()->GetCurStateIndex();
			QUndoCommand *pCmd = new UndoCmd_LinkStateAssets("Add Frames", *pCurOpenTabItem, iStateIndex, assetsList);
			pCurOpenTabItem->GetUndoStack()->push(pCmd);
		}
		break; }

	case ITEM_Entity: {
		const IMimeData *pMimeData = static_cast<const IMimeData *>(pEvent->mimeData());
		if(pMimeData->GetMimeType() == MIMETYPE_ProjectItems)
		{
			QList<ProjectItemData *> validItemList;
			// Parse mime data source for project item array
			QJsonDocument doc = QJsonDocument::fromJson(pEvent->mimeData()->data(HYGUI_MIMETYPE_ITEM));
			QJsonArray itemArray = doc.array();
			for(int iIndex = 0; iIndex < itemArray.size(); ++iIndex)
			{
				QJsonObject itemObj = itemArray[iIndex].toObject();

				// Ensure this item is apart of this project
				if(itemObj["project"].toString().toLower() == m_pProject->GetAbsPath().toLower())
				{
					QString sItemPath = itemObj["name"].toString();
					QUuid itemUuid(itemObj["UUID"].toString());

					ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(itemUuid);

					EntityTreeModel &entityTreeModelRef = static_cast<EntityModel *>(pCurOpenTabItem->GetModel())->GetNodeTreeModel();
					if(entityTreeModelRef.IsItemValid(pProjItem, true) == false)
						continue;

					validItemList.push_back(pProjItem);
				}
				else
					HyGuiLog("Item " % itemObj["itemName"].toString() % " is not apart of the entity's project and cannot be added.", LOGTYPE_Info);
			}

			QUndoCommand *pCmd = new EntityUndoCmd_AddChildren(*pCurOpenTabItem, validItemList);
			pCurOpenTabItem->GetUndoStack()->push(pCmd);
		}
		else // MIMETYPE_Assets
		{
		}
		break; }

	default:
		pEvent->ignore();
		break;
	}
}

/*virtual*/ void HarmonyWidget::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnKeyPressEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnMousePressEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::wheelEvent(QWheelEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnMouseWheelEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	QPointF ptCurMousePos = pEvent->localPos();
	m_pHyEngine->SetWidgetMousePos(glm::vec2(ptCurMousePos.x(), ptCurMousePos.y()));
	
	if(pCurItem->GetDraw())
		pCurItem->GetDraw()->OnMouseMoveEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectItemData *pCurItem = m_pProject->GetCurrentOpenItem();
	if(pCurItem == nullptr)
		return;

	pCurItem->GetDraw()->OnMouseReleaseEvent(pEvent);
}

void HarmonyWidget::OnBootCheck()
{
	if(m_pHyEngine /*&& m_pHyEngine->IsInitialized()*/)
	{
		Q_EMIT HarmonyWidgetReady(this);

		m_pTimer->stop();
		if(false == m_pTimer->disconnect())
			HyGuiLog("HarmonyWidget::OnBootCheck could not disconnect its signal.", LOGTYPE_Error);

		connect(m_pTimer, SIGNAL(timeout()), this, SLOT(update()));
		m_pTimer->start(10);

		makeCurrent();

		sm_bHarmonyLoaded = true;
	}
}
