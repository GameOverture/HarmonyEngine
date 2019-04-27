#pragma once
#include "Global.h"

#include <QAbstractItemModel>

class ExplorerItem;
class Project;

class ExplorerModel : public QAbstractItemModel
{
	QList<Project *>	m_ProjectList;

public:
	ExplorerModel();
	virtual ~ExplorerModel();

	Project *AddProject(const QString sNewProjectFilePath);

	// If importValue doesn't equal null, then this new ProjectItem will be saved upon creation
	void AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue importValue);
	void RemoveItem(ExplorerItem *pItem);

	virtual Qt::DropActions supportedDropActions() const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	virtual QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	// An insertRows() implementation must call beginInsertRows() before inserting new rows into the data structure, and endInsertRows() immediately afterwards.
	virtual bool insertRows(int iRow, int iCount, const QModelIndex &parentRef = QModelIndex()) override;
	
	// An insertColumns() implementation must call beginInsertColumns() before inserting new columns into the data structure, and endInsertColumns() immediately afterwards.
	virtual bool insertColumns(int iColumn, int iCount, const QModelIndex &parentRef = QModelIndex()) override;

	// A removeRows() implementation must call beginRemoveRows() before the rows are removed from the data structure, and endRemoveRows() immediately afterwards.
	virtual bool removeRows(int iRow, int iCount, const QModelIndex &parentRef = QModelIndex()) override;

	// A removeColumns() implementation must call beginRemoveColumns() before the columns are removed from the data structure, and endRemoveColumns() immediately afterwards.
	virtual bool removeColumns(int iColumn, int iCount, const QModelIndex &parentRef = QModelIndex()) override;

	//virtual void OnSave() override;
	//virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	//virtual QJsonValue GetJson() const override;
	//virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	//virtual QStringList GetFontUrls() const override;
	//virtual void Refresh() override;
};
