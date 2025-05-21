/**************************************************************************
*	SpineModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPINEMODEL_H
#define SPINEMODEL_H

#include "Global.h"
#include "IModel.h"
#include "GlobalWidgetMappers.h"

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

class SpineStateData : public IStateData
{
public:
	SpineStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~SpineStateData();
};
////////////////////////////////////////////////////////////////////////////////////////
struct SpineSubAtlas
{
	QFileInfo					m_ImageFileInfo;			// This may point to a temp directory (if unsaved) or in its meta dir
	AtlasFrame *				m_pAtlasFrame = nullptr;	// Once saved and packed into atlas manager this should be valid
};
////////////////////////////////////////////////////////////////////////////////////////
class SpineModel : public IModel
{
	Q_OBJECT

	QFileInfo					m_AtlasFileInfo;
	QFileInfo					m_SkeletonFileInfo;
	
	bool						m_bIsBinaryRuntime;
	float						m_fScale;					// This scale is meant to be applied to the json parser before generating the skeleton and needs to also be applied to the atlas

	DoubleSpinBoxMapper *		m_pDefaultMixMapper;

	SpineCrossFadeModel			m_CrossFadeModel;

#ifdef HY_USE_SPINE
	spine::Atlas *				m_pAtlasData;
	spine::SkeletonData *		m_pSkeletonData;
	spine::AnimationStateData *	m_pAnimStateData;
#endif

	QList<SpineSubAtlas>		m_SubAtlasList;

	bool						m_bUsingTempFiles;

public:
	SpineModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~SpineModel();

	DoubleSpinBoxMapper *GetDefaultMixMapper();
	SpineCrossFadeModel &GetCrossFadeModel();

	bool GetNextCrossFadeAnims(QList<QPair<QString, QString>> &crossFadePairListOut);

	virtual void OnPopState(int iPoppedStateIndex) override;
	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual void OnItemDeleted() override;

	bool IsUsingTempFiles() const;
	const QList<SpineSubAtlas> &GetSubAtlasList() const;

	// Bake the sub-atlas offset
	void RewriteAtlasFile(AtlasFrame *pUpdatedFrame, QSize fullAtlasSize);

	void Cmd_AppendMix(const QString &sAnimOne, const QString &sAnimTwo, float fMixValue);
	void Cmd_RemoveMix(const QString &sAnimOne, const QString &sAnimTwo);

protected:
	void AcquireSpineData();
	void RegenAnimationStateData();
};

#endif // SPINEMODEL_H
