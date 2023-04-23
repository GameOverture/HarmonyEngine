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

class SpineStateData : public IStateData
{
	QMap<QString, double>	m_CrossFadeMap;

public:
	SpineStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~SpineStateData();

	const QMap<QString, double> &GetCrossFadeMap() const;
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
	float						m_fScale;
	float						m_fDefaultMix;

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

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;

	bool IsUsingTempFiles() const;
	const QList<SpineSubAtlas> &GetSubAtlasList() const;

	// Bake the sub-atlas offset
	void RewriteAtlasFile(AtlasFrame *pUpdatedFrame, QSize fullAtlasSize);

protected:
	void AcquireSpineData();
};

#endif // SPINEMODEL_H
