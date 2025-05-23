/**************************************************************************
*	SpineCrossFadeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPINECROSSFADEMODEL_H
#define SPINECROSSFADEMODEL_H

#include "Global.h"

class SpineCrossFade
{
public:
	SpineCrossFade(QString sAnimOne, QString sAnimTwo, float fMix) :
		m_sAnimOne(sAnimOne),
		m_sAnimTwo(sAnimTwo),
		m_fMixValue(fMix)
	{ }

	QString			m_sAnimOne;
	QString			m_sAnimTwo;
	float			m_fMixValue;
};

class SpineCrossFadeModel : public QAbstractTableModel
{
	Q_OBJECT

	QList<SpineCrossFade *>						m_CrossFadeList;
	QList<QPair<int, SpineCrossFade *>>			m_RemovedCrossFadeList;  // Used to reinsert crossfades (via undo/redo) while keeping their attributes

public:
	enum eColumn
	{
		COLUMN_AnimOne = 0,
		COLUMN_Mix,
		COLUMN_AnimTwo,

		NUMCOLUMNS
	};

	SpineCrossFadeModel(QObject *pParent);

	void AddNew(QString sAnimOne, QString sAnimTwo, float fMix);
	void InsertExisting(SpineCrossFade *pCrossFade);
	void Remove(SpineCrossFade *pCrossFade);
	void MoveRowUp(int iIndex);
	void MoveRowDown(int iIndex);
	void SetAnimOne(int iIndex, QString sAnimOne);
	void SetMix(int iIndex, float fMix);
	void SetAnimTwo(int iIndex, QString sAnimTwo);

	QJsonArray GetCrossFadeInfo();
	SpineCrossFade *GetCrossFadeAt(int iIndex);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

Q_SIGNALS:
	void editCompleted(const QString &);
};

#endif // SPINECROSSFADEMODEL_H
