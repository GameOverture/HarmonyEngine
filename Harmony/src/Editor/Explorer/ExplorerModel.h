#pragma once
#include "Global.h"
#include "Shared/TreeModel/ITreeModel.h"
#include "Explorer/ExplorerItem.h"
#include "Project/Project.h"

class ExplorerModel : public ITreeModel
{

public:
	ExplorerModel();
	virtual ~ExplorerModel();

	Project *AddProject(const QString sNewProjectFilePath);

	// If importValue doesn't equal null, then this new ProjectItem will be saved upon creation
	void AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, QJsonValue importValue);
	void RemoveItem(ExplorerItem *pItem);

	void PasteItemSrc(QByteArray sSrc, Project *pProject, QString sPrefixOverride);

	virtual Qt::DropActions supportedDropActions() const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	//virtual void OnSave() override;
	//virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	//virtual QJsonValue GetJson() const override;
	//virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	//virtual QStringList GetFontUrls() const override;
	//virtual void Refresh() override;

private:
	void RecursiveRemoveItem(ExplorerItem *pItem);
};
