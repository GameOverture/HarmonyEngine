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

	setAcceptDrops(true);
	RestoreCursor();
}

/*virtual*/ HarmonyWidget::~HarmonyWidget()
{
	m_pTimer->stop();

	makeCurrent();
	delete m_pHyEngine;
	sm_bHarmonyLoaded = false;
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
	initStruct.sGameName = m_pProject->GetGameName().toStdString();
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
		return;

	ProjectTabBar *pTabBar = m_pProject->GetTabBar();
	ProjectItemData *pCurOpenTabItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItemData *>();

	if(pCurOpenTabItem && pCurOpenTabItem->GetType() == ITEM_Entity && pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE_ITEM))
	{
		EntityNodeTreeModel &childTreeModel = static_cast<EntityModel *>(pCurOpenTabItem->GetModel())->GetNodeTreeModel();

		if(childTreeModel.IsItemValid(static_cast<ProjectItemData *>(pEvent->source()), false))
			pEvent->acceptProposedAction();
		else
			pEvent->ignore();
	}
	else
		pEvent->ignore();
}

/*virtual*/ void HarmonyWidget::dropEvent(QDropEvent *pEvent) /*override*/
{
	if(m_pProject == nullptr)
		return;

	ProjectTabBar *pTabBar = m_pProject->GetTabBar();
	ProjectItemData *pCurOpenTabItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItemData *>();

	if(pCurOpenTabItem &&
	   pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE_ITEM) &&
	   static_cast<ProjectItemData *>(pEvent->source()) != pCurOpenTabItem &&
	   pCurOpenTabItem->GetType() == ITEM_Entity)
	{
		QList<QVariant> validItemList;
		// Parse mime data source for project item array
		QJsonDocument doc = QJsonDocument::fromJson(pEvent->mimeData()->data(HYGUI_MIMETYPE_ITEM));
		QJsonArray itemArray = doc.array();
		for(int iIndex = 0; iIndex < itemArray.size(); ++iIndex)
		{
			QJsonObject itemObj = itemArray[iIndex].toObject();

			// Ensure this item is apart of this project
			if(itemObj["project"].toString().toLower() == m_pProject->GetAbsPath().toLower())
			{
				QString sItemPath = itemObj["itemName"].toString();
				ExplorerItemData *pItem = m_pProject->GetExplorerModel().FindItemByItemPath(m_pProject, sItemPath, HyGlobal::GetTypeFromString(itemObj["itemType"].toString()));

				EntityNodeTreeModel &entityTreeModelRef = static_cast<EntityModel *>(pCurOpenTabItem->GetModel())->GetNodeTreeModel();
				if(entityTreeModelRef.IsItemValid(pItem, true) == false)
					continue;

				QVariant v;
				v.setValue<ExplorerItemData *>(pItem);
				validItemList.push_back(v);
			}
			else
				HyGuiLog("Item " % itemObj["itemName"].toString() % " is not apart of the entity's project and cannot be added.", LOGTYPE_Info);
		}

		QUndoCommand *pCmd = new EntityUndoCmd(ENTITYCMD_AddNewChildren, *pCurOpenTabItem, validItemList);
		pCurOpenTabItem->GetUndoStack()->push(pCmd);

		pEvent->setDropAction(Qt::LinkAction);
		pEvent->accept();
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
