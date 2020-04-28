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

class ExplorerModel : public ITreeModel
{
	Q_OBJECT

	QMap<QUuid, ProjectItemData *>		m_ItemUuidMap;

public:
	ExplorerModel();
	virtual ~ExplorerModel();

	QStringList GetOpenProjectPaths();
	QStringList GetPrefixList(Project *pProject);
	QList<ExplorerItemData *> GetItemsRecursively(const QModelIndex &indexRef);
	ExplorerItemData *FindItemByItemPath(Project *pProject, QString sPath, HyGuiItemType eType);

	Project *AddProject(const QString sNewProjectFilePath);
	ExplorerItemData *AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, FileDataPair initItemFileData, bool bIsPendingSave);
	bool RemoveItem(ExplorerItemData *pItem);

	QString AssemblePrefix(ExplorerItemData *pItem) const;

	bool PasteItemSrc(QByteArray sSrc, const QModelIndex &indexRef);

	ProjectItemData *FindByUuid(QUuid uuid);

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
	bool InsertNewItem(ExplorerItemData *pNewItem, TreeModelItem *pParentTreeItem, int iRow = -1);
	TreeModelItem *FindProjectTreeItem(Project *pProject);
	TreeModelItem *FindPrefixTreeItem(const QModelIndex &indexRef) const;
	QModelIndex FindIndexByItemPath(Project *pProject, QString sPath, HyGuiItemType eType);
};

#endif // EXPLORERMODEL_H
