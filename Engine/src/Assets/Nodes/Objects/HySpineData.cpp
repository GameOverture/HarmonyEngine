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
#include "Assets/Nodes/Objects/HySpineData.h"
#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyIO.h"

#ifdef HY_USE_SPINE
// TODO: Properly implement this for Harmony
spine::SpineExtension *spine::getDefaultExtension() {
	return HY_NEW spine::DefaultSpineExtension();
}

HySpineTextureLoader::HySpineTextureLoader()
{
}

/*virtual*/ void HySpineTextureLoader::load(spine::AtlasPage &page, const spine::String &path) /*override*/
{
	std::string sFileName = HyIO::GetFileNameFromPath(path.buffer());
	HyIO::MakeLowercase(sFileName); // Make lowercase for easier compare

	uint32 uiNumAtlases = m_SubAtlasList.size();
	for(uint32 i = 0; i < uiNumAtlases; ++i)
	{
		std::string sSubAtlasName = m_SubAtlasList[i].m_sName;
		std::transform(sSubAtlasName.begin(), sSubAtlasName.end(), sSubAtlasName.begin(), ::tolower); // Make lowercase for easier compare

		if(strcmp(sSubAtlasName.c_str(), sFileName.c_str()) == 0)
		{
			page.texture = reinterpret_cast<void *>(&m_SubAtlasList[i]); // Store the sub-atlas index. The void *'s value (aka address) is essentially the integer index
			return;
		}
	}

	HyError("Spine SubAtlas page was not found: " << sFileName);
}

/*virtual*/ void HySpineTextureLoader::unload(void *pTexture) /*override*/
{
	HyLogError("Spine HySpineTextureLoader::unload() not implemented");
}
#endif

HySpineData::HySpineData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(nodePath)
#ifdef HY_USE_SPINE
	, m_pAtlasData(nullptr),
	m_pSkeletonData(nullptr),
	m_pAnimStateData(nullptr)
