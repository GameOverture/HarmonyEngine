/**************************************************************************
 *	SurfaceMaterialsModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SurfaceMaterialsModel.h"

SurfaceMaterialsModel::SurfaceMaterialsModel() :
	QAbstractListModel()
{
}

/*virtual*/ SurfaceMaterialsModel::~SurfaceMaterialsModel()
{
}

void SurfaceMaterialsModel::Initialize(QJsonArray initArray)
{
	beginResetModel();
	qDeleteAll(m_SurfaceList);
	m_SurfaceList.clear();
	for (const QJsonValue &surfaceVal : initArray)
	{
		QJsonObject surfaceObj = surfaceVal.toObject();
		Surface *pNewSurface = new Surface(surfaceObj);
		m_SurfaceList.append(pNewSurface);
	}
	endResetModel();
}

int SurfaceMaterialsModel::AppendNewSurface(QUuid tileSetDependee)
{
	Surface *pNewSurface = new Surface();
	pNewSurface->m_DependeeTileSetCollisions.append(tileSetDependee);
	beginInsertRows(QModelIndex(), m_SurfaceList.size(), m_SurfaceList.size());
	m_SurfaceList.append(pNewSurface);
	endInsertRows();

	return m_SurfaceList.size() - 1;
}

int SurfaceMaterialsModel::GetNumSurfaces() const
{
	return m_SurfaceList.size();
}

QUuid SurfaceMaterialsModel::GetUuid(int iIndex) const
{
	if(m_SurfaceList.size() <= iIndex || iIndex < 0)
	{
		HyGuiLog("SurfaceMaterialsModel::GetUuid invalid index: " + QString::number(iIndex), LOGTYPE_Error);
		return QUuid();
	}
	return m_SurfaceList[iIndex]->m_Uuid;
}

QString SurfaceMaterialsModel::GetName(int iIndex) const
{
	if(m_SurfaceList.size() <= iIndex || iIndex < 0)
	{
		HyGuiLog("SurfaceMaterialsModel::GetName invalid index: " + QString::number(iIndex), LOGTYPE_Error);
		return QString();
	}
	return m_SurfaceList[iIndex]->m_sName;
}

QString SurfaceMaterialsModel::SetName(int iIndex, QString sNewName)
{
	if(m_SurfaceList.size() <= iIndex || iIndex < 0)
	{
		HyGuiLog("SurfaceMaterialsModel::SetName invalid index: " + QString::number(iIndex), LOGTYPE_Error);
		return QString();
	}
	QString sOldName = m_SurfaceList[iIndex]->m_sName;
	m_SurfaceList[iIndex]->m_sName = sNewName;

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iIndex, 0), createIndex(iIndex, 0), roleList);

	return sOldName;
}

void SurfaceMaterialsModel::GetInfo(int iIndex, QString &nameOut, HyColor &colorOut) const
{
	if(m_SurfaceList.size() <= iIndex || iIndex < 0)
	{
		HyGuiLog("SurfaceMaterialsModel::GetInfo invalid index: " + QString::number(iIndex), LOGTYPE_Error);
		nameOut = QString();
		colorOut = HyColor::Black;
		return;
	}

	nameOut = m_SurfaceList[iIndex]->m_sName;

	QRect rectColor = m_SurfaceList[iIndex]->m_Properties.FindPropertyValue("Surface Properties", "Color").toRect();
	colorOut = HyColor(rectColor.left(), rectColor.top(), rectColor.right());
}

PropertiesTreeModel *SurfaceMaterialsModel::GetPropertiesModel(int iIndex)
{
	if(m_SurfaceList.size() <= iIndex || iIndex < 0)
	{
		HyGuiLog("SurfaceMaterialsModel::GetPropertiesModel invalid index: " + QString::number(iIndex), LOGTYPE_Error);
		return nullptr;
	}
	return &m_SurfaceList[iIndex]->m_Properties;
}

void SurfaceMaterialsModel::AddTileSetDependee(int iIndex, QUuid tileSetDependee)
{
	if(m_SurfaceList.size() <= iIndex || iIndex < 0)
	{
		HyGuiLog("SurfaceMaterialsModel::AddTileSetDependee invalid index: " + QString::number(iIndex), LOGTYPE_Error);
		return;
	}
	m_SurfaceList[iIndex]->m_DependeeTileSetCollisions.append(tileSetDependee);
}

void SurfaceMaterialsModel::RemoveTileSetDependee(int iIndex, QUuid tileSetDependee)
{
	if(m_SurfaceList.size() <= iIndex || iIndex < 0)
	{
		HyGuiLog("SurfaceMaterialsModel::RemoveTileSetDependee invalid index: " + QString::number(iIndex), LOGTYPE_Error);
		return;
	}
	m_SurfaceList[iIndex]->m_DependeeTileSetCollisions.removeAll(tileSetDependee);
}

void SurfaceMaterialsModel::MoveSurfaceBack(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
		return;

	m_SurfaceList.swapItemsAt(iIndex, iIndex - 1);
	endMoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iIndex, 0), createIndex(iIndex, 0), roleList);
}

void SurfaceMaterialsModel::MoveSurfaceForward(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)
		return;

	m_SurfaceList.swapItemsAt(iIndex, iIndex + 1);
	endMoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iIndex, 0), createIndex(iIndex, 0), roleList);
}

QJsonArray SurfaceMaterialsModel::SurfaceMaterialsModel::Serialize() const
{
	QJsonArray surfaceMaterialArray;
	for(int iRow = 0; iRow < rowCount(); ++iRow)
	{
		Surface *pSurface = m_SurfaceList[iRow];

		QJsonObject surfaceMatObj = data(index(iRow, 0), Qt::UserRole).toJsonObject();
		surfaceMaterialArray.append(surfaceMatObj);
	}

	return surfaceMaterialArray;
}

/*virtual*/ int SurfaceMaterialsModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return m_SurfaceList.count();
}

/*virtual*/ QVariant SurfaceMaterialsModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const /*override*/
{
	if(!index.isValid())
		return QVariant();
	int iRow = index.row();
	
	if(role == Qt::DisplayRole)
	{
		Surface *pSurface = m_SurfaceList[iRow];
		if(!pSurface)
		{
			HyGuiLog("SurfaceMaterialsModel::data found null surface at row: " + QString::number(iRow), LOGTYPE_Error);
			return QVariant();
		}
		
		return QVariant(pSurface->m_sName);
	}
	else if(role == Qt::UserRole)
	{
		Surface *pSurface = m_SurfaceList[iRow];
		if(!pSurface)
		{
			HyGuiLog("SurfaceMaterialsModel::data found null surface at row: " + QString::number(iRow), LOGTYPE_Error);
			return QVariant();
		}
		
		return QVariant(pSurface->Serialize());
	}
	
	return QVariant();
}

/*virtual*/ QVariant SurfaceMaterialsModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const /*override*/
{
	return QVariant();
}
