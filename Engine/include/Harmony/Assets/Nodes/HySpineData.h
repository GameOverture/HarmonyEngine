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

#ifdef HY_USE_SPINE // Custom engine class extensions to assist spine-cpp with loading
	class HySpineTextureLoader : public spine::TextureLoader
	{
	public:
		virtual void load(spine::AtlasPage &page, const spine::String &path) override;
		virtual void unload(void *pTexture) override;
	};
#endif

struct HySpineAtlas
{
	std::string					m_sName;
	HyFileAtlas *				m_pAtlas;
	const HyRectangle<float>	m_rSRC_RECT;

	HySpineAtlas(std::string sName, HyFileAtlas *pAtlas, float fSrcLeft, float fSrcTop, float fSrcRight, float fSrcBot) :
		m_sName(sName),
		m_pAtlas(pAtlas),
		m_rSRC_RECT(fSrcLeft, fSrcBot, fSrcRight, fSrcTop)
	{ }

	HyTextureHandle GetGfxApiHandle() const
	{
		return m_pAtlas ? m_pAtlas->GetTextureHandle() : HY_UNUSED_HANDLE;
	}
	
	bool IsAtlasValid() const
	{
		return m_pAtlas != nullptr;
	}
};

class HySpineData : public IHyNodeData
{
	std::vector<HySpineAtlas>		m_SubAtlasList;

	spine::Atlas *					m_pAtlasData;
	spine::SkeletonData *			m_pSkeletonData;
	spine::AnimationStateData *		m_pAnimStateData;

public:
	HySpineData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HySpineData();

	spine::SkeletonData *GetSkeletonData() const;
	spine::AnimationStateData *GetAnimationStateData() const;

	// Sets every combination of animation blend to this amount
	void AnimInitBlend(float fInterpDur);

	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
	void AnimInitBlend(uint32 uiAnimIdFrom, uint32 uiAnimIdTo, float fInterpDur);
};

#endif /* HySpineData_h__ */
