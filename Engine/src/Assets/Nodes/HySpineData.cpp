/**************************************************************************
 *	HySpineData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HySpineData.h"
#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyIO.h"

#ifdef HY_USE_SPINE
spine::SpineExtension *spine::getDefaultExtension() {
	return new spine::DefaultSpineExtension();
}
#endif

HySpineData::HySpineData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_pAtlasData(nullptr),
	m_pSkeletonData(nullptr),
	m_pAnimStateData(nullptr)
{
#ifdef HY_USE_SPINE
	// Atlas ------------------------------------------------------------------------
	std::string sAtlasFilePath = assetsRef.GetDataDir() + "/" + HYASSETS_SpineDir;
	sAtlasFilePath += itemDataObj["UUID"].GetString();
	sAtlasFilePath = HyIO::CleanPath(sAtlasFilePath.c_str(), ".atlas", false);
	//std::vector<char> atlasFile;
	//HyIO::ReadTextFile(sAtlasFilePath.c_str(), atlasFile);
	
	m_pAtlasData = HY_NEW spine::Atlas(sAtlasFilePath.c_str(), nullptr, false); //atlasFile.data(), atlasFile.size(), "", nullptr, false);

	// Skeleton ---------------------------------------------------------------------
	float fScale = itemDataObj["scale"].GetFloat();
	std::string sSkeletonFilePath = assetsRef.GetDataDir() + "/" + HYASSETS_SpineDir;
	sSkeletonFilePath += itemDataObj["UUID"].GetString();
	if(itemDataObj["isBinary"].GetBool())
	{
		spine::SkeletonBinary binary(m_pAtlasData);
		binary.setScale(fScale);
		sSkeletonFilePath = HyIO::CleanPath(sSkeletonFilePath.c_str(), ".skel", false);

		m_pSkeletonData = binary.readSkeletonDataFile(sSkeletonFilePath.c_str());
		HyAssert(m_pSkeletonData, "HySpineData binary load failed: " << binary.getError().buffer());
	}
	else
	{
		spine::SkeletonJson json(m_pAtlasData);
		json.setScale(fScale);

		sSkeletonFilePath = HyIO::CleanPath(sSkeletonFilePath.c_str(), ".json", false);
		m_pSkeletonData = json.readSkeletonDataFile(sSkeletonFilePath.c_str());
		HyAssert(m_pSkeletonData, "HySpineData json load failed: " << json.getError().buffer());
	}

	// Animation Mixing ------------------------------------------------------------------
	m_pAnimStateData = HY_NEW spine::AnimationStateData(m_pSkeletonData);
	m_pAnimStateData->setDefaultMix(itemDataObj["defaultMix"].GetFloat()); // Set the default mix time between any pair of animations that isn't specified below (in seconds)

	HyJsonArray stateArray = itemDataObj["stateArray"].GetArray();
	m_uiNumStates = stateArray.Size();
	HyAssert(m_uiNumStates == m_pSkeletonData->getAnimations().size(), "Spine JSON data did not match # of states with spine skeleton file");
	for(uint32 i = 0; i < m_uiNumStates; ++i)
	{
		HyJsonObj stateObj = stateArray[i].GetObject();
		HyJsonObj crossFadesObj = stateObj["crossFades"].GetObject();

		const auto &sStateName = m_pSkeletonData->getAnimations()[i]->getName();
		for(auto iter = crossFadesObj.begin(); iter != crossFadesObj.end(); ++iter)
			m_pAnimStateData->setMix(sStateName, iter->name.GetString(), iter->value.GetFloat());
	}
	
	// Misc IHyNodeData --------------------------------------------------------------------
	m_RequiredAtlases;
	m_RequiredAudio;
	m_pGltf;

	//for(uint32 i = 0; i < m_uiNUMFRAMES; ++i)
	//{
	//	HyFileAtlas *pAtlas = nullptr;
	//	HyRectangle<float> rUVRect(0.0f, 0.0f, 0.0f, 0.0f);
	//	glm::ivec2 vOffset(0);
	//	float fDuration(0.0f);

	//	if(frameArray.Empty() == false)
	//	{
	//		HyJsonObj frameObj = frameArray[i].GetObject();

	//		pAtlas = assetsRef.GetAtlas(frameObj["checksum"].GetUint(), rUVRect);
	//		m_RequiredAtlases.Set(pAtlas->GetManifestIndex());

	//		HySetVec(vOffset, frameObj["offsetX"].GetInt(), frameObj["offsetY"].GetInt());
	//		fDuration = frameObj["duration"].GetFloat();
	//	}

	//	new (pFrameWriteLocation)HySpriteFrame(pAtlas,
	//		rUVRect.left, rUVRect.top, rUVRect.right, rUVRect.bottom,
	//		vOffset,
	//		fDuration);
	//}
#endif
}

/*virtual*/ HySpineData::~HySpineData()
{
	delete m_pAtlasData;
	delete m_pSkeletonData;
	delete m_pAnimStateData;
}

spine::SkeletonData *HySpineData::GetSkeletonData() const
{
	return m_pSkeletonData;
}

spine::AnimationStateData *HySpineData::GetAnimationStateData() const
{
	return m_pAnimStateData;
}

//
//HySpine2dData::~HySpine2dData()
//{
//	//spSkeletonData_dispose(m_SpineSkeletonData);
//	//spAtlas_dispose(m_SpineAtlasData);
//}
//
//// Below functions are invoked within the Spine API and expect to be overloaded
//void _spAtlasPage_createTexture(spAtlasPage* self, const char* path)
//{
//	// THIS IS INVOKED FROM THE LOAD THREAD from any IData::DoLoad()
//
//	// TODO: Convert 'path' to Atlas texture index
//	uint32 uiTextureIndex = 0;
//}
//
//void _spAtlasPage_disposeTexture(spAtlasPage* self)
//{
//}
//
//char* _spUtil_readFile(const char* path, int* length)
//{
//	// The returned data is freed within the spine API
//	return HyReadTextFile(path, length);
//}
