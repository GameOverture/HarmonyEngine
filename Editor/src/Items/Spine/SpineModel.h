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

	virtual QVariant OnLinkAsset(AssetItemData *pAsset) override;
	virtual void OnUnlinkAsset(AssetItemData *pAsset) override;
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

	spine::Atlas *				m_pAtlasData;
	spine::SkeletonData *		m_pSkeletonData;
	spine::AnimationStateData *	m_pAnimStateData;

	QList<SpineSubAtlas>		m_SubAtlasList;

	bool						m_bUsingTempFiles;

public:
	SpineModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~SpineModel();

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(HyGuiItemType eType) const override;
	virtual QStringList GetFontUrls() const override;

	bool IsUsingTempFiles() const;
	const QList<SpineSubAtlas> &GetSubAtlasList() const;

protected:
	void AcquireSpineData();
};

#endif // SPINEMODEL_H
