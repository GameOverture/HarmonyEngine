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
#include "AudioWidgetManager.h"
#include "AtlasFrame.h"
#include "IModel.h"
#include "SpriteWidget.h"
#include "SpriteDraw.h"
#include "FontWidget.h"
#include "FontDraw.h"
#include "AudioWidget.h"
#include "AudioDraw.h"
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
						 QJsonValue initValue,
						 bool bIsPendingSave) :
	ExplorerItem(projRef, eType, sName),
	m_SaveValue(initValue),
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
}

/*virtual*/ void ProjectItem::Rename(QString sNewName)
{
	QString sOldPath = GetName(true);
	m_sPath = sNewName;
	QString sNewPath = GetName(true);

	m_pProject->RenameGameData(m_eTYPE, sOldPath, sNewPath, m_SaveValue);
}

void ProjectItem::LoadModel()
{
	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		m_pModel = new SpriteModel(*this, m_SaveValue.toArray());
		break;
	case ITEM_Font:
		m_pModel = new FontModel(*this, m_SaveValue.toObject());
		break;
	case ITEM_Entity:
		m_pModel = new EntityModel(*this, m_SaveValue.toArray());
		break;
	case ITEM_Prefab:
		m_pModel = new PrefabModel(*this, m_SaveValue);
		break;
	case ITEM_Primitive:
		m_pModel = new PrimitiveModel(*this);
		break;
	default:
		HyGuiLog("Unimplemented item LoadModel(): " % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}
}

void ProjectItem::GiveMenuActions(QMenu *pMenu)
{
	pMenu->addAction(m_pActionUndo);
	pMenu->addAction(m_pActionRedo);
	pMenu->addSeparator();

	QJsonValue saveValue;
	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		static_cast<SpriteWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
		break;
	case ITEM_Font:
		static_cast<FontWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
		break;
	case ITEM_Entity:
		static_cast<EntityWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
		break;
	case ITEM_Prefab:
		static_cast<PrefabWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
		break;
	default:
		HyGuiLog("Unimplemented item GiveMenuActions(): " % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}
}

void ProjectItem::Save()
{
	m_pModel->Refresh();

	m_pModel->OnSave();
	m_SaveValue = m_pModel->GetJson();

	GetProject().SaveGameData(m_eTYPE, GetName(true), m_SaveValue);
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
	m_pModel->Refresh();
	WidgetUnload();
	
	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		m_pWidget = new SpriteWidget(*this);
		break;
	case ITEM_Font:
		m_pWidget = new FontWidget(*this);
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
}

void ProjectItem::WidgetUnload()
{
	delete m_pWidget;
	m_pWidget = nullptr;
}

void ProjectItem::DrawLoad()
{
	m_pModel->Refresh();
	DrawUnload();

	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		m_pDraw = new SpriteDraw(this);
		break;
	case ITEM_Font:
		m_pDraw = new FontDraw(this);
		break;
	case ITEM_Entity:
		m_pDraw = new EntityDraw(this);
		break;
	case ITEM_Prefab:
		m_pDraw = new PrefabDraw(this);
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

	switch(m_eTYPE)
	{
	case ITEM_Sprite:
		static_cast<SpriteWidget *>(m_pWidget)->FocusState(iStateIndex, subState);
		break;
	case ITEM_Font:
		static_cast<FontWidget *>(m_pWidget)->FocusState(iStateIndex, subState);
		break;
	case ITEM_Entity:
		static_cast<EntityWidget *>(m_pWidget)->FocusState(iStateIndex, subState);
		break;
	case ITEM_Prefab:
		static_cast<PrefabWidget *>(m_pWidget)->FocusState(iStateIndex, subState);
		break;
	default:
		HyGuiLog("Unimplemented ProjectItem::FocusWidgetState() type: " % QString::number(m_eTYPE), LOGTYPE_Error);
		break;
	}

	// This refreshes any changed values done programmatically
	m_pWidget->repaint();
}

void ProjectItem::DeleteFromProject()
{
	m_pUndoStack->setClean();

	// Unlinks all dependencies
	m_pModel->RelinquishAllFrames();

	GetProject().DeleteGameData(m_eTYPE, GetName(true));
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
	
	m_pModel->Refresh();
	m_pDraw->ApplyJsonData();
}

