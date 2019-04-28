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

	Project *AddProject(const QString sNewProjectFilePath);

	// If importValue doesn't equal null, then this new ProjectItem will be saved upon creation
	ExplorerItem *AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, QJsonValue importValue);

	void PasteItemSrc(QByteArray sSrc, Project *pProject, QString sPrefixOverride);

	virtual Qt::DropActions supportedDropActions() const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	//virtual void OnSave() override;
	//virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	//virtual QJsonValue GetJson() const override;
	//virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	//virtual QStringList GetFontUrls() const override;
	//virtual void Refresh() override;

private:
	QJsonObject ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames);
};

#endif // EXPLORERMODEL_H