#endif
{
#ifdef HY_USE_SPINE
	bool bIsUsingTempFiles = false;
	std::string sDataDir = assetsRef.GetDataDir() + HYASSETS_SpineDir;
	if(itemDataObj.HasMember("usingTempFiles"))
	{
		bIsUsingTempFiles = true;
		sDataDir = itemDataObj["usingTempFiles"].GetString();
	}
	sDataDir += "/";

	HyJsonArray spineAtlasesArray = itemDataObj["atlases"].GetArray();
	uint32 uiNumAtlases = spineAtlasesArray.Size();
	for(uint32 i = 0; i < uiNumAtlases; ++i)
	{
		HyJsonObj atlasObj = spineAtlasesArray[i].GetObject();

		std::string sName = atlasObj["name"].GetString();
		
		HyUvCoord rSubAtlasUVRect;
		uint64 uiCropMask = 0;

		if(bIsUsingTempFiles == false) // The spine atlas is a sub-atlas packed into the project's texture assets
		{
			HyFileAtlas *pAtlas = assetsRef.GetAtlas(atlasObj["checksum"].GetUint(), atlasObj["bankId"].GetUint(), rSubAtlasUVRect, uiCropMask);
			HyAssert(pAtlas, "HySpineData atlas was not found with checksum: " << atlasObj["checksum"].GetUint());

			m_RequiredFiles[HYFILE_Atlas].Set(pAtlas->GetManifestIndex());
			m_SpineTextureLoader.m_SubAtlasList.push_back(HySpineAtlas(sName, pAtlas, rSubAtlasUVRect.left, rSubAtlasUVRect.top, rSubAtlasUVRect.right, rSubAtlasUVRect.bottom));
		}
		else // Using GUI temp files for atlases
		{
			uint32 uiAtlasWidth = static_cast<uint32>(HyMath::Max(0, atlasObj["subAtlasWidth"].GetInt()));
			uint32 uiAtlasHeight = static_cast<uint32>(HyMath::Max(0, atlasObj["subAtlasHeight"].GetInt()));

			HyJsonArray guiTexturesArray = itemDataObj["guiTextures"].GetArray();
			HyTextureHandle hGfxApiHandle = guiTexturesArray[i].GetUint();
			m_SpineTextureLoader.m_SubAtlasList.push_back(HySpineAtlas(sName, hGfxApiHandle));
		}
	}

	// TODO: utilize Spine's audio triggers at some point...
	//m_RequiredAudio

	// Atlas ------------------------------------------------------------------------
	std::string sAtlasFilePath = sDataDir;
	sAtlasFilePath += itemDataObj["UUID"].GetString();
	sAtlasFilePath = HyIO::CleanPath(sAtlasFilePath.c_str(), ".atlas");
	//std::vector<char> atlasFile;
	//HyIO::ReadTextFile(sAtlasFilePath.c_str(), atlasFile);
	
	m_pAtlasData = HY_NEW spine::Atlas(sAtlasFilePath.c_str(), &m_SpineTextureLoader, true);

	// Skeleton ---------------------------------------------------------------------
	float fScale = itemDataObj["scale"].GetFloat();
	std::string sSkeletonFilePath = sDataDir;
	sSkeletonFilePath += itemDataObj["UUID"].GetString();
	if(itemDataObj["isBinary"].GetBool())
	{
		spine::SkeletonBinary binary(m_pAtlasData);
		binary.setScale(fScale);
		sSkeletonFilePath = HyIO::CleanPath(sSkeletonFilePath.c_str(), ".skel");

		m_pSkeletonData = binary.readSkeletonDataFile(sSkeletonFilePath.c_str());
		HyAssert(m_pSkeletonData, "HySpineData binary load failed: " << binary.getError().buffer());
	}
	else
	{
		spine::SkeletonJson json(m_pAtlasData);
		json.setScale(fScale);

		sSkeletonFilePath = HyIO::CleanPath(sSkeletonFilePath.c_str(), ".json");
		m_pSkeletonData = json.readSkeletonDataFile(sSkeletonFilePath.c_str());
		HyAssert(m_pSkeletonData, "HySpineData json load failed: " << json.getError().buffer());
	}

	// Animation Mixing ------------------------------------------------------------------
	m_pAnimStateData = HY_NEW spine::AnimationStateData(m_pSkeletonData);
	m_pAnimStateData->setDefaultMix(itemDataObj["defaultMix"].GetFloat()); // Set the default mix time between any pair of animations that isn't specified below (in seconds)

	HyJsonArray crossFadesArray = itemDataObj["crossFades"].GetArray();
	int iNumCrossFades = crossFadesArray.Size();
	for(int i = 0; i < iNumCrossFades; ++i)
	{
		HyJsonObj crossFadesObj = crossFadesArray[i].GetObject();
		m_pAnimStateData->setMix(crossFadesObj["animOne"].GetString(), crossFadesObj["animTwo"].GetString(), crossFadesObj["mix"].GetFloat());
	}

	// States ------------------------------------------------------------------
	HyJsonArray stateArray = itemDataObj["stateArray"].GetArray();
	m_uiNumStates = stateArray.Size();
#endif
}

/*virtual*/ HySpineData::~HySpineData()
{
#ifdef HY_USE_SPINE
	delete m_pAnimStateData;
	delete m_pSkeletonData;
	delete m_pAtlasData;
#endif
}

HyTextureHandle HySpineData::GetTexHandle(uint32 uiAtlasIndex) const
{
	return m_SpineTextureLoader.m_SubAtlasList[uiAtlasIndex].GetTexHandle();
}

#ifdef HY_USE_SPINE
spine::SkeletonData *HySpineData::GetSkeletonData() const
{
	return m_pSkeletonData;
}

spine::AnimationStateData *HySpineData::GetAnimationStateData() const
{
	return m_pAnimStateData;
}
#endif

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
