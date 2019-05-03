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
#include "Explorer/ExplorerItem.h"
#include "Project/Project.h"

class ExplorerModel : public ITreeModel
{
	Q_OBJECT

public:
	ExplorerModel();
	virtual ~ExplorerModel();

	QStringList GetOpenProjectPaths();
	QStringList GetPrefixList(Project *pProject);

	Project *AddProject(const QString sNewProjectFilePath);
	ExplorerItem *AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, QJsonValue initValue, bool bIsPendingSave);
	bool RemoveItem(ExplorerItem *pItem);

	QString AssemblePrefix(ExplorerItem *pItem) const;

	void PasteItemSrc(QByteArray sSrc, Project *pProject, QString sPrefixOverride);

	virtual QVariant data(const QModelIndex &indexRef, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& indexRef) const override;
	virtual Qt::DropActions supportedDropActions() const override;

private:
	bool InsertNewItem(ExplorerItem *pNewItem, TreeModelItem *pParentTreeItem, int iRow = -1);
	TreeModelItem *FindProjectTreeItem(Project *pProject);
	QJsonObject ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames);
};

#endif // EXPLORERMODEL_H
