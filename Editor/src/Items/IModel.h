/**************************************************************************
 *	IModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IMODEL_H
#define IMODEL_H

#include "ProjectItemData.h"

#include <QAbstractListModel>
#include <QStringListModel>

class IModel;
class IAssetItemData;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IStateData
{
protected:
	const int					m_iINDEX;
	IModel &					m_ModelRef;
	QString						m_sName;
	
public:
	IStateData(int iIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~IStateData();

	int GetIndex() const;
	
	QString GetName() const;
	void SetName(QString sNewName);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IModel : public QAbstractListModel
{
protected:
	ProjectItemData &				m_ItemRef;

	QList<IStateData *>				m_StateList;
	
public:
	IModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~IModel();

	template<typename STATEDATA>
	void InitStates(const FileDataPair &itemFileDataRef)
	{
		QJsonArray metaStateArray = itemFileDataRef.m_Meta["stateArray"].toArray();
		QJsonArray dataStateArray = itemFileDataRef.m_Data["stateArray"].toArray();

		assert(metaStateArray.size() == dataStateArray.size());

		// If item's 'itemFileDataRef' value is defined, parse and initialize with it, otherwise make default empty item
		if(metaStateArray.empty() == false)
		{
			for(int i = 0; i < metaStateArray.size(); ++i)
			{
				FileDataPair stateFileData;
				stateFileData.m_Meta = metaStateArray[i].toObject();
				stateFileData.m_Data = dataStateArray[i].toObject();
				AppendState<STATEDATA>(stateFileData);
			}
		}
		else
		{
			FileDataPair newStateFileData;
			newStateFileData.m_Meta["name"] = "";
			AppendState<STATEDATA>(newStateFileData);
		}
	}

	ProjectItemData &GetItem();
	const ProjectItemData &GetItem() const;

	const QUuid &GetUuid() const;
	
	int GetNumStates() const;
	IStateData *GetStateData(int iStateIndex);
	FileDataPair GetStateFileData(uint32 uiIndex) const;
	
	QString SetStateName(int iStateIndex, QString sNewName);
	void MoveStateBack(int iStateIndex);
	void MoveStateForward(int iStateIndex);

	FileDataPair PopState(uint32 uiIndex);

	QList<IAssetItemData *> GetAssetDependencies(AssetManagerType eAssetType) const;
	
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	
	template<typename STATEDATA>
	int AppendState(FileDataPair stateFileData)
	{
		int iIndex = m_StateList.size();
		InsertState<STATEDATA>(iIndex, stateFileData);
	
		return iIndex;
	}
	template<typename STATEDATA>
	void InsertState(int iStateIndex, FileDataPair stateFileData)
	{
		STATEDATA *pNewState = new STATEDATA(iStateIndex, *this, stateFileData);
	
		beginInsertRows(QModelIndex(), iStateIndex, iStateIndex);
		m_StateList.insert(iStateIndex, pNewState);
		endInsertRows();
	
		QVector<int> roleList;
		roleList.append(Qt::DisplayRole);
		dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);
	}

	virtual void OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) { }
	virtual bool OnPrepSave() = 0;	// Prepares item for saving, and returns if valid
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) = 0;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const = 0;
};

#endif // IMODEL_H
