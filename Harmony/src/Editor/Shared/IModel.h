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

#include "ProjectItem.h"

#include <QAbstractListModel>
#include <QStringListModel>

class IModel;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IStateData
{
protected:
	const int					m_iINDEX;
	IModel &					m_ModelRef;
	QString						m_sName;
	
public:
	IStateData(int iIndex, IModel &modelRef, QString sName);
	virtual ~IStateData();

	int GetIndex() const;
	
	QString GetName() const;
	void SetName(QString sNewName);
	
	virtual int AddFrame(AtlasFrame *pFrame) = 0;			// Returns the index the frame was inserted to
	virtual void RelinquishFrame(AtlasFrame *pFrame) = 0;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IModel : public QAbstractListModel
{
protected:
	ProjectItem &				m_ItemRef;
	QList<IStateData *>			m_StateList;
	
public:
	IModel(ProjectItem &itemRef);
	virtual ~IModel();

	ProjectItem &GetItem();
	const ProjectItem &GetItem() const;
	
	int GetNumStates();
	IStateData *GetStateData(int iStateIndex);
	
	QString SetStateName(int iStateIndex, QString sNewName);
	void MoveStateBack(int iStateIndex);
	void MoveStateForward(int iStateIndex);
	
	QList<AtlasFrame *> RequestFramesById(IStateData *pState, QList<quint32> requestList, int &iAffectedFrameIndexOut);
	QList<AtlasFrame *> RequestFrames(int iStateIndex, QList<AtlasFrame *> requestList, int &iAffectedFrameIndexOut);
	void RelinquishFrames(int iStateIndex, QList<AtlasFrame *> relinquishList);
	void RelinquishAllFrames();
	
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	
	template<typename STATEDATA>
	int AppendState(QJsonObject stateObj)
	{
		int iIndex = m_StateList.size();
		InsertState<STATEDATA>(iIndex, stateObj);
	
		return iIndex;
	}
	
	template<typename STATEDATA>
	void InsertState(int iStateIndex, QJsonObject stateObj)
	{
		STATEDATA *pNewState = new STATEDATA(iStateIndex, *this, stateObj);
	
		beginInsertRows(QModelIndex(), iStateIndex, iStateIndex);
		m_StateList.insert(iStateIndex, pNewState);
		endInsertRows();
	
		QVector<int> roleList;
		roleList.append(Qt::DisplayRole);
		dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);
	}
	
	virtual void OnSave() = 0;
	virtual QJsonObject PopStateAt(uint32 uiIndex) = 0;
	virtual QJsonValue GetJson() const = 0;
	virtual QList<AtlasFrame *> GetAtlasFrames() const = 0;
	virtual QStringList GetFontUrls() const = 0;
	virtual void Refresh() = 0;
};

#endif // IMODEL_H
