/**************************************************************************
 *	ProjectItemData.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECTITEMDATA_H
#define PROJECTITEMDATA_H

#include "ExplorerItemData.h"
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
class PropertiesTreeModel;

class ProjectItemData : public ExplorerItemData
{
	Q_OBJECT
	friend class ExplorerModel;
	friend class Project;

	FileDataPair			m_ItemFileData;
	bool					m_bExistencePendingSave;

	// Loaded in constructor
	IModel *				m_pModel;
	QUndoStack *			m_pUndoStack;
	QAction *				m_pActionUndo;
	QAction *				m_pActionRedo;

	// Loaded when item is opened
	IWidget *				m_pWidget;
	IDraw *					m_pDraw;
	
	ProjectItemData(Project &projRef, ItemType eType, const QString sName, const FileDataPair &initItemFileDataRef, bool bIsPendingSave);
public:
	virtual ~ProjectItemData();

	void LoadModel();

	IModel *GetModel();
	IWidget *GetWidget();
	IDraw *GetDraw();
	QUndoStack *GetUndoStack();
	
	void GiveMenuActions(QMenu *pMenu);

	void GetLatestFileData(FileDataPair &itemFileDataOut) const;
	void GetSavedFileData(FileDataPair &itemFileDataOut) const;
	bool Save(bool bWriteToDisk);
	bool IsExistencePendingSave() const;
	bool IsSaveClean() const;
	void DiscardChanges();
	bool HasError() const;
	
	void BlockAllWidgetSignals(bool bBlock);

	void PropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty);
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

#endif // PROJECTITEMDATA_H
