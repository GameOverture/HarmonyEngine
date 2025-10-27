/**************************************************************************
*	SpineCrossFadeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "SpineCrossFadeModel.h"

SpineCrossFadeModel::SpineCrossFadeModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

// Returns the index the crossfade was inserted to
void SpineCrossFadeModel::AddNew(QString sAnimOne, QString sAnimTwo, float fMix)
{
	int iInsertIndex = m_CrossFadeList.count();

	beginInsertRows(QModelIndex(), iInsertIndex, iInsertIndex);
	m_CrossFadeList.insert(iInsertIndex, new SpineCrossFade(sAnimOne, sAnimTwo, fMix));
	endInsertRows();
}

void SpineCrossFadeModel::InsertExisting(SpineCrossFade *pCrossFade)
{
	for(auto iter = m_RemovedCrossFadeList.begin(); iter != m_RemovedCrossFadeList.end(); ++iter)
	{
		if(iter->second == pCrossFade)
		{
			beginInsertRows(QModelIndex(), iter->first, iter->first);
			m_CrossFadeList.insert(iter->first, pCrossFade);
			endInsertRows();
			m_RemovedCrossFadeList.erase(iter);
			return;
		}
	}
}

void SpineCrossFadeModel::Remove(SpineCrossFade *pCrossFade)
{
	for(int i = 0; i < m_CrossFadeList.count(); ++i)
	{
		// NOTE: Don't delete this SpineCrossFade as the remove may be 'undone'
		if(m_CrossFadeList[i] == pCrossFade)
		{
			m_RemovedCrossFadeList.push_back(QPair<int, SpineCrossFade *>(i, m_CrossFadeList[i]));

			beginRemoveRows(QModelIndex(), i, i);
			m_CrossFadeList.removeAt(i);
			endRemoveRows();
			break;
		}
	}
}

void SpineCrossFadeModel::MoveRowUp(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
		return;

	m_CrossFadeList.swapItemsAt(iIndex, iIndex - 1);
	endMoveRows();
}

void SpineCrossFadeModel::MoveRowDown(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt logic deems it so
		return;

	m_CrossFadeList.swapItemsAt(iIndex, iIndex + 1);
	endMoveRows();
}

void SpineCrossFadeModel::SetAnimOne(int iIndex, QString sAnimOne)
{
	m_CrossFadeList[iIndex]->m_sAnimOne = sAnimOne;
	dataChanged(createIndex(iIndex, COLUMN_AnimOne), createIndex(iIndex, COLUMN_AnimTwo));
}

void SpineCrossFadeModel::SetMix(int iIndex, float fMix)
{
	m_CrossFadeList[iIndex]->m_fMixValue = fMix;
	dataChanged(createIndex(iIndex, COLUMN_AnimOne), createIndex(iIndex, COLUMN_AnimTwo));
}

void SpineCrossFadeModel::SetAnimTwo(int iIndex, QString sAnimTwo)
{
	m_CrossFadeList[iIndex]->m_sAnimTwo = sAnimTwo;
	dataChanged(createIndex(iIndex, COLUMN_AnimOne), createIndex(iIndex, COLUMN_AnimTwo));
}

QJsonArray SpineCrossFadeModel::GetCrossFadeInfo()
{
	QJsonArray framesArray;

	for(int i = 0; i < m_CrossFadeList.count(); ++i)
	{
		QJsonObject frameObj;
		frameObj.insert("animOne", QJsonValue(m_CrossFadeList[i]->m_sAnimOne));
		frameObj.insert("mix", QJsonValue(static_cast<double>(m_CrossFadeList[i]->m_fMixValue)));
		frameObj.insert("animTwo", QJsonValue(m_CrossFadeList[i]->m_sAnimTwo));

		framesArray.append(frameObj);
	}

	return framesArray;
}

SpineCrossFade *SpineCrossFadeModel::GetCrossFadeAt(int iIndex)
{
	if(iIndex < 0)
		return nullptr;

	return m_CrossFadeList[iIndex];
}

/*virtual*/ int SpineCrossFadeModel::rowCount(const QModelIndex & /*parent*/) const
{
	return m_CrossFadeList.count();
}

/*virtual*/ int SpineCrossFadeModel::columnCount(const QModelIndex & /*parent*/) const
{
	return NUMCOLUMNS;
}

/*virtual*/ QVariant SpineCrossFadeModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	SpineCrossFade *pCrossFade = m_CrossFadeList[index.row()];

	if(role == Qt::TextAlignmentRole && index.column() == COLUMN_Mix)
		return Qt::AlignCenter;

	if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole)
	{
		switch(index.column())
		{
		case COLUMN_AnimOne:
			return pCrossFade->m_sAnimOne;
		case COLUMN_Mix:
			if(role == Qt::UserRole)
				return QVariant(pCrossFade->m_fMixValue);
			else
				return QVariant(QString::number(pCrossFade->m_fMixValue, 'g', 3) % ((role == Qt::DisplayRole) ? " sec" : ""));
		case COLUMN_AnimTwo:
			return pCrossFade->m_sAnimTwo;
		}
	}

	return QVariant();
}

/*virtual*/ QVariant SpineCrossFadeModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if(role == Qt::DisplayRole)
	{
		if(orientation == Qt::Horizontal)
		{
			switch(iIndex)
			{
			case COLUMN_AnimOne:
				return QString("Animation One");
			case COLUMN_Mix:
				return QString("Mix");
			case COLUMN_AnimTwo:
				return QString("Animation Two");
			}
		}
		else
			return QString::number(iIndex);
	}

	return QVariant();
}

/*virtual*/ bool SpineCrossFadeModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	HyGuiLog("SpineCrossFadeModel::setData was invoked", LOGTYPE_Error);

	SpineCrossFade *pCrossFade = m_CrossFadeList[index.row()];

	if(role == Qt::EditRole)
	{
		switch(index.column())
		{
		case COLUMN_AnimOne:
			pCrossFade->m_sAnimOne = value.toString();
			break;
		case COLUMN_Mix:
			pCrossFade->m_fMixValue = value.toDouble();
			break;
		case COLUMN_AnimTwo:
			pCrossFade->m_sAnimTwo = value.toString();
			break;
		}
	}

	QVector<int> vRolesChanged;
	vRolesChanged.append(role);
	dataChanged(index, index, vRolesChanged);

	return true;
}

/*virtual*/ Qt::ItemFlags SpineCrossFadeModel::flags(const QModelIndex &index) const
{
	//if(index.column() == COLUMN_Frame)
	//	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	//else
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
