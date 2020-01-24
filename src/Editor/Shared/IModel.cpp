/**************************************************************************
 *	IModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IModel.h"
#include "AtlasModel.h"
#include "Project.h"

IStateData::IStateData(int iIndex, IModel &modelRef, FileDataPair stateFileData) :
	m_iINDEX(iIndex),
	m_ModelRef(modelRef),
	m_sName(stateFileData.m_Meta["name"].toString())
{ }

/*virtual*/ IStateData::~IStateData()
{ }

int IStateData::GetIndex() const
{
	return m_iINDEX;
}

QString IStateData::GetName() const
{
	return m_sName;
}

void IStateData::SetName(QString sNewName)
{
	m_sName = sNewName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IModel::IModel(ProjectItem &itemRef, const FileDataPair &itemFileDataRef) :
	QAbstractListModel(&itemRef),
	m_ItemRef(itemRef),
	m_UUID(itemFileDataRef.m_Meta["UUID"].toString())
{
}

/*virtual*/ IModel::~IModel()
{
}

ProjectItem &IModel::GetItem()
{
	return m_ItemRef;
}

const ProjectItem &IModel::GetItem() const
{
	return m_ItemRef;
}

int IModel::GetNumStates() const
{
	return m_StateList.size();
}

IStateData *IModel::GetStateData(int iStateIndex)
{
	return m_StateList[iStateIndex];
}

QString IModel::SetStateName(int iStateIndex, QString sNewName)
{
	QString sOldName = m_StateList[iStateIndex]->GetName();
	m_StateList[iStateIndex]->SetName(sNewName);

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);

	return sOldName;
}

void IModel::MoveStateBack(int iStateIndex)
{
	if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex - 1) == false)
		return;

	m_StateList.swap(iStateIndex, iStateIndex - 1);
	endMoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

void IModel::MoveStateForward(int iStateIndex)
{
	if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex + 2) == false)    // + 2 is here because Qt is retarded
		return;

	m_StateList.swap(iStateIndex, iStateIndex + 1);
	endMoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

QList<AtlasFrame *> IModel::RequestFramesByUuid(IStateData *pState, QList<QUuid> requestList, int &iAffectedFrameIndexOut)
{
	QList<AtlasFrame *> returnedAtlasFramesList = m_ItemRef.GetProject().GetAtlasModel().RequestFramesById(&m_ItemRef, requestList);
	
	if(pState)
	{
		for(int i = 0; i < returnedAtlasFramesList.size(); ++i)
			iAffectedFrameIndexOut = pState->AddFrame(returnedAtlasFramesList[i]);
	}
	
	return returnedAtlasFramesList;
}

QList<AtlasFrame *> IModel::RequestFrames(int iStateIndex, QList<AtlasFrame *> requestList, int &iAffectedFrameIndexOut)
{
	QList<AtlasFrame *> returnedAtlasFramesList = m_ItemRef.GetProject().GetAtlasModel().RequestFrames(&m_ItemRef, requestList);

	if(iStateIndex >= 0)
	{
		for(int i = 0; i < returnedAtlasFramesList.size(); ++i)
			iAffectedFrameIndexOut = m_StateList[iStateIndex]->AddFrame(returnedAtlasFramesList[i]);
	}

	return returnedAtlasFramesList;
}

void IModel::RelinquishFrames(int iStateIndex, QList<AtlasFrame *> relinquishList)
{
	if(iStateIndex >= 0)
	{
		for(int i = 0; i < relinquishList.size(); ++i)
			m_StateList[iStateIndex]->RelinquishFrame(relinquishList[i]);
	}
	
	m_ItemRef.GetProject().GetAtlasModel().RelinquishFrames(&m_ItemRef, relinquishList);
}

void IModel::RelinquishAllFrames()
{
	m_ItemRef.GetProject().GetAtlasModel().RelinquishFrames(&m_ItemRef, GetAtlasFrames());
}

FileDataPair IModel::PopState(uint32 uiIndex)
{
	FileDataPair retObj = GetStateFileData(uiIndex);

	beginRemoveRows(QModelIndex(), uiIndex, uiIndex);
	m_StateList.removeAt(uiIndex);
	endRemoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);

	return retObj;
}

FileDataPair IModel::GenerateFileData()
{
	FileDataPair fileData;

	// Assemble stateArray
	QJsonArray metaStateArray;
	QJsonArray dataStateArray;
	for(int i = 0; i < GetNumStates(); ++i)
	{
		FileDataPair stateFileData = GetStateFileData(i);
		metaStateArray.append(stateFileData.m_Meta);
		dataStateArray.append(stateFileData.m_Data);
	}
	fileData.m_Meta["stateArray"] = metaStateArray;
	fileData.m_Data["stateArray"] = dataStateArray;

	// Replace camera data if a draw instance is instantiated
	if(m_pDraw)
	{
		QJsonArray cameraPosArray;
		cameraPosArray.append(m_pDraw->GetCamera()->pos.X());
		cameraPosArray.append(m_pDraw->GetCamera()->pos.Y());
		m_ItemFileData.m_Meta["CameraPos"] = cameraPosArray;
		m_ItemFileData.m_Meta["CameraZoom"] = m_pDraw->GetCamera()->GetZoom();
	}

	// Assemble item specific data
	if(m_pModel->InsertItemSpecificData(m_ItemFileData) == false)
	{
		HyGuiLog(GetName(true) % " failed to save.", LOGTYPE_Warning);

		m_ItemFileData = bckup;
		return;
	}
}

/*virtual*/ int IModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return m_StateList.size();
}

/*virtual*/ QVariant IModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const /*override*/
{
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignLeft;

	if(role == Qt::DisplayRole || role == Qt::EditRole)
		return QString::number(index.row()) % " - " % m_StateList[index.row()]->GetName();

	return QVariant();
}

/*virtual*/ bool IModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/) /*override*/
{
	return QAbstractItemModel::setData(index, value, role);
}

/*virtual*/ QVariant IModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const /*override*/
{
	return (iIndex == 0) ? QVariant(QString("State Names")) : QVariant();
}
