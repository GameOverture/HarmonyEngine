/**************************************************************************
*	ExplorerModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef EXPLORERMODEL_H
#define EXPLORERMODEL_H

#include "Global.h"
#include "Shared/TreeModel/ITreeModel.h"
#include "Explorer/ExplorerItemData.h"
#include "Project/Project.h"

class ProjectItemMimeData;

class ExplorerModel : public ITreeModel
{
	Q_OBJECT

public:
	ExplorerModel();
	virtual ~ExplorerModel();

	QStringList GetOpenProjectPaths();
	QStringList GetPrefixList(Project *pProject);
	ExplorerItemData *FindItemByItemPath(Project *pProject, QString sPath, ItemType eType);

	Project *AddProject(const QString sNewProjectFilePath);
	ExplorerItemData *AddItem(Project *pProj, ItemType eNewItemType, const QString sPrefix, const QString sName, FileDataPair initItemFileData, bool bIsPendingSave);
	bool RemoveItem(ExplorerItemData *pItem);

	QString AssemblePrefix(ExplorerItemData *pItem) const;

	bool PasteItemSrc(const ProjectItemMimeData *pProjMimeData, const QModelIndex &indexRef);

	virtual QVariant data(const QModelIndex &indexRef, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& indexRef) const override;
	virtual Qt::DropActions supportedDragActions() const override;
	virtual Qt::DropActions supportedDropActions() const override;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
	virtual QStringList mimeTypes() const override;
	virtual bool canDropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) const override;
	virtual bool dropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;

private:
	TreeModelItem *FindProjectTreeItem(Project *pProject);
	TreeModelItem *FindPrefixTreeItem(const QModelIndex &indexRef) const;
	QModelIndex FindIndexByItemPath(Project *pProject, QString sPath, ItemType eType);
};

#endif // EXPLORERMODEL_H
