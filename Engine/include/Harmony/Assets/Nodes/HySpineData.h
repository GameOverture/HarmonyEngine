/**************************************************************************
 *	HySpineData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HySpineData_h__
#define HySpineData_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/Nodes/IHyNodeData.h"

struct HySpineAtlas
{
	std::string					m_sName;
	HyFileAtlas *m_pAtlas;
	HyTextureHandle				m_hGfxApiHandle;
	const HyRectangle<float>	m_rSRC_RECT;

	// GUI temp data constructor
	HySpineAtlas(std::string sName, HyTextureHandle hGfxApiHandle) :
		m_sName(sName),
		m_pAtlas(nullptr),
		m_hGfxApiHandle(hGfxApiHandle),
		m_rSRC_RECT(0.0f, 1.0f, 1.0f, 0.0f)
	{ }

	// Normal runtime files constructor
	HySpineAtlas(std::string sName, HyFileAtlas *pAtlas, float fSrcLeft, float fSrcTop, float fSrcRight, float fSrcBot) :
		m_sName(sName),
		m_pAtlas(pAtlas),
		m_hGfxApiHandle(HY_UNUSED_HANDLE),
		m_rSRC_RECT(fSrcLeft, fSrcBot, fSrcRight, fSrcTop)
	{ }

	HyTextureHandle GetTexHandle() const
	{
		if(m_pAtlas)
			return m_pAtlas->GetTextureHandle();
		else
			return m_hGfxApiHandle;
	}
};

#ifdef HY_USE_SPINE // Custom engine class extensions to assist spine-cpp with loading
	class HySpineTextureLoader : public spine::TextureLoader
	{
		std::vector<HySpineAtlas> &	m_SubAtlasListRef;

	public:
		HySpineTextureLoader(std::vector<HySpineAtlas> &subAtlasListRef);
		virtual void load(spine::AtlasPage &page, const spine::String &path) override;
		virtual void unload(void *pTexture) override;
	};
#endif

class HySpineData : public IHyNodeData
{
	std::vector<HySpineAtlas>		m_SubAtlasList;

	spine::Atlas *					m_pAtlasData;
	spine::SkeletonData *			m_pSkeletonData;
	spine::AnimationStateData *		m_pAnimStateData;

public:
	HySpineData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HySpineData();

	HyTextureHandle GetTexHandle(uint32 uiAtlasIndex) const
	{
		return m_SubAtlasList[uiAtlasIndex].GetTexHandle();
	}

	spine::SkeletonData *GetSkeletonData() const;
	spine::AnimationStateData *GetAnimationStateData() const;

	// Sets every combination of animation blend to this amount
	void AnimInitBlend(float fInterpDur);

	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
	void AnimInitBlend(uint32 uiAnimIdFrom, uint32 uiAnimIdTo, float fInterpDur);
};

#endif /* HySpineData_h__ */
