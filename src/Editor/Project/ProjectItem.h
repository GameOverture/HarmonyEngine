/**************************************************************************
 *	ProjectItem.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include "ExplorerItem.h"
#include "IDraw.h"
#include "IWidget.h"

#include <QUndoStack>
#include <QJsonObject>
#include <QUuid>

class AtlasWidget;
class AudioAssetsWidget;
class Project;
class AtlasFrame;
class IModel;

class ProjectItem : public ExplorerItem
{
	Q_OBJECT

	friend class Project;

	QJsonValue				m_SaveValue;
	QJsonValue				m_MetaValue;
	bool					m_bExistencePendingSave;

	// Loaded in constructor
	IModel *				m_pModel;
	QUndoStack *			m_pUndoStack;
	QAction *				m_pActionUndo;
	QAction *				m_pActionRedo;

	// Loaded when item is opened
	IWidget *				m_pWidget;
	IDraw *					m_pDraw;
	
public:
	ProjectItem(Project &projRef, HyGuiItemType eType, const QString sName, QJsonValue initSaveValue, QJsonValue initMetaValue, bool bIsPendingSave);
	virtual ~ProjectItem();

	void LoadModel();

	IModel *GetModel();
	IWidget *GetWidget();
	IDraw *GetDraw();
	QUndoStack *GetUndoStack();
	
	void GiveMenuActions(QMenu *pMenu);
	void Save(bool bWriteToDisk);
	bool IsExistencePendingSave();
	bool IsSaveClean();
	void DiscardChanges();
	
	void BlockAllWidgetSignals(bool bBlock);

	void FocusWidgetState(int iStateIndex, QVariant subState);
	
	virtual void DeleteFromProject() override;
	
private:
	void WidgetLoad();
	void WidgetUnload();
	
	void DrawLoad();
	void DrawUnload();
	void DrawShow();
	void DrawHide();

private Q_SLOTS:
	void on_undoStack_cleanChanged(bool bClean);
	void on_undoStack_indexChanged(int iIndex);
	
};
Q_DECLARE_METATYPE(ProjectItem *)

#endif // PROJECTITEM_H
