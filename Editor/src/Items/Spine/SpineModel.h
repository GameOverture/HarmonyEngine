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
#include "SpineCrossFadeModel.h"
#include "SpineSkinTreeModel.h"

class SpineStateData : public IStateData
{
	SpineSkinTreeModel		m_SkinTreeModel;

public:
	SpineStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~SpineStateData();

	SpineSkinTreeModel &GetSkinTreeModel();
	void Cmd_SetEnabledSkins(QStringList sEnabledSkinList); // Sets the model without invoking undo/redo commands
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
	QStringList GetSkins() const;

	// Bake the sub-atlas offset
	//void RewriteAtlasFile(AtlasFrame *pUpdatedFrame, QSize fullAtlasSize);

	void Cmd_AppendMix(const QString &sAnimOne, const QString &sAnimTwo, float fMixValue);
	void Cmd_RemoveMix(const QString &sAnimOne, const QString &sAnimTwo);

protected:
	void AcquireSpineData();
	void RegenAnimationStateData();
};

#endif // SPINEMODEL_H
