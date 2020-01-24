/**************************************************************************
 *	ProjectItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ProjectItem.h"
#include "MainWindow.h"
#include "AtlasWidget.h"
#include "AudioAssetsWidget.h"
#include "AudioModel.h"
#include "AtlasFrame.h"
#include "IModel.h"
#include "SpriteWidget.h"
#include "SpriteDraw.h"
#include "TextWidget.h"
#include "TextDraw.h"
#include "PrefabWidget.h"
#include "PrefabDraw.h"
#include "EntityWidget.h"
#include "EntityDraw.h"
#include "PrimitiveModel.h"

#include <QMenu>
#include <QJsonDocument>
#include <QJsonArray>

ProjectItem::ProjectItem(Project &projRef,
						 HyGuiItemType eType,
						 const QString sName,
						 const FileDataPair &initFileDataRef,
						 bool bIsPendingSave) :
	ExplorerItem(projRef, eType, sName),
	m_ItemFileData(initFileDataRef),
	m_bExistencePendingSave(bIsPendingSave),
	m_pModel(nullptr),
	m_pWidget(nullptr),
	m_pDraw(nullptr)
{
	m_bIsProjectItem = true;

	m_pUndoStack = new QUndoStack(this);
	m_pActionUndo = m_pUndoStack->createUndoAction(nullptr, "&Undo");
	m_pActionUndo->setIcon(QIcon(":/icons16x16/edit-undo.png"));
	m_pActionUndo->setShortcuts(QKeySequence::Undo);
	m_pActionUndo->setShortcutContext(Qt::ApplicationShortcut);
	m_pActionUndo->setObjectName("Undo");

	m_pActionRedo = m_pUndoStack->createRedoAction(nullptr, "&Redo");
	m_pActionRedo->setIcon(QIcon(":/icons16x16/edit-redo.png"));
	m_pActionRedo->setShortcuts(QKeySequence::Redo);
	m_pActionRedo->setShortcutContext(Qt::ApplicationShortcut);
	m_pActionRedo->setObjectName("Redo");

	LoadModel();

	connect(m_pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
	connect(m_pUndoStack, SIGNAL(indexChanged(int)), this, SLOT(on_undoStack_indexChanged(int)));
}

ProjectItem::~ProjectItem()
{
	DrawUnload();
	WidgetUnload();
	delete m_pModel;
	delete m_pUndoStack;
}

void ProjectItem::LoadModel()
{
	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		m_pModel = new SpriteModel(*this, m_ItemFileData);
		break;
	case ITEM_Text:
		m_pModel = new TextModel(*this, m_ItemFileData);
		break;
	case ITEM_Entity:
		m_pModel = new EntityModel(*this, m_ItemFileData);
		break;
	case ITEM_Prefab:
		m_pModel = new PrefabModel(*this, m_ItemFileData);
		break;
	case ITEM_Primitive:
		m_pModel = new PrimitiveModel(*this);
		break;
	case ITEM_Audio:
		m_pModel = new AudioModel(*this);
		break;
	default:
		HyGuiLog("Unimplemented item LoadModel(): " % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}
}

IModel *ProjectItem::GetModel()
{
	return m_pModel;
}

IWidget *ProjectItem::GetWidget()
{
	return m_pWidget;
}

IDraw *ProjectItem::GetDraw()
{
	return m_pDraw;
}

QUndoStack *ProjectItem::GetUndoStack()
{
	return m_pUndoStack;
}

void ProjectItem::GiveMenuActions(QMenu *pMenu)
{
	pMenu->addAction(m_pActionUndo);
	pMenu->addAction(m_pActionRedo);
	pMenu->addSeparator();

	m_pWidget->OnGiveMenuActions(pMenu);
}

void ProjectItem::Save(bool bWriteToDisk)
{
	FileDataPair bckup = m_ItemFileData;
	m_ItemFileData = m_pModel->GenerateFileData(m_pDraw);

	GetProject().SaveItemData(m_eTYPE, GetName(true), m_ItemFileData, bWriteToDisk);
	m_pUndoStack->setClean();

	m_bExistencePendingSave = false;
}

bool ProjectItem::IsExistencePendingSave()
{
	return m_bExistencePendingSave;
}

bool ProjectItem::IsSaveClean()
{
	return m_pUndoStack->isClean() && m_bExistencePendingSave == false;
}

void ProjectItem::DiscardChanges()
{
	m_pUndoStack->clear();

	delete m_pModel;
	LoadModel();
}

void ProjectItem::WidgetLoad()
{
	WidgetUnload();
	
	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		m_pWidget = new SpriteWidget(*this);
		break;
	case ITEM_Text:
		m_pWidget = new TextWidget(*this);
		break;
	case ITEM_Entity:
		m_pWidget = new EntityWidget(*this);
		break;
	case ITEM_Prefab:
		m_pWidget = new PrefabWidget(*this);
		break;
	default:
		HyGuiLog("Unimplemented WidgetLoad() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}

	m_pWidget->FocusState(0, -1);
}

void ProjectItem::WidgetUnload()
{
	delete m_pWidget;
	m_pWidget = nullptr;
}

void ProjectItem::DrawLoad()
{
	DrawUnload();

	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		m_pDraw = new SpriteDraw(this, m_ItemFileData);
		break;
	case ITEM_Text:
		m_pDraw = new TextDraw(this, m_ItemFileData);
		break;
	case ITEM_Entity:
		m_pDraw = new EntityDraw(this, m_ItemFileData);
		break;
	case ITEM_Prefab:
		m_pDraw = new PrefabDraw(this, m_ItemFileData);
		break;
	default:
		HyGuiLog("Unimplemented DrawLoad() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}

	m_pDraw->ApplyJsonData();
	m_pDraw->Load();
	m_pDraw->SetVisible(false, true);
}

void ProjectItem::DrawUnload()
{
	delete m_pDraw;
	m_pDraw = nullptr;
}

void ProjectItem::DrawShow()
{
	m_pDraw->Show();
}

void ProjectItem::DrawHide()
{
	m_pDraw->Hide();
}

void ProjectItem::BlockAllWidgetSignals(bool bBlock)
{
	if(m_pWidget == nullptr)
		return;
	
	QList<QWidget *> widgetList = m_pWidget->findChildren<QWidget *>();
	for(auto iter = widgetList.begin(); iter != widgetList.end(); ++iter)
		(*iter)->blockSignals(bBlock);
}

void ProjectItem::FocusWidgetState(int iStateIndex, QVariant subState)
{
	if(m_pWidget == nullptr)
		return;

	m_pWidget->FocusState(iStateIndex, subState);
	
	// This refreshes any changed values done programmatically
	m_pWidget->repaint();
}

/*virtual*/ void ProjectItem::DeleteFromProject() /*override*/
{
	m_pUndoStack->setClean();

	// Unlinks all dependencies
	m_pModel->RelinquishAllFrames();

	ExplorerItem::DeleteFromProject();
}

void ProjectItem::on_undoStack_cleanChanged(bool bClean)
{
	ProjectTabBar *pTabBar = m_pProject->GetTabBar();
	for(int i = 0; i < pTabBar->count(); ++i)
	{
		if(pTabBar->tabData(i).value<ProjectItem *>() == this)
		{
			if(bClean)
			{
				pTabBar->setTabText(i, GetName(false));
				pTabBar->setTabIcon(i, GetIcon(SUBICON_None));
			}
			else
			{
				pTabBar->setTabText(i, GetName(false) + "*");
				pTabBar->setTabIcon(i, GetIcon(SUBICON_Dirty));
			}

			QModelIndex index = m_pProject->GetExplorerModel().FindIndex<ProjectItem *>(this, 0);
			m_pProject->GetExplorerModel().dataChanged(index, index, QVector<int>() << Qt::DecorationRole);

			break;
		}
	}

	m_pProject->ApplySaveEnables();
}

void ProjectItem::on_undoStack_indexChanged(int iIndex)
{
	if(m_pDraw == nullptr)
	{
		HyGuiLog("m_pDraw was nullptr in on_undoStack_indexChanged", LOGTYPE_Error);
		return;
	}
	
	m_pDraw->ApplyJsonData();
}

